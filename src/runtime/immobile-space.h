/*
 * This software is part of the SBCL system. See the README file for
 * more information.
 *
 * This software is derived from the CMU CL system, which was
 * written at Carnegie Mellon University and released into the
 * public domain. The software is in the public domain and is
 * provided with absolutely no warranty. See the COPYING and CREDITS
 * files for more information.
 */

#ifndef _IMMOBILE_SPACE_H_
#define _IMMOBILE_SPACE_H_

#ifdef LISP_FEATURE_IMMOBILE_SPACE
#include <limits.h>
#include "core.h"

void prepare_immobile_space_for_save(lispobj init_function, boolean verbose);

typedef int low_page_index_t;

static inline void *
fixedobj_page_address(low_page_index_t page_num)
{
    return (void*)(FIXEDOBJ_SPACE_START + (page_num * IMMOBILE_CARD_BYTES));
}
static inline void *
varyobj_page_address(low_page_index_t page_num)
{
    return (void*)(VARYOBJ_SPACE_START + (page_num * IMMOBILE_CARD_BYTES));
}

static inline low_page_index_t find_fixedobj_page_index(void *addr)
{
  if (addr >= (void*)FIXEDOBJ_SPACE_START) {
      // Must use full register size here to avoid truncation of quotient
      // and bogus result!
      page_index_t index =
          ((uintptr_t)addr - (uintptr_t)FIXEDOBJ_SPACE_START) / IMMOBILE_CARD_BYTES;
      if (index < (int)(FIXEDOBJ_SPACE_SIZE/IMMOBILE_CARD_BYTES))
          return index;
  }
  return -1;
}
static inline low_page_index_t find_varyobj_page_index(void *addr)
{
  if (addr >= (void*)VARYOBJ_SPACE_START) {
      // Must use full register size here to avoid truncation of quotient
      // and bogus result!
      size_t offset = (uintptr_t)addr - (uintptr_t)VARYOBJ_SPACE_START;
      if (offset >= varyobj_space_size)
          return -1;
      return offset / IMMOBILE_CARD_BYTES;
  }
  return -1;
}

static inline boolean immobile_space_p(lispobj obj)
{
/* To test the two immobile ranges, we first check that a pointer is within
 * the outer bounds, and then that is not in the excluded middle (if any).
 * This requires only 1 comparison to weed out dynamic-space pointers,
 * vs doing the more obvious 2 tests, provided that dynamic space starts
 * above 4GB. range_1_max == range_2_min if there is no discontinuity. */
    uword_t offset = obj - immobile_space_lower_bound;
    if (offset >= immobile_space_max_offset) return 0;
    return !(immobile_range_1_max_offset <= offset
             && offset < immobile_range_2_min_offset);
}
#else

static inline boolean immobile_space_p(lispobj obj) { return 0; }

#endif

#endif // _IMMOBILE_SPACE_H_

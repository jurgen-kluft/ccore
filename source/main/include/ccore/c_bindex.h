#ifndef __CCORE_VIRTUAL_ALLOCATION_BINDEX_H__
#define __CCORE_VIRTUAL_ALLOCATION_BINDEX_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

namespace ncore
{
    namespace nbindex16
    {
        // An indexed allocation bin that can allocate small fixed size items that are tracked by index.
        // This bin can maintain a continuous array of items that can be compacted when freeing items,
        // making it suitable for use-cases where you need to iterate over all items without holes.
        // An index is returned when freeing an item, which is the index of the item that was moved
        // to fill the hole.
        // You can make a bin for any element size you want (soft limit to 1 KiB).
        // The maximum number of indices you can have in this indexed bin is 65,536 (64K),
        // since this uses indices of u16.

        struct bindex_t;
        bindex_t* make_bin(u16 element_size, u32 max_elements);   // create an indexed bin that can hold max elements of size element_size
        void      destroy(bindex_t* bin);                         // destroy the index bin
        i32       alloc(bindex_t* bin);                           // allocate from the bin, returns index or -1 if full
        i32       free(bindex_t* bin, u32 index);                 // free the index (return index used to fill hole, -1 if no swap performed)
        void*     idx2ptr(bindex_t const* bin, u32 index);        // convert an index to a pointer to the element
        i32       ptr2idx(bindex_t const* bin, void const* ptr);  // convert a pointer to an index (returns -1 if pointer is out of range)
        u32       size(bindex_t const* bin);                      // number of indices currently allocated
        u32       capacity(bindex_t const* bin);                  // maximum number of indices in the bin
    }  // namespace nbindex16
}  // namespace ncore

#endif  // __CCORE_VIRTUAL_ALLOCATION_BINDEX_H__

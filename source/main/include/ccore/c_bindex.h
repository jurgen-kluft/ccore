#ifndef __CCORE_VIRTUAL_ALLOCATION_BINDEX_H__
#define __CCORE_VIRTUAL_ALLOCATION_BINDEX_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
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
        // The maximum number of indices you can have in this indexed bin is 65,535,
        // since this uses indices of u16.

        struct bindex_t;
        bindex_t* make_bin(u16 element_size);                     // create an indexed bin that can hold max elements of size element_size
        void      commit(bindex_t* bin, u32 num_elements);        // prepare the bin (commit pages) to allocate without page commits
        void      destroy(bindex_t* bin);                         // destroy the index bin
        i32       alloc(bindex_t* bin, u16 owner_index);          // allocate from the bin, returns item index or -1 if full
        i32       free(bindex_t* bin, u32 item_index);            // free the index (return owner of item that was moved, -1 if no swap performed)
        void*     idx2ptr(bindex_t const* bin, u32 index);        // convert an index to a pointer to the element
        i32       ptr2idx(bindex_t const* bin, void const* ptr);  // convert a pointer to an index (returns -1 if pointer is out of range)
        u32       size(bindex_t const* bin);                      // current number of items in the bin
    }  // namespace nbindex16
}  // namespace ncore

#endif  // __CCORE_VIRTUAL_ALLOCATION_BINDEX_H__

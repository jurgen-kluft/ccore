#ifndef __CCORE_INDEXED_BIN_16_H__
#define __CCORE_INDEXED_BIN_16_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    struct arena_t;

    // An indexed allocation bin that can allocate small fixed size items that are tracked by index.
    // This bin can maintain a,  array of items that can be compacted when freeing items or when calling
    // compact, so that all alive items are packed towards the beginning of the array, and all free slots
    // are towards the end of the array, making it suitable for use-cases where you need to iterate over
    // all items without holes.
    // You can make a bin for any element size you want (soft limit to 1 KiB).
    // The maximum number of indices you can have in this indexed bin is 65,535,
    // since this uses indices of u16.

    struct indexed_bin16_t
    {
        arena_t* m_owner;             // index array (u16[])
        arena_t* m_items;             // item array (item[])
        u32      m_items_count;       // number of items currently in use
        u32      m_items_free_index;  // index of the first free slot in the items array
        u16      m_item_sizeof;       // sizeof(item)
        arena_t* m_binmap;            // binmap, will contain data for used and free binmap
    };

    // content of m_binmap
    // u64      m_free_bin0;          //
    // u64      m_used_bin0;          //
    // u64      m_free_bin1[16];      // track the 0 bits in m_entity_bin2 (16 * sizeof(u64) = 128 bytes)
    // u64      m_used_bin1[16];      // track the 1 bits in m_entity_bin2 (16 * sizeof(u64) = 128 bytes)
    // u64      m_bin2[1024];         // '1' bit = used item, '0' bit = free item (65536 bits = 8 KB)

    void  bin_setup(indexed_bin16_t* bin, u16 element_size);           // create an indexed bin that can hold max elements of size element_size
    void  bin_commit(indexed_bin16_t* bin, u32 num_elements);          // prepare the bin (commit pages) to allocate without page commits
    void  bin_destroy(indexed_bin16_t* bin);                           // destroy the index bin
    i32   bin_alloc(indexed_bin16_t* bin, u16 owner_index);            // allocate from the bin, returns item index or -1 if full
    void  bin_free_normal(indexed_bin16_t* bin, u32 item_index);       // free the index (return owner of item that was moved, -1 if no swap performed)
    i32   bin_free_compact(indexed_bin16_t* bin, u32 item_index);      // free the index (return owner of item that was moved, -1 if no swap performed)
    i32   bin_compact(indexed_bin16_t* bin, u32& item_index);          // compact the bin (return owner of item that was moved, and update item_index to the new index of the item)
    void* bin_idx2ptr(indexed_bin16_t const * bin, u32 index);         // convert an index to a pointer to the element
    i32   bin_ptr2idx(indexed_bin16_t const * bin, void const * ptr);  // convert a pointer to an index (returns -1 if pointer is out of range)
    u32   bin_size(indexed_bin16_t const * bin);                       // current number of items in the bin

}  // namespace ncore

#endif  // __CCORE_INDEXED_BIN_16_H__

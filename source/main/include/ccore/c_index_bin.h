#ifndef __CCORE_INDEX_BIN_H__
#define __CCORE_INDEX_BIN_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    struct arena_t;

    // An indexed allocation bin that can allocate small fixed size items that are tracked by index.
    // This bin can maintain an array of items that can be compacted when calling  compact, so that
    // all active items are packed towards the beginning of the array, and all free slots are towards
    // the end of the array, making it suitable for use-cases where you need to iterate over all
    // items without holes.
    // You can make a bin for any element size you want (soft limit to 1 KiB).
    // The maximum number of indices you can have in this indexed bin is 65_535, since this uses a
    // duomap of 3 levels (6 bits per level = 18 bits, but capped at 16 bits/64K).

    struct ibin16_t
    {
        arena_t* m_tags;         // tags array (u16[])
        arena_t* m_items;        // items array (item[])
        u32      m_items_count;  // number of items currently in use
        u32      m_item_sizeof;  // sizeof(item)
        arena_t* m_binmap;       // duomap
    };

    void  bin_setup(ibin16_t* bin, u16 element_size);           // create an indexed bin that can hold max elements of size element_size
    void  bin_commit(ibin16_t* bin, u32 num_elements);          // prepare the bin (commit pages) to allocate without page commits
    void  bin_destroy(ibin16_t* bin);                           // destroy the index bin
    i32   bin_alloc(ibin16_t* bin, u16 tag);                    // allocate an item from the bin, and associate it with the given tag (returns index of allocated item, or -1 if full)
    i32   bin_get_tag(ibin16_t const * bin, u32 item_index);    // get the tag associated with an item index, or -1 if index is out of range
    void  bin_set_tag(ibin16_t* bin, u32 item_index, u16 tag);  // set the tag associated with an item index
    void  bin_free(ibin16_t* bin, u32 item_index);              // free the index (no compaction, just mark as free)
    void* bin_idx2ptr(ibin16_t const * bin, u32 index);         // convert an index to a pointer to the element
    i32   bin_ptr2idx(ibin16_t const * bin, void const * ptr);  // convert a pointer to an index (returns -1 if pointer is out of range)
    u32   bin_size(ibin16_t const * bin);                       // current number of items in the bin

    // executes one compaction step, which moves the last used item to the first free slot
    // @out_item_index: if compaction was performed, this will be set to the new index of the item that was moved to fill the hole
    // returns the index of the item that was moved, or -1 if no compaction was performed
    i32 bin_compact(ibin16_t* bin, u32& out_item_index);

    // TODO: add helper functions to iterate over all items in the bin, and get their tags and indices

    struct ibin32_t
    {
        arena_t* m_tags;         // tags array (u32[])
        arena_t* m_items;        // items array (item[])
        u32      m_items_count;  // number of items currently in use
        u16      m_item_sizeof;  // sizeof(item)
        arena_t* m_binmap;       // duomap level 0, 1 and 2(f)
        arena_t* m_binmap2u;     // duomap level 2(u)
        arena_t* m_binmap3;      // duomap level 3
    };

    void  bin_setup(ibin32_t* bin, u16 element_size);           // create an indexed bin that can hold max elements of size element_size
    void  bin_commit(ibin32_t* bin, u32 num_elements);          // prepare the bin (commit pages) to allocate without page commits
    void  bin_destroy(ibin32_t* bin);                           // destroy the index bin
    i32   bin_alloc(ibin32_t* bin, u32 tag);                    // allocate an item from the bin, and associate it with the given tag (returns index of allocated item, or -1 if full)
    i32   bin_get_tag(ibin32_t const * bin, u32 item_index);    // get the tag associated with an item index, or -1 if index is out of range
    void  bin_set_tag(ibin32_t* bin, u32 item_index, u32 tag);  // set the tag associated with an item index
    void  bin_free(ibin32_t* bin, u32 item_index);              // free the index (no compaction, just mark as free)
    void* bin_idx2ptr(ibin32_t const * bin, u32 index);         // convert an index to a pointer to the element
    i32   bin_ptr2idx(ibin32_t const * bin, void const * ptr);  // convert a pointer to an index (returns -1 if pointer is out of range)
    u32   bin_size(ibin32_t const * bin);                       // current number of items in the bin

    // executes one compaction step, which moves the last used item to the first free slot
    // @out_item_index: if compaction was performed, this will be set to the new index of the item that was moved to fill the hole
    // returns the index of the item that was moved, or -1 if no compaction was performed
    i32 bin_compact(ibin32_t* bin, u32& out_item_index);

}  // namespace ncore

#endif  // __CCORE_INDEX_BIN_H__

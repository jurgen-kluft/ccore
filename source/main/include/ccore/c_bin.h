#ifndef __CCORE_BIN_H__
#define __CCORE_BIN_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    struct arena_t;

    // This is an allocation bin that can allocate small fixed size items, however you can
    // make a bin for any size you want (soft limit to 1 KiB per item).
    // The maximum number of items you can have in a single bin is 16,777,216 (16 Million),
    // this uses a hierarchical bitmap with 4 levels (6 bits per level = 24 bits = 16M).

    struct bin32_t
    {
        arena_t* m_items;              // memory for items
        u32      m_items_count;        // number of items currently in use
        u16      m_item_sizeof;        // sizeof(item)
        u8       m_bin_level_count;    // binmap, number of levels
        u8       m_bin_level2_offset;  // offset in u64 units to level 2
        arena_t* m_bin;                // level 0, 1 and 2 of binmap
        arena_t* m_bin3;               // level 3 of binmap (optional)
    };

    void  bin_setup(bin32_t* bin, u16 item_size, u32 max_items);  // e.g. item_size = 256, max_items = 65535, 16 MiB
    void  bin_destroy(bin32_t* bin);                              // destroy the bin
    u32   bin_size(bin32_t const* bin);                           // number of items currently in the bin
    void* bin_alloc(bin32_t* bin);                                // allocate an item from the bin
    void  bin_free(bin32_t* bin, void* ptr);                      // free an item back to the bin
    u32   bin_ptr2idx(bin32_t const* bin, void* ptr);             // convert a pointer to an index within the bin
    void* bin_idx2ptr(bin32_t* bin, u32 index);                   // convert an index to a pointer within the bin
    u32   bin_highwater_mark(bin32_t const* bin);                 // highest number of items that have been in the bin
    s32   bin_highest_free(bin32_t const* bin);                   // highest index of free item in the bin

    // This is an allocation bin that can allocate small fixed size items, however you can
    // make a bin for any size you want (soft limit to 1 KiB per item).
    // The maximum number of items you can have in a single bin16 is 65,536 (64K),
    // this uses a hierarchical bitmap with 3 levels (6 bits per level = but capped at 16 bits/64K).

    struct bin16_t
    {
        arena_t* m_items;             // pointer to items
        u32      m_items_count;       // number of items currently in use
        u16      m_item_sizeof;       // sizeof(item)
        u16      m_bin_level_count;   // binmap, number of levels
        arena_t* m_bin;               // level 0, 1 and 2 of binmap
    };

    void  bin_setup(bin16_t* bin, u16 item_size, u32 max_items);  // e.g. item_size = 256, max_items = 65535, 16 MiB
    void  bin_destroy(bin16_t* bin);                              // destroy the bin
    u32   bin_size(bin16_t const* bin);                           // number of items currently in the bin
    u32   bin_capacity(bin16_t const* bin);                       // maximum number of items the bin can hold
    void* bin_alloc(bin16_t* bin);                                // allocate an item from the bin
    void  bin_free(bin16_t* bin, void* ptr);                      // free an item back to the bin
    i32   bin_ptr2idx(bin16_t const* bin, void* ptr);             // convert a pointer to an index within the bin
    void* bin_idx2ptr(bin16_t* bin, u16 index);                   // convert an index to a pointer within the bin
    u32   bin_highwater_mark(bin16_t const* bin);                 // highest number of items that have been in the bin
    s32   bin_highest_free(bin16_t const* bin);                   // highest index of free item in the bin

}  // namespace ncore

#endif  // __CCORE_BIN_H__

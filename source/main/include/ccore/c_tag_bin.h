#ifndef __CCORE_TAGGED_BIN_H__
#define __CCORE_TAGGED_BIN_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_bin.h"

namespace ncore
{
    struct arena_t;

    // This is a bin but each item has an associated tag of type u32, stored in
    // a separate arena, that can be used to store metadata about the item.
    struct tbin_t
    {
        bin32_t  m_bin;   // underlying bin for items
        arena_t* m_tags;  // memory for tags
    };

    void  bin_setup(tbin_t* bin, u16 item_size, u32 max_items);  // e.g. item_size = 256, max_items = 65535, 16 MiB
    void  bin_destroy(tbin_t* bin);                              // destroy the bin
    u32   bin_size(tbin_t const* bin);                           // number of items currently in the bin
    u32   bin_capacity(tbin_t const* bin);                       // maximum number of items the bin can hold
    void* bin_alloc(tbin_t* bin, u32 tag);                       // allocate an item from the bin with a tag
    void  bin_free(tbin_t* bin, void* ptr);                      // free an item back to the bin
    u32   bin_ptr2idx(tbin_t const* bin, void* ptr);             // convert a pointer to an index within the bin
    void* bin_idx2ptr(tbin_t* bin, u32 index);                   // convert an index to a pointer within the bin
    u32   bin_highwater_mark(tbin_t const* bin);                 // highest number of items that have been in the bin
    s32   bin_highest_free(tbin_t const* bin);                   // highest index of free item in the bin

    u32  bin_get_tag(tbin_t const* bin, u32 index);     // get the tag associated with an item index
    void bin_set_tag(tbin_t* bin, u32 index, u32 tag);  // set the tag associated with an item index
}  // namespace ncore

#endif  // __CCORE_TAGGED_BIN_H__

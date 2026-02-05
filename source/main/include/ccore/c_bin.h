#ifndef __CCORE_VIRTUAL_ALLOCATION_BIN_H__
#define __CCORE_VIRTUAL_ALLOCATION_BIN_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    namespace nbin
    {
        // This is an allocation bin that can allocate small fixed size items, however you can
        // make a bin for any size you want (soft limit to 1 KiB per item).
        // The maximum number of items you can have in a single bin is 16,777,216 (16 Million),
        // this uses a hierarchical bitmap with 4 levels (6 bits per level = 24 bits = 16M).

        struct bin_t;
        bin_t* make_bin(u16 item_size, u32 max_items);  // e.g. item_size = 256, max_items = 65535, 16 MiB
        void   destroy(bin_t* bin);                     // destroy the bin
        u32    size(bin_t const* bin);                  // number of items currently in the bin
        u32    capacity(bin_t const* bin);              // maximum number of items the bin can hold
        void*  alloc(bin_t* bin);                       // allocate an item from the bin
        void   free(bin_t* bin, void* ptr);             // free an item back to the bin
        u32    ptr2idx(bin_t const* bin, void* ptr);    // convert a pointer to an index within the bin
        void*  idx2ptr(bin_t* bin, u32 index);          // convert an index to a pointer within the bin
        u32    highwater_mark(bin_t const* bin);        // highest number of items that have been in the bin
        s32    highest_free(bin_t const* bin);          // highest index of free item in the bin
    }  // namespace nbin

    namespace nbin16
    {
        // This is an allocation bin that can allocate small fixed size items, however you can
        // make a bin for any size you want (soft limit to 1 KiB per item).
        // The maximum number of items you can have in a single bin16 is 65,536 (64K),
        // this uses a hierarchical bitmap with 3 levels (6 bits per level = max 256 KiB).

        struct bin_t;
        bin_t* make_bin(u16 item_size, u32 max_items);  // e.g. item_size = 256, max_items = 65535, 16 MiB
        void   destroy(bin_t* bin);                     // destroy the bin
        u32    size(bin_t const* bin);                  // number of items currently in the bin
        u32    capacity(bin_t const* bin);              // maximum number of items the bin can hold
        void*  alloc(bin_t* bin);                       // allocate an item from the bin
        void   free(bin_t* bin, void* ptr);             // free an item back to the bin
        i32    ptr2idx(bin_t const* bin, void* ptr);    // convert a pointer to an index within the bin
        void*  idx2ptr(bin_t* bin, u16 index);          // convert an index to a pointer within the bin
        u32    highwater_mark(bin_t const* bin);        // highest number of items that have been in the bin
        s32    highest_free(bin_t const* bin);          // highest index of free item in the bin
    }  // namespace nbin16

}  // namespace ncore

#endif  // __CCORE_VIRTUAL_ALLOCATION_BIN_H__

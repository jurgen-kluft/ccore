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
        // This is an allocation bin that can allocate a fixed size items, however you can make
        // many bins with different item sizes.
        // It uses a hierarchical binmap to track used/free items and can grow both the committed
        // memory for the binmap as well as for the items themselves.
        // A certain part of the binmap is always committed, roughly 1 to 2 pages (page-size = 16KB)
        // depending on the maximum number of items.
        // One benefit of this bin is that it can handle up to 16M items (binmap with 4 levels) and
        // that the memory overhead for the binmap is quite low, especially when the number of items
        // is high, it basically consumes 1 bit per item.
        // Another good thing about a binmap is that allocations are always done from low to high,
        // so an allocation will always take the first free position that is lowest in the binmap.

        struct bin_t;
        bin_t* make_bin(u16 item_size, u32 max_items);  // 1 block = 64KB, maximum = 65535 blocks = 4GB
        void   destroy(bin_t* bin);                     // destroy the bin
        u32    size(bin_t* bin);                        // number of items currently in the bin
        u32    capacity(bin_t* bin);                    // maximum number of items the bin can hold
        u32    highwater_mark(bin_t* bin);              // highest number of items that have been in the bin

        void* alloc(bin_t* bin);
        void  free(bin_t* bin, void* ptr);
    }  // namespace nbin

}  // namespace ncore

#endif  // __CCORE_VIRTUAL_ALLOCATION_BIN_H__

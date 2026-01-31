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
        // This is an allocation bin that can allocate a fixed size item, however you can make a
        // bin for any size you want, limited to `system page size / 4`.
        // Note: The code here can be adjusted to use a page-size given by the user that is a
        //       multiple of the system page-size.
        // It uses a hierarchical bit-list to track free items and can grow both the committed
        // memory for the bit-list as well as for the items themselves.
        // A certain part of the bit-list is always committed, roughly 1 to 2 pages (page-size = 16KB)
        // depending on the maximum number of items.
        // One benefit of this bin is that it can handle up to max 16M items (bit-list with 4 levels)
        // and that the memory overhead for the bit-list is quite low, especially when the number of
        // items is high, it basically consumes 1 bit per item.
        // Another bit-list behaviour is that a new allocation will take the first free position that
        // is lowest in the bit-list, this means that space locality is very good.

        struct bin_t;
        bin_t* make_bin(u16 item_size, u32 max_items);  // e.g. item_size = 256, max_items = 65535, 16 MiB
        void   destroy(bin_t* bin);                     // destroy the bin
        u32    size(bin_t* bin);                        // number of items currently in the bin
        u32    capacity(bin_t* bin);                    // maximum number of items the bin can hold
        u32    highwater_mark(bin_t* bin);              // highest number of items that have been in the bin
        void*  alloc(bin_t* bin);                       // allocate an item from the bin
        void   free(bin_t* bin, void* ptr);             // free an item back to the bin
        s32    highest_free(bin_t* bin);                // highest index of free item in the bin
    }  // namespace nbin

}  // namespace ncore

#endif  // __CCORE_VIRTUAL_ALLOCATION_BIN_H__

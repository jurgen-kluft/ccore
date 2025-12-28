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
        struct bin_t;
        bin_t* make_bin(u16 item_size, u32 max_items);  // 1 block = 64KB, maximum = 65535 blocks = 4GB
        void   destroy(bin_t* bin);
        void*  alloc(bin_t* bin);
        void   free(bin_t* bin, void* ptr);
    }  // namespace nbin

}  // namespace ncore

#endif  // __CCORE_VIRTUAL_ALLOCATION_BIN_H__

#ifndef __CCORE_VIRTUAL_MEMORY_REGION_H__
#define __CCORE_VIRTUAL_MEMORY_REGION_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    namespace nregion
    {
        struct region_t;
        region_t* create(u32 max_number_of_blocks);  // 1 block = 4MB, maximum = 4096 blocks = 16GB

        // allocation sizes are power-of-two from minimum 16 to maximum 1024 bytes
        void* alloc(region_t* region, u32 size);
        void  free(region_t* region, void* ptr);
    }  // namespace nregion

}  // namespace ncore

#endif  // __CCORE_VIRTUAL_MEMORY_REGION_H__

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
        region_t* create();  // 1 block = 64KB, maximum = 65535 blocks = 4GB

        // allocation sizes are power-of-two from minimum 16 to maximum 1024 bytes
        void* alloc(region_t* region, u32 size);
        void  free(region_t* region, void* ptr);
    }  // namespace nregion

}  // namespace ncore

#endif  // __CCORE_VIRTUAL_MEMORY_REGION_H__

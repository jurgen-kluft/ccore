#ifndef __CCORE_DUOMAP_V2_H__
#define __CCORE_DUOMAP_V2_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

#include "ccore/c_allocator.h"

namespace ncore
{
    // Dual state tracking, 1.06 bits per bit
    struct bintree_t
    {
        u64 m_bitmap[2];
    };

    void setup(alloc_t* allocator, bintree_t* bt, u32 const maxbits);
    void clear(alloc_t* allocator, bintree_t* bt, u32 const maxbits);

    void set(alloc_t* allocator, bintree_t* bt, u32 const maxbits, u32 bit);
    void clr(alloc_t* allocator, bintree_t* bt, u32 const maxbits, u32 bit);
    bool get(bintree_t* bt, u32 const maxbits, u32 bit);

}  // namespace ncore

#endif

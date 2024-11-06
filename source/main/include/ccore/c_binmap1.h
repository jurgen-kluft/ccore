#ifndef __CCORE_DUOMAP_V2_H__
#define __CCORE_DUOMAP_V2_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_allocator.h"

namespace ncore
{
    // Note: Size has to be passed to each function since most of the time

    // 2^8 binmap, can handle a maximum of 2 * 64 = 128 bits.
    struct binmap8_t
    {
        u64 m_bin0[4];
    };

    void setup(binmap8_t* bt, u32 const maxbits);
    void clear(binmap8_t* bt, u32 const maxbits);

    void set(binmap8_t* bt, u32 const maxbits, u32 bit);
    void clr(binmap8_t* bt, u32 const maxbits, u32 bit);
    bool get(binmap8_t* bt, u32 const maxbits, u32 bit);
    s32  find(binmap8_t* bt, u32 const maxbits);
    s32  find_and_set(binmap8_t* bt, u32 const maxbits);

    // 2^12 binmap, can handle a maximum of 4096 bits.
    struct binmap12_t
    {
        u64  m_bin0;
        u64* m_bin1;
    };

    void setup(alloc_t* allocator, binmap12_t* bt, u32 const maxbits);
    void clear(alloc_t* allocator, binmap12_t* bt, u32 const maxbits);
    void release(alloc_t* allocator, binmap12_t* bt);

    void set(binmap12_t* bt, u32 const maxbits, u32 bit);
    void clr(binmap12_t* bt, u32 const maxbits, u32 bit);
    bool get(binmap12_t* bt, u32 const maxbits, u32 bit);
    s32  find(binmap12_t* bt, u32 const maxbits);
    s32  find_set(binmap12_t* bt, u32 const maxbits);


}  // namespace ncore

#endif

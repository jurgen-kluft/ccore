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

    // 2^8 binmap, can handle a maximum of 4 * 64 = 256 bits.
    struct binmap8_t
    {
        u64 m_bin0[4];
    };

    void g_setup(binmap8_t* bt, u32 const maxbits);
    void g_clear(binmap8_t* bt, u32 const maxbits);

    void g_set(binmap8_t* bt, u32 const maxbits, u32 bit);
    void g_clr(binmap8_t* bt, u32 const maxbits, u32 bit);
    bool g_get(binmap8_t* bt, u32 const maxbits, u32 bit);
    s32  g_find(binmap8_t* bt, u32 const maxbits);
    s32  g_find_and_set(binmap8_t* bt, u32 const maxbits);

    // 2^12 binmap, can handle a maximum of 4096 bits.
    struct binmap12_t
    {
        u64  m_bin0;
        u64* m_bin1;
    };

    void g_setup(alloc_t* allocator, binmap12_t* bt, u32 const maxbits);
    void g_clear(alloc_t* allocator, binmap12_t* bt, u32 const maxbits);
    void g_release(alloc_t* allocator, binmap12_t* bt);

    void g_set(binmap12_t* bt, u32 const maxbits, u32 bit);
    void g_clr(binmap12_t* bt, u32 const maxbits, u32 bit);
    bool g_get(binmap12_t* bt, u32 const maxbits, u32 bit);
    s32  g_find(binmap12_t* bt, u32 const maxbits);
    s32  g_find_set(binmap12_t* bt, u32 const maxbits);


}  // namespace ncore

#endif

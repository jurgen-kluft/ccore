#ifndef __CCORE_BINMAP_V2_H__
#define __CCORE_BINMAP_V2_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

namespace ncore
{
    class alloc_t;

    // Note: Size has to be passed to each function since most of the time

    // 2^8 binmap, can handle a maximum of 4 * 64 = 256 bits.
    struct binmap8_t
    {
        u64 m_bin0[4];

        void setup_free();
        void setup_used();
        void clear();

        void set(u32 const maxbits, u32 bit);
        void clr(u32 const maxbits, u32 bit);
        bool get(u32 const maxbits, u32 bit);
        s32 find(u32 const maxbits);
        s32 find_and_set(u32 const maxbits);
    };

    // 2^12 binmap, can handle a maximum of 4096 bits.
    struct binmap12_t
    {
        u64 m_bin0;
        u64 *m_bin1;

        void setup(alloc_t *allocator, u32 const maxbits);

        void setup_free_lazy(alloc_t *allocator, u32 const maxbits);
        void tick_free_lazy(u32 const maxbits, u32 bit);
        void setup_used_lazy(alloc_t *allocator, u32 const maxbits);
        void tick_used_lazy(u32 const maxbits, u32 bit);

        void clear(alloc_t *allocator, u32 const maxbits);
        void release(alloc_t *allocator);

        void set(u32 const maxbits, u32 bit);
        void clr(u32 const maxbits, u32 bit);
        bool get(u32 const maxbits, u32 bit);
        s32 find(u32 const maxbits);
        s32 find_and_set(u32 const maxbits);
    };

} // namespace ncore

#endif

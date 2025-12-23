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

    // 2^8 binmap
    // Initialize a binmap9_t to handle 'maxbits' at 256 bits.

    // 2^9 binmap, can handle a maximum of 512 bits.
    struct binmap9_t
    {
        u64 m_bin0;
        u8 *m_bin1;

        void setup(u8* bin1, u32 const maxbits);
        void setup(alloc_t *allocator, u32 const maxbits);

        void setup_free_lazy(alloc_t *allocator, u32 const maxbits);
        void setup_free_lazy(u8* bin1, u32 const maxbits);
        void tick_free_lazy(u32 const maxbits, u32 bit);
        void setup_used_lazy(alloc_t *allocator, u32 const maxbits);
        void setup_used_lazy(u8* bin1, u32 const maxbits);
        void tick_used_lazy(u32 const maxbits, u32 bit);

        void clear(u32 const maxbits);
        void release(alloc_t *allocator);

        void set(u32 const maxbits, u32 bit);
        void clr(u32 const maxbits, u32 bit);
        bool get(u32 const maxbits, u32 bit);
        s32 find(u32 const maxbits);
        s32 find_and_set(u32 const maxbits);
    };

    // 2^10 binmap, can handle a maximum of 1024 bits.
    struct binmap10_t
    {
        u64 m_bin0;
        u16 *m_bin1;

        void setup(u16* bin1, u32 const maxbits);
        void setup(alloc_t *allocator, u32 const maxbits);

        void setup_free_lazy(alloc_t *allocator, u32 const maxbits);
        void setup_free_lazy(u16* bin1, u32 const maxbits);
        void tick_free_lazy(u32 const maxbits, u32 bit);
        void setup_used_lazy(alloc_t *allocator, u32 const maxbits);
        void setup_used_lazy(u16* bin1, u32 const maxbits);
        void tick_used_lazy(u32 const maxbits, u32 bit);

        void clear(u32 const maxbits);
        void release(alloc_t *allocator);

        void set(u32 const maxbits, u32 bit);
        void clr(u32 const maxbits, u32 bit);
        bool get(u32 const maxbits, u32 bit);
        s32 find(u32 const maxbits);
        s32 find_and_set(u32 const maxbits);
    };

    // 2^11 binmap, can handle a maximum of 2048 bits.
    struct binmap11_t
    {
        u64 m_bin0;
        u32 *m_bin1;

        void setup(u32* bin1, u32 const maxbits);
        void setup(alloc_t *allocator, u32 const maxbits);

        void setup_free_lazy(alloc_t *allocator, u32 const maxbits);
        void setup_free_lazy(u32* bin1, u32 const maxbits);
        void tick_free_lazy(u32 const maxbits, u32 bit);
        void setup_used_lazy(alloc_t *allocator, u32 const maxbits);
        void setup_used_lazy(u32* bin1, u32 const maxbits);
        void tick_used_lazy(u32 const maxbits, u32 bit);

        void clear(u32 const maxbits);
        void release(alloc_t *allocator);

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

        void setup(u64* bin1, u32 const maxbits);
        void setup(alloc_t *allocator, u32 const maxbits);

        void setup_free_lazy(alloc_t *allocator, u32 const maxbits);
        void setup_free_lazy(u64* bin1, u32 const maxbits);
        void tick_free_lazy(u32 const maxbits, u32 bit);
        void setup_used_lazy(alloc_t *allocator, u32 const maxbits);
        void setup_used_lazy(u64* bin1, u32 const maxbits);
        void tick_used_lazy(u32 const maxbits, u32 bit);

        void clear(u32 const maxbits);
        void release(alloc_t *allocator);

        void set(u32 const maxbits, u32 bit);
        void clr(u32 const maxbits, u32 bit);
        bool get(u32 const maxbits, u32 bit);
        s32 find(u32 const maxbits);
        s32 find_and_set(u32 const maxbits);
    };

} // namespace ncore

#endif

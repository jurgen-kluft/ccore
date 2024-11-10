#include "ccore/c_allocator.h"
#include "ccore/c_math.h"

#include "ccore/c_binmap1.h"

namespace ncore
{

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    void g_setup(binmap8_t* bt, u32 const maxbits)
    {
        bt->m_bin0[0] = 0;
        bt->m_bin0[1] = 0;
        bt->m_bin0[2] = 0;
        bt->m_bin0[3] = 0;
    }

    void g_clear(binmap8_t* bt, u32 const maxbits)
    {
        bt->m_bin0[0] = 0;
        bt->m_bin0[1] = 0;
        bt->m_bin0[2] = 0;
        bt->m_bin0[3] = 0;
    }

    void g_set(binmap8_t* bt, u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits && maxbits <= 256);
        u32 const i = bit ;6;
        u32 const b = bit & 63;
        bt->m_bin0[i] |= (u64)1 << b;
    }

    void g_clr(binmap8_t* bt, u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits && maxbits <= 256);
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        bt->m_bin0[i] &= ~((u64)1 << b);
    }

    bool g_get(binmap8_t* bt, u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits && maxbits <= 256);
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        return (bt->m_bin0[i] & ((u64)1 << b)) != 0;
    }

    s32 g_find(binmap8_t* bt, u32 const maxbits)
    {
        ASSERT(maxbits <= 256);
        for (u32 i = 0; i < 4; ++i)
        {
            if (bt->m_bin0[i] != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            {
                s32 const bit = math::findFirstBit(~bt->m_bin0[i]) + (i << 6);
                return bit < (s32)maxbits ? bit : -1;
            }
        }
        return -1;
    }

    s32 g_find_and_set(binmap8_t* bt, u32 const maxbits)
    {
        ASSERT(maxbits <= 256);
        for (u32 i = 0; i < 4; ++i)
        {
            if (bt->m_bin0[i] != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            {
                u32 const b = math::findFirstBit(~bt->m_bin0[i]);
                bt->m_bin0[i] |= (u64)1 << b;
                s32 const bit = b + (i << 6);
                return bit < (s32)maxbits ? bit : -1;
            }
        }
        return -1;
    }

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // @note; we could also have 1 bit in bin0 cover 2 or 4 u64 in bin1, we would get a
    //        hit on performance but we could handle more bits in bin1.

    void g_setup(alloc_t* allocator, binmap12_t* bt, u32 const maxbits)
    {
        bt->m_bin0 = 0;
        bt->m_bin1 = g_allocate_array_and_clear<u64>(allocator, (maxbits + 63) >> 6);
    }

    void g_setup_free_lazy(alloc_t* allocator, binmap12_t* bt, u32 const maxbits)
    {
        bt->m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        bt->m_bin1 = g_allocate_array<u64>(allocator, (maxbits + 63) >> 6);
    }

    void g_tick_free_lazy(binmap12_t* bt, u32 const maxbits, u32 bit)
    {
        if (bit < maxbits)
        {
            u32 wi = bit;

            // bin1
            {
                const u32 bi   = wi & (64 - 1);
                wi             = wi >> 6;
                const u64 wd   = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : bt->m_bin1[wi];
                bt->m_bin1[wi] = wd & ~((u64)1 << bi);
                if (wd != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                    return;
            }
            // bin0
            {
                const u32 bi = wi & (64 - 1);
                const u64 wd = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : bt->m_bin0;
                bt->m_bin0   = wd & ~((u64)1 << bi);
            }
        }
    }

    void g_setup_used_lazy(alloc_t* allocator, binmap12_t* bt, u32 const maxbits)
    {
        bt->m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        bt->m_bin1 = g_allocate_array<u64>(allocator, (maxbits + 63) >> 6);
    }

    void g_tick_used_lazy(binmap12_t* bt, u32 const maxbits, u32 bit)
    {
        if (bit < maxbits)
        {
            // For '0' bit tracking, we need to slowly '1' out bin 1
            // Don't touch bin 0
            if ((bit & 63) == 0)
            {
                bt->m_bin1[bit >> 6] = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
            }
        }
    }

    void g_clear(alloc_t* allocator, binmap12_t* bt, u32 const maxbits)
    {
        bt->m_bin0     = 0;
        u32 const size = (maxbits + 63) >> 6;
        for (u32 i = 0; i < size; ++i)
            bt->m_bin1[i] = 0;
    }

    void g_release(alloc_t* allocator, binmap12_t* bt)
    {
        allocator->deallocate(bt->m_bin1);
        bt->m_bin1 = nullptr;
    }

    void g_set(binmap12_t* bt, u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i   = bit >> 6;
        u32 const b   = bit & 63;
        u64 const v   = bt->m_bin1[i];
        bt->m_bin1[i] = v | ((u64)1 << b);
        if (v == 0)
            bt->m_bin0 |= (u64)1 << i;
    }

    void g_clr(binmap12_t* bt, u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i   = bit >> 6;
        u32 const b   = bit & 63;
        u64 const v   = bt->m_bin1[i];
        bt->m_bin1[i] = v & ~((u64)1 << b);
        if (v == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            bt->m_bin0 &= ~((u64)1 << i);
    }

    bool g_get(binmap12_t* bt, u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        return (bt->m_bin1[i] & ((u64)1 << b)) != 0;
    }

    s32 g_find(binmap12_t* bt, u32 const maxbits)
    {
        if (bt->m_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            return -1;
        s32 const i = math::findFirstBit(~bt->m_bin0);
        s32 const bit = math::findFirstBit(~bt->m_bin1[i]) + (i << 6);
        return bit < (s32)maxbits ? bit : -1;
    }

    s32 g_find_and_set(binmap12_t* bt, u32 const maxbits)
    {
        if (bt->m_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            return -1;

        s32 const i = math::findFirstBit(~bt->m_bin0);
        ASSERT(bt->m_bin1[i] != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF));
        s32       b   = math::findFirstBit(~bt->m_bin1[i]) + (i << 6);
        u64 const v   = bt->m_bin1[i] | ((u64)1 << (b & 63));
        bt->m_bin1[i] = v;
        if (v == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            bt->m_bin0 |= ((u64)1 << i);
        return b < (s32)maxbits ? b : -1;
    }

};  // namespace ncore

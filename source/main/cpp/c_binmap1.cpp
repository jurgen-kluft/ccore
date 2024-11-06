#include "ccore/c_allocator.h"
#include "ccore/c_math.h"

#include "ccore/c_binmap1.h"

namespace ncore
{

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    void setup(binmap8_t* bt, u32 const maxbits)
    {
        bt->m_bin0[0] = 0;
        bt->m_bin0[1] = 0;
        bt->m_bin0[2] = 0;
        bt->m_bin0[3] = 0;
    }

    void clear(binmap8_t* bt, u32 const maxbits)
    {
        bt->m_bin0[0] = 0;
        bt->m_bin0[1] = 0;
        bt->m_bin0[2] = 0;
        bt->m_bin0[3] = 0;
    }

    void set(binmap8_t* bt, u32 const maxbits, u32 bit)
    {
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        bt->m_bin0[i] |= (u64)1 << b;
    }

    void clr(binmap8_t* bt, u32 const maxbits, u32 bit)
    {
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        bt->m_bin0[i] &= ~((u64)1 << b);
    }

    bool get(binmap8_t* bt, u32 const maxbits, u32 bit)
    {
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        return (bt->m_bin0[i] & ((u64)1 << b)) != 0;
    }

    s32  find(binmap8_t* bt, u32 const maxbits)
    {
        for (u32 i = 0; i < 4; ++i)
        {
            if (bt->m_bin0[0] != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                return math::findFirstBit(~bt->m_bin0[0]) + (i << 6);
        }
        return -1;
    }

    s32  find_and_set(binmap8_t* bt, u32 const maxbits)
    {
        for (u32 i = 0; i < 4; ++i)
        {
            if (bt->m_bin0[0] != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            {
                u32 const b = math::findFirstBit(~bt->m_bin0[0]);
                bt->m_bin0[0] |= (u64)1 << b;
                return b + (i << 6);
            }
        }
        return -1;
    }

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    void setup(alloc_t* allocator, binmap12_t* bt, u32 const maxbits)
    {
        bt->m_bin0  = 0;
        u32 const size = (maxbits + 63) >> 6;
        bt->m_bin1     = g_allocate_array_and_clear<u64>(allocator, size);
    }

    void clear(alloc_t* allocator, binmap12_t* bt, u32 const maxbits)
    {
        bt->m_bin0  = 0;
        u32 const size = (maxbits + 63) >> 6;
        for (u32 i = 0; i < size; ++i)
            bt->m_bin1[i] = 0;
    }

    void release(alloc_t* allocator, binmap12_t* bt)
    {
        allocator->deallocate(bt->m_bin1);
        bt->m_bin1 = nullptr;
    }

    void set(binmap12_t* bt, u32 const maxbits, u32 bit)
    {
        u32 const i   = bit >> 6;
        u32 const b   = bit & 63;
        u64 const v   = bt->m_bin1[i];
        bt->m_bin1[i] = v | ((u64)1 << b);
        if (v == 0)
            bt->m_bin0 |= (u64)1 << i;
    }

    void clr(binmap12_t* bt, u32 const maxbits, u32 bit)
    {
        u32 const i   = bit >> 6;
        u32 const b   = bit & 63;
        u64 const v   = bt->m_bin1[i];
        bt->m_bin1[i] = v & ~((u64)1 << b);
        if (v == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            bt->m_bin0 &= ~(u64)1 << i;
    }

    bool get(binmap12_t* bt, u32 const maxbits, u32 bit)
    {
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        return (bt->m_bin1[i] & ((u64)1 << b)) != 0;
    }

    s32 find(binmap12_t* bt, u32 const maxbits)
    {
        if (bt->m_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            return -1;
        s8 const i = math::findFirstBit(~bt->m_bin0);
        return math::findFirstBit(~bt->m_bin1[i]) + (i << 6);
    }

    s32 find_set(binmap12_t* bt, u32 const maxbits)
    {
        if (bt->m_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            return -1;

        s8 const  i   = math::findFirstBit(~bt->m_bin0);
        s32       bit = math::findFirstBit(~bt->m_bin1[i]) + (i << 6);
        u32 const b   = bit & 63;
        u64 const v   = bt->m_bin1[i];
        bt->m_bin1[i] = v & ~((u64)1 << b);
        if (v == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            bt->m_bin0 &= ~(u64)1 << i;
        return bit;
    }

};  // namespace ncore

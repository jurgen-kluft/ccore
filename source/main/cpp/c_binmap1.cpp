#include "ccore/c_allocator.h"
#include "ccore/c_math.h"

#include "ccore/c_binmap1.h"

namespace ncore
{

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    void binmap8_t::setup_free()
    {
        m_bin0[0] = 0;
        m_bin0[1] = 0;
        m_bin0[2] = 0;
        m_bin0[3] = 0;
    }

    void binmap8_t::setup_used()
    {
        m_bin0[0] = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin0[1] = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin0[2] = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin0[3] = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
    }

    void binmap8_t::clear()
    {
        m_bin0[0] = 0;
        m_bin0[1] = 0;
        m_bin0[2] = 0;
        m_bin0[3] = 0;
    }

    void binmap8_t::set(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits && maxbits <= 256);
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        m_bin0[i] |= (u64)1 << b;
    }

    void binmap8_t::clr(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits && maxbits <= 256);
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        m_bin0[i] &= ~((u64)1 << b);
    }

    bool binmap8_t::get(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits && maxbits <= 256);
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        return (m_bin0[i] & ((u64)1 << b)) != 0;
    }

    s32 binmap8_t::find(u32 const maxbits)
    {
        ASSERT(maxbits <= 256);
        for (u32 i = 0; i < 4; ++i)
        {
            if (m_bin0[i] != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            {
                s32 const bit = math::findFirstBit(~m_bin0[i]) + (i << 6);
                return bit < (s32)maxbits ? bit : -1;
            }
        }
        return -1;
    }

    s32 binmap8_t::find_and_set(u32 const maxbits)
    {
        ASSERT(maxbits <= 256);
        for (u32 i = 0; i < 4; ++i)
        {
            if (m_bin0[i] != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            {
                s32 const b = math::findFirstBit(~m_bin0[i]);
                m_bin0[i] |= (u64)1 << (b & 63);
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

    void binmap12_t::setup(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = 0;
        m_bin1 = g_allocate_array_and_clear<u64>(allocator, (maxbits + 63) >> 6);
    }

    void binmap12_t::setup_free_lazy(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = g_allocate_array<u64>(allocator, (maxbits + 63) >> 6);
    }

    void binmap12_t::tick_free_lazy(u32 const maxbits, u32 bit)
    {
        if (bit < maxbits)
        {
            u32 wi = bit;

            // bin1
            {
                const u32 bi   = wi & (64 - 1);
                wi             = wi >> 6;
                const u64 wd   = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : m_bin1[wi];
                m_bin1[wi] = wd & ~((u64)1 << bi);
                if (wd != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                    return;
            }
            // bin0
            {
                const u32 bi = wi & (64 - 1);
                const u64 wd = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : m_bin0;
                m_bin0   = wd & ~((u64)1 << bi);
            }
        }
    }

    void binmap12_t::setup_used_lazy(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = g_allocate_array<u64>(allocator, (maxbits + 63) >> 6);
    }

    void binmap12_t::tick_used_lazy(u32 const maxbits, u32 bit)
    {
        if (bit < maxbits)
        {
            // For '0' bit tracking, we need to slowly '1' out bin 1
            // Don't touch bin 0
            if ((bit & 63) == 0)
            {
                m_bin1[bit >> 6] = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
            }
        }
    }

    void binmap12_t::clear(u32 const maxbits)
    {
        m_bin0     = 0;
        u32 const size = (maxbits + 63) >> 6;
        for (u32 i = 0; i < size; ++i)
            m_bin1[i] = 0;
    }

    void binmap12_t::release(alloc_t* allocator)
    {
        allocator->deallocate(m_bin1);
        m_bin1 = nullptr;
    }

    void binmap12_t::set(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i   = bit >> 6;
        u32 const b   = bit & 63;
        u64 const v   = m_bin1[i];
        m_bin1[i] = v | ((u64)1 << b);
        if (v == 0)
            m_bin0 |= (u64)1 << i;
    }

    void binmap12_t::clr(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i   = bit >> 6;
        u32 const b   = bit & 63;
        u64 const v   = m_bin1[i];
        m_bin1[i] = v & ~((u64)1 << b);
        if (v == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            m_bin0 &= ~((u64)1 << i);
    }

    bool binmap12_t::get(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        return (m_bin1[i] & ((u64)1 << b)) != 0;
    }

    s32 binmap12_t::find(u32 const maxbits)
    {
        if (m_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            return -1;
        s32 const i   = math::findFirstBit(~m_bin0);
        s32 const bit = math::findFirstBit(~m_bin1[i]) + (i << 6);
        return bit < (s32)maxbits ? bit : -1;
    }

    s32 binmap12_t::find_and_set(u32 const maxbits)
    {
        if (m_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            return -1;

        s32 const i = math::findFirstBit(~m_bin0);
        ASSERT(m_bin1[i] != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF));
        s32       b   = math::findFirstBit(~m_bin1[i]) + (i << 6);
        u64 const v   = m_bin1[i] | ((u64)1 << (b & 63));
        m_bin1[i] = v;
        if (v == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            m_bin0 |= ((u64)1 << (i & 63));
        return b < (s32)maxbits ? b : -1;
    }

};  // namespace ncore

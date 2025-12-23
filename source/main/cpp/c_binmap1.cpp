#include "ccore/c_allocator.h"
#include "ccore/c_math.h"

#include "ccore/c_binmap1.h"

namespace ncore
{

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // @note; we could also have 1 bit in bin0 cover 2 or 4 u64 in bin1, we would get a
    //        hit on performance but we could handle more bits in bin1.

    void binmap9_t::setup(u8* bin1, u32 const maxbits)
    {
        m_bin0 = 0;
        m_bin1 = bin1;
    }

    void binmap9_t::setup(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = 0;
        m_bin1 = g_allocate_array_and_clear<u8>(allocator, (maxbits + 7) >> 3);
    }

    void binmap9_t::setup_free_lazy(u8* bin1, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = bin1;
    }

    void binmap9_t::setup_free_lazy(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = g_allocate_array<u8>(allocator, (maxbits + 7) >> 3);
    }

    void binmap9_t::tick_free_lazy(u32 const maxbits, u32 bit)
    {
        if (bit < maxbits)
        {
            u32 wi = bit;

            // bin1
            {
                const u32 bi = wi & (8 - 1);
                wi           = wi >> 3;
                const u32 wd = (bi == 0) ? D_CONSTANT_U8(0xFF) : m_bin1[wi];
                m_bin1[wi]   = wd & ~((u8)1 << bi);
                if (wd != D_CONSTANT_U8(0xFF))
                    return;
            }
            // bin0
            {
                const u32 bi = wi & (64 - 1);
                const u64 wd = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : m_bin0;
                m_bin0       = wd & ~((u64)1 << bi);
            }
        }
    }

    void binmap9_t::setup_used_lazy(u8* bin1, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = bin1;
    }

    void binmap9_t::setup_used_lazy(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = g_allocate_array<u8>(allocator, (maxbits + 7) >> 3);
    }

    void binmap9_t::tick_used_lazy(u32 const maxbits, u32 bit)
    {
        if (bit < maxbits)
        {
            // For '0' bit tracking, we need to slowly '1' out bin 1
            // Don't touch bin 0
            if ((bit & 7) == 0)
            {
                m_bin1[bit >> 3] = D_CONSTANT_U8(0xFF);
            }
        }
    }

    void binmap9_t::clear(u32 const maxbits)
    {
        m_bin0         = 0;
        u32 const size = (maxbits + 7) >> 3;
        for (u32 i = 0; i < size; ++i)
            m_bin1[i] = 0;
    }

    void binmap9_t::release(alloc_t* allocator)
    {
        allocator->deallocate(m_bin1);
        m_bin1 = nullptr;
    }

    void binmap9_t::set(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i = bit >> 3;
        u32 const b = bit & 7;
        u8 const v = m_bin1[i];
        m_bin1[i]   = v | ((u8)1 << b);
        if (v == 0)
            m_bin0 |= (u64)1 << i;
    }

    void binmap9_t::clr(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i = bit >> 3;
        u32 const b = bit & 7;
        u8 const v = m_bin1[i];
        m_bin1[i]   = v & ~((u8)1 << b);
        if (v == D_CONSTANT_U8(0xFF))
            m_bin0 &= ~((u64)1 << i);
    }

    bool binmap9_t::get(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i = bit >> 3;
        u32 const b = bit & 7;
        return (m_bin1[i] & ((u8)1 << b)) != 0;
    }

    s32 binmap9_t::find(u32 const maxbits)
    {
        if (m_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            return -1;
        s32 const i   = math::findFirstBit(~m_bin0);
        s32 const bit = math::findFirstBit((u8)~m_bin1[i]) + (i << 3);
        return bit < (s32)maxbits ? bit : -1;
    }

    s32 binmap9_t::find_and_set(u32 const maxbits)
    {
        if (m_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            return -1;

        s32 const i = math::findFirstBit(~m_bin0);
        ASSERT(m_bin1[i] != D_CONSTANT_U8(0xFF));
        s32       b = math::findFirstBit((u8)~m_bin1[i]) + (i << 3);
        u8 const v = m_bin1[i] | ((u8)1 << (b & 7));
        m_bin1[i]   = v;
        if (v == D_CONSTANT_U8(0xFF))
            m_bin0 |= ((u64)1 << i);
        return b < (s32)maxbits ? b : -1;
    }

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // @note; we could also have 1 bit in bin0 cover 2 or 4 u64 in bin1, we would get a
    //        hit on performance but we could handle more bits in bin1.

    void binmap10_t::setup(u16* bin1, u32 const maxbits)
    {
        m_bin0 = 0;
        m_bin1 = bin1;
    }

    void binmap10_t::setup(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = 0;
        m_bin1 = g_allocate_array_and_clear<u16>(allocator, (maxbits + 15) >> 4);
    }

    void binmap10_t::setup_free_lazy(u16* bin1, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U32(0xFFFFFFFF);
        m_bin1 = bin1;
    }

    void binmap10_t::setup_free_lazy(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U32(0xFFFFFFFF);
        m_bin1 = g_allocate_array<u16>(allocator, (maxbits + 15) >> 4);
    }

    void binmap10_t::tick_free_lazy(u32 const maxbits, u32 bit)
    {
        if (bit < maxbits)
        {
            u32 wi = bit;

            // bin1
            {
                const u32 bi = wi & (16 - 1);
                wi           = wi >> 4;
                const u32 wd = (bi == 0) ? D_CONSTANT_U16(0xFFFF) : m_bin1[wi];
                m_bin1[wi]   = wd & ~((u16)1 << bi);
                if (wd != D_CONSTANT_U16(0xFFFF))
                    return;
            }
            // bin0
            {
                const u32 bi = wi & (64 - 1);
                const u64 wd = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : m_bin0;
                m_bin0       = wd & ~((u64)1 << bi);
            }
        }
    }

    void binmap10_t::setup_used_lazy(u16* bin1, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = bin1;
    }

    void binmap10_t::setup_used_lazy(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = g_allocate_array<u16>(allocator, (maxbits + 15) >> 4);
    }

    void binmap10_t::tick_used_lazy(u32 const maxbits, u32 bit)
    {
        if (bit < maxbits)
        {
            // For '0' bit tracking, we need to slowly '1' out bin 1
            // Don't touch bin 0
            if ((bit & 15) == 0)
            {
                m_bin1[bit >> 4] = D_CONSTANT_U16(0xFFFF);
            }
        }
    }

    void binmap10_t::clear(u32 const maxbits)
    {
        m_bin0         = 0;
        u32 const size = (maxbits + 15) >> 4;
        for (u32 i = 0; i < size; ++i)
            m_bin1[i] = 0;
    }

    void binmap10_t::release(alloc_t* allocator)
    {
        allocator->deallocate(m_bin1);
        m_bin1 = nullptr;
    }

    void binmap10_t::set(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i = bit >> 4;
        u32 const b = bit & 15;
        u16 const v = m_bin1[i];
        m_bin1[i]   = v | ((u16)1 << b);
        if (v == 0)
            m_bin0 |= (u64)1 << i;
    }

    void binmap10_t::clr(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i = bit >> 4;
        u32 const b = bit & 15;
        u16 const v = m_bin1[i];
        m_bin1[i]   = v & ~((u16)1 << b);
        if (v == D_CONSTANT_U16(0xFFFF))
            m_bin0 &= ~((u64)1 << i);
    }

    bool binmap10_t::get(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i = bit >> 4;
        u32 const b = bit & 15;
        return (m_bin1[i] & ((u16)1 << b)) != 0;
    }

    s32 binmap10_t::find(u32 const maxbits)
    {
        if (m_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            return -1;
        s32 const i   = math::findFirstBit(~m_bin0);
        s32 const bit = math::findFirstBit((u16)~m_bin1[i]) + (i << 4);
        return bit < (s32)maxbits ? bit : -1;
    }

    s32 binmap10_t::find_and_set(u32 const maxbits)
    {
        if (m_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            return -1;

        s32 const i = math::findFirstBit(~m_bin0);
        ASSERT(m_bin1[i] != D_CONSTANT_U16(0xFFFF));
        s32       b = math::findFirstBit((u16)~m_bin1[i]) + (i << 4);
        u16 const v = m_bin1[i] | ((u16)1 << (b & 15));
        m_bin1[i]   = v;
        if (v == D_CONSTANT_U16(0xFFFF))
            m_bin0 |= ((u64)1 << i);
        return b < (s32)maxbits ? b : -1;
    }

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // @note; we could also have 1 bit in bin0 cover 2 or 4 u64 in bin1, we would get a
    //        hit on performance but we could handle more bits in bin1.

    void binmap11_t::setup(u32* bin1, u32 const maxbits)
    {
        m_bin0 = 0;
        m_bin1 = bin1;
    }

    void binmap11_t::setup(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = 0;
        m_bin1 = g_allocate_array_and_clear<u32>(allocator, (maxbits + 31) >> 5);
    }

    void binmap11_t::setup_free_lazy(u32* bin1, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = bin1;
    }

    void binmap11_t::setup_free_lazy(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = g_allocate_array<u32>(allocator, (maxbits + 31) >> 5);
    }

    void binmap11_t::tick_free_lazy(u32 const maxbits, u32 bit)
    {
        if (bit < maxbits)
        {
            u32 wi = bit;

            // bin1
            {
                const u32 bi = wi & (32 - 1);
                wi           = wi >> 5;
                const u32 wd = (bi == 0) ? D_CONSTANT_U32(0xFFFFFFFF) : m_bin1[wi];
                m_bin1[wi]   = wd & ~((u32)1 << bi);
                if (wd != D_CONSTANT_U32(0xFFFFFFFF))
                    return;
            }
            // bin0
            {
                const u32 bi = wi & (64 - 1);
                const u64 wd = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : m_bin0;
                m_bin0       = wd & ~((u64)1 << bi);
            }
        }
    }

    void binmap11_t::setup_used_lazy(u32* bin1, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = bin1;
    }

    void binmap11_t::setup_used_lazy(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = g_allocate_array<u32>(allocator, (maxbits + 31) >> 5);
    }

    void binmap11_t::tick_used_lazy(u32 const maxbits, u32 bit)
    {
        if (bit < maxbits)
        {
            // For '0' bit tracking, we need to slowly '1' out bin 1
            // Don't touch bin 0
            if ((bit & 31) == 0)
            {
                m_bin1[bit >> 5] = D_CONSTANT_U32(0xFFFFFFFF);
            }
        }
    }

    void binmap11_t::clear(u32 const maxbits)
    {
        m_bin0         = 0;
        u32 const size = (maxbits + 31) >> 5;
        for (u32 i = 0; i < size; ++i)
            m_bin1[i] = 0;
    }

    void binmap11_t::release(alloc_t* allocator)
    {
        allocator->deallocate(m_bin1);
        m_bin1 = nullptr;
    }

    void binmap11_t::set(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i = bit >> 5;
        u32 const b = bit & 31;
        u32 const v = m_bin1[i];
        m_bin1[i]   = v | ((u32)1 << b);
        if (v == 0)
            m_bin0 |= (u64)1 << i;
    }

    void binmap11_t::clr(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i = bit >> 5;
        u32 const b = bit & 31;
        u32 const v = m_bin1[i];
        m_bin1[i]   = v & ~((u32)1 << b);
        if (v == D_CONSTANT_U32(0xFFFFFFFF))
            m_bin0 &= ~((u64)1 << i);
    }

    bool binmap11_t::get(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i = bit >> 5;
        u32 const b = bit & 31;
        return (m_bin1[i] & ((u32)1 << b)) != 0;
    }

    s32 binmap11_t::find(u32 const maxbits)
    {
        if (m_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            return -1;
        s32 const i   = math::findFirstBit(~m_bin0);
        s32 const bit = math::findFirstBit(~m_bin1[i]) + (i << 5);
        return bit < (s32)maxbits ? bit : -1;
    }

    s32 binmap11_t::find_and_set(u32 const maxbits)
    {
        if (m_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            return -1;

        s32 const i = math::findFirstBit(~m_bin0);
        ASSERT(m_bin1[i] != D_CONSTANT_U32(0xFFFFFFFF));
        s32       b = math::findFirstBit(~m_bin1[i]) + (i << 5);
        u32 const v = m_bin1[i] | ((u32)1 << (b & 31));
        m_bin1[i]   = v;
        if (v == D_CONSTANT_U32(0xFFFFFFFF))
            m_bin0 |= ((u64)1 << (i & 63));
        return b < (s32)maxbits ? b : -1;
    }

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // @note; we could also have 1 bit in bin0 cover 2 or 4 u64 in bin1, we would get a
    //        hit on performance but we could handle more bits in bin1.

    void binmap12_t::setup(u64* bin1, u32 const maxbits)
    {
        m_bin0 = 0;
        m_bin1 = bin1;
    }

    void binmap12_t::setup(alloc_t* allocator, u32 const maxbits)
    {
        m_bin0 = 0;
        m_bin1 = g_allocate_array_and_clear<u64>(allocator, (maxbits + 63) >> 6);
    }

    void binmap12_t::setup_free_lazy(u64* bin1, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = bin1;
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
                const u32 bi = wi & (64 - 1);
                wi           = wi >> 6;
                const u64 wd = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : m_bin1[wi];
                m_bin1[wi]   = wd & ~((u64)1 << bi);
                if (wd != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                    return;
            }
            // bin0
            {
                const u32 bi = wi & (64 - 1);
                const u64 wd = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : m_bin0;
                m_bin0       = wd & ~((u64)1 << bi);
            }
        }
    }

    void binmap12_t::setup_used_lazy(u64* bin1, u32 const maxbits)
    {
        m_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        m_bin1 = bin1;
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
        m_bin0         = 0;
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
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        u64 const v = m_bin1[i];
        m_bin1[i]   = v | ((u64)1 << b);
        if (v == 0)
            m_bin0 |= (u64)1 << i;
    }

    void binmap12_t::clr(u32 const maxbits, u32 bit)
    {
        ASSERT(bit < maxbits);
        u32 const i = bit >> 6;
        u32 const b = bit & 63;
        u64 const v = m_bin1[i];
        m_bin1[i]   = v & ~((u64)1 << b);
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
        s32       b = math::findFirstBit(~m_bin1[i]) + (i << 6);
        u64 const v = m_bin1[i] | ((u64)1 << (b & 63));
        m_bin1[i]   = v;
        if (v == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
            m_bin0 |= ((u64)1 << (i & 63));
        return b < (s32)maxbits ? b : -1;
    }

};  // namespace ncore

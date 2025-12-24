#include "ccore/c_math.h"

#include "ccore/c_binmap1.h"

namespace ncore
{
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    namespace nbinmap9
    {
        void setup_free_lazy(u64* _bin0, u8* _bin1, u32 maxbits) { *_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF); }
        void tick_free_lazy(u64* _bin0, u8* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin1
                {
                    const u32 bi = wi & (8 - 1);
                    wi           = wi >> 3;
                    const u32 wd = (bi == 0) ? D_CONSTANT_U8(0xFF) : _bin1[wi];
                    _bin1[wi]    = wd & ~((u8)1 << bi);
                    if (wd != D_CONSTANT_U8(0xFF))
                        return;
                }
                // bin0
                {
                    const u32 bi = wi & (64 - 1);
                    const u64 wd = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : *_bin0;
                    *_bin0       = wd & ~((u64)1 << bi);
                }
            }
        }

        void setup_used_lazy(u64* _bin0, u8* _bin1, u32 maxbits) { *_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF); }
        void tick_used_lazy(u64* _bin0, u8* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                // For '0' bit tracking, we need to slowly '1' out bin 1
                // Don't touch bin 0
                if ((bit & 7) == 0)
                {
                    _bin1[bit >> 3] = D_CONSTANT_U8(0xFF);
                }
            }
        }

        void clear(u64* _bin0, u8* _bin1, u32 maxbits)
        {
            *_bin0         = 0;
            u32 const size = (maxbits + 7) >> 3;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
        }

        void set(u64* _bin0, u8* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> 3;
            u32 const b = bit & 7;
            u8 const  v = _bin1[i];
            _bin1[i]    = v | ((u8)1 << b);
            if (v == 0)
                *_bin0 |= (u64)1 << i;
        }

        void clr(u64* _bin0, u8* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> 3;
            u32 const b = bit & 7;
            u8 const  v = _bin1[i];
            _bin1[i]    = v & ~((u8)1 << b);
            if (v == D_CONSTANT_U8(0xFF))
                *_bin0 &= ~((u64)1 << i);
        }

        bool get(u64 const* _bin0, u8 const* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> 3;
            u32 const b = bit & 7;
            return (_bin1[i] & ((u8)1 << b)) != 0;
        }

        s32 find(u64 const* _bin0, u8 const* _bin1, u32 maxbits)
        {
            if (*_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                return -1;
            s32 const i   = math::findFirstBit(~*_bin0);
            s32 const bit = math::findFirstBit((u8)~_bin1[i]) + (i << 3);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_and_set(u64* _bin0, u8* _bin1, u32 maxbits)
        {
            if (*_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                return -1;

            s32 const i = math::findFirstBit(~*_bin0);
            ASSERT(_bin1[i] != D_CONSTANT_U8(0xFF));
            s32      b = math::findFirstBit((u8)~_bin1[i]) + (i << 3);
            u8 const v = _bin1[i] | ((u8)1 << (b & 7));
            _bin1[i]   = v;
            if (v == D_CONSTANT_U8(0xFF))
                *_bin0 |= ((u64)1 << i);
            return b < (s32)maxbits ? b : -1;
        }
    }  // namespace nbinmap9

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // @note; we could also have 1 bit in bin0 cover 2 or 4 u64 in bin1, we would get a
    //        hit on performance but we could handle more bits in bin1.
    namespace nbinmap10
    {
        void setup_free_lazy(u64* _bin0, u16* _bin1, u32 maxbits) { *_bin0 = D_CONSTANT_U32(0xFFFFFFFF); }
        void tick_free_lazy(u64* _bin0, u16* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin1
                {
                    const u32 bi = wi & (16 - 1);
                    wi           = wi >> 4;
                    const u32 wd = (bi == 0) ? D_CONSTANT_U16(0xFFFF) : _bin1[wi];
                    _bin1[wi]    = wd & ~((u16)1 << bi);
                    if (wd != D_CONSTANT_U16(0xFFFF))
                        return;
                }
                // bin0
                {
                    const u32 bi = wi & (64 - 1);
                    const u64 wd = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : *_bin0;
                    *_bin0       = wd & ~((u64)1 << bi);
                }
            }
        }

        void setup_used_lazy(u64* _bin0, u16* _bin1, u32 maxbits) { *_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF); }
        void tick_used_lazy(u64* _bin0, u16* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                // For '0' bit tracking, we need to slowly '1' out bin 1
                // Don't touch bin 0
                if ((bit & 15) == 0)
                {
                    _bin1[bit >> 4] = D_CONSTANT_U16(0xFFFF);
                }
            }
        }

        void clear(u64* _bin0, u16* _bin1, u32 maxbits)
        {
            *_bin0         = 0;
            u32 const size = (maxbits + 15) >> 4;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
        }

        void set(u64* _bin0, u16* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> 4;
            u32 const b = bit & 15;
            u16 const v = _bin1[i];
            _bin1[i]    = v | ((u16)1 << b);
            if (v == 0)
                *_bin0 |= (u64)1 << i;
        }

        void clr(u64* _bin0, u16* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> 4;
            u32 const b = bit & 15;
            u16 const v = _bin1[i];
            _bin1[i]    = v & ~((u16)1 << b);
            if (v == D_CONSTANT_U16(0xFFFF))
                *_bin0 &= ~((u64)1 << i);
        }

        bool get(u64 const* _bin0, u16 const* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> 4;
            u32 const b = bit & 15;
            return (_bin1[i] & ((u16)1 << b)) != 0;
        }

        s32 find(u64 const* _bin0, u16 const* _bin1, u32 maxbits)
        {
            if (*_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                return -1;
            s32 const i   = math::findFirstBit(~*_bin0);
            s32 const bit = math::findFirstBit((u16)~_bin1[i]) + (i << 4);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_and_set(u64* _bin0, u16* _bin1, u32 maxbits)
        {
            if (*_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                return -1;

            s32 const i = math::findFirstBit(~*_bin0);
            ASSERT(_bin1[i] != D_CONSTANT_U16(0xFFFF));
            s32       b = math::findFirstBit((u16)~_bin1[i]) + (i << 4);
            u16 const v = _bin1[i] | ((u16)1 << (b & 15));
            _bin1[i]    = v;
            if (v == D_CONSTANT_U16(0xFFFF))
                *_bin0 |= ((u64)1 << i);
            return b < (s32)maxbits ? b : -1;
        }
    }  // namespace nbinmap10

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // @note; we could also have 1 bit in bin0 cover 2 or 4 u64 in bin1, we would get a
    //        hit on performance but we could handle more bits in bin1.
    namespace nbinmap11
    {
        void setup_free_lazy(u64* _bin0, u32* _bin1, u32 maxbits) { *_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF); }
        void tick_free_lazy(u64* _bin0, u32* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin1
                {
                    const u32 bi = wi & (32 - 1);
                    wi           = wi >> 5;
                    const u32 wd = (bi == 0) ? D_CONSTANT_U32(0xFFFFFFFF) : _bin1[wi];
                    _bin1[wi]    = wd & ~((u32)1 << bi);
                    if (wd != D_CONSTANT_U32(0xFFFFFFFF))
                        return;
                }
                // bin0
                {
                    const u32 bi = wi & (64 - 1);
                    const u64 wd = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : *_bin0;
                    *_bin0       = wd & ~((u64)1 << bi);
                }
            }
        }

        void setup_used_lazy(u64* _bin0, u32* _bin1, u32 maxbits) { *_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF); }
        void tick_used_lazy(u64* _bin0, u32* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                // For '0' bit tracking, we need to slowly '1' out bin 1
                // Don't touch bin 0
                if ((bit & 31) == 0)
                {
                    _bin1[bit >> 5] = D_CONSTANT_U32(0xFFFFFFFF);
                }
            }
        }

        void clear(u64* _bin0, u32* _bin1, u32 maxbits)
        {
            *_bin0         = 0;
            u32 const size = (maxbits + 31) >> 5;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
        }

        void set(u64* _bin0, u32* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> 5;
            u32 const b = bit & 31;
            u32 const v = _bin1[i];
            _bin1[i]    = v | ((u32)1 << b);
            if (v == 0)
                *_bin0 |= (u64)1 << i;
        }

        void clr(u64* _bin0, u32* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> 5;
            u32 const b = bit & 31;
            u32 const v = _bin1[i];
            _bin1[i]    = v & ~((u32)1 << b);
            if (v == D_CONSTANT_U32(0xFFFFFFFF))
                *_bin0 &= ~((u64)1 << i);
        }

        bool get(u64 const* _bin0, u32 const* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> 5;
            u32 const b = bit & 31;
            return (_bin1[i] & ((u32)1 << b)) != 0;
        }

        s32 find(u64 const* _bin0, u32 const* _bin1, u32 maxbits)
        {
            if (*_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                return -1;
            s32 const i   = math::findFirstBit(~*_bin0);
            s32 const bit = math::findFirstBit(~_bin1[i]) + (i << 5);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_and_set(u64* _bin0, u32* _bin1, u32 maxbits)
        {
            if (*_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                return -1;

            s32 const i = math::findFirstBit(~*_bin0);
            ASSERT(_bin1[i] != D_CONSTANT_U32(0xFFFFFFFF));
            s32       b = math::findFirstBit(~_bin1[i]) + (i << 5);
            u32 const v = _bin1[i] | ((u32)1 << (b & 31));
            _bin1[i]    = v;
            if (v == D_CONSTANT_U32(0xFFFFFFFF))
                *_bin0 |= ((u64)1 << (i & 63));
            return b < (s32)maxbits ? b : -1;
        }
    }  // namespace nbinmap11

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // @note; we could also have 1 bit in bin0 cover 2 or 4 u64 in bin1, we would get a
    //        hit on performance but we could handle more bits in bin1.
    namespace nbinmap12
    {
        void setup_free_lazy(u64* _bin0, u64* _bin1, u32 maxbits) { *_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF); }
        void tick_free_lazy(u64* _bin0, u64* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin1
                {
                    const u32 bi = wi & (64 - 1);
                    wi           = wi >> 6;
                    const u64 wd = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : _bin1[wi];
                    _bin1[wi]    = wd & ~((u64)1 << bi);
                    if (wd != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                        return;
                }
                // bin0
                {
                    const u32 bi = wi & (64 - 1);
                    const u64 wd = (bi == 0) ? D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF) : *_bin0;
                    *_bin0       = wd & ~((u64)1 << bi);
                }
            }
        }

        void setup_used_lazy(u64* _bin0, u64* _bin1, u32 maxbits) { *_bin0 = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF); }
        void tick_used_lazy(u64* _bin0, u64* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                // For '0' bit tracking, we need to slowly '1' out bin 1
                // Don't touch bin 0
                if ((bit & 63) == 0)
                {
                    _bin1[bit >> 6] = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
                }
            }
        }

        void clear(u64* _bin0, u64* _bin1, u32 maxbits)
        {
            *_bin0         = 0;
            u32 const size = (maxbits + 63) >> 6;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
        }

        void set(u64* _bin0, u64* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> 6;
            u32 const b = bit & 63;
            u64 const v = _bin1[i];
            _bin1[i]    = v | ((u64)1 << b);
            if (v == 0)
                *_bin0 |= (u64)1 << i;
        }

        void clr(u64* _bin0, u64* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> 6;
            u32 const b = bit & 63;
            u64 const v = _bin1[i];
            _bin1[i]    = v & ~((u64)1 << b);
            if (v == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                *_bin0 &= ~((u64)1 << i);
        }

        bool get(u64 const* _bin0, u64 const* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> 6;
            u32 const b = bit & 63;
            return (_bin1[i] & ((u64)1 << b)) != 0;
        }

        s32 find(u64 const* _bin0, u64 const* _bin1, u32 maxbits)
        {
            if (*_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                return -1;
            s32 const i   = math::findFirstBit(~*_bin0);
            s32 const bit = math::findFirstBit(~_bin1[i]) + (i << 6);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_and_set(u64* _bin0, u64* _bin1, u32 maxbits)
        {
            if (*_bin0 == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                return -1;

            s32 const i = math::findFirstBit(~*_bin0);
            ASSERT(_bin1[i] != D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF));
            s32       b = math::findFirstBit(~_bin1[i]) + (i << 6);
            u64 const v = _bin1[i] | ((u64)1 << (b & 63));
            _bin1[i]    = v;
            if (v == D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF))
                *_bin0 |= ((u64)1 << (i & 63));
            return b < (s32)maxbits ? b : -1;
        }
    }  // namespace nbinmap12
};  // namespace ncore

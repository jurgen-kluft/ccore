#include "ccore/c_math.h"

#include "ccore/c_binmap1.h"

namespace ncore
{
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    namespace math
    {
        template <typename T>
        constexpr inline T invert(T value)
        {
            return (T)~value;
        }
    }  // namespace math

    namespace nbinmap9
    {
        typedef u64 bin0type;
        typedef u8  bin1type;

        constexpr u32 bin1shift = 3;

        constexpr u32 bin0bits = sizeof(bin0type) * 8;
        constexpr u32 bin1bits = sizeof(bin1type) * 8;

        constexpr bin0type bin0constant = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        constexpr bin1type bin1constant = D_CONSTANT_U8(0xFF);

        void setup_free_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits) { *_bin0 = bin0constant; }
        void tick_free_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin1
                {
                    const u32 bi      = wi & (bin1bits - 1);
                    wi                = wi >> bin1shift;
                    const bin1type wd = (bi == 0) ? bin1constant : _bin1[wi];
                    _bin1[wi]         = wd & math::invert((bin1type)1 << bi);
                    if (wd != bin1constant)
                        return;
                }
                // bin0
                {
                    const u32      bi = wi & (bin0bits - 1);
                    const bin0type wd = (bi == 0) ? bin0constant : *_bin0;
                    *_bin0            = wd & math::invert((bin0type)1 << bi);
                }
            }
        }

        void setup_used_lazy(bin0type* bin0, bin1type* bin1, u32 maxbits) { *bin0 = bin0constant; }
        void tick_used_lazy(bin0type* bin0, bin1type* bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin1
                {
                    const u32 bi      = wi & (bin1bits - 1);
                    wi                = wi >> bin1shift;
                    const bin1type wd = (bi == 0) ? bin1constant : bin1[wi];
                    bin1[wi]          = wd | ((bin1type)1 << bi);
                }
            }
        }

        void clear(bin0type* _bin0, bin1type* _bin1, u32 maxbits)
        {
            *_bin0         = 0;
            u32 const size = (maxbits + (bin1bits - 1)) >> bin1shift;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
        }

        void set(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const      i  = bit >> bin1shift;
            u32 const      b  = bit & (bin1bits - 1);
            bin1type const vn = _bin1[i] | ((bin1type)1 << b);
            _bin1[i]          = vn;
            if (vn == bin1constant)
            {
                *_bin0 |= ((bin0type)1 << i);
            }
        }

        void clr(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const      i  = bit >> bin1shift;
            u32 const      b  = bit & (bin1bits - 1);
            bin1type const vo = _bin1[i];  // old
            _bin1[i]          = vo & math::invert((bin1type)1 << b);
            if (vo == bin1constant)
                *_bin0 &= math::invert((bin0type)1 << i);
        }

        bool get(bin0type const* _bin0, bin1type const* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> bin1shift;
            u32 const b = bit & (bin1bits - 1);
            return (_bin1[i] & ((bin1type)1 << b)) != 0;
        }

        s32 find(bin0type const* _bin0, bin1type const* _bin1, u32 maxbits)
        {
            if (*_bin0 == bin0constant)
                return -1;
            s32 const      i   = math::findFirstBit(math::invert(*_bin0));
            bin1type const w   = math::invert(_bin1[i]);
            s32 const      bit = math::findFirstBit(w) + (i << bin1shift);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_and_set(bin0type* _bin0, bin1type* _bin1, u32 maxbits)
        {
            if (*_bin0 == bin0constant)
                return -1;

            s32 const i0 = math::findFirstBit(math::invert(*_bin0));
            ASSERT(_bin1[i0] != bin1constant);
            const s32      b1 = math::findFirstBit(math::invert(_bin1[i0]));
            bin1type const v1 = _bin1[i0] | ((bin1type)1 << b1);
            _bin1[i0]         = v1;
            if (v1 == bin1constant)
                *_bin0 |= ((bin0type)1 << i0);

            const s32 bit = b1 + (i0 << bin1shift);
            return bit < (s32)maxbits ? bit : -1;
        }
    }  // namespace nbinmap9

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    namespace nbinmap10
    {

    }  // namespace nbinmap10

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    namespace nbinmap11
    {

    }  // namespace nbinmap11

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    namespace nbinmap12
    {
        typedef u64 bin0type;
        typedef u64 bin1type;

        constexpr u32 bin1shift = 6;

        constexpr u32 bin0bits = sizeof(bin0type) * 8;
        constexpr u32 bin1bits = sizeof(bin1type) * 8;

        constexpr bin0type bin0constant = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);
        constexpr bin1type bin1constant = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);

        void setup_free_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits) { *_bin0 = bin0constant; }
        void tick_free_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin1
                {
                    const u32 bi      = wi & (bin1bits - 1);
                    wi                = wi >> bin1shift;
                    const bin1type wd = (bi == 0) ? bin1constant : _bin1[wi];
                    _bin1[wi]         = wd & math::invert((bin1type)1 << bi);
                    if (wd != bin1constant)
                        return;
                }
                // bin0
                {
                    const u32      bi = wi & (bin0bits - 1);
                    const bin0type wd = (bi == 0) ? bin0constant : *_bin0;
                    *_bin0            = wd & math::invert((bin0type)1 << bi);
                }
            }
        }

        void setup_used_lazy(bin0type* bin0, bin1type* bin1, u32 maxbits) { *bin0 = bin0constant; }
        void tick_used_lazy(bin0type* bin0, bin1type* bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin1
                {
                    const u32 bi      = wi & (bin1bits - 1);
                    wi                = wi >> bin1shift;
                    const bin1type wd = (bi == 0) ? bin1constant : bin1[wi];
                    bin1[wi]          = wd | ((bin1type)1 << bi);
                }
            }
        }

        void clear(bin0type* _bin0, bin1type* _bin1, u32 maxbits)
        {
            *_bin0         = 0;
            u32 const size = (maxbits + (bin1bits - 1)) >> bin1shift;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
        }

        void set(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const      i  = bit >> bin1shift;
            u32 const      b  = bit & (bin1bits - 1);
            bin1type const vo = _bin1[i];                 // old
            bin1type const vn = vo | ((bin1type)1 << b);  // new
            if (vo != bin1constant && vn == bin1constant)
            {
                _bin1[i] = vn;
                *_bin0 |= ((bin0type)1 << i);
            }
        }

        void clr(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const      i  = bit >> bin1shift;
            u32 const      b  = bit & (bin1bits - 1);
            bin1type const vo = _bin1[i];  // old
            _bin1[i]          = vo & math::invert((bin1type)1 << b);
            if (vo == bin1constant)
                *_bin0 &= math::invert((bin0type)1 << i);
        }

        bool get(bin0type const* _bin0, bin1type const* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> bin1shift;
            u32 const b = bit & (bin1bits - 1);
            return (_bin1[i] & ((bin1type)1 << b)) != 0;
        }

        s32 find(bin0type const* _bin0, bin1type const* _bin1, u32 maxbits)
        {
            if (*_bin0 == bin0constant)
                return -1;
            s32 const      i   = math::findFirstBit(math::invert(*_bin0));
            bin1type const w   = math::invert(_bin1[i]);
            s32 const      bit = math::findFirstBit(w) + (i << bin1shift);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_and_set(bin0type* _bin0, bin1type* _bin1, u32 maxbits)
        {
            if (*_bin0 == bin0constant)
                return -1;

            s32 const i0 = math::findFirstBit(math::invert(*_bin0));
            ASSERT(_bin1[i0] != bin1constant);
            const s32      b1 = math::findFirstBit(math::invert(_bin1[i0]));
            bin1type const v1 = _bin1[i0] | ((bin1type)1 << b1);
            _bin1[i0]         = v1;
            if (v1 == bin1constant)
                *_bin0 |= ((bin0type)1 << i0);

            const s32 bit = b1 + (i0 << bin1shift);
            return bit < (s32)maxbits ? bit : -1;
        }

    }  // namespace nbinmap12

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    namespace nbinmap13
    {
        typedef u32 bin0type;
        typedef u32 bin1type;
        typedef u8  bin2_type;

        constexpr u32 bin1shift = 5;
        constexpr u32 bin2shift = 3;

        constexpr u32 bin0bits = sizeof(bin0type) * 8;
        constexpr u32 bin1bits = sizeof(bin1type) * 8;
        constexpr u32 bin2bits = sizeof(bin2_type) * 8;

        constexpr bin0type  bin0constant  = D_CONSTANT_U32(0xFFFFFFFF);
        constexpr bin1type  bin1constant  = D_CONSTANT_U32(0xFFFFFFFF);
        constexpr bin2_type bin2_constant = D_CONSTANT_U8(0xFF);

        void setup_free_lazy(bin0type* _bin0, bin1type* _bin1, bin2_type* _bin2, u32 maxbits) { *_bin0 = bin0constant; }
        void tick_free_lazy(bin0type* _bin0, bin1type* _bin1, bin2_type* _bin2, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin2
                {
                    const u32 bi       = wi & (bin2bits - 1);
                    wi                 = wi >> bin2shift;
                    const bin2_type wd = (bi == 0) ? bin2_constant : _bin2[wi];
                    _bin2[wi]          = wd & ~((bin2_type)1 << bi);
                    if (wd != bin2_constant)
                        return;
                }
                // bin1
                {
                    const u32 bi      = wi & (bin1bits - 1);
                    wi                = wi >> bin1shift;
                    const bin1type wd = (bi == 0) ? bin1constant : _bin1[wi];
                    _bin1[wi]         = wd & ~((bin1type)1 << bi);
                    if (wd != bin1constant)
                        return;
                }
                // bin0
                {
                    const u32      bi = wi & (bin0bits - 1);
                    const bin0type wd = (bi == 0) ? bin0constant : *_bin0;
                    *_bin0            = wd & ~((bin0type)1 << bi);
                }
            }
        }

        void clear(bin0type* _bin0, bin1type* _bin1, bin2_type* _bin2, u32 maxbits)
        {
            *_bin0             = 0;
            u32 const bin2size = (maxbits + (bin2bits - 1)) >> bin2shift;
            for (u32 i = 0; i < bin2size; ++i)
                _bin2[i] = 0;
            u32 const bin1size = (bin2size + (bin1bits - 1)) >> bin1shift;
            for (u32 i = 0; i < bin1size; ++i)
                _bin1[i] = 0;
        }

        void set(bin0type* _bin0, bin1type* _bin1, bin2_type* _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const       i2 = bit >> bin2shift;
            u32 const       b2 = bit & (bin2bits - 1);
            bin2_type const v2 = _bin2[i2];
            _bin2[i2]          = v2 | ((bin2_type)1 << b2);
            if (v2 == 0)
            {
                u32 const      i1 = i2 >> bin1shift;
                u32 const      b1 = i2 & (bin1bits - 1);
                bin1type const v1 = _bin1[i1];
                _bin1[i1]         = v1 | ((bin1type)1 << b1);
                if (v1 == 0)
                {
                    u32 const      b0 = i1 & (bin0bits - 1);
                    bin0type const v0 = *_bin0;
                    *_bin0            = v0 | ((bin0type)1 << b0);
                }
            }
        }

        void clr(bin0type* _bin0, bin1type* _bin1, bin2_type* _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i2 = bit >> bin2shift;
            u32 const b2 = bit & (bin2bits - 1);
            u64 const v2 = _bin2[i2];
            _bin2[i2]    = v2 & math::invert((bin2_type)1 << b2);
            if (v2 == bin2_constant)
            {
                u32 const      i1 = i2 >> bin1shift;
                u32 const      b1 = i2 & (bin1bits - 1);
                bin1type const v1 = _bin1[i1];
                _bin1[i1]         = v1 & math::invert((bin1type)1 << b1);
                if (v1 == bin1constant)
                {
                    u32 const      b0 = i1 & (bin0bits - 1);
                    bin0type const v0 = *_bin0;
                    *_bin0            = v0 & math::invert((bin0type)1 << b0);
                }
            }
        }

        bool get(bin0type const* _bin0, bin1type const* _bin1, bin2type const* _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> bin2shift;
            u32 const b = bit & (bin2bits - 1);
            return (_bin2[i] & ((bin2_type)1 << b)) != 0;
        }

        s32 find(bin0type const* _bin0, bin1type const* _bin1, bin2type const* _bin2, u32 maxbits)
        {
            if (*_bin0 == bin0constant)
                return -1;

            s32 const b0 = math::findFirstBit(math::invert(*_bin0));

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(math::invert(_bin1[w1]));

            s32 const w2 = b1 + (w1 << bin1shift);
            s32 const b2 = math::findFirstBit(math::invert(_bin2[w2]));

            s32 const bit = b2 + (w2 << bin2shift);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_and_set(u32* _bin0, u32* _bin1, u8* _bin2, u32 maxbits)
        {
            if (*_bin0 == bin0constant)
                return -1;

            s32 const b0 = math::findFirstBit(math::invert(*_bin0));

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(math::invert(_bin1[w1]));

            s32 const w2 = b1 + (w1 << bin1shift);
            s32 const b2 = math::findFirstBit(math::invert(_bin2[w2]));

            s32 const bit = b2 + (w2 << bin2shift);

            // set bit
            {
                // bin2
                {
                    _bin2[w2] = _bin2[w2] | ((bin2_type)1 << b2);
                    if (_bin2[w2] != bin2_constant)
                        return bit < (s32)maxbits ? bit : -1;
                }
                // bin1
                {
                    _bin1[w1] = _bin1[w1] | ((bin1type)1 << b1);
                    if (_bin1[w1] != bin1constant)
                        return bit < (s32)maxbits ? bit : -1;
                }
                // bin0
                {
                    *_bin0 = *_bin0 | ((bin0type)1 << b0);
                }
            }

            return bit < (s32)maxbits ? bit : -1;
        }
    }  // namespace nbinmap13

};  // namespace ncore

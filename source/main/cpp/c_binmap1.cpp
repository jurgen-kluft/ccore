#include "ccore/c_math.h"

#include "ccore/c_binmap1.h"

namespace ncore
{
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // binmap, these functions are tracking '0' bits.

#define D_INVERT(value)        ((bintype) ~(value))
#define D_BIT_CLEAR(word, bit) ((word) & ~((bintype)1 << (bit)))
#define D_BIT_SET(word, bit)   ((word) | ((bintype)1 << (bit)))
#define D_BIT_TEST(word, bit)  (((word) & ((bintype)1 << (bit))) != 0)

    // binmaps with a single level

    namespace nbinmap5
    {
        static constexpr bintype binconstant = (bintype) ~(bintype)0;

        void set(bintype* bin0, u32 maxbits, u32 bit) { *bin0 = D_BIT_SET(*bin0, bit); }
        void clr(bintype* bin0, u32 maxbits, u32 bit) { *bin0 = D_BIT_CLEAR(*bin0, bit); }
        bool get(bintype const* bin0, u32 maxbits, u32 bit) { return D_BIT_TEST(*bin0, bit); }

        s32 find(bintype const* bin0, u32 maxbits)
        {
            if (*bin0 == binconstant)
                return -1;
            s32 const bit = math::findFirstBit(D_INVERT(*bin0));
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_and_set(bintype* bin0, u32 maxbits)
        {
            if (*bin0 == binconstant)
                return -1;
            s32 const bit = math::findFirstBit(D_INVERT(*bin0));
            *bin0         = D_BIT_SET(*bin0, bit);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_last(bintype* bin0, u32 maxbits)
        {
            if (*bin0 == binconstant)
                return -1;
            s32 const bit = math::findLastBit(D_INVERT(*bin0));
            return bit;
        }

        s32 find_last_and_set(bintype* bin0, u32 maxbits)
        {
            if (*bin0 == binconstant)
                return -1;
            s32 const bit = math::findLastBit(D_INVERT(*bin0));
            *bin0         = D_BIT_SET(*bin0, bit);
            return bit;
        }

        s32 find_after(bintype* bin0, u32 maxbits, u32 pivot)
        {
            // mask out anything above the pivot bit
            bintype masked = *bin0 & ~(((bintype)1 << pivot));
            masked         = D_INVERT(masked);
            if (masked == 0)
                return -1;
            s32 const bit = math::findFirstBit(masked);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_before(bintype* bin0, u32 maxbits, u32 pivot)
        {
            // mask out anything below the pivot bit
            bintype masked = *bin0 & (((bintype)1 << pivot) - 1);
            masked         = D_INVERT(masked);
            if (masked == 0)
                return -1;
            s32 const bit = math::findLastBit(masked);
            return bit;
        }

    }  // namespace nbinmap5

    namespace nbinmap6
    {
        static constexpr bintype binconstant = (bintype) ~(bintype)0;

        void set(bintype* bin0, u32 maxbits, u32 bit) { *bin0 |= ((bintype)1 << bit); }
        void clr(bintype* bin0, u32 maxbits, u32 bit) { *bin0 &= D_INVERT((bintype)1 << bit); }
        bool get(bintype const* bin0, u32 maxbits, u32 bit) { return (*bin0 & ((bintype)1 << bit)) != 0; }

        s32 find(bintype const* bin0, u32 maxbits)
        {
            if (*bin0 == binconstant)
                return -1;
            s32 const bit = math::findFirstBit(D_INVERT(*bin0));
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_and_set(bintype* bin0, u32 maxbits)
        {
            if (*bin0 == binconstant)
                return -1;
            s32 const bit = math::findFirstBit(D_INVERT(*bin0));
            *bin0         = D_BIT_SET(*bin0, bit);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_last(bintype* bin0, u32 maxbits)
        {
            if (*bin0 == binconstant)
                return -1;
            s32 const bit = math::findLastBit(D_INVERT(*bin0));
            return bit;
        }

        s32 find_last_and_set(bintype* bin0, u32 maxbits)
        {
            if (*bin0 == binconstant)
                return -1;
            s32 const bit = math::findLastBit(D_INVERT(*bin0));
            *bin0         = D_BIT_SET(*bin0, bit);
            return bit;
        }

        s32 find_after(bintype* bin0, u32 maxbits, u32 pivot)
        {
            // mask out anything above the pivot bit
            bintype masked = *bin0 & ~(((bintype)1 << pivot));
            masked         = D_INVERT(masked);
            if (masked == 0)
                return -1;
            s32 const bit = math::findFirstBit(masked);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_before(bintype* bin0, u32 maxbits, u32 pivot)
        {
            // mask out anything below the pivot bit
            bintype masked = *bin0 & (((bintype)1 << pivot) - 1);
            masked         = D_INVERT(masked);
            if (masked == 0)
                return -1;
            s32 const bit = math::findLastBit(masked);
            return bit;
        }

    }  // namespace nbinmap6

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // binmaps with two levels

    template <typename bintype, u32 binshift>
    class binmap_bin0_bin1_t
    {
    public:
        static constexpr u32     binbits     = sizeof(bintype) * 8;
        static constexpr bintype binconstant = (bintype) ~(bintype)0;

        static void setup_lazy(bintype* _bin0, bintype* _bin1, u32 maxbits) { *_bin0 = binconstant; }
        static void tick_lazy(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32       wi      = bit;

                const u32 bi1     = wi & (binbits - 1);
                wi                = wi >> binshift;
                const bintype wd1 = (bi1 == 0) ? binconstant : _bin1[wi];
                _bin1[wi]         = D_BIT_CLEAR(wd1, bi1);

                if (bi1 == 0)
                {
                    const u32     bi0 = wi & (binbits - 1);
                    const bintype wd0 = (bi0 == 0) ? binconstant : *_bin0;
                    *_bin0            = D_BIT_CLEAR(wd0, bi0);
                }
            }
        }

        static void clear(bintype* _bin0, bintype* _bin1, u32 maxbits)
        {
            u32 const size = (maxbits + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
            *_bin0 = 0;
        }

        static void set(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i1 = bit >> binshift;
            u32 const     b1 = bit & (binbits - 1);
            bintype const vo = _bin1[i1];          // old
            bintype const vn = D_BIT_SET(vo, b1);  // new
            _bin1[i1]        = vn;
            if (vo != binconstant && vn == binconstant)
            {
                *_bin0 = D_BIT_SET(*_bin0, i1);
            }
        }

        static void clr(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i1 = bit >> binshift;
            u32 const     b1 = bit & (binbits - 1);
            bintype const vo = _bin1[i1];  // old
            _bin1[i1]        = D_BIT_CLEAR(vo, b1);
            if (vo == binconstant)
            {
                *_bin0 = D_BIT_CLEAR(*_bin0, i1);
            }
        }

        static bool get(bintype const* _bin0, bintype const* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i1 = bit >> binshift;
            u32 const b1 = bit & (binbits - 1);
            return D_BIT_TEST(_bin1[i1], b1);
        }

        static s32 find(bintype const* _bin0, bintype const* _bin1, u32 maxbits)
        {
            if (*_bin0 == binconstant)
                return -1;
            s32 const     b0  = math::findFirstBit(D_INVERT(*_bin0));
            bintype const w1  = D_INVERT(_bin1[b0]);
            s32 const     bit = math::findFirstBit(w1) + (b0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_and_set(bintype* _bin0, bintype* _bin1, u32 maxbits)
        {
            if (*_bin0 == binconstant)
                return -1;
            s32 const i0 = math::findFirstBit(D_INVERT(*_bin0));
            ASSERT(_bin1[i0] != binconstant);
            const s32     b1 = math::findFirstBit(D_INVERT(_bin1[i0]));
            bintype const v1 = D_BIT_SET(_bin1[i0], b1);
            _bin1[i0]        = v1;
            if (v1 == binconstant)
            {
                *_bin0 = D_BIT_SET(*_bin0, i0);
            }

            const s32 bit = b1 + (i0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_last(bintype* _bin0, bintype* _bin1, u32 maxbits)
        {
            if (*_bin0 == binconstant)
                return -1;

            u32 wi = 0;
            u8  bi = (u8)math::findLastBit(D_INVERT(*_bin0));
            ASSERT(bi >= 0 && bi < binbits);
            {
                wi = (wi << binshift) + bi;
                ASSERT(D_INVERT(_bin1[wi]) != 0);
                bi = math::findLastBit(D_INVERT(_bin1[wi]));
                ASSERT(bi >= 0 && bi < binbits);
            }

            u32 const found_bit = (wi << binshift) + bi;
            return (found_bit < maxbits) ? found_bit : -1;
        }

        static s32 find_last_and_set(bintype* _bin0, bintype* _bin1, u32 maxbits)
        {
            s32 const bit = find_last(_bin0, _bin1, maxbits);
            if (bit >= 0)
                set(_bin0, _bin1, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_after(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;
            u32 iw = pivot >> binshift;
            s8  ib = (s8)(pivot & (binbits - 1));

            s8 const ml = 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const* level = il == 0 ? _bin0 : _bin1;
                bintype const  w     = (~level[iw]) & (binconstant << ib);
                if (w != 0)
                {
                    iw = (iw * binbits) + math::findFirstBit(w);
                    if (il == ml)
                        return iw < maxbits ? (s32)iw : -1;
                    il += 1;  // Go down one level
                    ib = 0;
                }
                else
                {
                    // move one unit in the direction of upper
                    iw += 1;
                    if (il == ml)
                    {
                        u32 const nwpl = (maxbits + binbits - 1) >> binshift;  // Number of words per level, lowest level
                        if (iw >= nwpl)
                            break;
                    }
                    ib = (iw & (binbits - 1));
                    iw = (iw >> binshift);
                    il -= 1;  // Go up one level
                }
            }
            return -1;
        }

        static s32 find_before(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 pivot)
        {
            if (pivot >= maxbits)
                return -1;

            u32 iw = (pivot >> binshift);      // The index of a 32-bit word in level 0
            u32 ib = (pivot & (binbits - 1));  // The bit number in that 32-bit word

            s8 const ml = 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const* level = il == 0 ? _bin0 : _bin1;
                bintype const  w     = (~level[iw]) & (binconstant >> (binbits - 1 - ib));
                if (w != 0)
                {
                    iw = (iw * binbits) + (u32)math::findFirstBit(w);
                    if (il == ml)
                        return iw < maxbits ? (s32)iw : -1;
                    il += 1;  // Go down one level
                    ib = binbits - 1;
                }
                else
                {
                    // move one unit in the direction of lower
                    if (iw == 0)
                        break;
                    iw -= 1;
                    ib = (iw & (binbits - 1));
                    iw = (iw >> binshift);
                    il -= 1;  // Go up one level
                }
            }

            return -1;
        }
    };

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    namespace nbinmap10
    {
        void setup_lazy(bintype* _bin0, bintype* _bin1, u32 maxbits) { binmap_bin0_bin1_t<bintype, 5>::setup_lazy(_bin0, _bin1, maxbits); }
        void tick_lazy(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bintype, 5>::tick_lazy(_bin0, _bin1, maxbits, bit); }
        void clear(bintype* _bin0, bintype* _bin1, u32 maxbits) { binmap_bin0_bin1_t<bintype, 5>::clear(_bin0, _bin1, maxbits); }
        void set(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bintype, 5>::set(_bin0, _bin1, maxbits, bit); }
        void clr(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bintype, 5>::clr(_bin0, _bin1, maxbits, bit); }
        bool get(bintype const* _bin0, bintype const* _bin1, u32 maxbits, u32 bit) { return binmap_bin0_bin1_t<bintype, 5>::get(_bin0, _bin1, maxbits, bit); }
        s32  find(bintype const* _bin0, bintype const* _bin1, u32 maxbits) { return binmap_bin0_bin1_t<bintype, 5>::find(_bin0, _bin1, maxbits); }
        s32  find_and_set(bintype* _bin0, bintype* _bin1, u32 maxbits) { return binmap_bin0_bin1_t<bintype, 5>::find_and_set(_bin0, _bin1, maxbits); }
        s32  find_last(bintype* _bin0, bintype* _bin1, u32 maxbits) { return binmap_bin0_bin1_t<bintype, 5>::find_last(_bin0, _bin1, maxbits); }
        s32  find_last_and_set(bintype* _bin0, bintype* _bin1, u32 maxbits) { return binmap_bin0_bin1_t<bintype, 5>::find_last_and_set(_bin0, _bin1, maxbits); }
        s32  find_after(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 pivot) { return binmap_bin0_bin1_t<bintype, 5>::find_after(_bin0, _bin1, maxbits, pivot); }
        s32  find_before(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 pivot) { return binmap_bin0_bin1_t<bintype, 5>::find_before(_bin0, _bin1, maxbits, pivot); }
    }  // namespace nbinmap10

    namespace nbinmap12
    {
        void setup_lazy(bintype* _bin0, bintype* _bin1, u32 maxbits) { binmap_bin0_bin1_t<bintype, 6>::setup_lazy(_bin0, _bin1, maxbits); }
        void tick_lazy(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bintype, 6>::tick_lazy(_bin0, _bin1, maxbits, bit); }
        void clear(bintype* _bin0, bintype* _bin1, u32 maxbits) { binmap_bin0_bin1_t<bintype, 6>::clear(_bin0, _bin1, maxbits); }
        void set(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bintype, 6>::set(_bin0, _bin1, maxbits, bit); }
        void clr(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bintype, 6>::clr(_bin0, _bin1, maxbits, bit); }
        bool get(bintype const* _bin0, bintype const* _bin1, u32 maxbits, u32 bit) { return binmap_bin0_bin1_t<bintype, 6>::get(_bin0, _bin1, maxbits, bit); }
        s32  find(bintype const* _bin0, bintype const* _bin1, u32 maxbits) { return binmap_bin0_bin1_t<bintype, 6>::find(_bin0, _bin1, maxbits); }
        s32  find_and_set(bintype* _bin0, bintype* _bin1, u32 maxbits) { return binmap_bin0_bin1_t<bintype, 6>::find_and_set(_bin0, _bin1, maxbits); }
        s32  find_last(bintype* _bin0, bintype* _bin1, u32 maxbits) { return binmap_bin0_bin1_t<bintype, 6>::find_last(_bin0, _bin1, maxbits); }
        s32  find_last_and_set(bintype* _bin0, bintype* _bin1, u32 maxbits) { return binmap_bin0_bin1_t<bintype, 6>::find_last_and_set(_bin0, _bin1, maxbits); }
        s32  find_after(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 pivot) { return binmap_bin0_bin1_t<bintype, 6>::find_after(_bin0, _bin1, maxbits, pivot); }
        s32  find_before(bintype* _bin0, bintype* _bin1, u32 maxbits, u32 pivot) { return binmap_bin0_bin1_t<bintype, 6>::find_before(_bin0, _bin1, maxbits, pivot); }
    }  // namespace nbinmap12

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // Below are the binmaps that have three levels.
    template <typename bintype, u32 binshift>
    class binmap_bin0_bin1_bin2_t
    {
    public:
        static constexpr u32     binbits     = sizeof(bintype) * 8;
        static constexpr bintype binconstant = (bintype) ~(bintype)0;

        static void setup_lazy(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits) { *_bin0 = binconstant; }
        static void tick_lazy(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                const u32 bi2     = wi & (binbits - 1);
                wi                = wi >> binshift;
                const bintype wd2 = (bi2 == 0) ? binconstant : _bin2[wi];
                _bin2[wi]         = wd2 & D_INVERT((bintype)1 << bi2);

                if (bi2 == 0)
                {
                    const u32 bi1     = wi & (binbits - 1);
                    wi                = wi >> binshift;
                    const bintype wd1 = (bi1 == 0) ? binconstant : _bin1[wi];
                    _bin1[wi]         = wd1 & D_INVERT((bintype)1 << bi1);

                    if (bi1 == 0)
                    {
                        const u32     bi0 = wi & (binbits - 1);
                        const bintype wd0 = (bi0 == 0) ? binconstant : *_bin0;
                        *_bin0            = wd0 & D_INVERT((bintype)1 << bi0);
                    }
                }
            }
        }

        static void clear(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits)
        {
            u32 wi = (maxbits + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < wi; ++i)
                _bin2[i] = 0;
            wi = (wi + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < wi; ++i)
                _bin1[i] = 0;
            *_bin0 = 0;
        }

        static void set(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i2 = bit >> binshift;
            u32 const     b2 = bit & (binbits - 1);
            bintype const v2 = D_BIT_SET(_bin2[i2], b2);
            _bin2[i2]        = v2;
            if (v2 == binconstant)
            {
                u32 const     i1 = i2 >> binshift;
                u32 const     b1 = i2 & (binbits - 1);
                bintype const v1 = D_BIT_SET(_bin1[i1], b1);
                _bin1[i1]        = v1;
                if (v1 == binconstant)
                {
                    u32 const     b0 = i1 & (binbits - 1);
                    bintype const v0 = *_bin0;
                    *_bin0           = D_BIT_SET(v0, b0);
                }
            }
        }

        static void clr(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i2 = bit >> binshift;
            u32 const b2 = bit & (binbits - 1);
            u64 const v2 = _bin2[i2];
            _bin2[i2]    = v2 & D_INVERT((bintype)1 << b2);
            if (v2 == binconstant)
            {
                u32 const     i1 = i2 >> binshift;
                u32 const     b1 = i2 & (binbits - 1);
                bintype const v1 = _bin1[i1];
                _bin1[i1]        = v1 & D_INVERT((bintype)1 << b1);
                if (v1 == binconstant)
                {
                    u32 const     b0 = i1 & (binbits - 1);
                    bintype const v0 = *_bin0;
                    *_bin0           = v0 & D_INVERT((bintype)1 << b0);
                }
            }
        }

        static bool get(bintype const* _bin0, bintype const* _bin1, bintype const* _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> binshift;
            u32 const b = bit & (binbits - 1);
            return D_BIT_TEST(_bin2[i], b);
        }

        static s32 find(bintype const* _bin0, bintype const* _bin1, bintype const* _bin2, u32 maxbits)
        {
            if (*_bin0 == binconstant)
                return -1;

            s32 const b0 = math::findFirstBit(D_INVERT(*_bin0));

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(D_INVERT(_bin1[w1]));

            s32 const w2 = b1 + (w1 << binshift);
            s32 const b2 = math::findFirstBit(D_INVERT(_bin2[w2]));

            s32 const bit = b2 + (w2 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_and_set(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits)
        {
            if (*_bin0 == binconstant)
                return -1;

            s32 const b0 = math::findFirstBit(D_INVERT(*_bin0));

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(D_INVERT(_bin1[w1]));

            s32 const w2 = b1 + (w1 << binshift);
            s32 const b2 = math::findFirstBit(D_INVERT(_bin2[w2]));

            s32 const bit = b2 + (w2 << binshift);

            // set bit
            {
                // bin2
                {
                    _bin2[w2] = D_BIT_SET(_bin2[w2], b2);
                    if (_bin2[w2] != binconstant)
                        return bit < (s32)maxbits ? bit : -1;
                }
                // bin1
                {
                    _bin1[w1] = D_BIT_SET(_bin1[w1], b1);
                    if (_bin1[w1] != binconstant)
                        return bit < (s32)maxbits ? bit : -1;
                }
                // bin0
                {
                    *_bin0 = D_BIT_SET(*_bin0, b0);
                }
            }

            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_last(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits)
        {
            if (*_bin0 == binconstant)
                return -1;

            u32 wi = 0;
            u8  bi = (u8)math::findLastBit(D_INVERT(*_bin0));
            ASSERT(bi >= 0 && bi < binbits);
            {
                wi = (wi << binshift) + bi;
                ASSERT(D_INVERT(_bin1[wi]) != 0);
                bi = math::findLastBit(D_INVERT(_bin1[wi]));
                ASSERT(bi >= 0 && bi < binbits);

                {
                    wi = (wi << binshift) + bi;
                    ASSERT(D_INVERT(_bin2[wi]) != 0);
                    bi = math::findLastBit(D_INVERT(_bin2[wi]));
                    ASSERT(bi >= 0 && bi < binbits);
                }
            }

            u32 const found_bit = (wi << binshift) + bi;
            return (found_bit < maxbits) ? found_bit : -1;
        }

        static s32 find_last_and_set(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits)
        {
            s32 const bit = find_last(_bin0, _bin1, _bin2, maxbits);
            if (bit >= 0)
                set(_bin0, _bin1, _bin2, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_after(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;
            u32 iw = pivot >> binshift;
            s8  ib = (s8)(pivot & (binbits - 1));

            s8 const ml = 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const* level = il == 0 ? _bin0 : (il == 1 ? _bin1 : _bin2);
                bintype const  w     = (~level[iw]) & (binconstant << ib);
                if (w != 0)
                {
                    iw = (iw * binbits) + math::findFirstBit(w);
                    if (il == ml)
                        return iw < maxbits ? (s32)iw : -1;
                    il += 1;  // Go down one level
                    ib = 0;
                }
                else
                {
                    // move one unit in the direction of upper
                    iw += 1;
                    if (il == ml)
                    {
                        u32 const nwpl = (maxbits + binbits - 1) >> binshift;  // Number of words per level, lowest level
                        if (iw >= nwpl)
                            break;
                    }
                    ib = (iw & (binbits - 1));
                    iw = (iw >> binshift);
                    il -= 1;  // Go up one level
                }
            }
            return -1;
        }

        static s32 find_before(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 pivot)
        {
            if (pivot >= maxbits)
                return -1;

            u32 iw = (pivot >> binshift);      // The index of a 32-bit word in level 0
            u32 ib = (pivot & (binbits - 1));  // The bit number in that 32-bit word

            s8 const ml = 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const* level = il == 0 ? _bin0 : (il == 1 ? _bin1 : _bin2);
                bintype const  w     = (~level[iw]) & (binconstant >> (binbits - 1 - ib));
                if (w != 0)
                {
                    iw = (iw * binbits) + (u32)math::findFirstBit(w);
                    if (il == ml)
                        return iw < maxbits ? (s32)iw : -1;
                    il += 1;  // Go down one level
                    ib = binbits - 1;
                }
                else
                {
                    // move one unit in the direction of lower
                    if (iw == 0)
                        break;
                    iw -= 1;
                    ib = (iw & (binbits - 1));
                    iw = (iw >> binshift);
                    il -= 1;  // Go up one level
                }
            }

            return -1;
        }
    };

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    namespace nbinmap15
    {
        // constexpr u32 binshift = 5;
        constexpr u32 binshift = 5;

        void setup_lazy(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits) { binmap_bin0_bin1_bin2_t<bintype, binshift>::setup_lazy(_bin0, _bin1, _bin2, maxbits); }
        void tick_lazy(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bintype, binshift>::tick_lazy(_bin0, _bin1, _bin2, maxbits, bit); }
        void clear(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits) { binmap_bin0_bin1_bin2_t<bintype, binshift>::clear(_bin0, _bin1, _bin2, maxbits); }
        void set(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bintype, binshift>::set(_bin0, _bin1, _bin2, maxbits, bit); }
        void clr(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bintype, binshift>::clr(_bin0, _bin1, _bin2, maxbits, bit); }
        bool get(bintype const* _bin0, bintype const* _bin1, bintype const* _bin2, u32 maxbits, u32 bit) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::get(_bin0, _bin1, _bin2, maxbits, bit); }
        s32  find(bintype const* _bin0, bintype const* _bin1, bintype const* _bin2, u32 maxbits) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::find(_bin0, _bin1, _bin2, maxbits); }
        s32  find_and_set(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::find_and_set(_bin0, _bin1, _bin2, maxbits); }

        s32 find_last(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::find_last(_bin0, _bin1, _bin2, maxbits); }
        s32 find_last_and_set(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::find_last_and_set(_bin0, _bin1, _bin2, maxbits); }
        s32 find_after(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 pivot) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::find_after(_bin0, _bin1, _bin2, maxbits, pivot); }
        s32 find_before(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 pivot) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::find_before(_bin0, _bin1, _bin2, maxbits, pivot); }
    }  // namespace nbinmap15

    namespace nbinmap18
    {
        // constexpr u32 binshift = 6;
        constexpr u32 binshift = 6;

        void setup_lazy(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits) { binmap_bin0_bin1_bin2_t<bintype, binshift>::setup_lazy(_bin0, _bin1, _bin2, maxbits); }
        void tick_lazy(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bintype, binshift>::tick_lazy(_bin0, _bin1, _bin2, maxbits, bit); }
        void clear(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits) { binmap_bin0_bin1_bin2_t<bintype, binshift>::clear(_bin0, _bin1, _bin2, maxbits); }
        void set(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bintype, binshift>::set(_bin0, _bin1, _bin2, maxbits, bit); }
        void clr(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bintype, binshift>::clr(_bin0, _bin1, _bin2, maxbits, bit); }
        bool get(bintype const* _bin0, bintype const* _bin1, bintype const* _bin2, u32 maxbits, u32 bit) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::get(_bin0, _bin1, _bin2, maxbits, bit); }
        s32  find(bintype const* _bin0, bintype const* _bin1, bintype const* _bin2, u32 maxbits) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::find(_bin0, _bin1, _bin2, maxbits); }
        s32  find_and_set(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::find_and_set(_bin0, _bin1, _bin2, maxbits); }

        s32 find_last(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::find_last(_bin0, _bin1, _bin2, maxbits); }
        s32 find_last_and_set(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::find_last_and_set(_bin0, _bin1, _bin2, maxbits); }
        s32 find_after(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 pivot) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::find_after(_bin0, _bin1, _bin2, maxbits, pivot); }
        s32 find_before(bintype* _bin0, bintype* _bin1, bintype* _bin2, u32 maxbits, u32 pivot) { return binmap_bin0_bin1_bin2_t<bintype, binshift>::find_before(_bin0, _bin1, _bin2, maxbits, pivot); }
    }  // namespace nbinmap18

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // Below are the binmaps that have three levels.
    template <typename bintype, u32 binshift>
    class binmap_bin0_bin1_bin2_bin3_t
    {
    public:
        static constexpr u32     binbits     = sizeof(bintype) * 8;
        static constexpr bintype binconstant = (bintype) ~(bintype)0;

        static void setup_lazy(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits) { *_bin0 = binconstant; }
        static void tick_lazy(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin3
                const u32 bi3     = wi & (binbits - 1);
                wi                = wi >> binshift;
                const bintype wd3 = (bi3 == 0) ? binconstant : _bin3[wi];
                _bin3[wi]         = wd3 & D_INVERT((bintype)1 << bi3);

                if (bi3 == 0)
                {
                    const u32 bi2     = wi & (binbits - 1);
                    wi                = wi >> binshift;
                    const bintype wd2 = (bi2 == 0) ? binconstant : _bin2[wi];
                    _bin2[wi]         = wd2 & D_INVERT((bintype)1 << bi2);

                    if (bi2 == 0)
                    {
                        const u32 bi1     = wi & (binbits - 1);
                        wi                = wi >> binshift;
                        const bintype wd1 = (bi1 == 0) ? binconstant : _bin1[wi];
                        _bin1[wi]         = wd1 & D_INVERT((bintype)1 << bi1);

                        if (bi1 == 0)
                        {
                            const u32     bi0 = wi & (binbits - 1);
                            const bintype wd0 = (bi0 == 0) ? binconstant : *_bin0;
                            *_bin0            = wd0 & D_INVERT((bintype)1 << bi0);
                        }
                    }
                }
            }
        }

        static void clear(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits)
        {
            u32 wi = (maxbits + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < wi; ++i)
                _bin3[i] = 0;
            wi = (wi + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < wi; ++i)
                _bin2[i] = 0;
            wi = (wi + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < wi; ++i)
                _bin1[i] = 0;
            *_bin0 = 0;
        }

        static void set(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i3 = bit >> binshift;
            u32 const     b3 = bit & (binbits - 1);
            bintype const v3 = D_BIT_SET(_bin3[i3], b3);
            _bin3[i3]        = v3;
            if (v3 == binconstant)
            {
                u32 const     i2 = i3 >> binshift;
                u32 const     b2 = i3 & (binbits - 1);
                bintype const v2 = D_BIT_SET(_bin2[i2], b2);
                _bin2[i2]        = v2;
                if (v2 == binconstant)
                {
                    u32 const     i1 = i2 >> binshift;
                    u32 const     b1 = i2 & (binbits - 1);
                    bintype const v1 = D_BIT_SET(_bin1[i1], b1);
                    _bin1[i1]        = v1;
                    if (v1 == binconstant)
                    {
                        u32 const     b0 = i1 & (binbits - 1);
                        bintype const v0 = D_BIT_SET(*_bin0, b0);
                        *_bin0           = v0;
                    }
                }
            }
        }

        static void clr(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i3 = bit >> binshift;
            u32 const b3 = bit & (binbits - 1);
            u64 const v3 = _bin3[i3];
            _bin3[i3]    = v3 & D_INVERT((bintype)1 << b3);
            if (v3 == binconstant)
            {
                u32 const i2 = i3 >> binshift;
                u32 const b2 = i3 & (binbits - 1);
                u64 const v2 = _bin2[i2];
                _bin2[i2]    = v2 & D_INVERT((bintype)1 << b2);
                if (v2 == binconstant)
                {
                    u32 const i1 = i2 >> binshift;
                    u32 const b1 = i2 & (binbits - 1);
                    u64 const v1 = _bin1[i1];
                    _bin1[i1]    = v1 & D_INVERT((bintype)1 << b1);
                    if (v1 == binconstant)
                    {
                        u32 const     b0 = i1 & (binbits - 1);
                        bintype const v0 = *_bin0;
                        *_bin0           = v0 & D_INVERT((bintype)1 << b0);
                    }
                }
            }
        }

        static bool get(bintype const* _bin0, bintype const* _bin1, bintype const* _bin2, bintype const* _bin3, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> binshift;
            u32 const b = bit & (binbits - 1);
            return D_BIT_TEST(_bin3[i], b);
        }

        static s32 find(bintype const* _bin0, bintype const* _bin1, bintype const* _bin2, bintype const* _bin3, u32 maxbits)
        {
            if (*_bin0 == binconstant)
                return -1;

            s32 const b0 = math::findFirstBit(D_INVERT(*_bin0));

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(D_INVERT(_bin1[w1]));

            s32 const w2 = b1 + (w1 << binshift);
            s32 const b2 = math::findFirstBit(D_INVERT(_bin2[w2]));

            s32 const w3 = b2 + (w2 << binshift);
            s32 const b3 = math::findFirstBit(D_INVERT(_bin3[w3]));

            s32 const bit = b3 + (w3 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_and_set(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits)
        {
            if (*_bin0 == binconstant)
                return -1;

            s32 const b0 = math::findFirstBit(D_INVERT(*_bin0));

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(D_INVERT(_bin1[w1]));

            s32 const w2 = b1 + (w1 << binshift);
            s32 const b2 = math::findFirstBit(D_INVERT(_bin2[w2]));

            s32 const w3 = b2 + (w2 << binshift);
            s32 const b3 = math::findFirstBit(D_INVERT(_bin3[w3]));

            s32 const bit = b3 + (w3 << binshift);

            // set bit
            {
                // bin3
                {
                    _bin3[w3] = D_BIT_SET(_bin3[w3], b3);
                    if (_bin3[w3] != binconstant)
                        return bit < (s32)maxbits ? bit : -1;
                }
                // bin2
                {
                    _bin2[w2] = D_BIT_SET(_bin2[w2], b2);
                    if (_bin2[w2] != binconstant)
                        return bit < (s32)maxbits ? bit : -1;
                }
                // bin1
                {
                    _bin1[w1] = D_BIT_SET(_bin1[w1], b1);
                    if (_bin1[w1] != binconstant)
                        return bit < (s32)maxbits ? bit : -1;
                }
                // bin0
                {
                    *_bin0 = D_BIT_SET(*_bin0, b0);
                }
            }

            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_last(bintype* _bin0, bintype* _bin1, bintype* bin2, bintype* bin3, u32 maxbits)
        {
            if (*_bin0 == binconstant)
                return -1;

            u32 wi = 0;
            u8  bi = (u8)math::findLastBit(D_INVERT(*_bin0));
            ASSERT(bi >= 0 && bi < binbits);

            wi = (wi << binshift) + bi;
            ASSERT(D_INVERT(_bin1[wi]) != 0);
            bi = math::findLastBit(D_INVERT(_bin1[wi]));
            ASSERT(bi >= 0 && bi < binbits);

            wi = (wi << binshift) + bi;
            ASSERT(D_INVERT(bin2[wi]) != 0);
            bi = math::findLastBit(D_INVERT(bin2[wi]));
            ASSERT(bi >= 0 && bi < binbits);

            wi = (wi << binshift) + bi;
            ASSERT(D_INVERT(bin3[wi]) != 0);
            bi = math::findLastBit(D_INVERT(bin3[wi]));
            ASSERT(bi >= 0 && bi < binbits);

            u32 const found_bit = (wi << binshift) + bi;
            return (found_bit < maxbits) ? found_bit : -1;
        }

        static s32 find_last_and_set(bintype* _bin0, bintype* _bin1, bintype* bin2, bintype* bin3, u32 maxbits)
        {
            s32 const bit = find_last(_bin0, _bin1, bin2, bin3, maxbits);
            if (bit >= 0)
                set(_bin0, _bin1, bin2, bin3, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_after(bintype* _bin0, bintype* _bin1, bintype* bin2, bintype* bin3, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;
            u32 iw = pivot >> binshift;
            s8  ib = (s8)(pivot & (binbits - 1));

            s8 const ml = 2;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const* level = il == 0 ? _bin0 : (il == 1 ? _bin1 : (il == 2 ? bin2 : bin3));
                bintype const  w     = (~level[iw]) & (binconstant << ib);
                if (w != 0)
                {
                    iw = (iw * binbits) + math::findFirstBit(w);
                    if (il == ml)
                        return iw < maxbits ? (s32)iw : -1;
                    il += 1;  // Go down one level
                    ib = 0;
                }
                else
                {
                    // move one unit in the direction of upper
                    iw += 1;
                    if (il == ml)
                    {
                        u32 const nwpl = (maxbits + binbits - 1) >> binshift;  // Number of words per level, lowest level
                        if (iw >= nwpl)
                            break;
                    }
                    ib = (iw & (binbits - 1));
                    iw = (iw >> binshift);
                    il -= 1;  // Go up one level
                }
            }
            return -1;
        }

        static s32 find_before(bintype* _bin0, bintype* _bin1, bintype* bin2, bintype* bin3, u32 maxbits, u32 pivot)
        {
            if (pivot >= maxbits)
                return -1;

            u32 iw = (pivot >> binshift);      // The index of a 32-bit word in level 0
            u32 ib = (pivot & (binbits - 1));  // The bit number in that 32-bit word

            s8 const ml = 2;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const* level = il == 0 ? _bin0 : (il == 1 ? _bin1 : (il == 2 ? bin2 : bin3));
                bintype const  w     = (~level[iw]) & (binconstant >> (binbits - 1 - ib));
                if (w != 0)
                {
                    iw = (iw * binbits) + (u32)math::findFirstBit(w);
                    if (il == ml)
                        return iw < maxbits ? (s32)iw : -1;
                    il += 1;  // Go down one level
                    ib = binbits - 1;
                }
                else
                {
                    // move one unit in the direction of lower
                    if (iw == 0)
                        break;
                    iw -= 1;
                    ib = (iw & (binbits - 1));
                    iw = (iw >> binshift);
                    il -= 1;  // Go up one level
                }
            }
            return -1;
        }
    };

    namespace nbinmap20
    {
        void setup_lazy(bintype* bin0, bintype* bin1, bintype* bin2, bintype* bin3, u32 maxbits) { binmap_bin0_bin1_bin2_bin3_t<bintype, 5>::setup_lazy(bin0, bin1, bin2, bin3, maxbits); }
        void tick_lazy(bintype* bin0, bintype* bin1, bintype* bin2, bintype* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bintype, 5>::tick_lazy(bin0, bin1, bin2, bin3, maxbits, bit); }

        void clear(bintype* bin0, bintype* bin1, bintype* bin2, bintype* bin3, u32 maxbits) { binmap_bin0_bin1_bin2_bin3_t<bintype, 5>::clear(bin0, bin1, bin2, bin3, maxbits); }

        void set(bintype* bin0, bintype* bin1, bintype* bin2, bintype* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bintype, 5>::set(bin0, bin1, bin2, bin3, maxbits, bit); }
        void clr(bintype* bin0, bintype* bin1, bintype* bin2, bintype* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bintype, 5>::clr(bin0, bin1, bin2, bin3, maxbits, bit); }
        bool get(bintype const* bin0, bintype const* bin1, bintype const* bin2, bintype const* bin3, u32 maxbits, u32 bit) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 5>::get(bin0, bin1, bin2, bin3, maxbits, bit); }
        s32  find(bintype const* bin0, bintype const* bin1, bintype const* bin2, bintype const* bin3, u32 maxbits) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 5>::find(bin0, bin1, bin2, bin3, maxbits); }
        s32  find_and_set(bintype* bin0, bintype* bin1, bintype* bin2, bintype* bin3, u32 maxbits) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 5>::find_and_set(bin0, bin1, bin2, bin3, maxbits); }

        s32 find_last(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 5>::find_last(_bin0, _bin1, _bin2, _bin3, maxbits); }
        s32 find_last_and_set(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 5>::find_last_and_set(_bin0, _bin1, _bin2, _bin3, maxbits); }
        s32 find_after(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits, u32 pivot) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 5>::find_after(_bin0, _bin1, _bin2, _bin3, maxbits, pivot); }
        s32 find_before(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits, u32 pivot) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 5>::find_before(_bin0, _bin1, _bin2, _bin3, maxbits, pivot); }
    }  // namespace nbinmap20

    namespace nbinmap24
    {
        void setup_lazy(bintype* bin0, bintype* bin1, bintype* bin2, bintype* bin3, u32 maxbits) { binmap_bin0_bin1_bin2_bin3_t<bintype, 6>::setup_lazy(bin0, bin1, bin2, bin3, maxbits); }
        void tick_lazy(bintype* bin0, bintype* bin1, bintype* bin2, bintype* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bintype, 6>::tick_lazy(bin0, bin1, bin2, bin3, maxbits, bit); }

        void clear(bintype* bin0, bintype* bin1, bintype* bin2, bintype* bin3, u32 maxbits) { binmap_bin0_bin1_bin2_bin3_t<bintype, 6>::clear(bin0, bin1, bin2, bin3, maxbits); }

        void set(bintype* bin0, bintype* bin1, bintype* bin2, bintype* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bintype, 6>::set(bin0, bin1, bin2, bin3, maxbits, bit); }
        void clr(bintype* bin0, bintype* bin1, bintype* bin2, bintype* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bintype, 6>::clr(bin0, bin1, bin2, bin3, maxbits, bit); }
        bool get(bintype const* bin0, bintype const* bin1, bintype const* bin2, bintype const* bin3, u32 maxbits, u32 bit) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 6>::get(bin0, bin1, bin2, bin3, maxbits, bit); }
        s32  find(bintype const* bin0, bintype const* bin1, bintype const* bin2, bintype const* bin3, u32 maxbits) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 6>::find(bin0, bin1, bin2, bin3, maxbits); }
        s32  find_and_set(bintype* bin0, bintype* bin1, bintype* bin2, bintype* bin3, u32 maxbits) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 6>::find_and_set(bin0, bin1, bin2, bin3, maxbits); }

        s32 find_last(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 6>::find_last(_bin0, _bin1, _bin2, _bin3, maxbits); }
        s32 find_last_and_set(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 6>::find_last_and_set(_bin0, _bin1, _bin2, _bin3, maxbits); }
        s32 find_after(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits, u32 pivot) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 6>::find_after(_bin0, _bin1, _bin2, _bin3, maxbits, pivot); }
        s32 find_before(bintype* _bin0, bintype* _bin1, bintype* _bin2, bintype* _bin3, u32 maxbits, u32 pivot) { return binmap_bin0_bin1_bin2_bin3_t<bintype, 6>::find_before(_bin0, _bin1, _bin2, _bin3, maxbits, pivot); }
    }  // namespace nbinmap24

    namespace nbinmap
    {
        // Note: maximum count is 16 Million (6 bits + 6 bits + 6 bits + 6 bits = 24 bits = 16 M)
        void compute(u32 number_of_bits, layout64_t& layout)
        {
            ASSERT(number_of_bits > 0 && number_of_bits <= 16 * 1024 * 1024);

            layout.m_maxbits = number_of_bits;
            layout.m_bin0    = 0;  // u64[N], N for level 1
            layout.m_bin1    = 0;  // u64[N], N for level 2
            layout.m_bin2    = 0;  // u64[N], N for level 3
            layout.m_bin3    = 0;  // u64[N], N for level 4

            u32 len = number_of_bits;
            // We can have a maximum of 4 levels, each level holds 6 bits (u64)
            layout.m_levels = (u16)(math::mostSignificantBit(len - 1) / 6);
            switch (layout.m_levels)
            {
                case 3: layout.m_bin3 = (len + 63) >> 6; len = (len + 63) >> 6;  // fall through
                case 2: layout.m_bin2 = (len + 63) >> 6; len = (len + 63) >> 6;  // fall through
                case 1: layout.m_bin1 = (len + 63) >> 6; len = (len + 63) >> 6;  // fall through
            }
            layout.m_bin0 = (len + 63) >> 6;
        }

        void pointers(byte* ptr, layout64_t const& l, u64*& bin0, u64*& bin1, u64*& bin2, u64*& bin3)
        {
            bin0 = (u64*)ptr;
            bin1 = (u64*)(ptr + sizeof(u64));
            bin2 = bin1 + l.m_bin1;
            bin3 = bin2 + l.m_bin2;
        }

        u32 sizeof_data(layout64_t const& l) { return l.m_bin0 + l.m_bin1 + l.m_bin2 + l.m_bin3; }

        u32 sizeof_data(layout64_t const& l, u32 bit)
        {
            u32 size = l.m_bin0;
            switch (l.m_levels)
            {
                case 3: size += l.m_bin2;
                case 2: size += l.m_bin1;
                case 1: break;
            }
            size += (bit + 63) >> 6;
            return size;
        }

        // Note: maximum count is 1 Million (5 bits + 5 bits + 5 bits + 5 bits = 20 bits = 1 M)
        void compute(u32 number_of_bits, layout32_t& layout)
        {
            ASSERT(number_of_bits > 0 && number_of_bits <= 1 * 1024 * 1024);

            layout.m_maxbits = number_of_bits;
            layout.m_bin0    = 0;  // u32[N], N for level 1
            layout.m_bin1    = 0;  // u32[N], N for level 2
            layout.m_bin2    = 0;  // u32[N], N for level 3
            layout.m_bin3    = 0;  // u32[N], N for level 4

            u32 len = number_of_bits;
            // We can have a maximum of 4 levels, each level holds 5 bits (u32)
            layout.m_levels = (u16)(math::mostSignificantBit(len - 1) / 5);
            switch (layout.m_levels)
            {
                case 3: layout.m_bin3 = (len + 31) >> 5; len = (len + 31) >> 5;  // fall through
                case 2: layout.m_bin2 = (len + 31) >> 5; len = (len + 31) >> 5;  // fall through
                case 1: layout.m_bin1 = (len + 31) >> 5; len = (len + 31) >> 5;  // fall through
            }
            layout.m_bin0 = (len + 31) >> 5;
        }

        void pointers(byte* ptr, layout32_t const& l, u32*& bin0, u32*& bin1, u32*& bin2, u32*& bin3)
        {
            bin0 = (u32*)ptr;
            bin1 = (u32*)(ptr + sizeof(u32));
            bin2 = bin1 + l.m_bin1;
            bin3 = bin2 + l.m_bin2;
        }

        u32 sizeof_data(layout32_t const& l) { return l.m_bin0 + l.m_bin1 + l.m_bin2 + l.m_bin3; }

        u32 sizeof_data(layout32_t const& l, u32 bit)
        {
            u32 size = l.m_bin0;
            switch (l.m_levels)
            {
                case 3: size += l.m_bin2;
                case 2: size += l.m_bin1;
                case 1: break;
            }
            size += (bit + 31) >> 5;
            return size;
        }

    }  // namespace nbinmap

};  // namespace ncore

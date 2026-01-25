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

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // binmaps with two levels

    template <typename bintype, u32 binshift>
    class binmap_bin00_bin10_bin1_t
    {
    public:
        static constexpr u32     binbits     = sizeof(bintype) * 8;
        static constexpr u32     binmask     = (sizeof(bintype) * 8) - 1;
        static constexpr bintype binconstant = (bintype) ~(bintype)0;

        static void setup_free_lazy(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits)
        {
            _bin00[0] = binconstant;
            _bin10[0] = binconstant;
        }

        static void tick_free_lazy(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits, u32 bit)
        {
            if (bit >= maxbits)
                return;

            if ((bit & binmask) == 0)
            {
                const u32 wi      = bit >> binshift;
                _bin1[wi]         = 0;
                const u32     bi0 = wi & binmask;
                const bintype wd0 = (bi0 == 0) ? binconstant : _bin00[0];
                _bin00[0]         = D_BIT_CLEAR(wd0, bi0);
            }
        }

        static void setup_used_lazy(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits)
        {
            _bin00[0] = binconstant;
            _bin10[0] = binconstant;
        }

        static void tick_used_lazy(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                if ((bit & binmask) == 0)
                {
                    const u32 wi      = bit >> binshift;
                    _bin1[wi]         = binconstant;
                    const u32     bi0 = wi & binmask;
                    const bintype wd0 = (bi0 == 0) ? binconstant : _bin10[0];
                    _bin10[0]         = D_BIT_CLEAR(wd0, bi0);
                }
            }
        }

        static void clear(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits)
        {
            u32 const size = (maxbits + binmask) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
            _bin00[0] = 0;
            _bin10[0] = binconstant;
        }

        static void set(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i1 = bit >> binshift;
            u32 const     b1 = bit & binmask;
            bintype const vo = _bin1[i1];          // old
            bintype const vn = D_BIT_SET(vo, b1);  // new
            _bin1[i1]        = vn;
            if (vn == binconstant)
            {                                          // no more '0' bits at this index in bin1
                _bin00[0] = D_BIT_SET(_bin00[0], i1);  // set bit in bin00, tracking '0' bits
            }
            _bin10[0] = D_BIT_CLEAR(_bin10[0], i1);  // clear bit in bin10, tracking '1' bits
        }

        static void clr(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i1 = bit >> binshift;
            u32 const     b1 = bit & binmask;
            bintype const vo = _bin1[i1];            // old
            bintype const vn = D_BIT_CLEAR(vo, b1);  // new
            _bin1[i1]        = vn;
            _bin00[0]        = D_BIT_CLEAR(_bin00[0], i1);  // clear bit in bin00, tracking '0' bits
            if (vn == 0)
            {                                          // no more '1' bits at this index in bin1
                _bin10[0] = D_BIT_SET(_bin10[0], i1);  // set bit in bin10, tracking '1' bits
            }
        }

        static bool get(bintype const* _bin00, bintype const* _bin10, bintype const* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i1 = bit >> binshift;
            u32 const b1 = bit & binmask;
            return D_BIT_TEST(_bin1[i1], b1);
        }

        static s32 find0(bintype const* _bin00, bintype const* _bin1, u32 maxbits)
        {
            if (_bin00[0] == binconstant)
                return -1;
            s32 const     b0  = math::findFirstBit(D_INVERT(_bin00[0]));
            bintype const w1  = D_INVERT(_bin1[b0]);
            s32 const     bit = math::findFirstBit(w1) + (b0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find1(bintype const* _bin10, bintype const* _bin1, u32 maxbits)
        {
            if (_bin10[0] == binconstant)
                return -1;
            s32 const     b0  = math::findFirstBit(D_INVERT(_bin10[0]));
            bintype const w1  = _bin1[b0];
            s32 const     bit = math::findFirstBit(w1) + (b0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find0_and_set(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits)
        {
            if (_bin00[0] == binconstant)
                return -1;

            s32 const i0 = math::findFirstBit(D_INVERT(_bin00[0]));
            ASSERT(_bin1[i0] != binconstant);
            const s32     b1 = math::findFirstBit(D_INVERT(_bin1[i0]));
            bintype const v1 = D_BIT_SET(_bin1[i0], b1);
            _bin1[i0]        = v1;
            if (v1 == binconstant)
            {                                          // no more '0' bits at this index in bin1
                _bin00[0] = D_BIT_SET(_bin00[0], i0);  // set bit in bin00, tracking '0' bits
            }
            _bin10[0] = _bin10[0] & D_INVERT((bintype)1 << i0);  // clear bit in bin10, tracking '1' bits

            const s32 bit = b1 + (i0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find1_and_set(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits)
        {
            if (_bin10[0] == binconstant)
                return -1;

            s32 const i0 = math::findFirstBit(D_INVERT(_bin10[0]));
            ASSERT(_bin1[i0] != 0);
            const s32     b1 = math::findFirstBit(_bin1[i0]);
            bintype const v1 = D_BIT_CLEAR(_bin1[i0], b1);
            _bin1[i0]        = v1;
            _bin00[0]        = D_BIT_CLEAR(_bin00[0], i0);  // clear bit in bin00, tracking '0' bits
            if (v1 == 0)
            {                                          // no more '1' bits at this index in bin1
                _bin10[0] = D_BIT_SET(_bin10[0], i0);  // set bit in bin10, tracking '1' bits
            }

            const s32 bit = b1 + (i0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find0_last(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits)
        {
            if (_bin00[0] == binconstant)
                return -1;

            u32 wi = 0;
            u8  bi = (u8)math::findLastBit(D_INVERT(_bin00[0]));
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

        static s32 find0_last_and_set(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits)
        {
            s32 const bit = find0_last(_bin00, _bin10, _bin1, maxbits);
            if (bit >= 0)
                set(_bin00, _bin10, _bin1, maxbits, (u32)bit);
            return bit;
        }

        static s32 find0_after(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;

            u32 iw = pivot >> binshift;
            s8  ib = (s8)(pivot & binmask);

            s8 const ml = 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const w = (il == 0 ? D_INVERT(_bin00[0]) : D_INVERT(_bin1[iw])) & (binconstant << ib);
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
                    ib = (iw & binmask);
                    iw = (iw >> binshift);
                    il -= 1;  // Go up one level
                }
            }
            return -1;
        }

        static s32 find0_before(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits, u32 pivot)
        {
            if (pivot >= maxbits)
                return -1;

            u32 iw = (pivot >> binshift);  // The index of a 32-bit word in level 0
            u32 ib = (pivot & binmask);    // The bit number in that 32-bit word

            s8 const ml = 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const w = (il == 0 ? _bin00[0] : D_INVERT(_bin1[iw])) & (binconstant >> (binbits - 1 - ib));
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
                    ib = (iw & binmask);
                    iw = (iw >> binshift);
                    il -= 1;  // Go up one level
                }
            }

            return -1;
        }

        // ---------------------------------------------------------------------------------------------------------
        static s32 find1_last(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits)
        {
            if (_bin10[0] == binconstant)
                return -1;

            u32 wi = 0;
            u8  bi = (u8)math::findLastBit(D_INVERT(_bin10[0]));
            ASSERT(bi >= 0 && bi < binbits);
            {
                wi = (wi << binshift) + bi;
                ASSERT(_bin1[wi] != 0);
                bi = math::findLastBit(_bin1[wi]);
                ASSERT(bi >= 0 && bi < binbits);
            }

            u32 const found_bit = (wi << binshift) + bi;
            return (found_bit < maxbits) ? found_bit : -1;
        }

        static s32 find1_last_and_set(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits)
        {
            s32 const bit = find1_last(_bin00, _bin10, _bin1, maxbits);
            if (bit >= 0)
                set(_bin00, _bin10, _bin1, maxbits, (u32)bit);
            return bit;
        }

        static s32 find1_after(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;

            u32 iw = pivot >> binshift;
            s8  ib = (s8)(pivot & binmask);

            s8 const ml = 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const w = (il == 0 ? D_INVERT(_bin10[0]) : level[iw]) & (binconstant << ib);
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
                    ib = (iw & binmask);
                    iw = (iw >> binshift);
                    il -= 1;  // Go up one level
                }
            }
            return -1;
        }

        static s32 find1_before(bintype* _bin00, bintype* _bin10, bintype* _bin1, u32 maxbits, u32 pivot)
        {
            if (pivot >= maxbits)
                return -1;

            u32 iw = (pivot >> binshift);  // The index of a 32-bit word in level 0
            u32 ib = (pivot & binmask);    // The bit number in that 32-bit word

            s8 const ml = 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const w = (il == 0 ? _bin00[0] : D_INVERT(_bin1[iw])) & (binconstant >> (binbits - 1 - ib));
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
                    ib = (iw & binmask);
                    iw = (iw >> binshift);
                    il -= 1;  // Go up one level
                }
            }

            return -1;
        }
    };

};  // namespace ncore

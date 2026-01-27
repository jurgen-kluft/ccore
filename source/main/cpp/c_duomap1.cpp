#include "ccore/c_math.h"

#include "ccore/c_duomap1.h"

namespace ncore
{
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

#define D_INVERT(value)        ((bintype) ~(value))
#define D_BIT_CLEAR(word, bit) ((word) & ~((bintype)1 << (bit)))
#define D_BIT_SET(word, bit)   ((word) | ((bintype)1 << (bit)))
#define D_BIT_TEST(word, bit)  (((word) & ((bintype)1 << (bit))) != 0)

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // duomaps with two levels

    template <typename bintype, u32 binshift>
    class duomap_bin00_bin10_bin1_t
    {
    public:
        static constexpr u32     binbits     = sizeof(bintype) * 8;
        static constexpr u32     binmask     = (sizeof(bintype) * 8) - 1;
        static constexpr bintype binconstant = (bintype) ~(bintype)0;
        static constexpr u32     binlevels   = 2;

        static void setup_lazy(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits)
        {
            _bin00[0] = binconstant;
            _bin10[0] = binconstant;
        }

        static void tick_lazy(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 bit)
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

        static void clear(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits)
        {
            u32 const size = (maxbits + binmask) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
            _bin00[0] = 0;
            _bin10[0] = binconstant;
        }

        static void set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 bit)
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

        static void clr(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 bit)
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

        static bool get(bintype const *_bin00, bintype const *_bin10, bintype const *_bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i1 = bit >> binshift;
            u32 const b1 = bit & binmask;
            return D_BIT_TEST(_bin1[i1], b1);
        }

        static s32 find0(bintype const *_bin00, bintype const *_bin1, u32 maxbits)
        {
            if (_bin00[0] == binconstant)
                return -1;
            s32 const     b0  = math::findFirstBit(D_INVERT(_bin00[0]));
            bintype const w1  = D_INVERT(_bin1[b0]);
            s32 const     bit = math::findFirstBit(w1) + (b0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find1(bintype const *_bin10, bintype const *_bin1, u32 maxbits)
        {
            if (_bin10[0] == binconstant)
                return -1;
            s32 const     b0  = math::findFirstBit(D_INVERT(_bin10[0]));
            bintype const w1  = _bin1[b0];
            s32 const     bit = math::findFirstBit(w1) + (b0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find0_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits)
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
            _bin10[0] = D_BIT_CLEAR(_bin10[0], i0);  // clear bit in bin10, tracking '1' bits

            const s32 bit = b1 + (i0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find1_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits)
        {
            if (_bin10[0] == binconstant)
                return -1;

            s32 const i0 = math::findFirstBit(D_INVERT(_bin10[0]));
            ASSERT(_bin1[i0] != 0);
            const s32     b1 = math::findFirstBit(_bin1[i0]);  // at bin1, find a '1' bit
            bintype const v1 = D_BIT_CLEAR(_bin1[i0], b1);     // clear that '1' bit
            _bin1[i0]        = v1;
            _bin00[0]        = D_BIT_CLEAR(_bin00[0], i0);  // clear bit in bin00, tracking '0' bits
            if (v1 == 0)
            {                                          // no more '1' bits at this index in bin1
                _bin10[0] = D_BIT_SET(_bin10[0], i0);  // set bit in bin10, tracking '1' bits
            }

            const s32 bit = b1 + (i0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find0_last(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits)
        {
            if (_bin00[0] == binconstant)
                return -1;

            u8 bi = (u8)math::findLastBit(D_INVERT(_bin00[0]));
            ASSERT(bi >= 0 && bi < binbits);
            u32 wi = bi;
            ASSERT(D_INVERT(_bin1[wi]) != 0);
            bi = math::findLastBit(D_INVERT(_bin1[wi]));
            ASSERT(bi >= 0 && bi < binbits);

            u32 const found_bit = (wi << binshift) + bi;
            return (found_bit < maxbits) ? found_bit : -1;
        }

        static s32 find0_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits)
        {
            s32 const bit = find0_last(_bin00, _bin10, _bin1, maxbits);
            if (bit >= 0)
                set(_bin00, _bin10, _bin1, maxbits, (u32)bit);
            return bit;
        }

        static s32 find0_after(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;

            u32 iw = pivot >> binshift;
            s8  ib = (s8)(pivot & binmask);

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const w = (il == 0 ? D_INVERT(_bin00[0]) : D_INVERT(_bin1[iw])) & (binconstant << ib);
                if (w != 0)
                {
                    iw = (iw << binshift) + math::findFirstBit(w);
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

        static s32 find0_before(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot)
        {
            if (pivot >= maxbits)
                return -1;

            u32 iw = (pivot >> binshift);  // The index of a word in level 0
            u32 ib = (pivot & binmask);    // The bit number in that word

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const w = (il == 0 ? _bin00[0] : D_INVERT(_bin1[iw])) & (binconstant >> (binbits - 1 - ib));
                if (w != 0)
                {
                    iw = (iw << binshift) + (u32)math::findFirstBit(w);
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
        static s32 find1_last(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits)
        {
            if (_bin10[0] == binconstant)
                return -1;

            u8 bi = (u8)math::findLastBit(D_INVERT(_bin10[0]));
            ASSERT(bi >= 0 && bi < binbits);
            u32 wi = bi;
            ASSERT(_bin1[wi] != 0);
            bi = math::findLastBit(_bin1[wi]);
            ASSERT(bi >= 0 && bi < binbits);

            u32 const found_bit = (wi << binshift) + bi;
            return (found_bit < maxbits) ? found_bit : -1;
        }

        static s32 find1_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits)
        {
            s32 const bit = find1_last(_bin00, _bin10, _bin1, maxbits);
            if (bit >= 0)
                set(_bin00, _bin10, _bin1, maxbits, (u32)bit);
            return bit;
        }

        static s32 find1_after(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;

            u32 iw = pivot >> binshift;
            s8  ib = (s8)(pivot & binmask);

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const w = (il == 0 ? D_INVERT(_bin10[0]) : _bin1[iw]) & (binconstant << ib);
                if (w != 0)
                {
                    iw = (iw << binshift) + math::findFirstBit(w);
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

        static s32 find1_before(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot)
        {
            if (pivot >= maxbits)
                return -1;

            u32 iw = (pivot >> binshift);  // The index of a 32-bit word in level 0
            u32 ib = (pivot & binmask);    // The bit number in that 32-bit word

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const w = (il == 0 ? _bin00[0] : D_INVERT(_bin1[iw])) & (binconstant >> (binbits - 1 - ib));
                if (w != 0)
                {
                    iw = (iw << binshift) + (u32)math::findFirstBit(w);
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

    namespace nduomap10
    {
        void setup_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits) { duomap_bin00_bin10_bin1_t<u32, 5>::setup_lazy(bin00, bin10, bin1, maxbits); }
        void tick_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit) { duomap_bin00_bin10_bin1_t<u32, 5>::tick_lazy(bin00, bin10, bin1, maxbits, bit); }

        void clear(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits) { duomap_bin00_bin10_bin1_t<u32, 5>::clear(bin00, bin10, bin1, maxbits); }

        void set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit) { duomap_bin00_bin10_bin1_t<u32, 5>::set(bin00, bin10, bin1, maxbits, bit); }
        void clr(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit) { duomap_bin00_bin10_bin1_t<u32, 5>::clr(bin00, bin10, bin1, maxbits, bit); }
        bool get(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 bit) { return duomap_bin00_bin10_bin1_t<u32, 5>::get(bin00, bin10, bin1, maxbits, bit); }

        s32 find0(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u32, 5>::find0(bin00, bin1, maxbits); }
        s32 find0_and_set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u32, 5>::find0_and_set(bin00, bin10, bin1, maxbits); }

        s32 find1(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u32, 5>::find1(bin10, bin1, maxbits); }
        s32 find1_and_set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u32, 5>::find1_and_set(bin00, bin10, bin1, maxbits); }

        s32 find0_last(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u32, 5>::find0_last(_bin00, _bin10, _bin1, maxbits); }
        s32 find0_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u32, 5>::find0_last_and_set(_bin00, _bin10, _bin1, maxbits); }
        s32 find0_after(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot) { return duomap_bin00_bin10_bin1_t<u32, 5>::find0_after(_bin00, _bin10, _bin1, maxbits, pivot); }
        s32 find0_before(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot) { return duomap_bin00_bin10_bin1_t<u32, 5>::find0_before(_bin00, _bin10, _bin1, maxbits, pivot); }

        s32 find1_last(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u32, 5>::find1_last(_bin00, _bin10, _bin1, maxbits); }
        s32 find1_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u32, 5>::find1_last_and_set(_bin00, _bin10, _bin1, maxbits); }
        s32 find1_after(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot) { return duomap_bin00_bin10_bin1_t<u32, 5>::find1_after(_bin00, _bin10, _bin1, maxbits, pivot); }
        s32 find1_before(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot) { return duomap_bin00_bin10_bin1_t<u32, 5>::find1_before(_bin00, _bin10, _bin1, maxbits, pivot); }
    }  // namespace nduomap10

    namespace nduomap12
    {
        void setup_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits) { duomap_bin00_bin10_bin1_t<u64, 6>::setup_lazy(bin00, bin10, bin1, maxbits); }
        void tick_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit) { duomap_bin00_bin10_bin1_t<u64, 6>::tick_lazy(bin00, bin10, bin1, maxbits, bit); }

        void clear(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits) { duomap_bin00_bin10_bin1_t<u64, 6>::clear(bin00, bin10, bin1, maxbits); }

        void set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit) { duomap_bin00_bin10_bin1_t<u64, 6>::set(bin00, bin10, bin1, maxbits, bit); }
        void clr(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit) { duomap_bin00_bin10_bin1_t<u64, 6>::clr(bin00, bin10, bin1, maxbits, bit); }
        bool get(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 bit) { return duomap_bin00_bin10_bin1_t<u64, 6>::get(bin00, bin10, bin1, maxbits, bit); }

        s32 find0(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u64, 6>::find0(bin00, bin1, maxbits); }
        s32 find0_and_set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u64, 6>::find0_and_set(bin00, bin10, bin1, maxbits); }

        s32 find1(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u64, 6>::find1(bin10, bin1, maxbits); }
        s32 find1_and_set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u64, 6>::find1_and_set(bin00, bin10, bin1, maxbits); }

        s32 find0_last(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u64, 6>::find0_last(_bin00, _bin10, _bin1, maxbits); }
        s32 find0_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u64, 6>::find0_last_and_set(_bin00, _bin10, _bin1, maxbits); }
        s32 find0_after(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot) { return duomap_bin00_bin10_bin1_t<u64, 6>::find0_after(_bin00, _bin10, _bin1, maxbits, pivot); }
        s32 find0_before(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot) { return duomap_bin00_bin10_bin1_t<u64, 6>::find0_before(_bin00, _bin10, _bin1, maxbits, pivot); }

        s32 find1_last(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u64, 6>::find1_last(_bin00, _bin10, _bin1, maxbits); }
        s32 find1_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits) { return duomap_bin00_bin10_bin1_t<u64, 6>::find1_last_and_set(_bin00, _bin10, _bin1, maxbits); }
        s32 find1_after(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot) { return duomap_bin00_bin10_bin1_t<u64, 6>::find1_after(_bin00, _bin10, _bin1, maxbits, pivot); }
        s32 find1_before(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot) { return duomap_bin00_bin10_bin1_t<u64, 6>::find1_before(_bin00, _bin10, _bin1, maxbits, pivot); }
    }  // namespace nduomap12

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // duomaps with three levels

    template <typename bintype, u32 binshift>
    class duomap_bin00_bin01_bin10_bin11_bin2_t
    {
    public:
        static constexpr u32     binbits     = sizeof(bintype) * 8;
        static constexpr u32     binmask     = (sizeof(bintype) * 8) - 1;
        static constexpr bintype binconstant = (bintype) ~(bintype)0;
        static constexpr u32     binlevels   = 3;

        static void setup_lazy(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits)
        {
            _bin00[0] = binconstant;
            _bin10[0] = binconstant;
        }

        static void tick_lazy(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit)
        {
            if (bit >= maxbits)
                return;

            u32       wi      = bit;
            const u32 bi2     = wi & (binbits - 1);
            wi                = wi >> binshift;
            const bintype wd2 = (bi2 == 0) ? binconstant : _bin2[wi];
            _bin2[wi]         = D_BIT_CLEAR(wd2, bi2);
            if (bi2 == 0)
            {
                const u32 bi1     = wi & (binbits - 1);
                wi                = wi >> binshift;
                const bintype wd1 = (bi1 == 0) ? binconstant : _bin01[wi];
                _bin01[wi]        = D_BIT_CLEAR(wd1, bi1);
                if (bi1 == 0)
                {
                    _bin11[wi]        = binconstant;
                    const u32     bi0 = wi & (binbits - 1);
                    const bintype wd0 = (bi0 == 0) ? binconstant : *_bin00;
                    *_bin00           = D_BIT_CLEAR(wd0, bi0);
                }
            }
        }

        static void clear(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits)
        {
            u32 wi = (maxbits + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < wi; ++i)
            {
                _bin2[i] = 0;
            }
            wi = (wi + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < wi; ++i)
            {
                _bin01[i] = 0;
                _bin11[i] = binconstant;
            }
            *_bin00 = 0;
            *_bin10 = binconstant;
        }

        static void set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i2 = bit >> binshift;
            u32 const b2 = bit & (binbits - 1);
            bintype   vo = _bin2[i2];
            bintype   vn = D_BIT_SET(vo, b2);
            _bin2[i2]    = vn;
            if (vn == binconstant)
            {
                // setting a '1' bit results in the word having no more '0' bits
                // so we have to update the upper level bin00/bin01 hierarchy
                // to reflect that there are no more '0' bits at this location
                u32 const     i1 = i2 >> binshift;
                u32 const     b1 = i2 & (binbits - 1);
                bintype const v1 = D_BIT_SET(_bin01[i1], b1);
                _bin01[i1]       = v1;
                if (v1 == binconstant)
                {
                    u32 const     b0 = i1 & (binbits - 1);
                    bintype const v0 = *_bin00;
                    *_bin00          = D_BIT_SET(v0, b0);
                }
            }
            if (vo == 0)
            {
                // setting a '1' bit results in the word having at least one '1' bit
                // so we have to update the upper level bin10/bin11 hierarchy
                // to reflect that there is a one '1' bit at this location
                u32 const     i1 = i2 >> binshift;
                u32 const     b1 = i2 & (binbits - 1);
                bintype const v1 = D_BIT_SET(_bin11[i1], b1);
                _bin11[i1]       = v1;
                if (v1 == binconstant)
                {
                    u32 const     b0 = i1 & (binbits - 1);
                    bintype const v0 = *_bin10;
                    *_bin10          = D_BIT_SET(v0, b0);
                }
            }
        }

        static void clr(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i2 = bit >> binshift;
            u32 const b2 = bit & (binbits - 1);
            bintype   vo = _bin2[i2];
            _bin2[i2]    = D_BIT_CLEAR(vo, b2);
            if (vo == binconstant)
            {
                // clearing a '1' bit results in the word having at least one '0' bit
                // so we have to update the upper level bin00/bin01 hierarchy
                // to reflect that there is a one '0' bit at this location
                u32 const     i1 = i2 >> binshift;
                u32 const     b1 = i2 & (binbits - 1);
                bintype const v1 = _bin01[i1];
                _bin01[i1]       = D_BIT_SET(v1, b1);
                if (v1 == binconstant)
                {
                    u32 const     b0 = i1 & (binbits - 1);
                    bintype const v0 = *_bin00;
                    *_bin00          = D_BIT_SET(v0, b0);
                }
            }
            if (vo == 0)
            {
                // clearing a '1' bit results in the word having no more '1' bits
                // so we have to update the upper level bin10/bin11 hierarchy
                // to reflect that there are no more '1' bits at this location
                u32 const     i1 = i2 >> binshift;
                u32 const     b1 = i2 & (binbits - 1);
                bintype const v1 = _bin11[i1];
                _bin11[i1]       = D_BIT_SET(v1, b1);
                if (v1 == binconstant)
                {
                    u32 const     b0 = i1 & (binbits - 1);
                    bintype const v0 = *_bin10;
                    *_bin10          = D_BIT_SET(v0, b0);
                }
            }
        }

        static bool get(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i2 = bit >> binshift;
            u32 const     b2 = bit & (binbits - 1);
            bintype const v2 = _bin2[i2];
            return D_BIT_TEST(v2, b2);
        }

        static s32 find0(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits)
        {
            if (*_bin00 == binconstant)
                return -1;

            s32 const b0 = math::findFirstBit(D_INVERT(*_bin00));

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(D_INVERT(_bin01[w1]));

            s32 const w2 = b1 + (w1 << binshift);
            s32 const b2 = math::findFirstBit(D_INVERT(_bin2[w2]));

            s32 const bit = b2 + (w2 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find1(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits)
        {
            if (*_bin10 == binconstant)
                return -1;

            s32 const b0 = math::findFirstBit(D_INVERT(*_bin10));

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(D_INVERT(_bin11[w1]));

            s32 const w2 = b1 + (w1 << binshift);
            s32 const b2 = math::findFirstBit(_bin2[w2]);

            s32 const bit = b2 + (w2 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find0_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits)
        {
            s32 const bit = find0(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits);
            if (bit >= 0)
                set(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, (u32)bit);
            return bit;
        }

        static s32 find1_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits)
        {
            s32 const bit = find1(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits);
            if (bit >= 0)
                set(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, (u32)bit);
            return bit;
        }

        static s32 find0_last(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits)
        {
            if (_bin00[0] == binconstant)
                return -1;

            s32 const b0  = math::findLastBit(D_INVERT(_bin00[0]));
            s32 const w1  = b0;
            s32 const b1  = math::findLastBit(D_INVERT(_bin01[w1]));
            s32 const w2  = b1 + (w1 << binshift);
            s32 const b2  = math::findLastBit(D_INVERT(_bin2[w2]));
            s32 const bit = b2 + (w2 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find0_last_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits)
        {
            s32 const bit = find0_last(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits);
            if (bit >= 0)
                set(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, (u32)bit);
            return bit;
        }

        static s32 find0_after(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;

            u32 iw = pivot >> binshift;
            s8  ib = (s8)(pivot & binmask);

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype w;
                switch (il)
                {
                    case 0: w = D_INVERT(_bin00[0]) & (binconstant << ib); break;
                    case 1: w = D_INVERT(_bin01[iw]) & (binconstant << ib); break;
                    case 2: w = D_INVERT(_bin2[iw]) & (binconstant << ib); break;
                    default: ASSERT(false); return -1;
                }

                if (w != 0)
                {
                    iw = (iw << binshift) + math::findFirstBit(w);
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

        static s32 find0_before(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 pivot)
        {
            if (pivot == 0 || pivot > maxbits)
                return -1;

            u32 iw = (pivot - 1) >> binshift;
            s8  ib = (s8)((pivot - 1) & binmask);

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype w;
                switch (il)
                {
                    case 0: w = D_INVERT(_bin00[0]) & ~(binconstant << (ib + 1)); break;
                    case 1: w = D_INVERT(_bin01[iw]) & ~(binconstant << (ib + 1)); break;
                    case 2: w = D_INVERT(_bin2[iw]) & ~(binconstant << (ib + 1)); break;
                    default: ASSERT(false); return -1;
                }

                if (w != 0)
                {
                    iw = (iw << binshift) + math::findLastBit(w);
                    if (il == ml)
                        return iw < maxbits ? (s32)iw : -1;
                    il += 1;  // Go down one level
                    ib = binmask;
                }
                else
                {
                    // move one unit in the direction of upper
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
        static s32 find1_last(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits)
        {
            if (_bin10[0] == binconstant)
                return -1;

            s32 const b0  = math::findLastBit(D_INVERT(_bin10[0]));
            s32 const w1  = b0;
            s32 const b1  = math::findLastBit(D_INVERT(_bin11[w1]));
            s32 const w2  = b1 + (w1 << binshift);
            s32 const b2  = math::findLastBit(_bin2[w2]);
            s32 const bit = b2 + (w2 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find1_last_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits)
        {
            s32 const bit = find1_last(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits);
            if (bit >= 0)
                set(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, (u32)bit);
            return bit;
        }

        static s32 find1_after(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;

            u32 iw = pivot >> binshift;
            s8  ib = (s8)(pivot & binmask);

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype w;
                switch (il)
                {
                    case 0: w = D_INVERT(_bin10[0]) & (binconstant << ib); break;
                    case 1: w = D_INVERT(_bin11[iw]) & (binconstant << ib); break;
                    case 2: w = _bin2[iw] & (binconstant << ib); break;
                    default: ASSERT(false); return -1;
                }

                if (w != 0)
                {
                    iw = (iw << binshift) + math::findFirstBit(w);
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

        static s32 find1_before(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 pivot)
        {
            if (pivot == 0 || pivot > maxbits)
                return -1;

            u32 iw = (pivot - 1) >> binshift;
            s8  ib = (s8)((pivot - 1) & binmask);

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype w;
                switch (il)
                {
                    case 0: w = D_INVERT(_bin10[0]) & ~(binconstant << (ib + 1)); break;
                    case 1: w = D_INVERT(_bin11[iw]) & ~(binconstant << (ib + 1)); break;
                    case 2: w = _bin2[iw] & ~(binconstant << (ib + 1)); break;
                    default: ASSERT(false); return -1;
                }

                if (w != 0)
                {
                    iw = (iw << binshift) + math::findLastBit(w);
                    if (il == ml)
                        return iw < maxbits ? (s32)iw : -1;
                    il += 1;  // Go down one level
                    ib = binmask;
                }
                else
                {
                    // move one unit in the direction of upper
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

    namespace nduomap15
    {
        void setup_lazy(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits) { duomap_bin00_bin01_bin10_bin11_bin2_t<u32, 5>::setup_lazy(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits); }
        void tick_lazy(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit) { duomap_bin00_bin01_bin10_bin11_bin2_t<u32, 5>::tick_lazy(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, bit); }

        void clear(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits) { duomap_bin00_bin01_bin10_bin11_bin2_t<u32, 5>::clear(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits); }

        void set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit) { duomap_bin00_bin01_bin10_bin11_bin2_t<u32, 5>::set(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, bit); }
        void clr(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit) { duomap_bin00_bin01_bin10_bin11_bin2_t<u32, 5>::clr(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, bit); }
        bool get(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 bit)
        {
            return duomap_bin00_bin01_bin10_bin11_bin2_t<u32, 5>::get(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, bit);
        }
        s32 find(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits) { return duomap_bin00_bin01_bin10_bin11_bin2_t<u32, 5>::find0(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits); }
        s32 find_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits) { return duomap_bin00_bin01_bin10_bin11_bin2_t<u32, 5>::find0_and_set(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits); }

        s32 find_last(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits) { return duomap_bin00_bin01_bin10_bin11_bin2_t<u32, 5>::find1_last(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits); }
        s32 find_last_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits) { return duomap_bin00_bin01_bin10_bin11_bin2_t<u32, 5>::find1_last_and_set(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits); }
        s32 find_after(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 pivot) { return duomap_bin00_bin01_bin10_bin11_bin2_t<u32, 5>::find1_after(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, pivot); }
        s32 find_before(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 pivot)
        {
            return duomap_bin00_bin01_bin10_bin11_bin2_t<u32, 5>::find1_before(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, pivot);
        }
    }  // namespace nduomap15

    namespace nduomap18
    {
        void setup_lazy(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits) { duomap_bin00_bin01_bin10_bin11_bin2_t<u64, 6>::setup_lazy(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits); }
        void tick_lazy(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit) { duomap_bin00_bin01_bin10_bin11_bin2_t<u64, 6>::tick_lazy(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, bit); }

        void clear(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits) { duomap_bin00_bin01_bin10_bin11_bin2_t<u64, 6>::clear(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits); }

        void set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit) { duomap_bin00_bin01_bin10_bin11_bin2_t<u64, 6>::set(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, bit); }
        void clr(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit) { duomap_bin00_bin01_bin10_bin11_bin2_t<u64, 6>::clr(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, bit); }
        bool get(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 bit)
        {
            return duomap_bin00_bin01_bin10_bin11_bin2_t<u64, 6>::get(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, bit);
        }
        s32 find(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits) { return duomap_bin00_bin01_bin10_bin11_bin2_t<u64, 6>::find0(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits); }
        s32 find_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits) { return duomap_bin00_bin01_bin10_bin11_bin2_t<u64, 6>::find0_and_set(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits); }

        s32 find_last(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits) { return duomap_bin00_bin01_bin10_bin11_bin2_t<u64, 6>::find1_last(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits); }
        s32 find_last_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits) { return duomap_bin00_bin01_bin10_bin11_bin2_t<u64, 6>::find1_last_and_set(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits); }
        s32 find_after(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 pivot) { return duomap_bin00_bin01_bin10_bin11_bin2_t<u64, 6>::find1_after(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, pivot); }
        s32 find_before(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 pivot)
        {
            return duomap_bin00_bin01_bin10_bin11_bin2_t<u64, 6>::find1_before(_bin00, _bin01, _bin10, _bin11, _bin2, maxbits, pivot);
        }
    }  // namespace nduomap18

};  // namespace ncore

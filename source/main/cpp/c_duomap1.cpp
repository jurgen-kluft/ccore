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
    class duomap_free0_used0_bin1_t
    {
    public:
        static constexpr u32     binbits     = sizeof(bintype) * 8;
        static constexpr u32     binmask     = binbits - 1;
        static constexpr bintype binconstant = (bintype) ~(bintype)0;
        static constexpr u32     binlevels   = 2;

        static inline u32 word_count_for_bits(u32 maxbits)
        {
            ASSERT(maxbits > 0);
            return (maxbits + binbits - 1) >> binshift;
        }

        static bintype valid_words_mask(u32 maxbits)
        {
            u32 const word_count = word_count_for_bits(maxbits);
            if (word_count >= binbits)
                return binconstant;
            return (bintype)(((bintype)1 << word_count) - 1);
        }

        static bintype valid_bits_mask(u32 word_index, u32 maxbits)
        {
            u32 const remaining = maxbits - (word_index << binshift);
            if (remaining >= binbits)
                return binconstant;
            return (bintype)(((bintype)1 << remaining) - 1);
        }

        static inline void update_summary_bits(bintype* _free0, bintype* _used0, bintype const * _bin1, u32 maxbits, u32 word_index)
        {
            bintype const valid_mask = valid_bits_mask(word_index, maxbits);
            bintype const word       = _bin1[word_index] & valid_mask;
            bool const    has_free   = word != valid_mask;
            bool const    has_used   = word != 0;

            _free0[0] = has_free ? D_BIT_SET(_free0[0], word_index) : D_BIT_CLEAR(_free0[0], word_index);
            _used0[0] = has_used ? D_BIT_SET(_used0[0], word_index) : D_BIT_CLEAR(_used0[0], word_index);
        }

        static inline void update_summary_bits_verify(bintype* _free0, bintype* _used0, bintype const * _bin1, u32 maxbits, u32 word_index)
        {
            if ((word_index << binshift) < maxbits)
                update_summary_bits(_free0, _used0, _bin1, maxbits, word_index);
        }

        static void setup_used_lazy(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits)
        {
            _free0[0] = 0;
            _used0[0] = 0;
            _bin1[0]  = 0;
        }

        static void tick_used_lazy(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits, u32 bit)
        {
            if (bit >= maxbits)
                return;

            if ((bit & binmask) == 0)
            {
                u32 const wi = bit >> binshift;
                _bin1[wi]    = 0;
                update_summary_bits(_free0, _used0, _bin1, maxbits, wi);
            }
        }

        static void clear_all_free(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits)
        {
            u32 const size = word_count_for_bits(maxbits);
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
            _free0[0] = valid_words_mask(maxbits);
            _used0[0] = 0;
        }

        static void clear_all_used(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits)
        {
            const u32 size = word_count_for_bits(maxbits);
            if (size > 0)
            {
                u32 const last = size - 1;
                _bin1[last]    = valid_bits_mask(last, maxbits);
                for (u32 i = 0; i < last; ++i)
                    _bin1[i] = binconstant;
            }
            _free0[0] = 0;
            _used0[0] = valid_words_mask(maxbits);
        }

        static inline void internal_set_used(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits, u32 bit)
        {
            u32 const i1 = bit >> binshift;
            u32 const b1 = bit & binmask;
            _bin1[i1]    = D_BIT_SET(_bin1[i1], b1);
            update_summary_bits(_free0, _used0, _bin1, maxbits, i1);
        }

        static inline void internal_set_free(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits, u32 bit)
        {
            u32 const i1 = bit >> binshift;
            u32 const b1 = bit & binmask;
            _bin1[i1]    = D_BIT_CLEAR(_bin1[i1], b1);
            update_summary_bits(_free0, _used0, _bin1, maxbits, i1);
        }

        static void set_used(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 const     i1 = bit >> binshift;
                u32 const     b1 = bit & binmask;
                bintype const vo = _bin1[i1];
                bintype const vn = D_BIT_SET(vo, b1);
                if (vo != vn)
                {
                    _bin1[i1] = vn;
                    update_summary_bits(_free0, _used0, _bin1, maxbits, i1);
                }
            }
        }

        static void set_free(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 const     i1 = bit >> binshift;
                u32 const     b1 = bit & binmask;
                bintype const vo = _bin1[i1];
                bintype const vn = D_BIT_CLEAR(vo, b1);
                if (vo != vn)
                {
                    _bin1[i1] = vn;
                    update_summary_bits(_free0, _used0, _bin1, maxbits, i1);
                }
            }
        }

        static bool get(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i1 = bit >> binshift;
            u32 const b1 = bit & binmask;
            return D_BIT_TEST(_bin1[i1], b1);
        }

        static s32 find_free(bintype const * _free0, bintype const * _bin1, u32 maxbits)
        {
            bintype const free_words = _free0[0];
            if (free_words == 0)
                return -1;

            s32 const     b0  = math::findFirstBit(free_words);
            bintype const w1  = D_INVERT(_bin1[b0]);
            s32 const     bit = math::findFirstBit(w1) + (b0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_used(bintype const * _used0, bintype const * _bin1, u32 maxbits)
        {
            bintype const used_words = _used0[0];
            if (used_words == 0)
                return -1;

            s32 const     b0  = math::findFirstBit(used_words);
            bintype const w1  = _bin1[b0];
            s32 const     bit = math::findFirstBit(w1) + (b0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 alloc(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits)
        {
            s32 const bit = find_free(_free0, _bin1, maxbits);
            if (bit >= 0)
                internal_set_used(_free0, _used0, _bin1, maxbits, (u32)bit);
            return bit;
        }

        static s32 free(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits)
        {
            s32 const bit = find_used(_used0, _bin1, maxbits);
            if (bit >= 0)
                internal_set_free(_free0, _used0, _bin1, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_free_last(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits)
        {
            bintype const free_words = _free0[0];
            if (free_words == 0)
                return -1;

            u8 bi = (u8)math::findLastBit(free_words);
            ASSERT(bi >= 0 && bi < binbits);
            u32           wi = bi;
            bintype const w1 = D_INVERT(_bin1[wi]);
            ASSERT(w1 != 0);
            bi = (u8)math::findLastBit(w1);
            ASSERT(bi >= 0 && bi < binbits);

            u32 const found_bit = (wi << binshift) + bi;
            return (found_bit < maxbits) ? found_bit : -1;
        }

        static s32 alloc_last(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits)
        {
            s32 const bit = find_free_last(_free0, _used0, _bin1, maxbits);
            if (bit >= 0)
                internal_set_used(_free0, _used0, _bin1, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_free_after(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in _bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;

            if ((pivot + 1) >= maxbits)
                return -1;

            u32 iw = (pivot + 1) >> binshift;
            s8  ib = (s8)((pivot + 1) & binmask);

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const w = (il == 0 ? _free0[0] : D_INVERT(_bin1[iw])) & (binconstant << ib);
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

        static s32 find_free_before(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 pivot)
        {
            if (pivot == 0 || pivot > maxbits)
                return -1;

            u32 iw = (pivot - 1) >> binshift;  // The index of a word in level 0
            u32 ib = (pivot - 1) & binmask;    // The bit number in that word

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const w = (il == 0 ? _free0[0] : D_INVERT(_bin1[iw])) & (binconstant >> (binbits - 1 - ib));
                if (w != 0)
                {
                    iw = (iw << binshift) + (u32)math::findLastBit(w);
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
        static s32 find_used_last(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits)
        {
            bintype const used_words = _used0[0];
            if (used_words == 0)
                return -1;

            u8 bi = (u8)math::findLastBit(used_words);
            ASSERT(bi >= 0 && bi < binbits);
            u32           wi = bi;
            bintype const w1 = _bin1[wi];
            ASSERT(w1 != 0);
            bi = (u8)math::findLastBit(w1);
            ASSERT(bi >= 0 && bi < binbits);

            u32 const found_bit = (wi << binshift) + bi;
            return (found_bit < maxbits) ? found_bit : -1;
        }

        static s32 free_last(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits)
        {
            s32 const bit = find_used_last(_free0, _used0, _bin1, maxbits);
            if (bit >= 0)
                internal_set_free(_free0, _used0, _bin1, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_used_after(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in _bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;

            if ((pivot + 1) >= maxbits)
                return -1;

            u32 iw = (pivot + 1) >> binshift;
            s8  ib = (s8)((pivot + 1) & binmask);

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const w = (il == 0 ? _used0[0] : _bin1[iw]) & (binconstant << ib);
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

        static s32 find_used_before(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 pivot)
        {
            if (pivot == 0 || pivot > maxbits)
                return -1;

            u32 iw = (pivot - 1) >> binshift;  // The index of a 32-bit word in level 0
            u32 ib = (pivot - 1) & binmask;    // The bit number in that 32-bit word

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype const w = (il == 0 ? _used0[0] : _bin1[iw]) & (binconstant >> (binbits - 1 - ib));
                if (w != 0)
                {
                    iw = (iw << binshift) + (u32)math::findLastBit(w);
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
        void setup_used_lazy(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { duomap_free0_used0_bin1_t<u32, 5>::setup_used_lazy(_free0, _used0, _bin1, maxbits); }
        void tick_used_lazy(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits, u32 bit) { duomap_free0_used0_bin1_t<u32, 5>::tick_used_lazy(_free0, _used0, _bin1, maxbits, bit); }

        void clear_all_free(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { duomap_free0_used0_bin1_t<u32, 5>::clear_all_free(_free0, _used0, _bin1, maxbits); }
        void clear_all_used(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { duomap_free0_used0_bin1_t<u32, 5>::clear_all_used(_free0, _used0, _bin1, maxbits); }

        void set_used(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits, u32 bit) { duomap_free0_used0_bin1_t<u32, 5>::set_used(_free0, _used0, _bin1, maxbits, bit); }
        void set_free(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits, u32 bit) { duomap_free0_used0_bin1_t<u32, 5>::set_free(_free0, _used0, _bin1, maxbits, bit); }
        bool get(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 bit) { return duomap_free0_used0_bin1_t<u32, 5>::get(_free0, _used0, _bin1, maxbits, bit); }

        s32 find_free(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u32, 5>::find_free(_free0, _bin1, maxbits); }
        s32 find_used(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u32, 5>::find_used(_used0, _bin1, maxbits); }
        s32 find_free_last(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u32, 5>::find_free_last(_free0, _used0, _bin1, maxbits); }
        s32 find_free_after(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 pivot) { return duomap_free0_used0_bin1_t<u32, 5>::find_free_after(_free0, _used0, _bin1, maxbits, pivot); }
        s32 find_free_before(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 pivot) { return duomap_free0_used0_bin1_t<u32, 5>::find_free_before(_free0, _used0, _bin1, maxbits, pivot); }
        s32 find_used_last(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u32, 5>::find_used_last(_free0, _used0, _bin1, maxbits); }
        s32 find_used_after(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 pivot) { return duomap_free0_used0_bin1_t<u32, 5>::find_used_after(_free0, _used0, _bin1, maxbits, pivot); }
        s32 find_used_before(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 pivot) { return duomap_free0_used0_bin1_t<u32, 5>::find_used_before(_free0, _used0, _bin1, maxbits, pivot); }

        s32 alloc(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u32, 5>::alloc(_free0, _used0, _bin1, maxbits); }
        s32 free(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u32, 5>::free(_free0, _used0, _bin1, maxbits); }
        s32 alloc_last(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u32, 5>::alloc_last(_free0, _used0, _bin1, maxbits); }
        s32 free_last(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u32, 5>::free_last(_free0, _used0, _bin1, maxbits); }
    }  // namespace nduomap10

    namespace nduomap12
    {
        void setup_used_lazy(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { duomap_free0_used0_bin1_t<u64, 6>::setup_used_lazy(_free0, _used0, _bin1, maxbits); }
        void tick_used_lazy(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits, u32 bit) { duomap_free0_used0_bin1_t<u64, 6>::tick_used_lazy(_free0, _used0, _bin1, maxbits, bit); }

        void clear_all_free(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { duomap_free0_used0_bin1_t<u64, 6>::clear_all_free(_free0, _used0, _bin1, maxbits); }
        void clear_all_used(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { duomap_free0_used0_bin1_t<u64, 6>::clear_all_used(_free0, _used0, _bin1, maxbits); }

        void set_used(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits, u32 bit) { duomap_free0_used0_bin1_t<u64, 6>::set_used(_free0, _used0, _bin1, maxbits, bit); }
        void set_free(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits, u32 bit) { duomap_free0_used0_bin1_t<u64, 6>::set_free(_free0, _used0, _bin1, maxbits, bit); }
        bool get(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 bit) { return duomap_free0_used0_bin1_t<u64, 6>::get(_free0, _used0, _bin1, maxbits, bit); }

        s32 find_free(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u64, 6>::find_free(_free0, _bin1, maxbits); }
        s32 find_used(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u64, 6>::find_used(_used0, _bin1, maxbits); }
        s32 find_free_last(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u64, 6>::find_free_last(_free0, _used0, _bin1, maxbits); }
        s32 find_free_after(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 pivot) { return duomap_free0_used0_bin1_t<u64, 6>::find_free_after(_free0, _used0, _bin1, maxbits, pivot); }
        s32 find_free_before(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 pivot) { return duomap_free0_used0_bin1_t<u64, 6>::find_free_before(_free0, _used0, _bin1, maxbits, pivot); }
        s32 find_used_last(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u64, 6>::find_used_last(_free0, _used0, _bin1, maxbits); }
        s32 find_used_after(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 pivot) { return duomap_free0_used0_bin1_t<u64, 6>::find_used_after(_free0, _used0, _bin1, maxbits, pivot); }
        s32 find_used_before(bintype const * _free0, bintype const * _used0, bintype const * _bin1, u32 maxbits, u32 pivot) { return duomap_free0_used0_bin1_t<u64, 6>::find_used_before(_free0, _used0, _bin1, maxbits, pivot); }

        s32 alloc(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u64, 6>::alloc(_free0, _used0, _bin1, maxbits); }
        s32 free(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u64, 6>::free(_free0, _used0, _bin1, maxbits); }
        s32 alloc_last(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u64, 6>::alloc_last(_free0, _used0, _bin1, maxbits); }
        s32 free_last(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits) { return duomap_free0_used0_bin1_t<u64, 6>::free_last(_free0, _used0, _bin1, maxbits); }
    }  // namespace nduomap12

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // duomaps with three levels

    template <typename bintype, u32 binshift>
    class duomap_free0_free1_used0_used1_bin2_t
    {
    public:
        static constexpr u32     binbits     = sizeof(bintype) * 8;
        static constexpr u32     binmask     = (sizeof(bintype) * 8) - 1;
        static constexpr bintype binconstant = (bintype) ~(bintype)0;
        static constexpr u32     binlevels   = 3;

        static inline u32 word_count_for_bits(u32 maxbits)
        {
            ASSERT(maxbits > 0);
            return (maxbits + binbits - 1) >> binshift;
        }

        static inline u32 level1_word_count(u32 maxbits)
        {
            u32 const level2_words = word_count_for_bits(maxbits);
            return (level2_words + binbits - 1) >> binshift;
        }

        static inline bintype mask_for_count(u32 count)
        {
            if (count >= binbits)
                return binconstant;
            return (bintype)(((bintype)1 << count) - 1);
        }

        static inline bintype valid_level0_mask(u32 maxbits) { return mask_for_count(level1_word_count(maxbits)); }

        static inline bintype valid_level1_mask(u32 word_index, u32 maxbits)
        {
            u32 const level2_words = word_count_for_bits(maxbits);
            u32 const base_index   = word_index << binshift;
            if (base_index >= level2_words)
                return 0;
            return mask_for_count(level2_words - base_index);
        }

        static inline bintype valid_level2_mask(u32 word_index, u32 maxbits)
        {
            u32 const base_bit = word_index << binshift;
            if (base_bit >= maxbits)
                return 0;
            return mask_for_count(maxbits - base_bit);
        }

        static inline void update_summary_bits(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype const * _bin2, u32 maxbits, u32 word_index)
        {
            u32 const     i1         = word_index >> binshift;
            u32 const     b1         = word_index & binmask;
            bintype const valid2     = valid_level2_mask(word_index, maxbits);
            bintype const word2      = _bin2[word_index] & valid2;
            bool const    has_free2  = word2 != valid2;
            bool const    has_used2  = word2 != 0;
            bintype const free1_prev = _free1[i1];
            bintype const used1_prev = _used1[i1];

            _free1[i1] = has_free2 ? D_BIT_SET(_free1[i1], b1) : D_BIT_CLEAR(_free1[i1], b1);
            _used1[i1] = has_used2 ? D_BIT_SET(_used1[i1], b1) : D_BIT_CLEAR(_used1[i1], b1);

            bintype const free1_word = _free1[i1];
            bintype const used1_word = _used1[i1];
            bool const    had_free1  = free1_prev != 0;
            bool const    had_used1  = used1_prev != 0;
            bool const    has_free1  = free1_word != 0;
            bool const    has_used1  = used1_word != 0;

            if (had_free1 != has_free1)
                _free0[0] = has_free1 ? D_BIT_SET(_free0[0], i1) : D_BIT_CLEAR(_free0[0], i1);
            if (had_used1 != has_used1)
                _used0[0] = has_used1 ? D_BIT_SET(_used0[0], i1) : D_BIT_CLEAR(_used0[0], i1);
        }

        static inline void update_summary_bits_verify(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype const * _bin2, u32 maxbits, u32 word_index)
        {
            if ((word_index << binshift) < maxbits)
                update_summary_bits(_free0, _free1, _used0, _used1, _bin2, maxbits, word_index);
        }

        static void setup_used_lazy(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits)
        {
            _free0[0] = 0;
            _used0[0] = 0;
        }

        static void tick_used_lazy(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit)
        {
            if (bit >= maxbits)
                return;

            u32 const bi2 = bit & binmask;
            if (bi2 == 0)
            {
                u32 const wi = bit >> binshift;
                _bin2[wi]    = binconstant;

                u32 const i1 = wi >> binshift;
                u32 const b1 = wi & binmask;
                if (b1 == 0)
                {
                    _free1[i1] = 0;
                    _used1[i1] = 0;
                    _used0[0]  = D_BIT_SET(_used0[0], i1);
                }
                _used1[i1] = D_BIT_SET(_used1[i1], b1);
            }
        }

        static void clear_all_free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits)
        {
            u32 const level2_words = word_count_for_bits(maxbits);
            for (u32 i = 0; i < level2_words; ++i)
            {
                _bin2[i] = 0;
            }

            u32 const level1_words = level1_word_count(maxbits);
            for (u32 i = 0; i < level1_words; ++i)
            {
                _free1[i] = valid_level1_mask(i, maxbits);
                _used1[i] = 0;
            }

            *_free0 = valid_level0_mask(maxbits);
            *_used0 = 0;
        }

        static void set_used(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i2 = bit >> binshift;
            u32 const     b2 = bit & (binbits - 1);
            bintype const vo = _bin2[i2];
            bintype const vn = D_BIT_SET(vo, b2);
            if (vo != vn)
            {
                _bin2[i2] = vn;
                update_summary_bits(_free0, _free1, _used0, _used1, _bin2, maxbits, i2);
            }
        }

        static void set_free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i2 = bit >> binshift;
            u32 const     b2 = bit & (binbits - 1);
            bintype const vo = _bin2[i2];
            bintype const vn = D_BIT_CLEAR(vo, b2);
            if (vo != vn)
            {
                _bin2[i2] = vn;
                update_summary_bits(_free0, _free1, _used0, _used1, _bin2, maxbits, i2);
            }
        }

        static bool get(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i2 = bit >> binshift;
            u32 const     b2 = bit & (binbits - 1);
            bintype const v2 = _bin2[i2];
            return D_BIT_TEST(v2, b2);
        }

        static s32 find_free(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits)
        {
            bintype const free0_word = *_free0;
            if (free0_word == 0)
                return -1;

            s32 const b0 = math::findFirstBit(free0_word);

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(_free1[w1]);

            s32 const w2 = b1 + (w1 << binshift);
            s32 const b2 = math::findFirstBit(D_INVERT(_bin2[w2]) & valid_level2_mask((u32)w2, maxbits));

            s32 const bit = b2 + (w2 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_used(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits)
        {
            bintype const used0_word = *_used0;
            if (used0_word == 0)
                return -1;

            s32 const b0 = math::findFirstBit(used0_word);

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(_used1[w1]);

            s32 const w2 = b1 + (w1 << binshift);
            s32 const b2 = math::findFirstBit(_bin2[w2] & valid_level2_mask((u32)w2, maxbits));

            s32 const bit = b2 + (w2 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 alloc(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits)
        {
            s32 const bit = find_free(_free0, _free1, _used0, _used1, _bin2, maxbits);
            if (bit >= 0)
                set_used(_free0, _free1, _used0, _used1, _bin2, maxbits, (u32)bit);
            return bit;
        }

        static s32 free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits)
        {
            s32 const bit = find_used(_free0, _free1, _used0, _used1, _bin2, maxbits);
            if (bit >= 0)
                set_free(_free0, _free1, _used0, _used1, _bin2, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_free_last(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits)
        {
            bintype const free0_word = _free0[0];
            if (free0_word == 0)
                return -1;

            s32 const b0  = math::findLastBit(free0_word);
            s32 const w1  = b0;
            s32 const b1  = math::findLastBit(_free1[w1]);
            s32 const w2  = b1 + (w1 << binshift);
            s32 const b2  = math::findLastBit(D_INVERT(_bin2[w2]) & valid_level2_mask((u32)w2, maxbits));
            s32 const bit = b2 + (w2 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 alloc_last(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits)
        {
            s32 const bit = find_free_last(_free0, _free1, _used0, _used1, _bin2, maxbits);
            if (bit >= 0)
                set_used(_free0, _free1, _used0, _used1, _bin2, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_free_after(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in _bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;

            if ((pivot + 1) >= maxbits)
                return -1;

            u32 iw = (pivot + 1) >> binshift;
            s8  ib = (s8)((pivot + 1) & binmask);

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype w;
                switch (il)
                {
                    case 0: w = _free0[0] & (binconstant << ib); break;
                    case 1: w = _free1[iw] & (binconstant << ib); break;
                    case 2: w = (D_INVERT(_bin2[iw]) & valid_level2_mask(iw, maxbits)) & (binconstant << ib); break;
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

        static s32 find_free_before(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot)
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
                    case 0: w = _free0[0] & ~(binconstant << (ib + 1)); break;
                    case 1: w = _free1[iw] & ~(binconstant << (ib + 1)); break;
                    case 2: w = (D_INVERT(_bin2[iw]) & valid_level2_mask(iw, maxbits)) & ~(binconstant << (ib + 1)); break;
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
        static s32 find_used_last(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits)
        {
            bintype const used0_word = _used0[0];
            if (used0_word == 0)
                return -1;

            s32 const b0  = math::findLastBit(used0_word);
            s32 const w1  = b0;
            s32 const b1  = math::findLastBit(_used1[w1]);
            s32 const w2  = b1 + (w1 << binshift);
            s32 const b2  = math::findLastBit(_bin2[w2] & valid_level2_mask((u32)w2, maxbits));
            s32 const bit = b2 + (w2 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 free_last(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits)
        {
            s32 const bit = find_used_last(_free0, _free1, _used0, _used1, _bin2, maxbits);
            if (bit >= 0)
                set_free(_free0, _free1, _used0, _used1, _bin2, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_used_after(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in _bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;

            if ((pivot + 1) >= maxbits)
                return -1;

            u32 iw = (pivot + 1) >> binshift;
            s8  ib = (s8)((pivot + 1) & binmask);

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype w;
                switch (il)
                {
                    case 0: w = _used0[0] & (binconstant << ib); break;
                    case 1: w = _used1[iw] & (binconstant << ib); break;
                    case 2: w = (_bin2[iw] & valid_level2_mask(iw, maxbits)) & (binconstant << ib); break;
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

        static s32 find_used_before(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot)
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
                    case 0: w = _used0[0] & ~(binconstant << (ib + 1)); break;
                    case 1: w = _used1[iw] & ~(binconstant << (ib + 1)); break;
                    case 2: w = (_bin2[iw] & valid_level2_mask(iw, maxbits)) & ~(binconstant << (ib + 1)); break;
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
        void setup_used_lazy(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits) { duomap_free0_free1_used0_used1_bin2_t<u32, 5>::setup_used_lazy(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        void tick_used_lazy(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit) { duomap_free0_free1_used0_used1_bin2_t<u32, 5>::tick_used_lazy(_free0, _free1, _used0, _used1, _bin2, maxbits, bit); }

        void clear_all_free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits) { duomap_free0_free1_used0_used1_bin2_t<u32, 5>::clear_all_free(_free0, _free1, _used0, _used1, _bin2, maxbits); }

        void set_used(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit) { duomap_free0_free1_used0_used1_bin2_t<u32, 5>::set_used(_free0, _free1, _used0, _used1, _bin2, maxbits, bit); }
        void set_free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit) { duomap_free0_free1_used0_used1_bin2_t<u32, 5>::set_free(_free0, _free1, _used0, _used1, _bin2, maxbits, bit); }
        bool get(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 bit)
        { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::get(_free0, _free1, _used0, _used1, _bin2, maxbits, bit); }

        s32 find_free(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits)
        { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::find_free(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 find_used(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits)
        { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::find_used(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 find_free_last(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits)
        { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::find_free_last(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 find_free_after(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::find_free_after(_free0, _free1, _used0, _used1, _bin2, maxbits, pivot); }
        s32 find_free_before(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::find_free_before(_free0, _free1, _used0, _used1, _bin2, maxbits, pivot); }
        s32 find_used_last(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits)
        { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::find_used_last(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 find_used_after(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::find_used_after(_free0, _free1, _used0, _used1, _bin2, maxbits, pivot); }
        s32 find_used_before(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::find_used_before(_free0, _free1, _used0, _used1, _bin2, maxbits, pivot); }

        s32 alloc(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits) { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::alloc(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits) { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::free(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 alloc_last(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits) { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::alloc_last(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 free_last(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits) { return duomap_free0_free1_used0_used1_bin2_t<u32, 5>::free_last(_free0, _free1, _used0, _used1, _bin2, maxbits); }
    }  // namespace nduomap15

    namespace nduomap18
    {
        void setup_used_lazy(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits) { duomap_free0_free1_used0_used1_bin2_t<u64, 6>::setup_used_lazy(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        void tick_used_lazy(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit) { duomap_free0_free1_used0_used1_bin2_t<u64, 6>::tick_used_lazy(_free0, _free1, _used0, _used1, _bin2, maxbits, bit); }

        void clear_all_free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits) { duomap_free0_free1_used0_used1_bin2_t<u64, 6>::clear_all_free(_free0, _free1, _used0, _used1, _bin2, maxbits); }

        void set_used(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit) { duomap_free0_free1_used0_used1_bin2_t<u64, 6>::set_used(_free0, _free1, _used0, _used1, _bin2, maxbits, bit); }
        void set_free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit) { duomap_free0_free1_used0_used1_bin2_t<u64, 6>::set_free(_free0, _free1, _used0, _used1, _bin2, maxbits, bit); }
        bool get(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 bit)
        { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::get(_free0, _free1, _used0, _used1, _bin2, maxbits, bit); }

        s32 find_free(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits)
        { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::find_free(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 find_used(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits)
        { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::find_used(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 find_free_last(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits)
        { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::find_free_last(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 find_free_after(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::find_free_after(_free0, _free1, _used0, _used1, _bin2, maxbits, pivot); }
        s32 find_free_before(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::find_free_before(_free0, _free1, _used0, _used1, _bin2, maxbits, pivot); }
        s32 find_used_last(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits)
        { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::find_used_last(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 find_used_after(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::find_used_after(_free0, _free1, _used0, _used1, _bin2, maxbits, pivot); }
        s32 find_used_before(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::find_used_before(_free0, _free1, _used0, _used1, _bin2, maxbits, pivot); }

        s32 alloc(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits) { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::alloc(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits) { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::free(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 alloc_last(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits) { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::alloc_last(_free0, _free1, _used0, _used1, _bin2, maxbits); }
        s32 free_last(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits) { return duomap_free0_free1_used0_used1_bin2_t<u64, 6>::free_last(_free0, _free1, _used0, _used1, _bin2, maxbits); }
    }  // namespace nduomap18

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // duomaps with four levels

    template <typename bintype, u32 binshift>
    class duomap_free0_free1_free2_used0_used1_used2_bin3_t
    {
    public:
        static constexpr u32     binbits     = sizeof(bintype) * 8;
        static constexpr u32     binmask     = (sizeof(bintype) * 8) - 1;
        static constexpr bintype binconstant = (bintype) ~(bintype)0;
        static constexpr u32     binlevels   = 4;

        static inline u32 word_count_for_bits(u32 maxbits)
        {
            ASSERT(maxbits > 0);
            return (maxbits + binbits - 1) >> binshift;
        }

        static inline u32 level2_word_count(u32 maxbits)
        {
            u32 const level3_words = word_count_for_bits(maxbits);
            return (level3_words + binbits - 1) >> binshift;
        }

        static inline u32 level1_word_count(u32 maxbits)
        {
            u32 const level2_words = level2_word_count(maxbits);
            return (level2_words + binbits - 1) >> binshift;
        }

        static inline bintype mask_for_count(u32 count)
        {
            if (count >= binbits)
                return binconstant;
            return (bintype)(((bintype)1 << count) - 1);
        }

        static inline bintype valid_level0_mask(u32 maxbits) { return mask_for_count(level1_word_count(maxbits)); }

        static inline bintype valid_level1_mask(u32 word_index, u32 maxbits)
        {
            u32 const level2_words = level2_word_count(maxbits);
            u32 const base_index   = word_index << binshift;
            if (base_index >= level2_words)
                return 0;
            return mask_for_count(level2_words - base_index);
        }

        static inline bintype valid_level2_mask(u32 word_index, u32 maxbits)
        {
            u32 const level3_words = word_count_for_bits(maxbits);
            u32 const base_index   = word_index << binshift;
            if (base_index >= level3_words)
                return 0;
            return mask_for_count(level3_words - base_index);
        }

        static inline bintype valid_level3_mask(u32 word_index, u32 maxbits)
        {
            u32 const base_bit = word_index << binshift;
            if (base_bit >= maxbits)
                return 0;
            return mask_for_count(maxbits - base_bit);
        }

        static inline void update_summary_bits(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype const * _bin3, u32 maxbits, u32 word_index)
        {
            u32 const     i2         = word_index >> binshift;
            u32 const     b2         = word_index & binmask;
            bintype const valid3     = valid_level3_mask(word_index, maxbits);
            bintype const word3      = _bin3[word_index] & valid3;
            bool const    has_free3  = word3 != valid3;
            bool const    has_used3  = word3 != 0;
            bintype const free2_prev = _free2[i2];
            bintype const used2_prev = _used2[i2];

            _free2[i2] = has_free3 ? D_BIT_SET(_free2[i2], b2) : D_BIT_CLEAR(_free2[i2], b2);
            _used2[i2] = has_used3 ? D_BIT_SET(_used2[i2], b2) : D_BIT_CLEAR(_used2[i2], b2);

            bintype const free2_word = _free2[i2];
            bintype const used2_word = _used2[i2];
            bool const    had_free2  = free2_prev != 0;
            bool const    had_used2  = used2_prev != 0;
            bool const    has_free2  = free2_word != 0;
            bool const    has_used2  = used2_word != 0;

            if (had_free2 != has_free2 || had_used2 != has_used2)
            {
                u32 const     i1         = i2 >> binshift;
                u32 const     b1         = i2 & binmask;
                bintype const free1_prev = _free1[i1];
                bintype const used1_prev = _used1[i1];

                _free1[i1] = has_free2 ? D_BIT_SET(_free1[i1], b1) : D_BIT_CLEAR(_free1[i1], b1);
                _used1[i1] = has_used2 ? D_BIT_SET(_used1[i1], b1) : D_BIT_CLEAR(_used1[i1], b1);

                bintype const free1_word = _free1[i1];
                bintype const used1_word = _used1[i1];
                bool const    had_free1  = free1_prev != 0;
                bool const    had_used1  = used1_prev != 0;
                bool const    has_free1  = free1_word != 0;
                bool const    has_used1  = used1_word != 0;

                if (had_free1 != has_free1)
                    _free0[0] = has_free1 ? D_BIT_SET(_free0[0], i1) : D_BIT_CLEAR(_free0[0], i1);
                if (had_used1 != has_used1)
                    _used0[0] = has_used1 ? D_BIT_SET(_used0[0], i1) : D_BIT_CLEAR(_used0[0], i1);
            }
        }

        static inline void update_summary_bits_verify(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype const * _bin3, u32 maxbits, u32 word_index)
        {
            if ((word_index << binshift) < maxbits)
                update_summary_bits(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, word_index);
        }

        static void setup_used_lazy(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        {
            _free0[0] = 0;
            _used0[0] = 0;
        }

        static void tick_used_lazy(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit)
        {
            if (bit >= maxbits)
                return;

            u32 const b3 = bit & binmask;
            if (b3 == 0)
            {
                u32 const w3 = bit >> binshift;
                _bin3[w3]    = binconstant;

                u32 const i2 = w3 >> binshift;
                u32 const b2 = w3 & binmask;
                if (b2 == 0)
                {
                    _free2[i2] = 0;
                    _used2[i2] = 0;

                    u32 const i1 = i2 >> binshift;
                    u32 const b1 = i2 & binmask;
                    if (b1 == 0)
                    {
                        _free1[i1] = 0;
                        _used1[i1] = 0;
                        _used0[0]  = D_BIT_SET(_used0[0], i1);
                    }
                    _used1[i1] = D_BIT_SET(_used1[i1], b1);
                }
                _used2[i2] = D_BIT_SET(_used2[i2], b2);
            }
        }

        static void clear_all_free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        {
            u32 const level3_words = word_count_for_bits(maxbits);
            for (u32 i = 0; i < level3_words; ++i)
            {
                _bin3[i] = 0;
            }

            u32 const level2_words = level2_word_count(maxbits);
            for (u32 i = 0; i < level2_words; ++i)
            {
                _free2[i] = valid_level2_mask(i, maxbits);
                _used2[i] = 0;
            }

            u32 const level1_words = level1_word_count(maxbits);
            for (u32 i = 0; i < level1_words; ++i)
            {
                _free1[i] = valid_level1_mask(i, maxbits);
                _used1[i] = 0;
            }

            *_free0 = valid_level0_mask(maxbits);
            *_used0 = 0;
        }

        static void clear_all_used(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        {
            u32 const level3_words = word_count_for_bits(maxbits);
            if (level3_words > 0)
            {
                u32 const last = level3_words - 1;
                _bin3[last]    = valid_level3_mask(last, maxbits);
                for (u32 i = 0; i < last; ++i)
                    _bin3[i] = binconstant;
            }

            u32 const level2_words = level2_word_count(maxbits);
            for (u32 i = 0; i < level2_words; ++i)
            {
                _free2[i] = 0;
                _used2[i] = valid_level2_mask(i, maxbits);
            }

            u32 const level1_words = level1_word_count(maxbits);
            for (u32 i = 0; i < level1_words; ++i)
            {
                _free1[i] = 0;
                _used1[i] = valid_level1_mask(i, maxbits);
            }

            *_free0 = 0;
            *_used0 = valid_level0_mask(maxbits);
        }

        static void set_used(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i3 = bit >> binshift;
            u32 const     b3 = bit & (binbits - 1);
            bintype const vo = _bin3[i3];
            bintype const vn = D_BIT_SET(vo, b3);
            if (vo != vn)
            {
                _bin3[i3] = vn;
                update_summary_bits_verify(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, i3);
            }
        }

        static void set_free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i3 = bit >> binshift;
            u32 const     b3 = bit & (binbits - 1);
            bintype const vo = _bin3[i3];
            bintype const vn = D_BIT_CLEAR(vo, b3);
            if (vo != vn)
            {
                _bin3[i3] = vn;
                update_summary_bits_verify(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, i3);
            }
        }

        static bool get(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i3 = bit >> binshift;
            u32 const     b3 = bit & (binbits - 1);
            bintype const v3 = _bin3[i3];
            return D_BIT_TEST(v3, b3);
        }

        static s32 find_free(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits)
        {
            bintype const free0_word = *_free0;
            if (free0_word == 0)
                return -1;

            s32 const b0 = math::findFirstBit(free0_word);

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(_free1[w1]);

            s32 const w2 = b1 + (w1 << binshift);
            s32 const b2 = math::findFirstBit(_free2[w2]);

            s32 const w3 = b2 + (w2 << binshift);
            s32 const b3 = math::findFirstBit(D_INVERT(_bin3[w3]) & valid_level3_mask((u32)w3, maxbits));

            s32 const bit = b3 + (w3 << binshift);

            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_used(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits)
        {
            bintype const used0_word = *_used0;
            if (used0_word == 0)
                return -1;

            s32 const b0 = math::findFirstBit(used0_word);

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(_used1[w1]);

            s32 const w2 = b1 + (w1 << binshift);
            s32 const b2 = math::findFirstBit(_used2[w2]);

            s32 const w3 = b2 + (w2 << binshift);
            s32 const b3 = math::findFirstBit(_bin3[w3] & valid_level3_mask((u32)w3, maxbits));

            s32 const bit = b3 + (w3 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 alloc(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        {
            s32 const bit = find_free(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits);
            if (bit >= 0)
                set_used(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, (u32)bit);
            return bit;
        }

        static s32 free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        {
            s32 const bit = find_used(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits);
            if (bit >= 0)
                set_free(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_free_last(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits)
        {
            bintype const free0_word = _free0[0];
            if (free0_word == 0)
                return -1;

            s32 const b0 = math::findLastBit(free0_word);
            s32 const w1 = b0;
            s32 const b1 = math::findLastBit(_free1[w1]);
            s32 const w2 = b1 + (w1 << binshift);
            s32 const b2 = math::findLastBit(_free2[w2]);
            s32 const w3 = b2 + (w2 << binshift);
            s32 const b3 = math::findLastBit(D_INVERT(_bin3[w3]) & valid_level3_mask((u32)w3, maxbits));

            s32 const bit = b3 + (w3 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 alloc_last(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        {
            s32 const bit = find_free_last(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits);
            if (bit >= 0)
                set_used(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_free_after(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in _bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;

            if ((pivot + 1) >= maxbits)
                return -1;

            u32 iw = (pivot + 1) >> binshift;
            s8  ib = (s8)((pivot + 1) & binmask);

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype w;
                switch (il)
                {
                    case 0: w = _free0[0] & (binconstant << ib); break;
                    case 1: w = _free1[iw] & (binconstant << ib); break;
                    case 2: w = _free2[iw] & (binconstant << ib); break;
                    case 3: w = (D_INVERT(_bin3[iw]) & valid_level3_mask(iw, maxbits)) & (binconstant << ib); break;
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

        static s32 find_free_before(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot)
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
                    case 0: w = _free0[0] & ~(binconstant << (ib + 1)); break;
                    case 1: w = _free1[iw] & ~(binconstant << (ib + 1)); break;
                    case 2: w = _free2[iw] & ~(binconstant << (ib + 1)); break;
                    case 3: w = (D_INVERT(_bin3[iw]) & valid_level3_mask(iw, maxbits)) & ~(binconstant << (ib + 1)); break;
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
        static s32 find_used_last(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits)
        {
            bintype const used0_word = _used0[0];
            if (used0_word == 0)
                return -1;

            s32 const b0 = math::findLastBit(used0_word);
            s32 const w1 = b0;
            s32 const b1 = math::findLastBit(_used1[w1]);
            s32 const w2 = b1 + (w1 << binshift);
            s32 const b2 = math::findLastBit(_used2[w2]);
            s32 const w3 = b2 + (w2 << binshift);
            s32 const b3 = math::findLastBit(_bin3[w3] & valid_level3_mask((u32)w3, maxbits));

            s32 const bit = b3 + (w3 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 free_last(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        {
            s32 const bit = find_used_last(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits);
            if (bit >= 0)
                set_free(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_used_after(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot)
        {
            // First check at location pivot in _bin1, then if no bit found move up one
            // level and check there, then go down to find the actual free bit.
            // So this is like find, but we start at the pivot location in the most detailed level.
            if (pivot >= maxbits)
                return -1;

            if ((pivot + 1) >= maxbits)
                return -1;

            u32 iw = (pivot + 1) >> binshift;
            s8  ib = (s8)((pivot + 1) & binmask);

            s8 const ml = binlevels - 1;
            s8       il = ml;
            while (il >= 0 && il <= ml)
            {
                bintype w;
                switch (il)
                {
                    case 0: w = _used0[0] & (binconstant << ib); break;
                    case 1: w = _used1[iw] & (binconstant << ib); break;
                    case 2: w = _used2[iw] & (binconstant << ib); break;
                    case 3: w = (_bin3[iw] & valid_level3_mask(iw, maxbits)) & (binconstant << ib); break;
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

        static s32 find_used_before(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot)
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
                    case 0: w = _used0[0] & ~(binconstant << (ib + 1)); break;
                    case 1: w = _used1[iw] & ~(binconstant << (ib + 1)); break;
                    case 2: w = _used2[iw] & ~(binconstant << (ib + 1)); break;
                    case 3: w = (_bin3[iw] & valid_level3_mask(iw, maxbits)) & ~(binconstant << (ib + 1)); break;
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

    namespace nduomap20
    {
        typedef u32 bintype;

        void setup_used_lazy(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::setup_used_lazy(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        void tick_used_lazy(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit)
        { duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::tick_used_lazy(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, bit); }

        void clear_all_free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::clear_all_free(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        void clear_all_used(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::clear_all_used(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }

        void set_used(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit)
        { duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::set_used(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, bit); }
        void set_free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit)
        { duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::set_free(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, bit); }
        bool get(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 bit)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::get(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, bit); }

        s32 find_free(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::find_free(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 find_used(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::find_used(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 find_free_last(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::find_free_last(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 find_free_after(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::find_free_after(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, pivot); }
        s32 find_free_before(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::find_free_before(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, pivot); }
        s32 find_used_last(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::find_used_last(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 find_used_after(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::find_used_after(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, pivot); }
        s32 find_used_before(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::find_used_before(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, pivot); }

        s32 alloc(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::alloc(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::free(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 alloc_last(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::alloc_last(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 free_last(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u32, 5>::free_last(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
    }  // namespace nduomap20

    namespace nduomap24
    {
        typedef u64 bintype;

        void setup_used_lazy(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::setup_used_lazy(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        void tick_used_lazy(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit)
        { duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::tick_used_lazy(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, bit); }

        void clear_all_free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::clear_all_free(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        void clear_all_used(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::clear_all_used(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }

        void set_used(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit)
        { duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::set_used(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, bit); }
        void set_free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit)
        { duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::set_free(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, bit); }
        bool get(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 bit)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::get(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, bit); }

        s32 find_free(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::find_free(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 find_used(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::find_used(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 find_free_last(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::find_free_last(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 find_free_after(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::find_free_after(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, pivot); }
        s32 find_free_before(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::find_free_before(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, pivot); }
        s32 find_used_last(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::find_used_last(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 find_used_after(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::find_used_after(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, pivot); }
        s32 find_used_before(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::find_used_before(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits, pivot); }

        s32 alloc(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::alloc(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::free(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 alloc_last(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::alloc_last(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
        s32 free_last(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits)
        { return duomap_free0_free1_free2_used0_used1_used2_bin3_t<u64, 6>::free_last(_free0, _free1, _free2, _used0, _used1, _used2, _bin3, maxbits); }
    }  // namespace nduomap24

}  // namespace ncore

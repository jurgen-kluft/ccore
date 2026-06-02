#include "ccore/c_math.h"

#include "ccore/c_bitvec.h"

namespace ncore
{
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // bit-vector, these functions are tracking '0' bits.

#define D_INVERT(value)        ((bintype) ~(value))
#define D_BIT_CLEAR(word, bit) ((word) & ~((bintype)1 << (bit)))
#define D_BIT_SET(word, bit)   ((word) | ((bintype)1 << (bit)))
#define D_BIT_TEST(word, bit)  (((word) & ((bintype)1 << (bit))) != 0)

    // bit-vectors with a single level

    namespace nbitvec5
    {
        void setup(bintype* CC_RESTRICT bin0, u32 maxbits)
        {
            ASSERT(maxbits <= 32);
            *bin0 = 0;
        }

        void set_free(bintype* CC_RESTRICT bin0, u32 maxbits, u32 bit) { *bin0 = D_BIT_SET(*bin0, bit); }
        void set_used(bintype* CC_RESTRICT bin0, u32 maxbits, u32 bit) { *bin0 = D_BIT_CLEAR(*bin0, bit); }
        bool get(bintype const * CC_RESTRICT bin0, u32 maxbits, u32 bit) { return D_BIT_TEST(*bin0, bit); }

        s32 find_free(bintype const * CC_RESTRICT bin0, u32 maxbits)
        {
            if (*bin0 == 0)
                return -1;
            s32 const bit = math::findFirstBit(*bin0);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_free_and_remove(bintype* CC_RESTRICT bin0, u32 maxbits)
        {
            if (*bin0 == 0)
                return -1;
            s32 const bit = math::findFirstBit(*bin0);
            *bin0         = D_BIT_CLEAR(*bin0, bit);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_free_last(bintype const * CC_RESTRICT bin0, u32 maxbits)
        {
            if (*bin0 == 0)
                return -1;
            s32 const bit = math::findLastBit(*bin0);
            return bit;
        }

        s32 find_free_last_and_remove(bintype* CC_RESTRICT bin0, u32 maxbits)
        {
            if (*bin0 == 0)
                return -1;
            s32 const bit = math::findLastBit(*bin0);
            *bin0         = D_BIT_CLEAR(*bin0, bit);
            return bit;
        }

        s32 find_free_after(bintype const * CC_RESTRICT bin0, u32 maxbits, u32 pivot)
        {
            // mask out anything above the pivot bit
            bintype masked = *bin0 & ~(((bintype)1 << pivot));
            if (masked == 0)
                return -1;
            s32 const bit = math::findFirstBit(masked);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_free_before(bintype const * CC_RESTRICT bin0, u32 maxbits, u32 pivot)
        {
            // mask out anything below the pivot bit
            bintype masked = *bin0 & (((bintype)1 << pivot) - 1);
            if (masked == 0)
                return -1;
            s32 const bit = math::findLastBit(masked);
            return bit;
        }

    }  // namespace nbitvec5

    namespace nbitvec6
    {
        void setup(bintype* CC_RESTRICT bin0, u32 maxbits)
        {
            ASSERT(maxbits <= 64);
            *bin0 = 0;
        }

        void set_free(bintype* CC_RESTRICT bin0, u32 maxbits, u32 bit) { *bin0 |= ((bintype)1 << bit); }
        void set_used(bintype* CC_RESTRICT bin0, u32 maxbits, u32 bit) { *bin0 &= D_INVERT((bintype)1 << bit); }
        bool get(bintype const * CC_RESTRICT bin0, u32 maxbits, u32 bit) { return (*bin0 & ((bintype)1 << bit)) != 0; }

        s32 find_free(bintype const * CC_RESTRICT bin0, u32 maxbits)
        {
            if (*bin0 == 0)
                return -1;
            s32 const bit = math::findFirstBit(*bin0);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_free_and_remove(bintype* CC_RESTRICT bin0, u32 maxbits)
        {
            if (*bin0 == 0)
                return -1;
            s32 const bit = math::findFirstBit(*bin0);
            *bin0         = D_BIT_CLEAR(*bin0, bit);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_free_last(bintype const * CC_RESTRICT bin0, u32 maxbits)
        {
            if (*bin0 == 0)
                return -1;
            s32 const bit = math::findLastBit(*bin0);
            return bit;
        }

        s32 find_free_last_and_remove(bintype* CC_RESTRICT bin0, u32 maxbits)
        {
            if (*bin0 == 0)
                return -1;
            s32 const bit = math::findLastBit(*bin0);
            *bin0         = D_BIT_CLEAR(*bin0, bit);
            return bit;
        }

        s32 find_free_after(bintype const * CC_RESTRICT bin0, u32 maxbits, u32 pivot)
        {
            // mask out anything above the pivot bit
            bintype masked = *bin0 & ~(((bintype)1 << pivot));
            if (masked == 0)
                return -1;
            s32 const bit = math::findFirstBit(masked);
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_free_before(bintype const * CC_RESTRICT bin0, u32 maxbits, u32 pivot)
        {
            // mask out anything below the pivot bit
            bintype masked = *bin0 & (((bintype)1 << pivot) - 1);
            if (masked == 0)
                return -1;
            s32 const bit = math::findLastBit(masked);
            return bit;
        }

    }  // namespace nbitvec6

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // bit-vectors with two levels

    template <typename bintype, u32 binshift>
    class bitvec_bin0_bin1_t
    {
    public:
        static constexpr u32     binbits     = (u32)1 << binshift;
        static constexpr u32     binmask     = ((u32)1 << binshift) - 1;
        static constexpr bintype binconstant = (bintype) ~(bintype)0;

        static inline u32 word_count_for_bits(u32 maxbits)
        {
            ASSERT(maxbits > 0);
            return (maxbits + (binbits - 1)) >> binshift;
        }

        static inline bintype mask_for_count(u32 count)
        {
            if (count >= binbits)
                return binconstant;
            return (bintype)(((bintype)1 << count) - 1);
        }

        static inline bintype valid_level0_mask(u32 maxbits) { return mask_for_count(word_count_for_bits(maxbits)); }

        static inline bintype mask_from(u32 bit)
        {
            if (bit >= binbits)
                return 0;
            return (bit == 0) ? binconstant : (bintype)(binconstant << bit);
        }

        static inline bintype mask_through(u32 bit)
        {
            if (bit >= binmask)
                return binconstant;
            return (bintype)(((bintype)1 << (bit + 1)) - 1);
        }

        static void setup_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits) { *_bin0 = 0; }
        static void tick_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                const u32 bi1 = wi & binmask;
                wi            = wi >> binshift;
                if (bi1 == 0)
                    _bin1[wi] = 0;
            }
        }

        static void set_all_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits)
        {
            u32 const size = (maxbits + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = binconstant;
            if ((maxbits & binmask) && size > 0)
                _bin1[size - 1] &= ((bintype)1 << (maxbits & binmask)) - 1;
            *_bin0 = valid_level0_mask(maxbits);
        }

        static void set_all_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits)
        {
            u32 const size = (maxbits + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
            *_bin0 = 0;
        }

        static void set_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits, u32 bit)
        {
            if (bit >= maxbits)
                return;
            u32 const     i1 = bit >> binshift;
            u32 const     b1 = bit & binmask;
            bintype const vo = _bin1[i1];          // old
            _bin1[i1]        = D_BIT_SET(vo, b1);  // new
            if (vo == 0)
            {
                *_bin0 = D_BIT_SET(*_bin0, i1);
            }
        }

        static void set_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits, u32 bit)
        {
            if (bit >= maxbits)
                return;
            u32 const i1 = bit >> binshift;
            u32 const b1 = bit & binmask;
            _bin1[i1]    = D_BIT_CLEAR(_bin1[i1], b1);
            if (_bin1[i1] == 0)
            {
                *_bin0 = D_BIT_CLEAR(*_bin0, i1);
            }
        }

        static bool get(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i1 = bit >> binshift;
            u32 const b1 = bit & binmask;
            return D_BIT_TEST(_bin1[i1], b1);
        }

        static s32 find_free(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits)
        {
            bintype summary = *_bin0 & valid_level0_mask(maxbits);
            if (summary == 0)
                return -1;

            u32 const     b0 = (u32)math::findFirstBit(summary);
            bintype const w1 = _bin1[b0];
            ASSERT(w1 != 0);

            s32 const bit = math::findFirstBit(w1) + (b0 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_free_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits)
        {
            s32 const bit = find_free(_bin0, _bin1, maxbits);
            if (bit >= 0)
                set_used(_bin0, _bin1, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_free_last(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits)
        {
            bintype summary = *_bin0 & valid_level0_mask(maxbits);
            if (summary == 0)
                return -1;

            u32 const     wi = (u32)math::findLastBit(summary);
            bintype const w1 = _bin1[wi];
            ASSERT(w1 != 0);

            u32 const found_bit = (wi << binshift) + (u32)math::findLastBit(w1);
            return (found_bit < maxbits) ? found_bit : -1;
        }

        static s32 find_free_last_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits)
        {
            s32 const bit = find_free_last(_bin0, _bin1, maxbits);
            if (bit >= 0)
                set_used(_bin0, _bin1, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_free_after(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits, u32 pivot)
        {
            if (pivot >= maxbits || (pivot + 1) >= maxbits)
                return -1;

            u32 const start_bit  = pivot + 1;
            u32 const start_word = start_bit >> binshift;
            u32 const start_mask = start_bit & binmask;

            bintype word = _bin1[start_word] & mask_from(start_mask);
            if (word != 0)
                return (s32)((start_word << binshift) + (u32)math::findFirstBit(word));

            bintype const summary = (*_bin0 & valid_level0_mask(maxbits)) & mask_from(start_word + 1);
            if (summary == 0)
                return -1;

            u32 const     next_word = (u32)math::findFirstBit(summary);
            bintype const next_bits = _bin1[next_word];
            ASSERT(next_bits != 0);
            return (s32)((next_word << binshift) + (u32)math::findFirstBit(next_bits));
        }

        static s32 find_free_before(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits, u32 pivot)
        {
            if (pivot == 0 || pivot > maxbits)
                return -1;

            u32 const start_bit  = pivot - 1;
            u32 const start_word = start_bit >> binshift;
            u32 const start_mask = start_bit & binmask;

            bintype word = _bin1[start_word] & mask_through(start_mask);
            if (word != 0)
                return (s32)((start_word << binshift) + (u32)math::findLastBit(word));

            if (start_word == 0)
                return -1;

            bintype const summary = (*_bin0 & valid_level0_mask(maxbits)) & mask_through(start_word - 1);
            if (summary == 0)
                return -1;

            u32 const     prev_word = (u32)math::findLastBit(summary);
            bintype const prev_bits = _bin1[prev_word];
            ASSERT(prev_bits != 0);
            return (s32)((prev_word << binshift) + (u32)math::findLastBit(prev_bits));
        }
    };

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    namespace nbitvec10
    {
        void setup_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits) { bitvec_bin0_bin1_t<bintype, 5>::setup_used_lazy(_bin0, _bin1, maxbits); }
        void tick_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits, u32 bit) { bitvec_bin0_bin1_t<bintype, 5>::tick_used_lazy(_bin0, _bin1, maxbits, bit); }
        void set_all_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits) { bitvec_bin0_bin1_t<bintype, 5>::set_all_free(_bin0, _bin1, maxbits); }
        void set_all_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits) { bitvec_bin0_bin1_t<bintype, 5>::set_all_used(_bin0, _bin1, maxbits); }
        void set_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits, u32 bit) { bitvec_bin0_bin1_t<bintype, 5>::set_free(_bin0, _bin1, maxbits, bit); }
        void set_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits, u32 bit) { bitvec_bin0_bin1_t<bintype, 5>::set_used(_bin0, _bin1, maxbits, bit); }
        bool get(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits, u32 bit) { return bitvec_bin0_bin1_t<bintype, 5>::get(_bin0, _bin1, maxbits, bit); }
        s32  find_free(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits) { return bitvec_bin0_bin1_t<bintype, 5>::find_free(_bin0, _bin1, maxbits); }
        s32  find_free_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits) { return bitvec_bin0_bin1_t<bintype, 5>::find_free_and_remove(_bin0, _bin1, maxbits); }
        s32  find_free_last(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits) { return bitvec_bin0_bin1_t<bintype, 5>::find_free_last(_bin0, _bin1, maxbits); }
        s32  find_free_last_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits) { return bitvec_bin0_bin1_t<bintype, 5>::find_free_last_and_remove(_bin0, _bin1, maxbits); }
        s32  find_free_after(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits, u32 pivot) { return bitvec_bin0_bin1_t<bintype, 5>::find_free_after(_bin0, _bin1, maxbits, pivot); }
        s32  find_free_before(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits, u32 pivot) { return bitvec_bin0_bin1_t<bintype, 5>::find_free_before(_bin0, _bin1, maxbits, pivot); }
    }  // namespace nbitvec10

    namespace nbitvec12
    {
        void setup_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits) { bitvec_bin0_bin1_t<bintype, 6>::setup_used_lazy(_bin0, _bin1, maxbits); }
        void tick_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits, u32 bit) { bitvec_bin0_bin1_t<bintype, 6>::tick_used_lazy(_bin0, _bin1, maxbits, bit); }
        void set_all_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits) { bitvec_bin0_bin1_t<bintype, 6>::set_all_free(_bin0, _bin1, maxbits); }
        void set_all_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits) { bitvec_bin0_bin1_t<bintype, 6>::set_all_used(_bin0, _bin1, maxbits); }
        void set_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits, u32 bit) { bitvec_bin0_bin1_t<bintype, 6>::set_free(_bin0, _bin1, maxbits, bit); }
        void set_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits, u32 bit) { bitvec_bin0_bin1_t<bintype, 6>::set_used(_bin0, _bin1, maxbits, bit); }
        bool get(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits, u32 bit) { return bitvec_bin0_bin1_t<bintype, 6>::get(_bin0, _bin1, maxbits, bit); }
        s32  find_free(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits) { return bitvec_bin0_bin1_t<bintype, 6>::find_free(_bin0, _bin1, maxbits); }
        s32  find_free_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits) { return bitvec_bin0_bin1_t<bintype, 6>::find_free_and_remove(_bin0, _bin1, maxbits); }
        s32  find_free_last(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits) { return bitvec_bin0_bin1_t<bintype, 6>::find_free_last(_bin0, _bin1, maxbits); }
        s32  find_free_last_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, u32 maxbits) { return bitvec_bin0_bin1_t<bintype, 6>::find_free_last_and_remove(_bin0, _bin1, maxbits); }
        s32  find_free_after(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits, u32 pivot) { return bitvec_bin0_bin1_t<bintype, 6>::find_free_after(_bin0, _bin1, maxbits, pivot); }
        s32  find_free_before(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, u32 maxbits, u32 pivot) { return bitvec_bin0_bin1_t<bintype, 6>::find_free_before(_bin0, _bin1, maxbits, pivot); }
    }  // namespace nbitvec12

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // Below are the bit-vectors that have three levels.
    template <typename bintype, u32 binshift>
    class bitvec_bin0_bin1_bin2_t
    {
    public:
        static constexpr u32     binbits     = sizeof(bintype) * 8;
        static constexpr u32     binmask     = binbits - 1;
        static constexpr bintype binconstant = (bintype) ~(bintype)0;

        static inline u32 word_count_for_bits(u32 maxbits)
        {
            ASSERT(maxbits > 0);
            return (maxbits + (binbits - 1)) >> binshift;
        }

        static inline u32 level1_word_count(u32 maxbits)
        {
            u32 const level2_words = word_count_for_bits(maxbits);
            return (level2_words + (binbits - 1)) >> binshift;
        }

        static inline bintype mask_for_count(u32 count)
        {
            if (count >= binbits)
                return binconstant;
            return (bintype)(((bintype)1 << count) - 1);
        }

        static inline bintype valid_level0_mask(u32 maxbits) { return mask_for_count(level1_word_count(maxbits)); }

        static inline bintype mask_from(u32 bit)
        {
            if (bit >= binbits)
                return 0;
            return (bit == 0) ? binconstant : (bintype)(binconstant << bit);
        }

        static inline bintype mask_through(u32 bit)
        {
            if (bit >= binmask)
                return binconstant;
            return (bintype)(((bintype)1 << (bit + 1)) - 1);
        }

        static void setup_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits) { *_bin0 = 0; }
        static void tick_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                const u32 bi2 = wi & binmask;
                wi            = wi >> binshift;

                if (bi2 == 0)
                {
                    _bin2[wi] = 0;

                    const u32 bi1 = wi & binmask;
                    wi            = wi >> binshift;
                    if (bi1 == 0)
                    {
                        _bin1[wi] = 0;
                    }
                }
            }
        }

        static void set_all_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits)
        {
            u32 size = (maxbits + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin2[i] = (binconstant);
            if ((maxbits & binmask) && size > 0)
                _bin2[size - 1] &= ((bintype)1 << (maxbits & binmask)) - 1;

            size = (size + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = (binconstant);
            if (((maxbits >> binshift) & binmask) && size > 0)
                _bin1[size - 1] &= ((bintype)1 << ((maxbits >> binshift) & binmask)) - 1;

            *_bin0 = valid_level0_mask(maxbits);
        }

        static void set_all_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits)
        {
            u32 size = (maxbits + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin2[i] = 0;
            size = (size + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
            *_bin0 = 0;
        }

        static void set_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i2   = bit >> binshift;
            u32 const     b2   = bit & binmask;
            bintype const old2 = _bin2[i2];
            _bin2[i2]          = D_BIT_SET(old2, b2);

            if (old2 != 0)
                return;

            u32 const     i1   = i2 >> binshift;
            u32 const     b1   = i2 & binmask;
            bintype const old1 = _bin1[i1];
            _bin1[i1]          = D_BIT_SET(old1, b1);

            if (old1 != 0)
                return;

            *_bin0 = D_BIT_SET(*_bin0, i1);
        }

        static void set_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i2   = bit >> binshift;
            u32 const     b2   = bit & binmask;
            bintype const new2 = D_BIT_CLEAR(_bin2[i2], b2);
            _bin2[i2]          = new2;

            if (new2 != 0)
                return;

            u32 const     i1   = i2 >> binshift;
            u32 const     b1   = i2 & binmask;
            bintype const new1 = D_BIT_CLEAR(_bin1[i1], b1);
            _bin1[i1]          = new1;

            if (new1 != 0)
                return;

            *_bin0 = D_BIT_CLEAR(*_bin0, i1);
        }

        static bool get(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> binshift;
            u32 const b = bit & binmask;
            return D_BIT_TEST(_bin2[i], b);
        }

        static s32 find_free(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits)
        {
            bintype summary0 = *_bin0 & valid_level0_mask(maxbits);
            if (summary0 == 0)
                return -1;

            u32 const     b0       = (u32)math::findFirstBit(summary0);
            bintype const summary1 = _bin1[b0];
            ASSERT(summary1 != 0);

            u32 const     b1     = (u32)math::findFirstBit(summary1);
            u32 const     w2     = b1 + (b0 << binshift);
            bintype const level2 = _bin2[w2];
            ASSERT(level2 != 0);

            s32 const bit = math::findFirstBit(level2) + (w2 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_free_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits)
        {
            s32 const bit = find_free(_bin0, _bin1, _bin2, maxbits);
            if (bit >= 0)
                set_used(_bin0, _bin1, _bin2, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_free_last(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits)
        {
            bintype summary0 = *_bin0 & valid_level0_mask(maxbits);
            if (summary0 == 0)
                return -1;

            u32 const     w1_index = (u32)math::findLastBit(summary0);
            bintype const summary1 = _bin1[w1_index];
            ASSERT(summary1 != 0);

            u32 const     b1     = (u32)math::findLastBit(summary1);
            u32 const     w2     = b1 + (w1_index << binshift);
            bintype const level2 = _bin2[w2];
            ASSERT(level2 != 0);

            u32 const found_bit = (w2 << binshift) + (u32)math::findLastBit(level2);
            return (found_bit < maxbits) ? found_bit : -1;
        }

        static s32 find_free_last_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits)
        {
            s32 const bit = find_free_last(_bin0, _bin1, _bin2, maxbits);
            if (bit >= 0)
                set_used(_bin0, _bin1, _bin2, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_free_after(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits, u32 pivot)
        {
            if (pivot >= maxbits || (pivot + 1) >= maxbits)
                return -1;

            u32 const start_bit  = pivot + 1;
            u32 const start_word = start_bit >> binshift;
            u32 const start_mask = start_bit & binmask;

            bintype level2 = _bin2[start_word] & mask_from(start_mask);
            if (level2 != 0)
                return (s32)((start_word << binshift) + (u32)math::findFirstBit(level2));

            u32 const start_level1     = start_word >> binshift;
            u32 const start_level2_bit = start_word & binmask;

            bintype level1 = _bin1[start_level1] & mask_from(start_level2_bit + 1);
            if (level1 != 0)
            {
                u32 const     b1    = (u32)math::findFirstBit(level1);
                u32 const     w2    = b1 + (start_level1 << binshift);
                bintype const word2 = _bin2[w2];
                ASSERT(word2 != 0);
                return (s32)((w2 << binshift) + (u32)math::findFirstBit(word2));
            }

            bintype level0 = (*_bin0 & valid_level0_mask(maxbits)) & mask_from(start_level1 + 1);
            if (level0 == 0)
                return -1;

            u32 const     next_level1   = (u32)math::findFirstBit(level0);
            bintype const next_summary1 = _bin1[next_level1];
            ASSERT(next_summary1 != 0);

            u32 const     b1    = (u32)math::findFirstBit(next_summary1);
            u32 const     w2    = b1 + (next_level1 << binshift);
            bintype const word2 = _bin2[w2];
            ASSERT(word2 != 0);
            return (s32)((w2 << binshift) + (u32)math::findFirstBit(word2));
        }

        static s32 find_free_before(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits, u32 pivot)
        {
            if (pivot == 0 || pivot > maxbits)
                return -1;

            u32 const start_bit  = pivot - 1;
            u32 const start_word = start_bit >> binshift;
            u32 const start_mask = start_bit & binmask;

            bintype level2 = _bin2[start_word] & mask_through(start_mask);
            if (level2 != 0)
                return (s32)((start_word << binshift) + (u32)math::findLastBit(level2));

            u32 const start_level1     = start_word >> binshift;
            u32 const start_level2_bit = start_word & binmask;

            bintype level1_mask = (start_level2_bit == 0) ? 0 : mask_through(start_level2_bit - 1);
            bintype level1      = _bin1[start_level1] & level1_mask;
            if (level1 != 0)
            {
                u32 const     b1    = (u32)math::findLastBit(level1);
                u32 const     w2    = b1 + (start_level1 << binshift);
                bintype const word2 = _bin2[w2];
                ASSERT(word2 != 0);
                return (s32)((w2 << binshift) + (u32)math::findLastBit(word2));
            }

            if (start_level1 == 0)
                return -1;

            bintype level0 = (*_bin0 & valid_level0_mask(maxbits)) & mask_through(start_level1 - 1);
            if (level0 == 0)
                return -1;

            u32 const     prev_level1   = (u32)math::findLastBit(level0);
            bintype const prev_summary1 = _bin1[prev_level1];
            ASSERT(prev_summary1 != 0);

            u32 const     b1    = (u32)math::findLastBit(prev_summary1);
            u32 const     w2    = b1 + (prev_level1 << binshift);
            bintype const word2 = _bin2[w2];
            ASSERT(word2 != 0);
            return (s32)((w2 << binshift) + (u32)math::findLastBit(word2));
        }
    };

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    namespace nbitvec15
    {
        // constexpr u32 binshift = 5;
        constexpr u32 binshift = 5;

        void setup_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits) { bitvec_bin0_bin1_bin2_t<bintype, binshift>::setup_used_lazy(_bin0, _bin1, _bin2, maxbits); }
        void tick_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits, u32 bit) { bitvec_bin0_bin1_bin2_t<bintype, binshift>::tick_used_lazy(_bin0, _bin1, _bin2, maxbits, bit); }
        void set_all_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits) { bitvec_bin0_bin1_bin2_t<bintype, binshift>::set_all_free(_bin0, _bin1, _bin2, maxbits); }
        void set_all_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits) { bitvec_bin0_bin1_bin2_t<bintype, binshift>::set_all_used(_bin0, _bin1, _bin2, maxbits); }
        void set_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits, u32 bit) { bitvec_bin0_bin1_bin2_t<bintype, binshift>::set_free(_bin0, _bin1, _bin2, maxbits, bit); }
        void set_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits, u32 bit) { bitvec_bin0_bin1_bin2_t<bintype, binshift>::set_used(_bin0, _bin1, _bin2, maxbits, bit); }
        bool get(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits, u32 bit) { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::get(_bin0, _bin1, _bin2, maxbits, bit); }
        s32  find_free(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits) { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::find_free(_bin0, _bin1, _bin2, maxbits); }
        s32  find_free_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits) { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::find_free_and_remove(_bin0, _bin1, _bin2, maxbits); }

        s32 find_free_last(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits) { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::find_free_last(_bin0, _bin1, _bin2, maxbits); }
        s32 find_free_last_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits) { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::find_free_last_and_remove(_bin0, _bin1, _bin2, maxbits); }
        s32 find_free_after(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits, u32 pivot)
        { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::find_free_after(_bin0, _bin1, _bin2, maxbits, pivot); }
        s32 find_free_before(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits, u32 pivot)
        { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::find_free_before(_bin0, _bin1, _bin2, maxbits, pivot); }
    }  // namespace nbitvec15

    namespace nbitvec18
    {
        // constexpr u32 binshift = 6;
        constexpr u32 binshift = 6;

        void setup_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits) { bitvec_bin0_bin1_bin2_t<bintype, binshift>::setup_used_lazy(_bin0, _bin1, _bin2, maxbits); }
        void tick_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits, u32 bit) { bitvec_bin0_bin1_bin2_t<bintype, binshift>::tick_used_lazy(_bin0, _bin1, _bin2, maxbits, bit); }
        void set_all_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits) { bitvec_bin0_bin1_bin2_t<bintype, binshift>::set_all_free(_bin0, _bin1, _bin2, maxbits); }
        void set_all_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits) { bitvec_bin0_bin1_bin2_t<bintype, binshift>::set_all_used(_bin0, _bin1, _bin2, maxbits); }
        void set_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits, u32 bit) { bitvec_bin0_bin1_bin2_t<bintype, binshift>::set_free(_bin0, _bin1, _bin2, maxbits, bit); }
        void set_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits, u32 bit) { bitvec_bin0_bin1_bin2_t<bintype, binshift>::set_used(_bin0, _bin1, _bin2, maxbits, bit); }
        bool get(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits, u32 bit) { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::get(_bin0, _bin1, _bin2, maxbits, bit); }
        s32  find_free(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits) { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::find_free(_bin0, _bin1, _bin2, maxbits); }
        s32  find_free_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits) { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::find_free_and_remove(_bin0, _bin1, _bin2, maxbits); }

        s32 find_free_last(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits) { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::find_free_last(_bin0, _bin1, _bin2, maxbits); }
        s32 find_free_last_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, u32 maxbits) { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::find_free_last_and_remove(_bin0, _bin1, _bin2, maxbits); }
        s32 find_free_after(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits, u32 pivot)
        { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::find_free_after(_bin0, _bin1, _bin2, maxbits, pivot); }
        s32 find_free_before(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, u32 maxbits, u32 pivot)
        { return bitvec_bin0_bin1_bin2_t<bintype, binshift>::find_free_before(_bin0, _bin1, _bin2, maxbits, pivot); }
    }  // namespace nbitvec18

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // Below are the bit-vectors that have four levels.
    template <typename bintype, u32 binshift>
    class bitvec_bin0_bin1_bin2_bin3_t
    {
    public:
        static constexpr u32     binbits     = sizeof(bintype) * 8;
        static constexpr u32     binmask     = binbits - 1;
        static constexpr bintype binconstant = (bintype) ~(bintype)0;

        static inline u32 word_count_for_bits(u32 maxbits)
        {
            ASSERT(maxbits > 0);
            return (maxbits + (binbits - 1)) >> binshift;
        }

        static inline u32 level2_word_count(u32 maxbits)
        {
            u32 const level3_words = word_count_for_bits(maxbits);
            return (level3_words + (binbits - 1)) >> binshift;
        }

        static inline u32 level1_word_count(u32 maxbits)
        {
            u32 const level2_words = level2_word_count(maxbits);
            return (level2_words + (binbits - 1)) >> binshift;
        }

        static inline bintype mask_for_count(u32 count)
        {
            if (count >= binbits)
                return binconstant;
            return (bintype)(((bintype)1 << count) - 1);
        }

        static inline bintype valid_level0_mask(u32 maxbits) { return mask_for_count(level1_word_count(maxbits)); }

        static inline bintype mask_from(u32 bit)
        {
            if (bit >= binbits)
                return 0;
            return (bit == 0) ? binconstant : (bintype)(binconstant << bit);
        }

        static inline bintype mask_through(u32 bit)
        {
            if (bit >= binmask)
                return binconstant;
            return (bintype)(((bintype)1 << (bit + 1)) - 1);
        }

        static void setup_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, bintype* CC_RESTRICT _bin3, u32 maxbits) { *_bin0 = 0; }
        static void tick_used_lazy(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, bintype* CC_RESTRICT _bin3, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                const u32 bi3 = wi & binmask;
                wi            = wi >> binshift;
                if (bi3 == 0)
                {
                    _bin3[wi] = 0;

                    const u32 bi2 = wi & binmask;
                    wi            = wi >> binshift;
                    if (bi2 == 0)
                    {
                        _bin2[wi] = 0;

                        const u32 bi1 = wi & binmask;
                        wi            = wi >> binshift;
                        if (bi1 == 0)
                        {
                            _bin1[wi] = 0;
                        }
                    }
                }
            }
        }

        static void set_all_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, bintype* CC_RESTRICT _bin3, u32 maxbits)
        {
            // free means bits are set to 1
            u32 size = (maxbits + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin3[i] = binconstant;

            // mask off the excess bits in the last word of this level, if any
            if ((maxbits & binmask) && size > 0)
                _bin3[size - 1] &= mask_for_count(maxbits & binmask);

            size = (size + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin2[i] = binconstant;

            // mask off the excess bits in the last word of this level, if any
            if ((maxbits & (binmask << binshift)) && size > 0)
                _bin2[size - 1] &= mask_for_count((maxbits >> binshift) & binmask);

            size = (size + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = binconstant;

            if ((maxbits & (binmask << (binshift * 2))) && size > 0)
                _bin1[size - 1] &= mask_for_count((maxbits >> (binshift * 2)) & binmask);

            *_bin0 = binconstant;
        }

        static void set_all_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, bintype* CC_RESTRICT _bin3, u32 maxbits)
        {
            u32 size = (maxbits + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin3[i] = 0;
            size = (size + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin2[i] = 0;
            size = (size + (binbits - 1)) >> binshift;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
            *_bin0 = 0;
        }

        static void set_free(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, bintype* CC_RESTRICT _bin3, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i3   = bit >> binshift;
            u32 const     b3   = bit & binmask;
            bintype const old3 = _bin3[i3];
            _bin3[i3]          = D_BIT_SET(old3, b3);
            if (old3 != 0)
                return;

            u32 const     i2   = i3 >> binshift;
            u32 const     b2   = i3 & binmask;
            bintype const old2 = _bin2[i2];
            _bin2[i2]          = D_BIT_SET(old2, b2);
            if (old2 != 0)
                return;

            u32 const     i1   = i2 >> binshift;
            u32 const     b1   = i2 & binmask;
            bintype const old1 = _bin1[i1];
            _bin1[i1]          = D_BIT_SET(old1, b1);
            if (old1 != 0)
                return;

            *_bin0 = D_BIT_SET(*_bin0, i1);
        }

        static void set_used(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, bintype* CC_RESTRICT _bin3, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const     i3 = bit >> binshift;
            u32 const     b3 = bit & binmask;
            bintype const v3 = D_BIT_CLEAR(_bin3[i3], b3);
            _bin3[i3]        = v3;
            if (v3 != 0)
                return;

            u32 const     i2 = i3 >> binshift;
            u32 const     b2 = i3 & binmask;
            bintype const v2 = D_BIT_CLEAR(_bin2[i2], b2);
            _bin2[i2]        = v2;
            if (v2 != 0)
                return;

            u32 const     i1 = i2 >> binshift;
            u32 const     b1 = i2 & binmask;
            bintype const v1 = D_BIT_CLEAR(_bin1[i1], b1);
            _bin1[i1]        = v1;
            if (v1 != 0)
                return;

            *_bin0 = D_BIT_CLEAR(*_bin0, i1);
        }

        static bool get(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, bintype const * CC_RESTRICT _bin3, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> binshift;
            u32 const b = bit & binmask;
            return D_BIT_TEST(_bin3[i], b);
        }

        static s32 find_free(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, bintype const * CC_RESTRICT _bin3, u32 maxbits)
        {
            bintype const summary0 = *_bin0 & valid_level0_mask(maxbits);
            if (summary0 == 0)
                return -1;

            u32 const     b0       = (u32)math::findFirstBit(summary0);
            bintype const summary1 = _bin1[b0];
            ASSERT(summary1 != 0);

            u32 const     b1       = (u32)math::findFirstBit(summary1);
            u32 const     w2       = b1 + (b0 << binshift);
            bintype const summary2 = _bin2[w2];
            ASSERT(summary2 != 0);

            u32 const     b2     = (u32)math::findFirstBit(summary2);
            u32 const     w3     = b2 + (w2 << binshift);
            bintype const level3 = _bin3[w3];
            ASSERT(level3 != 0);

            s32 const bit = math::findFirstBit(level3) + (w3 << binshift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_free_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, bintype* CC_RESTRICT _bin3, u32 maxbits)
        {
            s32 const bit = find_free(_bin0, _bin1, _bin2, _bin3, maxbits);
            if (bit >= 0)
                set_used(_bin0, _bin1, _bin2, _bin3, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_free_last(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits)
        {
            bintype const summary0 = *_bin0 & valid_level0_mask(maxbits);
            if (summary0 == 0)
                return -1;

            u32 const     w1_index = (u32)math::findLastBit(summary0);
            bintype const summary1 = _bin1[w1_index];
            ASSERT(summary1 != 0);

            u32 const     b1       = (u32)math::findLastBit(summary1);
            u32 const     w2       = b1 + (w1_index << binshift);
            bintype const summary2 = bin2[w2];
            ASSERT(summary2 != 0);

            u32 const     b2     = (u32)math::findLastBit(summary2);
            u32 const     w3     = b2 + (w2 << binshift);
            bintype const level3 = bin3[w3];
            ASSERT(level3 != 0);

            u32 const found_bit = (w3 << binshift) + (u32)math::findLastBit(level3);
            return (found_bit < maxbits) ? found_bit : -1;
        }

        static s32 find_free_last_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits)
        {
            s32 const bit = find_free_last(_bin0, _bin1, bin2, bin3, maxbits);
            if (bit >= 0)
                set_used(_bin0, _bin1, bin2, bin3, maxbits, (u32)bit);
            return bit;
        }

        static s32 find_free_after(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits, u32 pivot)
        {
            if (pivot >= maxbits || (pivot + 1) >= maxbits)
                return -1;

            u32 const start_bit  = pivot + 1;
            u32 const start_word = start_bit >> binshift;
            u32 const start_mask = start_bit & binmask;

            bintype level3 = bin3[start_word] & mask_from(start_mask);
            if (level3 != 0)
                return (s32)((start_word << binshift) + (u32)math::findFirstBit(level3));

            u32 const start_level2     = start_word >> binshift;
            u32 const start_level3_bit = start_word & binmask;

            bintype level2 = bin2[start_level2] & mask_from(start_level3_bit + 1);
            if (level2 != 0)
            {
                u32 const     b2    = (u32)math::findFirstBit(level2);
                u32 const     w3    = b2 + (start_level2 << binshift);
                bintype const word3 = bin3[w3] & mask_from(start_level3_bit + 1);
                ASSERT(word3 != 0);
                return (s32)((w3 << binshift) + (u32)math::findFirstBit(word3));
            }

            u32 const start_level1     = start_level2 >> binshift;
            u32 const start_level2_bit = start_level2 & binmask;

            bintype level1 = _bin1[start_level1] & mask_from(start_level2_bit + 1);
            if (level1 != 0)
            {
                u32 const     b1       = (u32)math::findFirstBit(level1);
                u32 const     w2       = b1 + (start_level1 << binshift);
                bintype const summary2 = bin2[w2];
                ASSERT(summary2 != 0);
                u32 const     b2    = (u32)math::findFirstBit(summary2);
                u32 const     w3    = b2 + (w2 << binshift);
                bintype const word3 = bin3[w3];
                ASSERT(word3 != 0);
                return (s32)((w3 << binshift) + (u32)math::findFirstBit(word3));
            }

            bintype level0 = (*_bin0 & valid_level0_mask(maxbits)) & mask_from(start_level1 + 1);
            if (level0 == 0)
                return -1;

            u32 const     next_level1   = (u32)math::findFirstBit(level0);
            bintype const next_summary1 = _bin1[next_level1];
            ASSERT(next_summary1 != 0);
            u32 const     b1       = (u32)math::findFirstBit(next_summary1);
            u32 const     w2       = b1 + (next_level1 << binshift);
            bintype const summary2 = bin2[w2];
            ASSERT(summary2 != 0);
            u32 const     b2    = (u32)math::findFirstBit(summary2);
            u32 const     w3    = b2 + (w2 << binshift);
            bintype const word3 = bin3[w3];
            ASSERT(word3 != 0);
            return (s32)((w3 << binshift) + (u32)math::findFirstBit(word3));
        }

        static s32 find_free_before(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits, u32 pivot)
        {
            if (pivot == 0 || pivot > maxbits)
                return -1;

            u32 const start_bit  = pivot - 1;
            u32 const start_word = start_bit >> binshift;
            u32 const start_mask = start_bit & binmask;

            bintype level3 = bin3[start_word] & mask_through(start_mask);
            if (level3 != 0)
                return (s32)((start_word << binshift) + (u32)math::findLastBit(level3));

            u32 const start_level2     = start_word >> binshift;
            u32 const start_level3_bit = start_word & binmask;
            bintype   level2_mask      = (start_level3_bit == 0) ? 0 : mask_through(start_level3_bit - 1);
            bintype   level2           = bin2[start_level2] & level2_mask;
            if (level2 != 0)
            {
                u32 const     b2    = (u32)math::findLastBit(level2);
                u32 const     w3    = b2 + (start_level2 << binshift);
                bintype const word3 = bin3[w3];
                ASSERT(word3 != 0);
                return (s32)((w3 << binshift) + (u32)math::findLastBit(word3));
            }

            u32 const start_level1     = start_level2 >> binshift;
            u32 const start_level2_bit = start_level2 & binmask;
            bintype   level1_mask      = (start_level2_bit == 0) ? 0 : mask_through(start_level2_bit - 1);
            bintype   level1           = _bin1[start_level1] & level1_mask;
            if (level1 != 0)
            {
                u32 const     b1       = (u32)math::findLastBit(level1);
                u32 const     w2       = b1 + (start_level1 << binshift);
                bintype const summary2 = bin2[w2];
                ASSERT(summary2 != 0);
                u32 const     b2    = (u32)math::findLastBit(summary2);
                u32 const     w3    = b2 + (w2 << binshift);
                bintype const word3 = bin3[w3];
                ASSERT(word3 != 0);
                return (s32)((w3 << binshift) + (u32)math::findLastBit(word3));
            }

            if (start_level1 == 0)
                return -1;

            bintype level0 = (*_bin0 & valid_level0_mask(maxbits)) & mask_through(start_level1 - 1);
            if (level0 == 0)
                return -1;

            u32 const     prev_level1   = (u32)math::findLastBit(level0);
            bintype const prev_summary1 = _bin1[prev_level1];
            ASSERT(prev_summary1 != 0);
            u32 const     b1       = (u32)math::findLastBit(prev_summary1);
            u32 const     w2       = b1 + (prev_level1 << binshift);
            bintype const summary2 = bin2[w2];
            ASSERT(summary2 != 0);
            u32 const     b2    = (u32)math::findLastBit(summary2);
            u32 const     w3    = b2 + (w2 << binshift);
            bintype const word3 = bin3[w3];
            ASSERT(word3 != 0);
            return (s32)((w3 << binshift) + (u32)math::findLastBit(word3));
        }
    };

    namespace nbitvec20
    {
        void setup_used_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits) { bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::setup_used_lazy(bin0, bin1, bin2, bin3, maxbits); }
        void tick_used_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits, u32 bit)
        { bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::tick_used_lazy(bin0, bin1, bin2, bin3, maxbits, bit); }

        void set_all_free(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits) { bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::set_all_free(bin0, bin1, bin2, bin3, maxbits); }
        void set_all_used(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits) { bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::set_all_used(bin0, bin1, bin2, bin3, maxbits); }

        void set_free(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits, u32 bit) { bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::set_free(bin0, bin1, bin2, bin3, maxbits, bit); }
        void set_used(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits, u32 bit) { bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::set_used(bin0, bin1, bin2, bin3, maxbits, bit); }
        bool get(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits, u32 bit)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::get(bin0, bin1, bin2, bin3, maxbits, bit); }
        s32 find_free(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::find_free(bin0, bin1, bin2, bin3, maxbits); }
        s32 find_free_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::find_free_and_remove(bin0, bin1, bin2, bin3, maxbits); }

        s32 find_free_last(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, bintype const * CC_RESTRICT _bin3, u32 maxbits)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::find_free_last(_bin0, _bin1, _bin2, _bin3, maxbits); }
        s32 find_free_last_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, bintype* CC_RESTRICT _bin3, u32 maxbits)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::find_free_last_and_remove(_bin0, _bin1, _bin2, _bin3, maxbits); }
        s32 find_free_after(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, bintype const * CC_RESTRICT _bin3, u32 maxbits, u32 pivot)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::find_free_after(_bin0, _bin1, _bin2, _bin3, maxbits, pivot); }
        s32 find_free_before(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, bintype const * CC_RESTRICT _bin3, u32 maxbits, u32 pivot)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 5>::find_free_before(_bin0, _bin1, _bin2, _bin3, maxbits, pivot); }
    }  // namespace nbitvec20

    namespace nbitvec24
    {
        void setup_used_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits) { bitvec_bin0_bin1_bin2_bin3_t<bintype, 6>::setup_used_lazy(bin0, bin1, bin2, bin3, maxbits); }
        void tick_used_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits, u32 bit)
        { bitvec_bin0_bin1_bin2_bin3_t<bintype, 6>::tick_used_lazy(bin0, bin1, bin2, bin3, maxbits, bit); }

        void set_all_free(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits) { bitvec_bin0_bin1_bin2_bin3_t<bintype, 6>::set_all_free(bin0, bin1, bin2, bin3, maxbits); }

        void set_free(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits, u32 bit) { bitvec_bin0_bin1_bin2_bin3_t<bintype, 6>::set_free(bin0, bin1, bin2, bin3, maxbits, bit); }
        void set_used(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits, u32 bit) { bitvec_bin0_bin1_bin2_bin3_t<bintype, 6>::set_used(bin0, bin1, bin2, bin3, maxbits, bit); }
        bool get(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits, u32 bit)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 6>::get(bin0, bin1, bin2, bin3, maxbits, bit); }
        s32 find_free(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 6>::find_free(bin0, bin1, bin2, bin3, maxbits); }
        s32 find_free_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 6>::find_free_and_remove(bin0, bin1, bin2, bin3, maxbits); }

        s32 find_free_last(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, bintype const * CC_RESTRICT _bin3, u32 maxbits)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 6>::find_free_last(_bin0, _bin1, _bin2, _bin3, maxbits); }
        s32 find_free_last_and_remove(bintype* CC_RESTRICT _bin0, bintype* CC_RESTRICT _bin1, bintype* CC_RESTRICT _bin2, bintype* CC_RESTRICT _bin3, u32 maxbits)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 6>::find_free_last_and_remove(_bin0, _bin1, _bin2, _bin3, maxbits); }
        s32 find_free_after(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, bintype const * CC_RESTRICT _bin3, u32 maxbits, u32 pivot)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 6>::find_free_after(_bin0, _bin1, _bin2, _bin3, maxbits, pivot); }
        s32 find_free_before(bintype const * CC_RESTRICT _bin0, bintype const * CC_RESTRICT _bin1, bintype const * CC_RESTRICT _bin2, bintype const * CC_RESTRICT _bin3, u32 maxbits, u32 pivot)
        { return bitvec_bin0_bin1_bin2_bin3_t<bintype, 6>::find_free_before(_bin0, _bin1, _bin2, _bin3, maxbits, pivot); }
    }  // namespace nbitvec24

    namespace nbitvec
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

        void pointers(byte* ptr, layout64_t const & l, u64*& bin0, u64*& bin1, u64*& bin2, u64*& bin3)
        {
            bin0 = (u64*)ptr;
            bin1 = (u64*)(ptr + sizeof(u64));
            bin2 = bin1 + l.m_bin1;
            bin3 = bin2 + l.m_bin2;
        }

        u32 sizeof_data(layout64_t const & l) { return l.m_bin0 + l.m_bin1 + l.m_bin2 + l.m_bin3; }

        u32 sizeof_data(layout64_t const & l, u32 bit)
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

        void pointers(byte* ptr, layout32_t const & l, u32*& bin0, u32*& bin1, u32*& bin2, u32*& bin3)
        {
            bin0 = (u32*)ptr;
            bin1 = (u32*)(ptr + sizeof(u32));
            bin2 = bin1 + l.m_bin1;
            bin3 = bin2 + l.m_bin2;
        }

        u32 sizeof_data(layout32_t const & l) { return l.m_bin0 + l.m_bin1 + l.m_bin2 + l.m_bin3; }

        u32 sizeof_data(layout32_t const & l, u32 bit)
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

    }  // namespace nbitvec

};  // namespace ncore

#ifndef __CCORE_BITVEC_V2_H__
#define __CCORE_BITVEC_V2_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    // --------------------------------------------------------------------------------------------
    // bit-vector functionality for 1, 2, 3, and 4 level bit-vectors
    // --------------------------------------------------------------------------------------------
    namespace nbitvec
    {
        struct layout64_t
        {
            u32 m_maxbits;  // maximum number of bits this layout can handle
            u32 m_bin3;     // number of u64 at this level (unit = number of u64)
            u32 m_bin2;     // number of u64 at this level (unit = number of u64)
            u16 m_bin1;     // number of u64 at this level (unit = number of u64, should be 2 <= N <= 64)
            u8  m_bin0;     // number of u64 at this level (unit = number of u64, should always be == 1)
            u8  m_levels;   // number of levels (0 = bin0, 1 = bin0+bin1, 2 = bin0+bin1+bin2, 3 = bin0+bin1+bin2+bin3)
        };

        void compute(u32 number_of_bits, layout64_t& l);
        void pointers(byte* ptr, layout64_t const & l, u64*& bin0, u64*& bin1, u64*& bin2, u64*& bin3);
        u32  sizeof_data(layout64_t const & l);  // u64[N], where N is computed based on layout
        // This will compute the data size in u64[N], where the full level is considered 'growable' up to 'bit'
        u32 sizeof_data(layout64_t const & l, u32 bit);

        struct layout32_t
        {
            u32 m_maxbits;  // maximum number of bits this layout can handle
            u32 m_bin3;     // number of u32 at this level (unit = number of u32)
            u32 m_bin2;     // number of u32 at this level (unit = number of u32)
            u16 m_bin1;     // number of u32 at this level (unit = number of u32, should be 2 <= N <= 32)
            u8  m_bin0;     // number of u32 at this level (unit = number of u32, should always be == 1)
            u8  m_levels;   // number of levels (0 = bin0, 1 = bin0+bin1, 2 = bin0+bin1+bin2, 3 = bin0+bin1+bin2+bin3)
        };
        void compute(u32 number_of_bits, layout32_t& l);
        void pointers(byte* ptr, layout32_t const & l, u32*& bin0, u32*& bin1, u32*& bin2, u32*& bin3);
        u32  sizeof_data(layout32_t const & l);  // u32[N], where N is computed based on layout
        // This will compute the data size in u32[N], where the full level is considered 'growable' up to 'bit'
        u32 sizeof_data(layout32_t const & l, u32 bit);

    }  // namespace nbitvec

    // --------------------------------------------------------------------------------------------
    // 1 level binmaps
    // --------------------------------------------------------------------------------------------

    // 2^5 bit-vector, can handle a maximum of 32 bits.
    namespace nbitvec5
    {
        // This binmap implementation is optimized for tracking '1' bits, so the find functions
        // are looking for '1' bits and the set/clear functions are setting/clearing '1' bits.

        typedef u32 bintype;
        void        setup(bintype* CC_RESTRICT bin0, u32 maxbits);
        void        set(bintype* CC_RESTRICT bin0, u32 maxbits, u32 bit);
        void        clr(bintype* CC_RESTRICT bin0, u32 maxbits, u32 bit);
        bool        get(bintype const * CC_RESTRICT bin0, u32 maxbits, u32 bit);
        s32         find(bintype const * CC_RESTRICT bin0, u32 maxbits);
        s32         find_and_remove(bintype* CC_RESTRICT bin0, u32 maxbits);
        s32         find_last(bintype const * CC_RESTRICT bin0, u32 maxbits);               // Finds the last free bit and returns the bit index
        s32         find_last_and_remove(bintype* CC_RESTRICT bin0, u32 maxbits);           // Finds the last free bit and sets it to used and returns the bit index
        s32         find_after(bintype const * CC_RESTRICT bin0, u32 maxbits, u32 pivot);   // Finds the first free bit after the pivot
        s32         find_before(bintype const * CC_RESTRICT bin0, u32 maxbits, u32 pivot);  // Finds the first free bit before the pivot (high to low)
    }  // namespace nbitvec5

    // 2^6 binmap, can handle a maximum of 64 bits.
    namespace nbitvec6
    {
        // This binmap implementation is optimized for tracking '1' bits, so the find functions
        // are looking for '1' bits and the set/clear functions are setting/clearing '1' bits.

        typedef u64 bintype;
        void        setup(bintype* CC_RESTRICT bin0, u32 maxbits);
        void        set(bintype* CC_RESTRICT bin0, u32 maxbits, u32 bit);
        void        clr(bintype* CC_RESTRICT bin0, u32 maxbits, u32 bit);
        bool        get(bintype const * CC_RESTRICT bin0, u32 maxbits, u32 bit);
        s32         find(bintype const * CC_RESTRICT bin0, u32 maxbits);
        s32         find_and_remove(bintype* CC_RESTRICT bin0, u32 maxbits);
        s32         find_last(bintype const * CC_RESTRICT bin0, u32 maxbits);               // Finds the last free bit and returns the bit index
        s32         find_last_and_remove(bintype* CC_RESTRICT bin0, u32 maxbits);           // Finds the last free bit and sets it to used and returns the bit index
        s32         find_after(bintype const * CC_RESTRICT bin0, u32 maxbits, u32 pivot);   // Finds the first free bit after the pivot
        s32         find_before(bintype const * CC_RESTRICT bin0, u32 maxbits, u32 pivot);  // Finds the first free bit before the pivot (high to low)
    }  // namespace nbitvec6

    // --------------------------------------------------------------------------------------------
    // 2 level bit-vectors
    // --------------------------------------------------------------------------------------------

    // 2^10 bit-vector, can handle a maximum of 1024 bits.
    namespace nbitvec10
    {
        // layer0 = a single u32 (5)
        // layer1 = an array of u32, max u32[32] (5)
        typedef u32 bintype;

        void setup_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits);
        void tick_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits, u32 bit);

        void clear(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits);

        void set(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits, u32 bit);
        void clr(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits, u32 bit);
        bool get(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, u32 maxbits, u32 bit);
        s32  find(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, u32 maxbits);
        s32  find_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits);

        s32 find_last(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, u32 maxbits);               // Finds the last free bit and returns the bit index
        s32 find_last_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits);                  // Finds the last free bit and sets it to used and returns the bit index
        s32 find_after(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, u32 maxbits, u32 pivot);   // Finds the first free bit after the pivot
        s32 find_before(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, u32 maxbits, u32 pivot);  // Finds the first free bit before the pivot (high to low)
    }  // namespace nbitvec10

    // 2^12 bit-vector, can handle a maximum of 4096 bits.
    namespace nbitvec12
    {
        // max 2^12 = 4096 bits
        // layer0 = a single u64
        // layer1 = an array of u64, max u64[64]
        typedef u64 bintype;

        void setup_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits);
        void tick_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits, u32 bit);

        void clear(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits);

        void set(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits, u32 bit);
        void clr(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits, u32 bit);
        bool get(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, u32 maxbits, u32 bit);
        s32  find(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, u32 maxbits);
        s32  find_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits);

        s32 find_last(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, u32 maxbits);               // Finds the last free bit and returns the bit index
        s32 find_last_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, u32 maxbits);                  // Finds the last free bit and sets it to used and returns the bit index
        s32 find_after(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, u32 maxbits, u32 pivot);   // Finds the first free bit after the pivot
        s32 find_before(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, u32 maxbits, u32 pivot);  // Finds the first free bit before the pivot (high to low)
    }  // namespace nbitvec12

    // --------------------------------------------------------------------------------------------
    // 3 level bit-vectors
    // --------------------------------------------------------------------------------------------

    namespace nbitvec15
    {
        // max 2^15 = 32768 bits
        // layer0 = u32 (5)
        // layer1 = an array of u32, max u32[32] (5)
        // layer2 = an array of u32, max u32[32*32*32] (5)
        typedef u32 bintype;

        void setup_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits);
        void tick_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits, u32 bit);

        void clear(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits);

        void set(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits, u32 bit);
        void clr(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits, u32 bit);
        bool get(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, u32 maxbits, u32 bit);
        s32  find(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, u32 maxbits);
        s32  find_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits);

        s32 find_last(bintype const * _bin0, bintype const * _bin1, bintype const * _bin2, u32 maxbits);                                                // Finds the last free bit and returns the bit index
        s32 find_last_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits);                         // Finds the last free bit and sets it to used and returns the bit index
        s32 find_after(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, u32 maxbits, u32 pivot);   // Finds the first free bit after the pivot
        s32 find_before(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, u32 maxbits, u32 pivot);  // Finds the first free bit before the pivot (high to low)
    }  // namespace nbitvec15

    namespace nbitvec18
    {
        // max bits = 2^18 = 262144 bits
        // layer0 = u64 (6)
        // layer1 = an array of u64, max u64[64] (6)
        // layer2 = an array of u64, max u64[64*64] (6)
        typedef u64 bintype;

        void setup_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits);
        void tick_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits, u32 bit);

        void clear(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits);

        void set(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits, u32 bit);
        void clr(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits, u32 bit);
        bool get(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, u32 maxbits, u32 bit);
        s32  find(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, u32 maxbits);
        s32  find_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits);

        s32 find_last(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, u32 maxbits);               // Finds the last free bit and returns the bit index
        s32 find_last_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, u32 maxbits);                         // Finds the last free bit and sets it to used and returns the bit index
        s32 find_after(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, u32 maxbits, u32 pivot);   // Finds the first free bit after the pivot
        s32 find_before(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, u32 maxbits, u32 pivot);  // Finds the first free bit before the pivot (high to low)
    }  // namespace nbitvec18

    // --------------------------------------------------------------------------------------------
    // 4 level bit-vectors
    // --------------------------------------------------------------------------------------------

    namespace nbitvec20
    {
        // max 2^20 = 1048576 (1M) bits
        // layer0 = a single u32 (5,4,3)
        // layer1 = an array of u32, max u32[32] (5)
        // layer2 = an array of u32, max u32[32*32] (5)
        // layer3 = an array of u32, max u32[32*32*32] (5)

        typedef u32 bintype;

        void setup_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits);
        void tick_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits, u32 bit);

        void clear(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits);

        void set(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits, u32 bit);
        void clr(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits, u32 bit);
        bool get(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits, u32 bit);
        s32  find(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits);
        s32  find_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits);

        s32 find_last(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits);  // Finds the last free bit and returns the bit index
        s32 find_last_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits);                   // Finds the last free bit and sets it to used and returns the bit index
        s32 find_after(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits, u32 pivot);   // Finds the first free bit after the pivot
        s32 find_before(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits, u32 pivot);  // Finds the first free bit before the pivot (high to low)
    }  // namespace nbitvec20

    namespace nbitvec24
    {
        // max 2^24 = 16777216 (16M) bits
        // layer0 = a single u64 (6,5,4,3)
        // layer1 = an array of u64, max u64[64] (6)
        // layer2 = an array of u64, max u64[64*64] (6)
        // layer3 = an array of u64, max u64[64*64*64] (6)

        typedef u64 bintype;

        void setup_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits);
        void tick_lazy(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits, u32 bit);

        void clear(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits);

        void set(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits, u32 bit);
        void clr(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits, u32 bit);
        bool get(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits, u32 bit);
        s32  find(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits);
        s32  find_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits);

        s32 find_last(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits);  // Finds the last free bit and returns the bit index
        s32 find_last_and_remove(bintype* CC_RESTRICT bin0, bintype* CC_RESTRICT bin1, bintype* CC_RESTRICT bin2, bintype* CC_RESTRICT bin3, u32 maxbits);                   // Finds the last free bit and sets it to used and returns the bit index
        s32 find_after(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits, u32 pivot);   // Finds the first free bit after the pivot
        s32 find_before(bintype const * CC_RESTRICT bin0, bintype const * CC_RESTRICT bin1, bintype const * CC_RESTRICT bin2, bintype const * CC_RESTRICT bin3, u32 maxbits, u32 pivot);  // Finds the first free bit before the pivot (high to low)
    }  // namespace nbitvec24

}  // namespace ncore

#endif  // __CCORE_BITVEC_V2_H__

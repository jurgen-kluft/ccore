#ifndef __CCORE_DUOMAP_V2_H__
#define __CCORE_DUOMAP_V2_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    // --------------------------------------------------------------------------------------------
    // duomap functionality for 1, 2, 3, and 4 level duomaps
    // --------------------------------------------------------------------------------------------

    // --------------------------------------------------------------------------------------------
    // 2 level binmaps
    // --------------------------------------------------------------------------------------------

    // 2^10 duomap, can handle a maximum of 1024 bits.
    namespace nduomap10
    {
        // bin00 = a single u32 (5), tracking '0' bits
        // bin10 = a single u32 (5), tracking '1' bits
        // bin1 = an array of u32, max u32[32] (5)
        typedef u32 bintype;

        void setup_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);
        void tick_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);

        void clear(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);

        void set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);
        void clr(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);
        bool get(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 bit);

        s32 find0(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits);
        s32 find1(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits);
        s32 find0_last(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits);               // Finds the last '0' bit and returns the bit index
        s32 find0_after(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 pivot);   // Finds the first '0' bit after the pivot
        s32 find0_before(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 pivot);  // Finds the first '0' bit before the pivot (high to low)
        s32 find1_last(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits);               // Finds the last '1' bit and returns the bit index
        s32 find1_after(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 pivot);   // Finds the first '1' bit after the pivot
        s32 find1_before(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 pivot);  // Finds the first '1' bit before the pivot (high to low)

        s32 find0_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);
        s32 find1_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);
        s32 find0_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);  // Finds the last '0' bit and sets it to used and returns the bit index
        s32 find1_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);  // Finds the last '1' bit and sets it to used and returns the bit index
    }  // namespace nduomap10

    // 2^12 duomap, can handle a maximum of 4096 bits.
    namespace nduomap12
    {
        // bin00 = a single u64 (6), tracking '0' bits
        // bin10 = a single u64 (6), tracking '1' bits
        // bin1 = an array of u64, max u64[64] (6)
        typedef u64 bintype;

        void setup_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);
        void tick_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);

        void clear(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);

        void set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);
        void clr(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);
        bool get(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 bit);

        s32 find0(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits);
        s32 find1(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits);
        s32 find0_last(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits);               // Finds the last '0' bit and returns the bit index
        s32 find0_after(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 pivot);   // Finds the first '0' bit after the pivot
        s32 find0_before(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 pivot);  // Finds the first '0' bit before the pivot (high to low)
        s32 find1_last(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits);               // Finds the last '1' bit and returns the bit index
        s32 find1_after(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 pivot);   // Finds the first '1' bit after the pivot
        s32 find1_before(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 pivot);  // Finds the first '1' bit before the pivot (high to low)

        s32 find0_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);
        s32 find1_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);
        s32 find0_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);  // Finds the last '0' bit and sets it to used and returns the bit index
        s32 find1_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);  // Finds the last '1' bit and sets it to used and returns the bit index
    }  // namespace nduomap12

    // --------------------------------------------------------------------------------------------
    // 3 level duomaps
    // --------------------------------------------------------------------------------------------

    namespace nduomap15
    {
        // max 2^15 = 32768 bits
        // bin0 = u32 (5)
        // bin1 = an array of u32, max u32[32] (5)
        // bin2 = an array of u32, max u32[32*32*32] (5)
        typedef u32 bintype;

        void setup_lazy(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits);
        void tick_lazy(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit);

        void clear(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits);

        void set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit);
        void clr(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit);
        bool get(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 bit);

        s32 find0(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits);
        s32 find1(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits);
        s32 find0_last(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits);               // Finds the last '0' bit and returns the bit index
        s32 find0_after(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 pivot);   // Finds the first '0' bit after the pivot
        s32 find0_before(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 pivot);  // Finds the first '0' bit before the pivot (high to low)
        s32 find1_last(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits);               // Finds the last '1' bit and returns the bit index
        s32 find1_after(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 pivot);   // Finds the first '1' bit after the pivot
        s32 find1_before(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 pivot);  // Finds the first '1' bit before the pivot (high to low)

        s32 find0_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits);
        s32 find1_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits);
        s32 find0_last_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits);  // Finds the last '0' bit and sets it to used and returns the bit index
        s32 find1_last_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits);  // Finds the last '1' bit and sets it to used and returns the bit index
    }  // namespace nduomap15

    namespace nduomap18
    {
        // max bits = 2^18 = 262144 bits
        // bin0 = u64 (6)
        // bin1 = an array of u64, max u64[64] (6)
        // bin2 = an array of u64, max u64[64*64] (6)
        typedef u64 bintype;

        void setup_lazy(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits);
        void tick_lazy(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit);

        void clear(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits);

        void set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit);
        void clr(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits, u32 bit);
        bool get(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 bit);

        s32 find0(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits);
        s32 find1(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits);
        s32 find0_last(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits);               // Finds the last '0' bit and returns the bit index
        s32 find0_after(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 pivot);   // Finds the first '0' bit after the pivot
        s32 find0_before(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 pivot);  // Finds the first '0' bit before the pivot (high to low)
        s32 find1_last(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits);               // Finds the last '1' bit and returns the bit index
        s32 find1_after(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 pivot);   // Finds the first '1' bit after the pivot
        s32 find1_before(bintype const *_bin00, bintype const *_bin01, bintype const *_bin10, bintype const *_bin11, bintype const *_bin2, u32 maxbits, u32 pivot);  // Finds the first '1' bit before the pivot (high to low)

        s32 find0_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits);
        s32 find1_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits);
        s32 find0_last_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits);  // Finds the last '0' bit and sets it to used and returns the bit index
        s32 find1_last_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin10, bintype *_bin11, bintype *_bin2, u32 maxbits);  // Finds the last '1' bit and sets it to used and returns the bit index
    }  // namespace nduomap18

    // --------------------------------------------------------------------------------------------
    // 4 level duomaps
    // --------------------------------------------------------------------------------------------

    namespace nduomap20
    {
        // max 2^20 = 1048576 bits
        // bin00, bin10 = u32 (5)
        // bin01, bin11 = an array of u32, max u32[32] (5)
        // bin02, bin12 = an array of u32, max u32[32*32*32] (5)
        // bin3 = an array of u32, max u32[32*32*32*32] (5)
        typedef u32 bintype;

        void setup_lazy(bintype *_bin00, bintype *_bin01, bintype *_bin02, bintype *_bin10, bintype *_bin11, bintype *_bin12, bintype *_bin3, u32 maxbits);
        void tick_lazy(bintype *_bin00, bintype *_bin01, bintype *_bin02, bintype *_bin10, bintype *_bin11, bintype *_bin12, bintype *_bin3, u32 maxbits, u32 bit);

        void clear(bintype *_bin00, bintype *_bin01, bintype *_bin02, bintype *_bin10, bintype *_bin11, bintype *_bin12, bintype *_bin3, u32 maxbits);

        void set(bintype *_bin00, bintype *_bin01, bintype *_bin02, bintype *_bin10, bintype *_bin11, bintype *_bin12, bintype *_bin3, u32 maxbits, u32 bit);
        void clr(bintype *_bin00, bintype *_bin01, bintype *_bin02, bintype *_bin10, bintype *_bin11, bintype *_bin12, bintype *_bin3, u32 maxbits, u32 bit);
        bool get(bintype const *_bin00, bintype const *_bin01, bintype const *_bin02, bintype const *_bin10, bintype const *_bin11, bintype const *_bin12, bintype const *_bin3, u32 maxbits, u32 bit);

        s32 find0(bintype const *_bin00, bintype const *_bin01, bintype const *_bin02, bintype const *_bin10, bintype const *_bin11, bintype const *_bin12, bintype const *_bin3, u32 maxbits);
        s32 find1(bintype const *_bin00, bintype const *_bin01, bintype const *_bin02, bintype const *_bin10, bintype const *_bin11, bintype const *_bin12, bintype const *_bin3, u32 maxbits);
        s32 find0_last(bintype const *_bin00, bintype const *_bin01, bintype const *_bin02, bintype const *_bin10, bintype const *_bin11, bintype const *_bin12, bintype const *_bin3, u32 maxbits);
        s32 find0_after(bintype const *_bin00, bintype const *_bin01, bintype const *_bin02, bintype const *_bin10, bintype const *_bin11, bintype const *_bin12, bintype const *_bin3, u32 maxbits, u32 pivot);
        s32 find0_before(bintype const *_bin00, bintype const *_bin01, bintype const *_bin02, bintype const *_bin10, bintype const *_bin11, bintype const *_bin12, bintype const *_bin3, u32 maxbits, u32 pivot);
        s32 find1_last(bintype const *_bin00, bintype const *_bin01, bintype const *_bin02, bintype const *_bin10, bintype const *_bin11, bintype const *_bin12, bintype const *_bin3, u32 maxbits);
        s32 find1_after(bintype const *_bin00, bintype const *_bin01, bintype const *_bin02, bintype const *_bin10, bintype const *_bin11, bintype const *_bin12, bintype const *_bin3, u32 maxbits, u32 pivot);
        s32 find1_before(bintype const *_bin00, bintype const *_bin01, bintype const *_bin02, bintype const *_bin10, bintype const *_bin11, bintype const *_bin12, bintype const *_bin3, u32 maxbits, u32 pivot);

        s32 find0_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin02, bintype *_bin10, bintype *_bin11, bintype *_bin12, bintype *_bin3, u32 maxbits);
        s32 find1_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin02, bintype *_bin10, bintype *_bin11, bintype *_bin12, bintype *_bin3, u32 maxbits);
        s32 find0_last_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin02, bintype *_bin10, bintype *_bin11, bintype *_bin12, bintype *_bin3, u32 maxbits);
        s32 find1_last_and_set(bintype *_bin00, bintype *_bin01, bintype *_bin02, bintype *_bin10, bintype *_bin11, bintype *_bin12, bintype *_bin3, u32 maxbits);
    }  // namespace nduomap20

}  // namespace ncore

#endif  // __CCORE_BINMAPS_V2_H__

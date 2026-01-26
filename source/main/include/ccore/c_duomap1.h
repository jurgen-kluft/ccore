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

        void setup_free_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);
        void tick_free_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);
        void setup_used_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);
        void tick_used_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);

        void clear(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);

        void set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);
        void clr(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);
        bool get(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 bit);

        s32 find0(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits);
        s32 find0_and_set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);

        s32 find1(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits);
        s32 find1_and_set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);

        s32 find0_last(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);               // Finds the last '0' bit and returns the bit index
        s32 find0_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);       // Finds the last '0' bit and sets it to used and returns the bit index
        s32 find0_after(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot);   // Finds the first '0' bit after the pivot
        s32 find0_before(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot);  // Finds the first '0' bit before the pivot (high to low)

        s32 find1_last(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);               // Finds the last '1' bit and returns the bit index
        s32 find1_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);       // Finds the last '1' bit and sets it to used and returns the bit index
        s32 find1_after(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot);   // Finds the first '1' bit after the pivot
        s32 find1_before(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot);  // Finds the first '1' bit before the pivot (high to low)

    }  // namespace nduomap10

    // 2^12 duomap, can handle a maximum of 4096 bits.
    namespace nduomap12
    {
        // bin00 = a single u64 (6), tracking '0' bits
        // bin10 = a single u64 (6), tracking '1' bits
        // bin1 = an array of u64, max u64[64] (6)
        typedef u64 bintype;

        void setup_free_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);
        void tick_free_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);
        void setup_used_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);
        void tick_used_lazy(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);

        void clear(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);

        void set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);
        void clr(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits, u32 bit);
        bool get(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits, u32 bit);

        s32 find0(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits);
        s32 find0_and_set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);

        s32 find1(bintype const *bin00, bintype const *bin10, bintype const *bin1, u32 maxbits);
        s32 find1_and_set(bintype *bin00, bintype *bin10, bintype *bin1, u32 maxbits);

        s32 find0_last(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);               // Finds the last '0' bit and returns the bit index
        s32 find0_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);       // Finds the last '0' bit and sets it to used and returns the bit index
        s32 find0_after(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot);   // Finds the first '0' bit after the pivot
        s32 find0_before(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot);  // Finds the first '0' bit before the pivot (high to low)

        s32 find1_last(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);               // Finds the last '1' bit and returns the bit index
        s32 find1_last_and_set(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits);       // Finds the last '1' bit and sets it to used and returns the bit index
        s32 find1_after(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot);   // Finds the first '1' bit after the pivot
        s32 find1_before(bintype *_bin00, bintype *_bin10, bintype *_bin1, u32 maxbits, u32 pivot);  // Finds the first '1' bit before the pivot (high to low)

    }  // namespace nduomap10

}  // namespace ncore

#endif  // __CCORE_BINMAPS_V2_H__

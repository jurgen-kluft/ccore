#ifndef __CCORE_STATE_VECTOR_V2_H__
#define __CCORE_STATE_VECTOR_V2_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    // --------------------------------------------------------------------------------------------
    // state-vector functionality for 1, 2, 3, and 4 level state-vectors
    // --------------------------------------------------------------------------------------------
    // free = tracking '0' bits
    // used = tracking '1' bits

    // --------------------------------------------------------------------------------------------
    // 2 level state-vectors, can handle a maximum of 1024 bits.
    // --------------------------------------------------------------------------------------------

    // 2^10 state-vector, can handle a maximum of 1024 bits.
    namespace nstatevec10
    {
        // bin1 = an array of u32, max u32[32] (5)
        typedef u32 bintype;

        void setup_used_lazy(bintype* free0, bintype* used0, bintype* bin1, u32 maxbits);
        void tick_used_lazy(bintype* free0, bintype* used0, bintype* bin1, u32 maxbits, u32 bit);

        void clear_all_free(bintype* free0, bintype* used0, bintype* bin1, u32 maxbits);
        void clear_all_used(bintype* free0, bintype* used0, bintype* bin1, u32 maxbits);

        void set_used(bintype* free0, bintype* used0, bintype* bin1, u32 maxbits, u32 bit);
        void set_free(bintype* free0, bintype* used0, bintype* bin1, u32 maxbits, u32 bit);
        bool get(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits, u32 bit);

        s32 find_free(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits);
        s32 find_used(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits);
        s32 find_free_last(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits);               // Finds the last '0' bit and returns the bit index
        s32 find_free_after(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits, u32 pivot);   // Finds the first '0' bit after the pivot
        s32 find_free_before(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits, u32 pivot);  // Finds the first '0' bit before the pivot (high to low)
        s32 find_used_last(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits);               // Finds the last '1' bit and returns the bit index
        s32 find_used_after(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits, u32 pivot);   // Finds the first '1' bit after the pivot
        s32 find_used_before(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits, u32 pivot);  // Finds the first '1' bit before the pivot (high to low)

        s32 alloc(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits);       // Finds the first '0' bit and sets it to used and returns the bit index
        s32 free(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits);        // Finds the first '1' bit and sets it to free and returns the bit index
        s32 alloc_last(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits);  // Finds the last '0' bit and sets it to used and returns the bit index
        s32 free_last(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits);   // Finds the last '1' bit and sets it to used and returns the bit index
    }  // namespace nstatevec10

    // 2^12 state-vector, can handle a maximum of 4096 bits.
    namespace nstatevec12
    {
        // free0 = a single u64 (6), tracking '0' bits
        // used0 = a single u64 (6), tracking '1' bits
        // bin1 = an array of u64, max u64[64] (6)
        typedef u64 bintype;

        void setup_used_lazy(bintype* free0, bintype* used0, bintype* bin1, u32 maxbits);
        void tick_used_lazy(bintype* free0, bintype* used0, bintype* bin1, u32 maxbits, u32 bit);

        void clear_all_free(bintype* free0, bintype* used0, bintype* bin1, u32 maxbits);
        void clear_all_used(bintype* free0, bintype* used0, bintype* bin1, u32 maxbits);

        void set_used(bintype* free0, bintype* used0, bintype* bin1, u32 maxbits, u32 bit);
        void set_free(bintype* free0, bintype* used0, bintype* bin1, u32 maxbits, u32 bit);
        bool get(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits, u32 bit);

        s32 find_free(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits);
        s32 find_used(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits);
        s32 find_free_last(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits);               // Finds the last '0' bit and returns the bit index
        s32 find_free_after(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits, u32 pivot);   // Finds the first '0' bit after the pivot
        s32 find_free_before(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits, u32 pivot);  // Finds the first '0' bit before the pivot (high to low)
        s32 find_used_last(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits);               // Finds the last '1' bit and returns the bit index
        s32 find_used_after(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits, u32 pivot);   // Finds the first '1' bit after the pivot
        s32 find_used_before(bintype const * free0, bintype const * used0, bintype const * bin1, u32 maxbits, u32 pivot);  // Finds the first '1' bit before the pivot (high to low)

        s32 alloc(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits);
        s32 free(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits);
        s32 alloc_last(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits);  // Finds the last '0' bit and sets it to used and returns the bit index
        s32 free_last(bintype* _free0, bintype* _used0, bintype* _bin1, u32 maxbits);   // Finds the last '1' bit and sets it to used and returns the bit index
    }  // namespace nstatevec12

    // --------------------------------------------------------------------------------------------
    // 3 level duomaps
    // --------------------------------------------------------------------------------------------

    namespace nstatevec15
    {
        // max 2^15 = 32768 bits
        // bin0 = u32 (5)
        // bin1 = an array of u32, max u32[32] (5)
        // bin2 = an array of u32, max u32[32*32*32] (5)
        typedef u32 bintype;

        void setup_used_lazy(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits);
        void tick_used_lazy(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit);

        void clear_all_free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits);

        void set_used(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit);
        void set_free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit);
        bool get(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 bit);

        s32 find_free(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits);
        s32 find_used(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits);
        s32 find_free_last(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits);               // Finds the last '0' bit and returns the bit index
        s32 find_free_after(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot);   // Finds the first '0' bit after the pivot
        s32 find_free_before(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot);  // Finds the first '0' bit before the pivot (high to low)
        s32 find_used_last(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits);               // Finds the last '1' bit and returns the bit index
        s32 find_used_after(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot);   // Finds the first '1' bit after the pivot
        s32 find_used_before(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot);  // Finds the first '1' bit before the pivot (high to low)

        s32 alloc(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits);
        s32 free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits);
        s32 alloc_last(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits);  // Finds the last '0' bit and sets it to used and returns the bit index
        s32 free_last(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits);   // Finds the last '1' bit and sets it to used and returns the bit index
    }  // namespace nstatevec15

    namespace nstatevec18
    {
        // max bits = 2^18 = 262144 bits
        // bin0 = u64 (6)
        // bin1 = an array of u64, max u64[64] (6)
        // bin2 = an array of u64, max u64[64*64] (6)
        typedef u64 bintype;

        void setup_used_lazy(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits);
        void tick_used_lazy(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit);

        void clear_all_free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits);

        void set_used(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit);
        void set_free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits, u32 bit);
        bool get(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 bit);

        s32 find_free(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits);
        s32 find_used(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits);
        s32 find_free_last(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits);               // Finds the last '0' bit and returns the bit index
        s32 find_free_after(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot);   // Finds the first '0' bit after the pivot
        s32 find_free_before(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot);  // Finds the first '0' bit before the pivot (high to low)
        s32 find_used_last(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits);               // Finds the last '1' bit and returns the bit index
        s32 find_used_after(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot);   // Finds the first '1' bit after the pivot
        s32 find_used_before(bintype const * _free0, bintype const * _free1, bintype const * _used0, bintype const * _used1, bintype const * _bin2, u32 maxbits, u32 pivot);  // Finds the first '1' bit before the pivot (high to low)

        s32 alloc(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits);
        s32 free(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits);
        s32 alloc_last(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits);  // Finds the last '0' bit and sets it to used and returns the bit index
        s32 free_last(bintype* _free0, bintype* _free1, bintype* _used0, bintype* _used1, bintype* _bin2, u32 maxbits);   // Finds the last '1' bit and sets it to used and returns the bit index
    }  // namespace nstatevec18

    // --------------------------------------------------------------------------------------------
    // 4 level duomaps
    // --------------------------------------------------------------------------------------------

    namespace nstatevec20
    {
        // max 2^20 = 1048576 bits
        // free0, used0 = u32 (5)
        // free1, used1 = an array of u32, max u32[32] (5)
        // free2, used2 = an array of u32, max u32[32*32*32] (5)
        // bin3 = an array of u32, max u32[32*32*32*32] (5)
        typedef u32 bintype;

        void setup_used_lazy(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);
        void tick_used_lazy(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit);

        void clear_all_free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);
        void clear_all_used(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);

        void set_used(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit);
        void set_free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit);
        bool get(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 bit);

        s32 find_free(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits);
        s32 find_used(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits);
        s32 find_free_last(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits);
        s32 find_free_after(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot);
        s32 find_free_before(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot);
        s32 find_used_last(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits);
        s32 find_used_after(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot);
        s32 find_used_before(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot);

        s32 alloc(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);
        s32 free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);
        s32 alloc_last(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);
        s32 free_last(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);
    }  // namespace nstatevec20

    namespace nstatevec24
    {
        // max 2^24 = 16777216 bits
        // free0, used0 = u64 (6)
        // free1, used1 = an array of u64, max u64[64] (6)
        // free2, used2 = an array of u64, max u64[64*64*64] (6)
        // bin3 = an array of u64, max u64[64*64*64*64] (6)
        typedef u64 bintype;

        void setup_used_lazy(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);
        void tick_used_lazy(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit);

        void clear_all_free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);
        void clear_all_used(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);

        void set_used(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit);
        void set_free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits, u32 bit);
        bool get(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 bit);

        s32 find_free(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits);
        s32 find_used(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits);
        s32 find_free_last(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits);
        s32 find_free_after(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot);
        s32 find_free_before(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot);
        s32 find_used_last(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits);
        s32 find_used_after(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot);
        s32 find_used_before(bintype const * _free0, bintype const * _free1, bintype const * _free2, bintype const * _used0, bintype const * _used1, bintype const * _used2, bintype const * _bin3, u32 maxbits, u32 pivot);

        s32 alloc(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);
        s32 free(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);
        s32 alloc_last(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);
        s32 free_last(bintype* _free0, bintype* _free1, bintype* _free2, bintype* _used0, bintype* _used1, bintype* _used2, bintype* _bin3, u32 maxbits);
    }  // namespace nstatevec24

}  // namespace ncore

#endif  // __CCORE_STATE_VECTOR_V2_H__

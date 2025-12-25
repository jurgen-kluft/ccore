#ifndef __CCORE_BINMAP_V2_H__
#define __CCORE_BINMAP_V2_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    namespace nbinmap9
    {
        // bin0 = a single u64
        // bin1 = an array of u8, max u8[64]
        typedef u64 bin0type;
        typedef u8  bin1type;

        void setup_free_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits);
        void tick_free_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);
        void setup_used_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits);
        void tick_used_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);

        void clear(bin0type *bin0, bin1type *bin1, u32 maxbits);

        void set(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);
        void clr(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);
        bool get(bin0type const *bin0, bin1type const *bin1, u32 maxbits, u32 bit);
        s32  find(bin0type const *bin0, bin1type const *bin1, u32 maxbits);
        s32  find_and_set(bin0type *bin0, bin1type *bin1, u32 maxbits);
    }  // namespace nbinmap9

    namespace nbinmap10
    {
        // bin0 = a single u64
        // bin1 = an array of u16, max u16[64]
        typedef u64 bin0type;
        typedef u16 bin1type;

        void setup_free_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits);
        void tick_free_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);
        void setup_used_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits);
        void tick_used_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);

        void clear(bin0type *bin0, bin1type *bin1, u32 maxbits);

        void set(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);
        void clr(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);
        bool get(bin0type const *bin0, bin1type const *bin1, u32 maxbits, u32 bit);
        s32  find(bin0type const *bin0, bin1type const *bin1, u32 maxbits);
        s32  find_and_set(bin0type *bin0, bin1type *bin1, u32 maxbits);
    }  // namespace nbinmap10

    // 2^11 binmap, can handle a maximum of 2048 bits.
    namespace nbinmap11
    {
        // bin0 = a single u64
        // bin1 = an array of u32, max u32[64]
        typedef u64 bin0type;
        typedef u32 bin1type;

        void setup_free_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits);
        void tick_free_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);
        void setup_used_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits);
        void tick_used_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);

        void clear(bin0type *bin0, bin1type *bin1, u32 maxbits);

        void set(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);
        void clr(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);
        bool get(bin0type const *bin0, bin1type const *bin1, u32 maxbits, u32 bit);
        s32  find(bin0type const *bin0, bin1type const *bin1, u32 maxbits);
        s32  find_and_set(bin0type *bin0, bin1type *bin1, u32 maxbits);
    }  // namespace nbinmap11

    // 2^12 binmap, can handle a maximum of 4096 bits.
    namespace nbinmap12
    {
        // max 2^12 = 4096 bits
        // bin0 = a single u64
        // bin1 = an array of u64, max u64[64]
        typedef u64 bin0type;
        typedef u64 bin1type;

        void setup_free_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits);
        void tick_free_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);
        void setup_used_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits);
        void tick_used_lazy(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);

        void clear(bin0type *bin0, bin1type *bin1, u32 maxbits);

        void set(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);
        void clr(bin0type *bin0, bin1type *bin1, u32 maxbits, u32 bit);
        bool get(bin0type const *bin0, bin1type const *bin1, u32 maxbits, u32 bit);
        s32  find(bin0type const *bin0, bin1type const *bin1, u32 maxbits);
        s32  find_and_set(bin0type *bin0, bin1type *bin1, u32 maxbits);

    }  // namespace nbinmap12

    namespace nbinmap13
    {
        // max 2^13 = 8192 bits
        // bin0 = a single u32 (5)
        // bin1 = an array of u32, max u32[32] (5)
        // bin2 = an array of u8, max u8[32*32*8] (3)
        typedef u32 bin0type;
        typedef u32 bin1type;
        typedef u8  bin2type;

        void setup_free_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits);
        void tick_free_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits, u32 bit);

        void clear(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits);

        void set(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits, u32 bit);
        void clr(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits, u32 bit);
        bool get(bin0type const *bin0, bin1type const *bin1, bin2type const *bin2, u32 maxbits, u32 bit);
        s32  find(bin0type const *bin0, bin1type const *bin1, bin2type const *bin2, u32 maxbits);
        s32  find_and_set(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits);
    }  // namespace nbinmap13

    namespace nbinmap14
    {
        // max 2^14 = 16384 bits
        // bin0 = a single u32 (5)
        // bin1 = an array of u32, max u32[32] (5)
        // bin2 = an array of u16, max u16[32*32*16] (4)
    }

    namespace nbinmap15
    {
        // max 2^15 = 32768 bits
        // bin0 = a single u32 (5)
        // bin1 = an array of u32, max u32[32] (5)
        // bin2 = an array of u32, max u32[32*32*32] (5)

    }  // namespace nbinmap15

    namespace nbinmap16
    {
        // max bits = 2^16 = 65536 bits
        // bin0 = a single u64 (6)
        // bin1 = an array of u32, max u32[64] (5)
        // bin2 = an array of u32, max u32[64*32*32] (5)
    }

    namespace nbinmap17
    {
        // max 2^17 bits = 131072 bits
        // bin0 = a single u64 (6)
        // bin1 = an array of u64, max u64[64] (6)
        // bin2 = an array of u32, max u32[64*64*32] (5)

    }

    namespace nbinmap18
    {
        // max bits = 2^18 = 262144 bits
        // bin0 = a single u64 (6)
        // bin1 = an array of u64, max u64[64] (6)
        // bin2 = an array of u64, max u64[64*64] (6)

    }

}  // namespace ncore

#endif

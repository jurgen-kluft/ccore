#ifndef __CCORE_BINMAPS_V2_H__
#define __CCORE_BINMAPS_V2_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    // --------------------------------------------------------------------------------------------
    // binmap functionality for 1, 2, 3, and 4 level binmaps
    // --------------------------------------------------------------------------------------------
    namespace nbinmap
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

        void compute(u32 number_of_bits, layout64_t &l);
        void pointers(byte *ptr, layout64_t const &l, u64 *&bin0, u64 *&bin1, u64 *&bin2, u64 *&bin3);
        u32  sizeof_data(layout64_t const &l);  // u64[N], where N is computed based on layout
        // This will compute the data size in u64[N], where the full level is considered 'growable' up to 'bit'
        u32 sizeof_data(layout64_t const &l, u32 bit);

        struct layout32_t
        {
            u32 m_maxbits;  // maximum number of bits this layout can handle
            u32 m_bin3;     // number of u32 at this level (unit = number of u32)
            u32 m_bin2;     // number of u32 at this level (unit = number of u32)
            u16 m_bin1;     // number of u32 at this level (unit = number of u32, should be 2 <= N <= 32)
            u8  m_bin0;     // number of u32 at this level (unit = number of u32, should always be == 1)
            u8  m_levels;   // number of levels (0 = bin0, 1 = bin0+bin1, 2 = bin0+bin1+bin2, 3 = bin0+bin1+bin2+bin3)
        };
        void compute(u32 number_of_bits, layout32_t &l);
        void pointers(byte *ptr, layout32_t const &l, u32 *&bin0, u32 *&bin1, u32 *&bin2, u32 *&bin3);
        u32  sizeof_data(layout32_t const &l);  // u32[N], where N is computed based on layout
        // This will compute the data size in u32[N], where the full level is considered 'growable' up to 'bit'
        u32 sizeof_data(layout32_t const &l, u32 bit);

    }  // namespace nbinmap

    // --------------------------------------------------------------------------------------------
    // 1 level binmaps
    // --------------------------------------------------------------------------------------------

    // 2^6 binmap, can handle a maximum of 64 bits.
    namespace nbinmap6
    {
        typedef u64 bin0type;

        void set(bin0type *bin0, u32 maxbits, u32 bit);
        void clr(bin0type *bin0, u32 maxbits, u32 bit);
        bool get(bin0type const *bin0, u32 maxbits, u32 bit);
        s32  find(bin0type const *bin0, u32 maxbits);
        s32  find_and_set(bin0type *bin0, u32 maxbits);
    }  // namespace nbinmap6

    // --------------------------------------------------------------------------------------------
    // 2 level binmaps
    // --------------------------------------------------------------------------------------------

    // 2^10 binmap, can handle a maximum of 1024 bits.
    namespace nbinmap10
    {
        // bin0 = a single u32 (5)
        // bin1 = an array of u32, max u32[32] (5)
        typedef u32 bin0type;
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
    }  // namespace nbinmap10

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

    // --------------------------------------------------------------------------------------------
    // 3 level binmaps
    // --------------------------------------------------------------------------------------------

    namespace nbinmap15
    {
        // max 2^15 = 32768 bits
        // bin0 = u32 (5)
        // bin1 = an array of u32, max u32[32] (5)
        // bin2 = an array of u32, max u32[32*32*32] (5)
        typedef u32 bin0type;
        typedef u32 bin1type;
        typedef u32 bin2type;

        void setup_free_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits);
        void tick_free_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits, u32 bit);
        void setup_used_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits);
        void tick_used_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits, u32 bit);

        void clear(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits);

        void set(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits, u32 bit);
        void clr(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits, u32 bit);
        bool get(bin0type const *bin0, bin1type const *bin1, bin2type const *bin2, u32 maxbits, u32 bit);
        s32  find(bin0type const *bin0, bin1type const *bin1, bin2type const *bin2, u32 maxbits);
        s32  find_and_set(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits);
    }  // namespace nbinmap15

    namespace nbinmap18
    {
        // max bits = 2^18 = 262144 bits
        // bin0 = u64 (6)
        // bin1 = an array of u64, max u64[64] (6)
        // bin2 = an array of u64, max u64[64*64] (6)
        typedef u64 bin0type;
        typedef u64 bin1type;
        typedef u64 bin2type;

        void setup_free_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits);
        void tick_free_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits, u32 bit);
        void setup_used_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits);
        void tick_used_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits, u32 bit);

        void clear(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits);

        void set(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits, u32 bit);
        void clr(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits, u32 bit);
        bool get(bin0type const *bin0, bin1type const *bin1, bin2type const *bin2, u32 maxbits, u32 bit);
        s32  find(bin0type const *bin0, bin1type const *bin1, bin2type const *bin2, u32 maxbits);
        s32  find_and_set(bin0type *bin0, bin1type *bin1, bin2type *bin2, u32 maxbits);
    }  // namespace nbinmap18

    // --------------------------------------------------------------------------------------------
    // 4 level binmaps
    // --------------------------------------------------------------------------------------------

    namespace nbinmap20
    {
        // max 2^20 = 1048576 (1M) bits
        // bin0 = a single u32 (5,4,3)
        // bin1 = an array of u32, max u32[32] (5)
        // bin2 = an array of u32, max u32[32*32] (5)
        // bin3 = an array of u32, max u32[32*32*32] (5)

        typedef u32 bin0type;
        typedef u32 bin1type;
        typedef u32 bin2type;
        typedef u32 bin3type;

        void setup_free_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits);
        void tick_free_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits, u32 bit);
        void setup_used_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits);
        void tick_used_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits, u32 bit);

        void clear(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits);

        void set(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits, u32 bit);
        void clr(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits, u32 bit);
        bool get(bin0type const *bin0, bin1type const *bin1, bin2type const *bin2, bin3type const *bin3, u32 maxbits, u32 bit);
        s32  find(bin0type const *bin0, bin1type const *bin1, bin2type const *bin2, bin3type const *bin3, u32 maxbits);
        s32  find_and_set(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits);
    }  // namespace nbinmap20

    namespace nbinmap24
    {
        // max 2^24 = 16777216 (16M) bits
        // bin0 = a single u64 (6,5,4,3)
        // bin1 = an array of u64, max u64[64] (6)
        // bin2 = an array of u64, max u64[64*64] (6)
        // bin3 = an array of u64, max u64[64*64*64] (6)

        typedef u64 bin0type;
        typedef u64 bin1type;
        typedef u64 bin2type;
        typedef u64 bin3type;

        void setup_free_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits);
        void tick_free_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits, u32 bit);
        void setup_used_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits);
        void tick_used_lazy(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits, u32 bit);

        void clear(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits);

        void set(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits, u32 bit);
        void clr(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits, u32 bit);
        bool get(bin0type const *bin0, bin1type const *bin1, bin2type const *bin2, bin3type const *bin3, u32 maxbits, u32 bit);
        s32  find(bin0type const *bin0, bin1type const *bin1, bin2type const *bin2, bin3type const *bin3, u32 maxbits);
        s32  find_and_set(bin0type *bin0, bin1type *bin1, bin2type *bin2, bin3type *bin3, u32 maxbits);
    }  // namespace nbinmap24

}  // namespace ncore

#endif  // __CCORE_BINMAPS_V2_H__

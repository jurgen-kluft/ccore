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
        
        void setup_free_lazy(u64 *bin0, u8 *bin1, u32 maxbits);
        void tick_free_lazy(u64 *bin0, u8 *bin1, u32 maxbits, u32 bit);
        void setup_used_lazy(u64 *bin0, u8 *bin1, u32 maxbits);
        void tick_used_lazy(u64 *bin0, u8 *bin1, u32 maxbits, u32 bit);

        void clear(u64 *bin0, u8 *bin1, u32 maxbits);

        void set(u64 *bin0, u8 *bin1, u32 maxbits, u32 bit);
        void clr(u64 *bin0, u8 *bin1, u32 maxbits, u32 bit);
        bool get(u64 const *bin0, u8 const *bin1, u32 maxbits, u32 bit);
        s32  find(u64 const *bin0, u8 const *bin1, u32 maxbits);
        s32  find_and_set(u64 *bin0, u8 *bin1, u32 maxbits);
    };  // namespace nbinmap9

    namespace nbinmap10
    {
        // bin0 = a single u64
        // bin1 = an array of u8, max u8[64]

        void setup_free_lazy(u64 *bin0, u16 *bin1, u32 maxbits);
        void tick_free_lazy(u64 *bin0, u16 *bin1, u32 maxbits, u32 bit);
        void setup_used_lazy(u64 *bin0, u16 *bin1, u32 maxbits);
        void tick_used_lazy(u64 *bin0, u16 *bin1, u32 maxbits, u32 bit);

        void clear(u64 *bin0, u16 *bin1, u32 maxbits);

        void set(u64 *bin0, u16 *bin1, u32 maxbits, u32 bit);
        void clr(u64 *bin0, u16 *bin1, u32 maxbits, u32 bit);
        bool get(u64 const *bin0, u16 const *bin1, u32 maxbits, u32 bit);
        s32  find(u64 const *bin0, u16 const *bin1, u32 maxbits);
        s32  find_and_set(u64 *bin0, u16 *bin1, u32 maxbits);
    };  // namespace nbinmap10

    // 2^11 binmap, can handle a maximum of 2048 bits.
    namespace nbinmap11
    {
        // bin0 = a single u64
        // bin1 = an array of u32, max u32[64]

        void setup_free_lazy(u64 *bin0, u32 *bin1, u32 maxbits);
        void tick_free_lazy(u64 *bin0, u32 *bin1, u32 maxbits, u32 bit);
        void setup_used_lazy(u64 *bin0, u32 *bin1, u32 maxbits);
        void tick_used_lazy(u64 *bin0, u32 *bin1, u32 maxbits, u32 bit);

        void clear(u64 *bin0, u32 *bin1, u32 maxbits);

        void set(u64 *bin0, u32 *bin1, u32 maxbits, u32 bit);
        void clr(u64 *bin0, u32 *bin1, u32 maxbits, u32 bit);
        bool get(u64 const *bin0, u32 const *bin1, u32 maxbits, u32 bit);
        s32  find(u64 const *bin0, u32 const *bin1, u32 maxbits);
        s32  find_and_set(u64 *bin0, u32 *bin1, u32 maxbits);
    };  // namespace nbinmap11

    // 2^12 binmap, can handle a maximum of 4096 bits.
    namespace nbinmap12
    {
        // bin0 = a single u64
        // bin1 = an array of u64, max u64[64]

        void setup_free_lazy(u64 *bin0, u64 *bin1, u32 maxbits);
        void tick_free_lazy(u64 *bin0, u64 *bin1, u32 maxbits, u32 bit);
        void setup_used_lazy(u64 *bin0, u64 *bin1, u32 maxbits);
        void tick_used_lazy(u64 *bin0, u64 *bin1, u32 maxbits, u32 bit);

        void clear(u64 *bin0, u64 *bin1, u32 maxbits);

        void set(u64 *bin0, u64 *bin1, u32 maxbits, u32 bit);
        void clr(u64 *bin0, u64 *bin1, u32 maxbits, u32 bit);
        bool get(u64 const *bin0, u64 const *bin1, u32 maxbits, u32 bit);
        s32  find(u64 const *bin0, u64 const *bin1, u32 maxbits);
        s32  find_and_set(u64 *bin0, u64 *bin1, u32 maxbits);
    };  // namespace nbinmap12

}  // namespace ncore

#endif

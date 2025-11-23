#ifndef __CCORE_ENDIAN_H__
#define __CCORE_ENDIAN_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    namespace nendian_be
    {
        // Read from memory (big-endian)
        inline u16 read_u16(u8 const* p)
        {
            const u8 b0 = p[0];
            const u8 b1 = p[1];
            return (u16)((b0 << 8) | b1);
        }

        inline u32 read_u32(u8 const* p)
        {
            const u16 s0 = read_u16(p);
            const u16 s1 = read_u16(p + 2);
            return ((u32)s0 << 16) | (u32)s1;
        }

        inline f32 read_f32(u8 const* p)
        {
            u32 iv = read_u32(p);
            return *(f32*)&iv;
        }

        inline u64 read_u64(u8 const* p)
        {
            const u32 w0 = read_u32(p);
            const u32 w1 = read_u32(p + 4);
            return ((u64)w0 << 32) | (u64)w1;
        }

        inline f64 read_f64(u8 const* p)
        {
            u64 iv = read_u64(p);
            return *(f64*)&iv;
        }

        inline s16 read_s16(u8 const* p) { return (s16)read_u16(p); }
        inline s32 read_s32(u8 const* p) { return (s32)read_u32(p); }
        inline s64 read_s64(u8 const* p) { return (s64)read_u64(p); }

    };  // namespace nendian_be

    namespace nendian_le
    {
        // Read from memory (little-endian)
        inline u16 read_u16(u8 const* p)
        {
            const u8 b0 = p[0];
            const u8 b1 = p[1];
            return (u16)((b1 << 8) | b0);
        }

        inline u32 read_u32(u8 const* p)
        {
            const u16 s0 = read_u16(p);
            const u16 s1 = read_u16(p + 2);
            return ((u32)s1 << 16) | (u32)s0;
        }

        inline f32 read_f32(u8 const* p)
        {
            u32 iv = read_u32(p);
            return *(f32*)&iv;
        }

        inline u64 read_u64(u8 const* p)
        {
            const u32 w0 = read_u32(p);
            const u32 w1 = read_u32(p + 4);
            return ((u64)w1 << 32) | (u64)w0;
        }

        inline f64 read_f64(u8 const* p)
        {
            u64 iv = read_u64(p);
            return *(f64*)&iv;
        }

        inline s16 read_s16(u8 const* p) { return (s16)read_u16(p); }
        inline s32 read_s32(u8 const* p) { return (s32)read_u32(p); }
        inline s64 read_s64(u8 const* p) { return (s64)read_u64(p); }

    };  // namespace nendian_le


#ifdef D_LITTLE_ENDIAN
    namespace nendian    = nendian_le;
#else
    namespace nendian    = nendian_be;
#endif

    // Network byte order is always Big Endian.
    namespace nendian_ne = nendian_be;
};  // namespace ncore

#endif  // __CCORE_ENDIAN_H__

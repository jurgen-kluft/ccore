#ifndef __CCORE_DEFAULT_HASH_H__
#define __CCORE_DEFAULT_HASH_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    namespace nhash
    {
        u32 datahash32(u8 const* data, u32 size, u32 seed = 0);
        u64 datahash64(u8 const* data, u32 size, u64 seed = 0);

        u32 strhash32(const char* str, u32 seed = 0);
        u32 strhash32(const char* str, const char* end, u32 seed = 0);
        u32 strhash32_lowercase(const char* str, u32 seed = 0);
        u32 strhash32_lowercase(const char* str, const char* end, u32 seed = 0);

        u64 strhash64(const char* str, u64 seed = 0);
        u64 strhash64(const char* str, const char* end, u64 seed = 0);
        u64 strhash64_lowercase(const char* str, u64 seed = 0);
        u64 strhash64_lowercase(const char* str, const char* end, u64 seed = 0);
    }  // namespace nhash
}  // namespace ncore

#endif  // __CCORE_DEFAULT_HASH_H__

#ifndef __CCORE_BINARY_SEARCH_H__
#define __CCORE_BINARY_SEARCH_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

namespace ncore
{
    typedef bool (*less_predicate_fn)(const void* key, const void* array, u32 index, const void* user_data); // less predicate
    typedef bool (*equal_predicate_fn)(const void* key, const void* array, u32 index, const void* user_data); // equal predicate

    // Opaque type (e.g. struct, class, etc.)
    s32 g_BinarySearch(void const *array, u32 array_size, const void* key, const void* user_data, less_predicate_fn is_less, equal_predicate_fn is_equal);

    // Type T (is required to have operator< and operator==)
    template <typename T>
    inline s32 g_BinarySearchT(const T* array, u32 array_size, const T& key)
    {
        if (array_size == 0)
            return -1;

        u32 bot = 0;
        u32 top = array_size;
        while (top > 1)
        {
            u32 const mid = top >> 1;
            if (!(key < array[bot + mid]))
            {
				bot += mid;
			}
            top -= mid;
        }

        if (key == array[bot])
        {
            return bot;
        }
        return -1;
    }


    inline s32 g_BinarySearch(s8 const* array, u32 array_size, s8 key) { return g_BinarySearchT<s8>(array, array_size, key); }
    inline s32 g_BinarySearch(s16 const* array, u32 array_size, s16 key) { return g_BinarySearchT<s16>(array, array_size, key); }
    inline s32 g_BinarySearch(s32 const* array, u32 array_size, s32 key) { return g_BinarySearchT<s32>(array, array_size, key); }
    inline s32 g_BinarySearch(f32 const* array, u32 array_size, f32 key) { return g_BinarySearchT<f32>(array, array_size, key); }
    inline s32 g_BinarySearch(s64 const* array, u32 array_size, s64 key) { return g_BinarySearchT<s64>(array, array_size, key); }

    inline s32 g_BinarySearch(u8 const* array, u32 array_size, u8 key) { return g_BinarySearchT<u8>(array, array_size, key); }
    inline s32 g_BinarySearch(u16 const* array, u32 array_size, u16 key) { return g_BinarySearchT<u16>(array, array_size, key); }
    inline s32 g_BinarySearch(u32 const* array, u32 array_size, u32 key) { return g_BinarySearchT<u32>(array, array_size, key); }
    inline s32 g_BinarySearch(u64 const* array, u32 array_size, u64 key) { return g_BinarySearchT<u64>(array, array_size, key); }
    inline s32 g_BinarySearch(f64 const* array, u32 array_size, f64 key) { return g_BinarySearchT<f64>(array, array_size, key); }


}; // namespace ncore

#endif ///< __CCORE_BINARY_SEARCH_H__

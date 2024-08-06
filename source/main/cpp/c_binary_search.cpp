#include "ccore/c_binary_search.h"

namespace ncore
{
    // Mono Bound binary search algorithm
    
    template <typename T>
    s32 g_BinarySearch_mb_T(T const* array, u32 array_size, T key)
    {
        if (array_size == 0)
            return -1;

        u32 bot = 0;
        u32 top = array_size;
        while (top > 1)
        {
            u32 const mid = top >> 1;
            if (key >= array[bot + mid])
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

    template <typename T>
    s32 g_BinarySearch_mb_T_P(T const* array, u32 array_size, const void* key, const void* user_data, less_predicate_fn is_less, equal_predicate_fn is_equal)
    {
        if (array_size == 0)
            return -1;

        u32 bot = 0;
        u32 top = array_size;
        while (top > 1)
        {
            u32 const mid = top >> 1;
            //if (key >= array[bot + mid])
            if (!is_less(key, array, bot + mid))
            {
                bot += mid;
            }
            top -= mid;
        }

        // if (key == array[bot])
        if (is_equal(key, array, bot))
        {
            return bot;
        }
        return -1;
    }

    s32 g_BinarySearch(s16 const* array, u32 array_size, s16 key) { return g_BinarySearch_mb_T(array, array_size, key); }
    s32 g_BinarySearch(s32 const* array, u32 array_size, s32 key) { return g_BinarySearch_mb_T(array, array_size, key); }
    s32 g_BinarySearch(s64 const* array, u32 array_size, s64 key) { return g_BinarySearch_mb_T(array, array_size, key); }
    s32 g_BinarySearch(u16 const* array, u32 array_size, u16 key) { return g_BinarySearch_mb_T(array, array_size, key); }
    s32 g_BinarySearch(u32 const* array, u32 array_size, u32 key) { return g_BinarySearch_mb_T(array, array_size, key); }
    s32 g_BinarySearch(u64 const* array, u32 array_size, u64 key) { return g_BinarySearch_mb_T(array, array_size, key); }

    s32 g_BinarySearch(s16 const *array, u32 array_size, const void* key, const void* user_data, less_predicate_fn less, equal_predicate_fn equal) { return g_BinarySearch_mb_T_P(array, array_size, key, user_data, less, equal); }
    s32 g_BinarySearch(s32 const *array, u32 array_size, const void* key, const void* user_data, less_predicate_fn less, equal_predicate_fn equal) { return g_BinarySearch_mb_T_P(array, array_size, key, user_data, less, equal); }
    s32 g_BinarySearch(s64 const *array, u32 array_size, const void* key, const void* user_data, less_predicate_fn less, equal_predicate_fn equal) { return g_BinarySearch_mb_T_P(array, array_size, key, user_data, less, equal); }

};  // namespace ncore

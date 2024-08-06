#ifndef __CCORE_BINARY_SEARCH_H__
#define __CCORE_BINARY_SEARCH_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

namespace ncore
{
    typedef s8 (*compare_predicate_fn)(const void* key, const void* array, s64 index);
    typedef bool (*less_predicate_fn)(const void* key, const void* array, u32 index, const void* user_data); // less predicate
    typedef bool (*equal_predicate_fn)(const void* key, const void* array, u32 index, const void* user_data); // equal predicate

    s32 g_BinarySearch(s16 const *array, u32 array_size, s16 key);
    s32 g_BinarySearch(s32 const *array, u32 array_size, s32 key);
    s32 g_BinarySearch(s64 const *array, u32 array_size, s64 key);

    s32 g_BinarySearch(u16 const *array, u32 array_size, u16 key);
    s32 g_BinarySearch(u32 const *array, u32 array_size, u32 key);
    s32 g_BinarySearch(u64 const *array, u32 array_size, u64 key);

    s32 g_BinarySearch(s16 const *array, u32 array_size, const void* key, const void* user_data, less_predicate_fn less, equal_predicate_fn equal);
    s32 g_BinarySearch(s32 const *array, u32 array_size, const void* key, const void* user_data, less_predicate_fn less, equal_predicate_fn equal);
    s32 g_BinarySearch(s64 const *array, u32 array_size, const void* key, const void* user_data, less_predicate_fn less, equal_predicate_fn equal);

}; // namespace ncore

#endif ///< __CCORE_BINARY_SEARCH_H__

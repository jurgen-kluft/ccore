#ifndef __CCORE_QUICK_SORT_H__
#define __CCORE_QUICK_SORT_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#pragma once
#endif

#include "ccore/c_debug.h"

namespace ncore
{
    namespace nsort
    {
        //----------------------------------------------------------------------------------------------------------------
        // Custom Sort (QuickSort)

        // element_array, element_count, element_size, compare delegate, user_data
        extern void sort(void *a, u32 ec, u32 es, s8 (*cmp)(const void *, const void *, const void *), const void *user_data = 0);

        // element_array (2 bytes, u16, s16), element_count, compare delegate, user_data
        extern void sort(u16 *a, u32 n);
        extern void sort(s16 *a, u32 n);

        extern void sort(u16 *a, u32 n, s8 (*cmp)(const void *, const void *, const void *), const void *user_data = 0);
        extern void sort(s16 *a, u32 n, s8 (*cmp)(const void *, const void *, const void *), const void *user_data = 0);

        // element_array (4 bytes, u32, s32), element_count, compare delegate, user_data
        extern void sort(u32 *a, u32 n);
        extern void sort(s32 *a, u32 n);
        extern void sort(f32 *a, u32 n);

        extern void sort(u32 *a, u32 n, s8 (*cmp)(const void *, const void *, const void *), const void *user_data = 0);
        extern void sort(s32 *a, u32 n, s8 (*cmp)(const void *, const void *, const void *), const void *user_data = 0);
        extern void sort(f32 *a, u32 n, s8 (*cmp)(const void *, const void *, const void *), const void *user_data = 0);

        // element_array (8 bytes, u64, s64), element_count
        extern void sort(u64 *a, u32 n);
        extern void sort(s64 *a, u32 n);
        extern void sort(f64 *a, u32 n);

        extern void sort(u64 *a, u32 n, s8 (*cmp)(const void *, const void *, const void *), const void *user_data = 0);
        extern void sort(s64 *a, u32 n, s8 (*cmp)(const void *, const void *, const void *), const void *user_data = 0);
        extern void sort(f64 *a, u32 n, s8 (*cmp)(const void *, const void *, const void *), const void *user_data = 0);

        template <typename T>
        inline s8 generic_compare(const void *_lhs, const void *_rhs, const void *_user_data)
        {
            T const *const lhs = (T const *)_lhs;
            T const *const rhs = (T const *)_rhs;
            if (*lhs < *rhs)
                return -1;
            if (*lhs > *rhs)
                return 1;
            return 0;
        };

        template <typename T>
        inline void sort(T *a, u32 n)
        {
            sort((void *)a, n, (u32)sizeof(T), generic_compare<T>, nullptr);
        }

        template <typename T>
        inline void sort(T *a, u32 n, s8 (*cmp)(const void *, const void *, const void *), const void *user_data = nullptr)
        {
            sort((void *)a, n, (u32)sizeof(T), cmp, user_data);
        }
    }
}; // namespace ncore

#endif

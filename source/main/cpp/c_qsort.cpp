#include "ccore/c_qsort.h"

namespace ncore
{
    //----------------------------------------------------------------------------------------------------------------
    // Custom QuickSort
    namespace __qsort
    {
        template <typename T>
        static inline T sMinimum(T a, T b)
        {
            return (a) < (b) ? a : b;
        }

        template <typename T>
        static inline void sSwap(T *a, T *b, s32 n)
        {
            while (n > 0)
            {
                T t = *a;
                *a++ = *b;
                *b++ = t;
                --n;
            }
        }

        template <typename T>
        static inline T *sMed3(T *a, T *b, T *c, s8 (*cmp)(const void *const, const void *const, const void *), const void *data)
        {
            return cmp(a, b, data) < 0 ? (cmp(b, c, data) < 0 ? b : (cmp(a, c, data) < 0 ? c : a)) : (cmp(b, c, data) > 0 ? b : (cmp(a, c, data) < 0 ? a : c));
        }

        template <typename T>
        static inline s8 sCmp(T *a, T *b)
        {
            return *a < *b ? -1 : *a > *b ? 1
                                          : 0;
        }

        template <typename T>
        static inline T *sMed3T(T *a, T *b, T *c)
        {
            return sCmp(a, b) < 0 ? (sCmp(b, c) < 0 ? b : (sCmp(a, c) < 0 ? c : a)) : (sCmp(b, c) > 0 ? b : (sCmp(a, c) < 0 ? a : c));
        }

    } // namespace __qsort

    // Unsigned 16, 32, 64 bit integer
    namespace nsort
    {
        // Sort routine for element size other than 2, 4, or 8 bytes
        void sortN(u8 *a, u32 n, u32 es, s8 (*cmp)(const void *, const void *, const void *), const void *user_data)
        {
            u8 *pa, *pb, *pc, *pd, *pl, *pm, *pn;
            s32 d, r, swap_cnt;
        loop:
            swap_cnt = 0;
            if (n < 7)
            {
                for (pm = (u8 *)a + es; pm < (u8 *)a + n * es; pm += es)
                    for (pl = pm; pl > (u8 *)a && cmp(pl - es, pl, user_data) > 0; pl -= es)
                        __qsort::sSwap(pl, pl - es, es);
                return;
            }
            pm = (u8 *)a + (n / 2) * es;
            if (n > 7)
            {
                pl = (u8 *)a;
                pn = (u8 *)a + (n - 1) * es;
                if (n > 40)
                {
                    d = (n / 8) * es;
                    pl = __qsort::sMed3(pl, pl + d, pl + 2 * d, cmp, user_data);
                    pm = __qsort::sMed3(pm - d, pm, pm + d, cmp, user_data);
                    pn = __qsort::sMed3(pn - 2 * d, pn - d, pn, cmp, user_data);
                }
                pm = __qsort::sMed3(pl, pm, pn, cmp, user_data);
            }
            __qsort::sSwap((u8 *)a, pm, es);
            pa = pb = (u8 *)a + es;
            pc = pd = (u8 *)a + (n - 1) * es;
            for (;;)
            {
                while (pb <= pc && (r = cmp(pb, (u8 *)a, user_data)) <= 0)
                {
                    if (r == 0)
                    {
                        swap_cnt = 1;
                        __qsort::sSwap(pa, pb, es);
                        pa += es;
                    }
                    pb += es;
                }

                while (pb <= pc && (r = cmp(pc, (u8 *)a, user_data)) >= 0)
                {
                    if (r == 0)
                    {
                        swap_cnt = 1;
                        __qsort::sSwap(pc, pd, es);
                        pd -= es;
                    }
                    pc -= es;
                }

                if (pb > pc)
                    break;

                __qsort::sSwap(pb, pc, es);
                swap_cnt = 1;
                pb += es;
                pc -= es;
            }

            if (swap_cnt == 0) // Switch to insertion sort
            {
                for (pm = (u8 *)a + es; pm < (u8 *)a + n * es; pm += es)
                    for (pl = pm; pl > (u8 *)a && cmp(pl - es, pl, user_data) > 0; pl -= es)
                        __qsort::sSwap(pl, pl - es, es);
                return;
            }

            pn = (u8 *)a + n * es;
            r = (s32)(__qsort::sMinimum(pa - (u8 *)a, pb - pa));
            //__qsort_VecSwap((u8*)a, pb - r, r);
            __qsort::sSwap((u8 *)a, pb - r, r);
            r = (s32)(__qsort::sMinimum(pd - pc, pn - pd - es));
            //__qsort_VecSwap(pb, pn - r, r);
            __qsort::sSwap(pb, pn - r, r);

            if ((r = (s32)(pb - pa)) > (s32)es)
                sortN((u8 *)a, r / es, es, cmp, user_data);

            if ((r = (s32)(pd - pc)) > (s32)es)
            {
                // Iterate rather than recurse to save stack space
                a = pn - r;
                n = r / es;
                goto loop;
            }
        }

        template <typename T>
        void sortT(T *a, u32 n)
        {
            T *pa, *pb, *pc, *pd, *pl, *pm, *pn;
            s32 d, r, swap_cnt;
        loop:
            swap_cnt = 0;
            if (n < 7)
            {
                for (pm = (T *)a + 1; pm < (T *)a + n * 1; pm += 1)
                    for (pl = pm; pl > (T *)a && __qsort::sCmp(pl - 1, pl) > 0; pl -= 1)
                        __qsort::sSwap(pl, pl - 1, 1);
                return;
            }
            pm = a + (n / 2) * 1;
            if (n > 7)
            {
                pl = a;
                pn = a + (n - 1) * 1;
                if (n > 40)
                {
                    d = (n / 8) * 1;
                    pl = __qsort::sMed3T(pl, pl + d, pl + 2 * d);
                    pm = __qsort::sMed3T(pm - d, pm, pm + d);
                    pn = __qsort::sMed3T(pn - 2 * d, pn - d, pn);
                }
                pm = __qsort::sMed3T(pl, pm, pn);
            }
            __qsort::sSwap(a, pm, 1);
            pa = pb = a + 1;
            pc = pd = a + (n - 1) * 1;
            for (;;)
            {
                while (pb <= pc && (r = __qsort::sCmp(pb, a)) <= 0)
                {
                    if (r == 0)
                    {
                        swap_cnt = 1;
                        __qsort::sSwap(pa, pb, 1);
                        pa += 1;
                    }
                    pb += 1;
                }

                while (pb <= pc && (r = __qsort::sCmp(pc, a)) >= 0)
                {
                    if (r == 0)
                    {
                        swap_cnt = 1;
                        __qsort::sSwap(pc, pd, 1);
                        pd -= 1;
                    }
                    pc -= 1;
                }

                if (pb > pc)
                    break;

                __qsort::sSwap(pb, pc, 1);
                swap_cnt = 1;
                pb += 1;
                pc -= 1;
            }

            if (swap_cnt == 0) // Switch to insertion sort
            {
                for (pm = a + 1; pm < a + n * 1; pm += 1)
                    for (pl = pm; pl > a && (__qsort::sCmp(pl - 1, pl)) > 0; pl -= 1)
                        __qsort::sSwap(pl, pl - 1, 1);
                return;
            }

            pn = a + n * 1;
            r = (s32)(__qsort::sMinimum(pa - a, pb - pa));
            __qsort::sSwap(a, pb - r, r);
            r = (s32)(__qsort::sMinimum(pd - pc, pn - pd - 1));
            __qsort::sSwap(pb, pn - r, r);

            if ((r = (s32)(pb - pa)) > 1)
                sortT(a, r / 1);

            if ((r = (s32)(pd - pc)) > 1)
            {
                // Iterate rather than recurse to save stack space
                a = pn - r;
                n = r;
                goto loop;
            }
        }

        void sort(u16 *a, u32 n) { sortT<u16>(a, n); }
        void sort(u32 *a, u32 n) { sortT<u32>(a, n); }
        void sort(u64 *a, u32 n) { sortT<u64>(a, n); }

        // Signed 16, 32, 64 bit integer

        void sort(s16 *a, u32 n) { sortT<s16>(a, n); }
        void sort(s32 *a, u32 n) { sortT<s32>(a, n); }
        void sort(s64 *a, u32 n) { sortT<s64>(a, n); }

        // Floating point 32, 64 bit

        void sort(f32 *a, u32 n) { sortT<f32>(a, n); }
        void sort(f64 *a, u32 n) { sortT<f64>(a, n); }

        // Generic QuickSort

        void sort(void *a, u32 n, u32 es, s8 (*cmp)(const void *, const void *, const void *), const void *user_data) { sortN((u8 *)a, n, es, cmp, user_data); }
    }
}; // namespace ncore

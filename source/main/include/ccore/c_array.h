#ifndef __CCORE_ARRAY_H__
#define __CCORE_ARRAY_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    template <typename T, typename I>
    inline T& g_at(T* array, I capacity, I index)
    {
        ASSERT(index < capacity);
        return array[index];
    }

    template <typename T, typename I>
    inline T const& g_at(T const* array, I capacity, I index)
    {
        ASSERT(index < capacity);
        return array[index];
    }

    template <typename T, typename I>
    inline T const* g_view(T const* array, I capacity, I from, I to)
    {
        ASSERT(from < to && to <= capacity);
        return &array[from];
    }

    template <typename T, typename I>
    inline void g_push_back(T* array, I capacity, I& count, T const& item)
    {
        ASSERT(count < capacity);
        array[count] = item;
        count += 1;
    }

    template <typename T, typename I>
    inline bool g_pop_back(T* array, I capacity, I& count, T& out_item)
    {
        if (count > 0)
        {
            --count;
            out_item = array[count];
            return true;
        }
        return false;
    }

    template <typename T, typename I>
    void g_array_insert(T* array, I capacity, I& count, I index, T const& value)
    {
        ASSERT(count < capacity);
        ASSERT(index <= count);
        for (I i = count; i > index; --i)
        {
            array[i] = array[i - 1];
        }
        array[index] = value;
        count++;
    }

    template <typename T, typename I>
    inline void g_swap(T* array, I capacity, I index_a, I index_b)
    {
        if (index_a != index_b && index_a < capacity && index_b < capacity)
        {
            ASSERT(index_a < capacity && index_b < capacity);
            const T temp   = array[index_a];
            array[index_a] = array[index_b];
            array[index_b] = temp;
        }
    }

    template <typename T, typename I>
    void g_remove(T* array, I capacity, I& count, I index)
    {
        ASSERT(index < count);
        for (I i = index; i < count - 1; ++i)
        {
            array[i] = array[i + 1];
        }
        count--;
    }

    template <typename T, typename I>
    inline void g_swap_remove(T* array, I capacity, I& count, I index)
    {
        if (count > 0)
        {
            count -= 1;
            if (index < count)
            {
                array[index] = array[count];
            }
        }
    }

}  // namespace ncore

#endif  // __CCORE_ARRAY_H__

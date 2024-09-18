#include "ccore/c_binary_search.h"

namespace ncore
{
    // Mono-Bound binary search algorithm

    s32 g_BinarySearch(void const* array, u32 array_size, const void* key, const void* user_data, less_predicate_fn is_less, equal_predicate_fn is_equal)
    {
        if (array_size == 0)
            return -1;

        u32 bottom = 0;
        u32 range = array_size;
        while (range > 1)
        {
            u32 const middle = range >> 1;
            if (!is_less(key, array, bottom + middle, user_data))
            {
                bottom += middle;
            }
            range -= middle;
        }

        if (is_equal(key, array, bottom, user_data))
        {
            return bottom;
        }
        return -1;
    }

};  // namespace ncore

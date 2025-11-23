#include "ccore/c_memory.h"

#ifdef TARGET_MAC
#    include <math.h>
#endif

namespace ncore
{
    namespace math
    {
        // CORDIC based integer square root calculation
        s16 g_sqrt(s16 x)
        {
            s16 base = 128;
            s16 y    = 0;
            for (s32 i = 1; i <= 8; i++)
            {
                y += base;
                if ((y * y) > x)
                {
                    y -= base;  // base should not have been added, so we substract again
                }
                base >>= 1;  // shift 1 digit to the right = divide by 2
            }
            return y;
        }

        s32 g_sqrt(s32 value)
        {
            s32 base = D_CONSTANT_U64(1) << 30;
            s32 y    = 0;
            for (s32 i = 1; i <= 16; i++)
            {
                y += base;
                if ((y * y) > value)
                {
                    y -= base;  // base should not have been added, so we substract again
                }
                base >>= 1;  // shift 1 digit to the right = divide by 2
            }
            return y;
        }

        s64 g_sqrt(s64 value)
        {
            s64 base = D_CONSTANT_U64(1) << 62;
            s64 y    = 0;
            for (s32 i = 1; i <= 32; i++)
            {
                y += base;
                if ((y * y) > value)
                {
                    y -= base;  // base should not have been added, so we substract again
                }
                base >>= 1;  // shift 1 digit to the right = divide by 2
            }
            return y;
        }

#ifdef TARGET_MAC
        f32 g_sqrt(f32 value) { return (f32)::sqrtf((float)value); }  // Return the square root of value
        f64 g_sqrt(f64 value) { return (f64)::sqrt((double)value); }  // Return the square root of value
#endif

    }  // namespace math
}  // namespace ncore

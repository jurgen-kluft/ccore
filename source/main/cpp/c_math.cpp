#include "ccore/c_memory.h"
#include "ccore/c_math.h"

#ifdef TARGET_MAC
#    include <math.h>
#endif

#ifdef TARGET_PC
#    include <math.h>
#endif

#if defined(TARGET_PC) || defined(TARGET_MAC)

namespace ncore
{
    namespace math
    {
        namespace nf32
        {
            // float implementations

            value_t abs(value_t a) { return ::abs(a); }
            value_t min(value_t a, value_t b) { return a <= b ? a : b; }
            value_t max(value_t a, value_t b) { return a >= b ? a : b; }
            value_t clamp(value_t a, value_t b, value_t c) { return a < b ? b : (a > c ? c : a); }
            value_t average(value_t a, value_t b) { return (a + b) * 0.5f; }
            void    sort(value_t& a, value_t& b)
            {
                if (a > b)
                {
                    value_t t = a;
                    a         = b;
                    b         = t;
                }
            }
            void sort(value_t& a, value_t& b, value_t& c)
            {
                sort(a, b);
                sort(b, c);
                sort(a, b);
            }

            value_t floor(value_t a) { return ::floorf(a); }
            value_t ceil(value_t a) { return ::ceilf(a); }
            value_t round(value_t a) { return ::roundf(a); }
            value_t sqr(value_t a) { return a * a; }
            value_t map(value_t a, value_t b, value_t c, value_t d, value_t e) { return (a - b) * (e - d) / (c - b) + d; }

            value_t smooth_step(value_t a, value_t b, value_t c)
            {
                if (c <= a)
                    return 0.0f;
                if (c >= b)
                    return 1.0f;
                value_t t = (c - a) / (b - a);
                return t * t * (3.0f - 2.0f * t);
            }
            value_t lerp(value_t a, value_t b, value_t c) { return a + (b - a) * c; }
            value_t slerp(value_t a, value_t b, value_t c)
            {
                value_t theta = ::acosf(a * b);
                if (::fabsf(theta) < 0.00001f)
                    return a;
                value_t sin_theta = ::sinf(theta);
                return (::sinf((1.0f - c) * theta) * a + ::sinf(c * theta) * b) / sin_theta;
            }

            value_t atan(value_t a) { return ::atanf(a); }
            value_t cos(value_t a) { return ::cosf(a); }
            value_t sin(value_t a) { return ::sinf(a); }
            value_t tan(value_t a) { return ::tanf(a); }
            value_t tanh(value_t a) { return ::tanhf(a); }
            value_t frexp(value_t a, i32* out_exponent) { return ::frexpf(a, out_exponent); }
            value_t modf(value_t a, value_t* out_value) { return ::modff(a, out_value); }
            value_t fabs(value_t a) { return ::fabsf(a); }

            value_t acos(value_t a) { return ::acosf(a); }
            value_t asin(value_t a) { return ::asinf(a); }
            value_t atan2(value_t a, value_t b) { return ::atan2f(a, b); }
            value_t cosh(value_t a) { return ::coshf(a); }
            value_t sinh(value_t a) { return ::sinhf(a); }
            value_t exp(value_t a) { return ::expf(a); }
            value_t ldexp(value_t a, i32 exponent) { return ::ldexpf(a, exponent); }
            value_t log(value_t a) { return ::logf(a); }
            value_t log10(value_t a) { return ::log10f(a); }
            value_t pow(value_t a, value_t b) { return ::powf(a, b); }
            value_t sqrt(value_t a) { return ::sqrtf(a); }
            value_t fmod(value_t a, value_t b) { return ::fmodf(a, b); }
        }  // namespace nf32

        namespace nf64
        {
            // double implementations

            value_t abs(value_t a) { return ::abs(a); }
            value_t min(value_t a, value_t b) { return a <= b ? a : b; }
            value_t max(value_t a, value_t b) { return a >= b ? a : b; }
            value_t clamp(value_t a, value_t b, value_t c) { return a < b ? b : (a > c ? c : a); }
            value_t average(value_t a, value_t b) { return (a + b) * 0.5; }
            void    sort(value_t& a, value_t& b)
            {
                if (a > b)
                {
                    value_t t = a;
                    a         = b;
                    b         = t;
                }
            }
            void sort(value_t& a, value_t& b, value_t& c)
            {
                sort(a, b);
                sort(b, c);
                sort(a, b);
            }

            value_t floor(value_t a) { return ::floor(a); }
            value_t ceil(value_t a) { return ::ceil(a); }
            value_t round(value_t a) { return ::round(a); }
            value_t sqr(value_t a) { return a * a; }
            value_t map(value_t a, value_t b, value_t c, value_t d, value_t e) { return (a - b) * (e - d) / (c - b) + d; }

            value_t smooth_step(value_t a, value_t b, value_t c)
            {
                if (c <= a)
                    return 0.0;
                if (c >= b)
                    return 1.0;
                value_t t = (c - a) / (b - a);
                return t * t * (3.0 - 2.0 * t);
            }
            value_t lerp(value_t a, value_t b, value_t c) { return a + (b - a) * c; }
            value_t slerp(value_t a, value_t b, value_t c)
            {
                value_t theta = ::acos(a * b);
                if (::fabs(theta) < 0.00001)
                    return a;
                value_t sin_theta = ::sin(theta);
                return (::sin((1.0 - c) * theta) * a + ::sin(c * theta) * b) / sin_theta;
            }

            value_t atan(value_t a) { return ::atan(a); }
            value_t cos(value_t a) { return ::cos(a); }
            value_t sin(value_t a) { return ::sin(a); }
            value_t tan(value_t a) { return ::tan(a); }
            value_t tanh(value_t a) { return ::tanh(a); }
            value_t frexp(value_t a, i32* out_exponent) { return ::frexp(a, out_exponent); }
            value_t modf(value_t a, value_t* out_value) { return ::modf(a, out_value); }
            value_t fabs(value_t a) { return ::fabs(a); }

            value_t acos(value_t a) { return ::acos(a); }
            value_t asin(value_t a) { return ::asin(a); }
            value_t atan2(value_t a, value_t b) { return ::atan2(a, b); }
            value_t cosh(value_t a) { return ::cosh(a); }
            value_t sinh(value_t a) { return ::sinh(a); }
            value_t exp(value_t a) { return ::exp(a); }
            value_t ldexp(value_t a, i32 exponent) { return ::ldexp(a, exponent); }
            value_t log(value_t a) { return ::log(a); }
            value_t log10(value_t a) { return ::log10(a); }
            value_t pow(value_t a, value_t b) { return ::pow(a, b); }
            value_t sqrt(value_t a) { return ::sqrt(a); }
            value_t fmod(value_t a, value_t b) { return ::fmod(a, b); }
        }  // namespace nf64

        namespace ni32
        {
            // integer implementations (signed 32-bit)

            value_t abs(value_t a) { return a >= 0 ? a : -a; }
            value_t min(value_t a, value_t b) { return a <= b ? a : b; }
            value_t max(value_t a, value_t b) { return a >= b ? a : b; }
            value_t clamp(value_t a, value_t b, value_t c) { return a < b ? b : (a > c ? c : a); }
            value_t average(value_t a, value_t b) { return (a & b) + ((a ^ b) >> 1); }
            void    sort(value_t& a, value_t& b)
            {
                if (a > b)
                {
                    value_t t = a;
                    a         = b;
                    b         = t;
                }
            }
            void sort(value_t& a, value_t& b, value_t& c)
            {
                sort(a, b);
                sort(b, c);
                sort(a, b);
            }

            value_t sqr(value_t a) { return a * a; }
            value_t map(value_t a, value_t b, value_t c, value_t d, value_t e) { return (a - b) * (e - d) / (c - b) + d; }

            value_t lerp(value_t start, value_t end, value_t t, u8 shift)
            {
                value_t max_t = (value_t)1 << shift;
                if (t <= 0)
                    return start;
                if (t >= max_t)
                    return end;
                return start + (((end - start) * t) >> shift);
            }

            value_t lerp_angle(value_t start, value_t end, value_t t, u8 shift)
            {
                value_t max_t = (value_t)1 << shift;
                if (t <= 0)
                    return start;
                if (t >= max_t)
                    return end;

                value_t difference = end - start;

                while (difference < -180)
                    difference += 360;
                while (difference > 180)
                    difference -= 360;

                return start + ((difference * t) >> shift);
            }

            // @brief Smoothstep (S-Curve) interpolation with a dynamic fixed-point scale.
            // @param start  The starting signed value.
            // @param end    The ending signed value.
            // @param t      Current progress, bounded between 0 and (1 << shift).
            // @param shift  The number of bits used for precision.
            // @note Shifts too great will easily overflow 32-bit math (t * t * 3), but it
            //       it also depends on the range of start and end values. Use with caution!
            value_t smoothstep(value_t start, value_t end, value_t t, u8 shift)
            {
                // 1. Calculate the maximum value of t based on the shift
                value_t max_t = 1 << shift;

                // Guard against out-of-bounds inputs
                if (t <= 0)
                    return start;
                if (t >= max_t)
                    return end;

                // Safety Cap: Shifts greater than 15 will overflow 32-bit math (t * t * (3 << shift)), so we limit it.
                if (shift > 15)
                    shift = 15;

                // 2. Evaluate the fixed-point smoothstep curve: t * t * (3 - 2*t)
                // We scale '3' to match our current fixed-point depth: 3 << shift
                // For 32-bit math, we can expand to 64-bit to avoid overflow, but we 
                // need to cast back to 32-bit at the end.
                i64 three_scaled = (i64)3 << shift;
                i64 two_t        = (i64)t << 1;

                // Core S-Curve formula
                i64 num = t * t * (three_scaled - two_t);

                // Scale back down because t*t*t compounds the shift factor (shift + shift + shift)
                // We need the final smoothed_t to be back in the 1-unit shift range, so we shift down by (shift * 2)
                value_t smoothed_t = (value_t)(num >> (shift * 2));

                // 3. Apply the smoothed progress factor to the signed range
                return start + (((end - start) * smoothed_t) >> shift);
            }
        }  // namespace ni32

        namespace ni64
        {
            // integer implementations (signed 64-bit)

            value_t abs(value_t a) { return a >= 0 ? a : -a; }
            value_t min(value_t a, value_t b) { return a <= b ? a : b; }
            value_t max(value_t a, value_t b) { return a >= b ? a : b; }
            value_t clamp(value_t a, value_t b, value_t c) { return a < b ? b : (a > c ? c : a); }
            value_t average(value_t a, value_t b) { return (a & b) + ((a ^ b) >> 1); }
            void    sort(value_t& a, value_t& b)
            {
                if (a > b)
                {
                    value_t t = a;
                    a         = b;
                    b         = t;
                }
            }
            void sort(value_t& a, value_t& b, value_t& c)
            {
                sort(a, b);
                sort(b, c);
                sort(a, b);
            }

            value_t sqr(value_t a) { return a * a; }
            value_t map(value_t a, value_t b, value_t c, value_t d, value_t e) { return (a - b) * (e - d) / (c - b) + d; }

            // Linearly interpolates a single value with a dynamic fixed-point scale.
            // @param t      Current progress, bounded between 0 and (1 << shift).
            // @param shift  The number of bits used for precision (e.g., 8, 10, 12).
            value_t lerp(value_t start, value_t end, value_t t, u8 shift)
            {
                value_t max_t = (value_t)1 << shift;
                if (t <= 0)
                    return start;
                if (t >= max_t)
                    return end;
                return start + (((end - start) * t) >> shift);
            }

            value_t smooth_step(value_t a, value_t b, value_t c) { return lerp(a, b, c <= a ? 0 : (c >= b ? 1 : (c - a) / (b - a))); }
            value_t lerp_angle(value_t start, value_t end, value_t t, u8 shift)
            {
                value_t max_t = (value_t)1 << shift;
                if (t <= 0)
                    return start;
                if (t >= max_t)
                    return end;

                value_t difference = end - start;

                while (difference < -180)
                    difference += 360;
                while (difference > 180)
                    difference -= 360;

                return start + ((difference * t) >> shift);
            }

            // @brief Smoothstep (S-Curve) interpolation with a dynamic fixed-point scale.
            // @param start  The starting signed value.
            // @param end    The ending signed value.
            // @param t      Current progress, bounded between 0 and (1 << shift).
            // @param shift  The number of bits used for precision.
            // @note Shifts too great will easily overflow 64-bit math (t * t * 3), but it
            //       it also depends on the range of start and end values. Use with caution.
            value_t smoothstep(value_t start, value_t end, value_t t, u8 shift)
            {
                // 1. Calculate the maximum value of t based on the shift
                value_t max_t = 1 << shift;

                // Guard against out-of-bounds inputs
                if (t <= 0)
                    return start;
                if (t >= max_t)
                    return end;

                // Safety Cap: Shifts greater than 15 will overflow 64-bit math (t * t * (3 << shift))
                if (shift > 15)
                    shift = 15;

                // 2. Evaluate the fixed-point smoothstep curve: t * t * (3 - 2*t)
                // We scale '3' to match our current fixed-point depth: 3 << shift
                value_t three_scaled = (value_t)3 << shift;
                value_t two_t        = (value_t)t << 1;

                // Core S-Curve formula
                value_t num = t * t * (three_scaled - two_t);

                // Scale back down because t*t*t compounds the shift factor (shift + shift + shift)
                // We need the final smoothed_t to be back in the 1-unit shift range, so we shift down by (shift * 2)
                value_t smoothed_t = (value_t)(num >> (shift * 2));

                // 3. Apply the smoothed progress factor to the signed range
                return start + (((end - start) * smoothed_t) >> shift);
            }

        }  // namespace ni64

    }  // namespace math
}  // namespace ncore

#endif
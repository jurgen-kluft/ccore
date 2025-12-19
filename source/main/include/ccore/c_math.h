#ifndef __CCORE_MATH_AND_BIT_UTILS_H__
#define __CCORE_MATH_AND_BIT_UTILS_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_debug.h"

namespace ncore
{
#undef min
#undef max

#ifdef log2
#    undef log2
#endif

    namespace math
    {
        template <class T>
        T min(T integerA, T integerB);  // Return the smallest value of 2
        template <class T>
        T min(T integerA, T integerB, T integerC);  // Return the smallest value of 3
        template <class T>
        T max(T integerA, T integerB);  // Return the biggest value
        template <class T>
        T max(T integerA, T integerB, T integerC);  // Return the biggest value of 3

        template <class T>
        void sort(T& integerA, T& integerB);  // Return integerA = smallest value, integerB = biggest value

        template <class T>
        T sign(T integerA)
        {
            return (integerA > 0) ? 1 : ((integerA < 0) ? -1 : 0);
        }

        template <class T>
        T clamp(T integerA, T low, T high);  // Return the clamp value

        /// clamp sub-range inside of encompassing range
        template <class T>
        void clampRange(T& subfrom, T& subto, T enfrom, T ento)
        {
            sort(subfrom, subto);
            sort(enfrom, ento);
            clamp(subfrom, enfrom, ento);
            clamp(subto, enfrom, ento);
        }

        template <typename T>
        inline T align(T integer, T alignment);  // Return the aligned value of integer
        template <typename T>
        inline T alignDown(T integer, T alignment);  // Return the aligned value of integer
        template <typename T, typename U>
        inline T alignUp(T integer, U alignment);  // Return the aligned value of integer
        template <typename T>
        inline bool isAligned(T integer, T alignment);  // Return True if integer is aligned

        template <typename T>
        inline T abs(T integer);  // Return the absolute value of integer
        template <typename T>
        inline T neg(T integer, bool inDoNotNegate);  // Return the conditionally negated value of integer
        template <typename T>
        inline T sqr(T integer);  // Return the square of integer
        template <typename T>
        inline T mod(T integer, T inModuloValue);  // Return the modulo of integer using inModuloValue
        template <typename T>
        inline T gcd(T inIntegerA, T integerB);  // Greatest common denominator (biggest modulo value of both integers)
        template <typename T>
        inline T average(T integerA, T integerB);  // Calculate average of A & B without overflow or s64 use
        template <typename T>
        inline bool ispo2(T integer);  // Check if integer is a power-of-two

        s16 sqrt(s16 value);  // Return the square root of value
        s32 sqrt(s32 value);  // Return the square root of value
        s64 sqrt(s64 value);  // Return the square root of value

        f32 sqrt(f32 value);  // Return the square root of value
        f64 sqrt(f64 value);  // Return the square root of value

        inline u32 ceilpo2(u32 integer);   // Return the smallest power-of-two larger than integer
        inline u64 ceilpo2(u64 integer);   // Return the smallest power-of-two larger than integer
        inline u32 floorpo2(u32 integer);  // Return the biggest power-of-two smaller than integer
        inline u64 floorpo2(u64 integer);  // Return the biggest power-of-two smaller than integer
        inline u32 next_power_of_two(u32 val) { return ceilpo2(val); }
        inline s8  countBits(u8 integer);   // count one bits in 8 bit word
        inline s8  countBits(u16 integer);  // count one bits in 16 bit word
        inline s8  countBits(u32 integer);  // count one bits in 32 bit word
        inline s8  countBits(u64 integer);  // count one bits in 32 bit word

        inline s8 ilog2(u32 integer);  // Log2 of a 32-bit integer
        inline s8 ilog2(s32 integer) { return ilog2((u32)integer); }
        inline s8 ilog2(u64 integer);  // Log2 of a 64-bit integer
        inline s8 ilog2(s64 integer) { return ilog2((u64)integer); }

        inline u64 getMaskForValue(u64 value);             // Return the mask of the value
        inline s8  countTrailingZeros(u8 integer);         // find the number of trailing zeros in 8-bit v
        inline s8  countLeadingZeros(u8 integer);          // find the number of trailing zeros in 8-bit v
        inline s8  countTrailingZeros(u16 integer);        // find the number of trailing zeros in 16-bit v
        inline s8  countLeadingZeros(u16 integer);         // find the number of leading zeros in 16-bit v
        inline s8  countTrailingZeros(u32 integer);        // find the number of trailing zeros in 32-bit v
        inline s8  countLeadingZeros(u32 integer);         // find the number of leading zeros in 32-bit v
        inline s8  countTrailingZeros(u64 integer);        // find the number of trailing zeros in 64-bit v
        inline s8  countLeadingZeros(u64 integer);         // find the number of leading zeros in 64-bit v
        inline u32 leastSignificantOneBit(u32 inInteger);  // Return v but with only the Least Significant Bit "1"
        inline u32 mostSignificantOneBit(u32 integer);     // Return v but with only the Most Significant Bit "1"
        inline s8  leastSignificantBit(u32 integer);       // Return the bit index of the Least Significant Bit "1"
        inline s8  mostSignificantBit(u32 integer);        // Return the bit index of the Most Significant Bit "1"
        inline s8  findFirstBit(u8 integer);               // find the bit position/index of the first bit from low to high
        inline s8  findLastBit(u8 integer);                // find the bit position/index of the first bit from high to low
        inline s8  findFirstBit(u16 integer);              // find the bit position/index of the first bit from low to high
        inline s8  findLastBit(u16 integer);               // find the bit position/index of the first bit from high to low
        inline s8  findFirstBit(u32 integer);              // find the bit position/index of the first bit from low to high
        inline s8  findLastBit(u32 integer);               // find the bit position/index of the first bit from high to low
        inline s8  findFirstBit(u64 integer);              // find the bit position/index of the first bit from low to high
        inline s8  findLastBit(u64 integer);               // find the bit position/index of the first bit from high to low
        inline u32 rol32(u32 integer, u32 shift);          // Roll all the bits in integer to the left by shift number of bits
        inline u32 ror32(u32 integer, u32 shift);          // Roll all the bits in integer to the right by shift number of bits

    }  // namespace math
};  // namespace ncore

//==============================================================================
// INLINES
//==============================================================================
#include "ccore/private/c_math_inline.h"

#if defined TARGET_PC && defined CC_COMPILER_MSVC
#    include "ccore/private/c_math_inline_win32.h"
#elif defined TARGET_PC && defined COMPILER_CLANG
#    include "ccore/private/c_math_inline_generic.h"
#elif defined TARGET_MAC
#    include "ccore/private/c_math_inline_mac.h"
#else
#    include "ccore/private/c_math_inline_generic.h"
#endif

#endif  // __CBASE_INTEGER_UTILS_H__

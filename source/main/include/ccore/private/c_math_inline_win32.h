#include <intrin.h>
#pragma intrinsic(_BitScanForward, _BitScanForward64, _BitScanReverse, _BitScanReverse64)

namespace ncore
{
    namespace math
    {
        // find the number of trailing zeros in 16-bit value
        // if 'value==0' this function returns 8
        inline s8 g_countTrailingZeros(u8 value)
        {
            unsigned long result;
            return ::_BitScanForward(&result, value) ? (s8)result : (s8)8;
        }
        // find the number of trailing zeros in 16-bit value
        // if 'value==0' this function returns 16
        inline s8 g_countTrailingZeros(u16 value)
        {
            unsigned long result;
            return ::_BitScanForward(&result, value) ? (s8)result : (s8)16;
        }
        // find the number of trailing zeros in 32-bit value
        // if 'value==0' this function returns 32
        inline s8 g_countTrailingZeros(u32 value)
        {
            unsigned long result;
            return ::_BitScanForward(&result, value) ? (s8)result : (s8)32;
        }
        // find the number of trailing zeros in 64-bit value
        // if 'value==0' this function returns 64
        inline s8 g_countTrailingZeros(u64 value)
        {
            unsigned long result;
            return ::_BitScanForward64(&result, value) ? (s8)result : (s8)64;
        }

        // find the number of leading zeros in 8-bit value
        // if 'value==0' this function returns 8
        inline s8 g_countLeadingZeros(u8 value)
        {
            unsigned long v;
            return ::_BitScanReverse(&v, value) ? (s8)7 - (s8)v : (s8)8;
        }
        // find the number of leading zeros in 16-bit value
        // if 'value==0' this function returns 16
        inline s8 g_countLeadingZeros(u16 value)
        {
            unsigned long v;
            return ::_BitScanReverse(&v, value) ? (s8)15 - (s8)v : (s8)16;
        }
        // find the number of leading zeros in 32-bit value
        // if 'value==0' this function returns 32
        inline s8 g_countLeadingZeros(u32 value)
        {
            unsigned long v;
            return ::_BitScanReverse(&v, value) ? (s8)31 - (s8)v : 32;
        }
        // find the number of leading zeros in 64-bit value
        // if 'value==0' this function returns 64
        inline s8 g_countLeadingZeros(u64 value)
        {
            unsigned long v;
            return ::_BitScanReverse64(&v, value) ? (s8)63 - (s8)v : (s8)64;
        }

        // Return value but with only the Least Significant Bit "1"
        inline u32 g_leastSignificantOneBit(u32 value) { return (value ^ (value & (value - 1))); }

        // Return value but with only the Most Significant Bit "1"
        // If 'value == 0' this function will return 0
        inline u32 g_mostSignificantOneBit(u32 value)
        {
            unsigned long v;
            return ::_BitScanReverse(&v, value) ? (1 << v) : 0;
        }

        // Return the bit index of the Least Significant Bit "1"
        // If 'value == 0' this function will return 32
        inline s8 g_leastSignificantBit(u32 value) { return g_countTrailingZeros(value); }

        // Return the bit index of the Most Significant Bit "1"
        // If 'value == 0' this function will return 32
        inline s8 g_mostSignificantBit(u32 value)
        {
            unsigned long v;
            return ::_BitScanReverse(&v, value) ? (s8)v : (s8)32;
        }

        // find the bit position/index of the first bit from low to high
        // If 'value == 0' this function will return 8
        inline s8 g_findFirstBit(u8 value) { return g_countTrailingZeros(value); }

        // find the bit position/index of the first bit from high to low
        // If 'value == 0' this function will return -1
        inline s8 g_findLastBit(u8 value) { return g_countLeadingZeros(value); }

        // find the bit position/index of the first bit from low to high
        // If 'value == 0' this function will return
        inline s8 g_findFirstBit(u16 value) { return g_countTrailingZeros(value); }

        // find the bit position/index of the first bit from high to low
        // If 'value == 0' this function will return -1
        inline s8 g_findLastBit(u16 value) { return g_countLeadingZeros(value); }

        // find the bit position/index of the first bit from low to high
        // If 'value == 0' this function will return -1
        inline s8 g_findFirstBit(u32 value) { return g_countTrailingZeros(value); }

        // find the bit position/index of the first bit from high to low
        // If 'value == 0' this function will return -1
        inline s8 g_findLastBit(u32 value) { return g_countLeadingZeros(value); }

        // find the bit position/index of the first bit from low to high
        // If 'value == 0' this function will return 0
        inline s8 g_findFirstBit(u64 value) { return g_countTrailingZeros(value); }

        // find the bit position/index of the first bit from high to low
        // If 'value == 0' this function will return -1
        inline s8 g_findLastBit(u64 value) { return g_countLeadingZeros(value); }

        /**
         * count one bits in 32 bit word
         */
        inline s8 g_countBits(u8 value)
        {
            u16 i16 = value;
            return (s8)__popcnt16(i16);
        }

        /**
         * count one bits in 32 bit word
         */
        inline s8 g_countBits(u16 value) { return (s8)__popcnt16(value); }

        /**
         * count one bits in 32 bit word
         */
        inline s8 g_countBits(u32 value) { return (s8)__popcnt(value); }

        /**
         * count one bits in 64 bit word
         */
        inline s8 g_countBits(u64 value) { return (s8)__popcnt64(value); }
    }  // namespace math
}  // namespace ncore
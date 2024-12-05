namespace ncore
{
    namespace math
    {
        template <class T>
        inline T g_min(T integerA, T integerB)
        {
            return integerA <= integerB ? integerA : integerB;
        }

        template <class T>
        inline T g_min(T integerA, T integerB, T integerC)
        {
            return g_min(integerC, g_min(integerA, integerB));
        }

        template <class T>
        inline T g_max(T integerA, T integerB)
        {
            return integerA >= integerB ? integerA : integerB;
        }

        template <class T>
        T g_max(T integerA, T integerB, T integerC)
        {
            return g_max(integerC, g_max(integerA, integerB));
        }

        // Function template, integerA = smallest value, integerB = biggest value
        template <class T>
        inline void g_sort(T& integerA, T& integerB)
        {
            if (integerA > integerB)
            {
                T t      = integerA;
                integerA = integerB;
                integerA = t;
            }
        }

        // Function template, returns the clamp value
        template <class T>
        inline T g_clamp(T value, T low, T high)
        {
            ASSERTS(low <= high, "Error: low > high");
            return value < low ? low : (value > high ? high : value);
        }

        // Return the aligned value of value
        // Same as alignUp
        inline u32 g_align(u32 value, u32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return (value) & (~(alignment - 1));
        }
        inline u32 g_align(u32 value, s32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return (value) & (~((u32)alignment - 1));
        }
        inline s32 g_align(s32 value, s32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return (s32)((u32)(value) & (~((u32)alignment - 1)));
        }

        // Return the floor based aligned value of value
        inline u32 g_alignDown(u32 value, u32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return (value) & (~(alignment - 1));
        }
        inline u32 g_alignDown(u32 value, s32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return (value) & (~((u32)alignment - 1));
        }

        // Return the ceiling based aligned value of value
        inline s32 g_alignUp(s32 value, s32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return ((value + (alignment - 1)) & (~(alignment - 1)));
        }

        // Return the ceiling based aligned value of value
        inline u32 g_alignUp(u32 value, u32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return ((value + (alignment - 1)) & (~(alignment - 1)));
        }
        inline u32 g_alignUp(u32 value, s32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return ((value + ((u32)alignment - 1)) & (~((u32)alignment - 1)));
        }

        // Return the ceiling based aligned value of value
        inline u64 g_alignUp(u64 value, u32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return ((value + (alignment - 1)) & (~((u64)alignment - 1)));
        }
        inline u64 g_alignUp(u64 value, s32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return ((value + ((u64)alignment - 1)) & (~((u64)alignment - 1)));
        }

        // Return the rounded up value as a power of 2
        inline u32 g_roundUpPow2(u32 value, u32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return (value + (alignment - 1)) & (~(alignment - 1));
        }

        // Check if value is aligned
        inline bool g_isAligned(s32 value, s32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return value == (s32)((u32)(value) & (~((u32)alignment - 1)));
        }
        inline bool g_isAligned(s32 value, u32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return value == (s32)(value & (~(alignment - 1)));
        }

        inline bool g_isAligned(u32 value, u32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return value == ((value) & (~(alignment - 1)));
        }
        inline bool g_isAligned(u32 value, s32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return value == ((value) & (~((u32)alignment - 1)));
        }

        inline bool g_isAligned(s64 value, u32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return value == (s64)(((u64)value) & (~((u64)alignment - 1)));
        }
        inline bool g_isAligned(s64 value, s32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return value == (s64)(((u64)value) & (~((u64)alignment - 1)));
        }

        inline bool g_isAligned(u64 value, u32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return value == ((value) & (~((u64)alignment - 1)));
        }
        inline bool g_isAligned(u64 value, s32 alignment)
        {
            ASSERTS(g_ispo2(alignment) == true, "Error: alignment value should be a power of 2");
            return value == ((value) & (~((u64)alignment - 1)));
        }

        // Return the absolute value of value
        inline s32 g_abs(s32 value)
        {
            if (value > 0)
                return value;
            else
                return -value;
        }

        // Return the negated value of value
        // value:		Input value to negate if inNegate is false.
        // inDoNotNegate : Flag indicating we should not negate value.
        inline s32 g_neg(s32 value, bool inDoNotNegate)
        {
            s32 r = ((s32)inDoNotNegate ^ ((s32)inDoNotNegate - 1)) * value;
            return r;
        }

        // Return the square of value
        inline s32 g_sqr(s32 value) { return value * value; }

        // Return the unsigned square of value
        inline u32 g_sqr(u32 value) { return value * value; }

        // Return the modulo of value using inModuloValue
        inline s32 g_mod(s32 value, s32 inModuloValue) { return value % inModuloValue; }

        // Greatest common denominator (biggest modulo value of both integers)
        inline s32 g_gcd(s32 valueA, s32 valueB)
        {
            while (valueB != 0)
            {
                s32 t  = valueA % valueB;
                valueA = valueB;
                valueB = t;
            }
            return valueA;
        }

        // Calculate average of A & B without overflow or s64 use
        inline s32 g_average(s32 valueA, s32 valueB) { return (valueA & valueB) + ((valueA ^ valueB) >> 1); }

        // Check if value is a power-of-two
        inline bool g_ispo2(s32 value) { return (value != 0) && (((value) & (value - 1)) == 0); }
        inline bool g_ispo2(u32 value) { return (value != 0) && (((value) & (value - 1)) == 0); }
        inline bool g_ispo2(u64 value) { return (value != 0) && (((value) & (value - 1)) == 0); }

        // Return the power-of-two larger than or equal to value
        inline u32 g_ceilpo2(u32 value) { return 1 << (32 - g_countLeadingZeros(value - 1)); }

        // Return the power-of-two smaller than or equal to value
        inline u32 g_floorpo2(u32 value) { return 1 << (31 - g_countLeadingZeros(value)); }

        // Reverse bits in 32 bit word
        inline u32 g_bitReverse(u32 value)
        {
            u32 n = value;
            n     = ((n >> 1) & 0x55555555) | ((n << 1) & 0xaaaaaaaa);
            n     = ((n >> 2) & 0x33333333) | ((n << 2) & 0xcccccccc);
            n     = ((n >> 4) & 0x0f0f0f0f) | ((n << 4) & 0xf0f0f0f0);
            n     = ((n >> 8) & 0x00ff00ff) | ((n << 8) & 0xff00ff00);
            n     = ((n >> 16) & 0x0000ffff) | ((n << 16) & 0xffff0000);
            return n;
        }

        inline s8 g_ilog2(u32 value) { return 31 - g_countLeadingZeros(value); }
        inline s8 g_ilog2(u64 value) { return 63 - g_countLeadingZeros(value); }

        // Roll all the bits in value to the left by shift number of bits
        inline u32 g_rol32(u32 value, u32 shift)
        {
            shift = shift & 31;
            return (value << shift) | (value >> (32 - shift));
        }

        // Roll all the bits in value to the right by shift number of bits
        inline u32 g_ror32(u32 value, u32 shift)
        {
            shift = shift & 31;
            return (value >> shift) | (value << (32 - shift));
        }

        template <typename T, typename A>
        inline T g_align(T addr, A alignTo)
        {
            return T((addr) & ~((u32)alignTo - 1));
        }

        template <typename T, typename A>
        inline T g_align_up(T addr, A alignTo)
        {
            return T((addr + ((T)alignTo - 1)) & ~((u32)alignTo - 1));
        }

        template <typename T, typename A>
        inline T g_align_floor(T addr, A alignTo)
        {
            return T(((addr) & ~((u32)alignTo - 1)));
        }

        template <typename T, typename A>
        inline bool g_aligned(T addr, A alignTo)
        {
            return bool(((addr) & ((u32)alignTo - 1)) == 0);
        }
    }  // namespace math
}  // namespace ncore

#ifndef __CBASE_LIMITS_H__
#define __CBASE_LIMITS_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    static const u8 u8_min = 0;
    static const u8 u8_max = 0xFF;

    static const u16 u16_min = 0;
    static const u16 u16_max = 0xFFFF;

    static const u32 u32_min = 0;
    static const u32 u32_max = 0xFFFFFFFF;

    static const u64 u64_min = D_CONSTANT_U64(0);
    static const u64 u64_max = D_CONSTANT_U64(0xFFFFFFFFFFFFFFFF);

    static const s8 s8_min = -128;
    static const s8 s8_max = 127;

    static const s16 s16_min = -32768;
    static const s16 s16_max = 32767;

    static const s32 s32_min = (s32)0x80000000;
    static const s32 s32_max = 2147483647;

    static const s64 s64_min = (s64)D_CONSTANT_S64(0x8000000000000000);
    static const s64 s64_max = (s64)D_CONSTANT_S64(0x7FFFFFFFFFFFFFFF);

    constexpr f32 f32_min = -3.402823466e+38f;
    constexpr f32 f32_max = 3.402823466e+38f;

    constexpr f64 f64_min = -1.7976931348623158e+308;
    constexpr f64 f64_max = 1.7976931348623158e+308;

    static const byte byte_min = 0;
    static const byte byte_max = 0xFF;

    static const sbyte sbyte_min = -128;
    static const sbyte sbyte_max = 127;

    /**
     * Description:
     *     class type_t is a template class, which use the C++ class template specialization.
     *     The idea of C++ class template specialization is similar to function template
     *     overloading. This can make the template code for certain data types to be fixed.
     *
     * Example:
     *<CODE>
     *      template<class T>
     *      bool isInRange(T inNum)
     *      {
     *          return (type_t<T>::in_range(inNum));
     *      }
     *
     *</CODE>
     *
     */

    struct type_id
    {
        enum ETypeId
        {
            UInt8  = 0,
            UInt16 = 1,
            UInt32 = 2,
            UInt64 = 3,
            Int8   = 4,
            Int16  = 5,
            Int32  = 6,
            Int64  = 7,
            Float  = 8,
            Double = 9,
            Bool   = 10,
            Char   = 11,
        };

        inline type_id()
            : value(Bool)
        {
        }
        inline type_id(ETypeId value)
            : value(value)
        {
        }
        inline type_id(const type_id& other)
            : value(other.value)
        {
        }

        inline bool has_sign() const { return value >= Int8 && value <= Double; }

        u8 value;
    };

    template <typename T>
    class type_t
    {
    };

#undef min
#undef max

    template <>
    class type_t<bool>
    {
    public:
        typedef bool __type;

        const static __type   min = false;
        const static __type   max = true;
        static inline type_id id() { return type_id(type_id::Bool); }
    };

    template <>
    class type_t<u8>
    {
    public:
        typedef u8 __type;

        const static __type   min = u8_min;
        const static __type   max = u8_max;
        inline bool           in_range(__type value) const { return (value >= min && value <= max); }
        static inline type_id id() { return type_id(type_id::UInt8); }
    };

    template <>
    class type_t<u16>
    {
    public:
        typedef u16 __type;

        const static __type   min = u16_min;
        const static __type   max = u16_max;
        inline bool           in_range(__type value) const { return (value >= min && value <= max); }
        static inline type_id id() { return type_id(type_id::UInt16); }
    };

    template <>
    class type_t<u32>
    {
    public:
        typedef u32 __type;

        const static __type   min = u32_min;
        const static __type   max = u32_max;
        inline bool           in_range(__type value) const { return (value >= min && value <= max); }
        static inline type_id id() { return type_id(type_id::UInt32); }
    };

    template <>
    class type_t<u64>
    {
    public:
        typedef u64 __type;

        const static __type   min = u64_min;
        const static __type   max = u64_max;
        inline bool           in_range(__type value) const { return (value >= min && value <= max); }
        static inline type_id id() { return type_id(type_id::UInt64); }
    };

    template <>
    class type_t<s8>
    {
    public:
        typedef s8 __type;

        const static __type   min = s8_min;
        const static __type   max = s8_max;
        inline bool           in_range(__type value) const { return (value >= min && value <= max); }
        static inline type_id id() { return type_id(type_id::Int8); }
    };

    template <>
    class type_t<s16>
    {
    public:
        typedef s16 __type;

        const static __type   min = s16_min;
        const static __type   max = s16_max;
        inline bool           in_range(__type value) const { return (value >= min && value <= max); }
        static inline type_id id() { return type_id(type_id::Int16); }
    };

    template <>
    class type_t<s32>
    {
    public:
        typedef s32 __type;

        const static __type   min = s32_min;
        const static __type   max = s32_max;
        inline bool           in_range(__type value) const { return (value >= min && value <= max); }
        static inline type_id id() { return type_id(type_id::Int32); }
    };

    template <>
    class type_t<s64>
    {
    public:
        typedef s64 __type;

        const static __type   min = s64_min;
        const static __type   max = s64_max;
        inline bool           in_range(__type value) const { return (value >= min && value <= max); }
        static inline type_id id() { return type_id(type_id::Int64); }
    };

    template <>
    class type_t<f32>
    {
    public:
        typedef f32 __type;

        constexpr static __type min = f32_min;
        constexpr static __type max = f32_max;
        inline bool             in_range(__type value) const { return (value >= min && value <= max); }
        static inline type_id   id() { return type_id(type_id::Float); }
    };

    template <>
    class type_t<f64>
    {
    public:
        typedef f64 __type;

        constexpr static __type min = f64_min;
        constexpr static __type max = f64_max;
        inline bool             in_range(__type value) const { return (value >= min && value <= max); }
        static inline type_id   id() { return type_id(type_id::Double); }
    };

};  // namespace ncore

#endif

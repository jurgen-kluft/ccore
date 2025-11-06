#ifndef __CCORE_TARGET_V2_H__
#define __CCORE_TARGET_V2_H__
#include "ccore/config/c_compiler.h"
#include "ccore/config/c_compiler_traits.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

// Identify the platform and declare the CC_xxxx defines
#include "ccore/config/c_platform.h"

namespace ncore
{
    // ------------------------------------------------------------------------
    // Build configurations, which are used to determine the build type.
    // Configuration:
    //    - TARGET_DEBUG
    //    - TARGET_RELEASE
    //    - TARGET_FINAL
    // Variants:
    //    - TARGET_DEV
    //    - TARGET_RETAIL
    //    - TARGET_TEST
    //    - TARGET_PROFILE

#ifdef _DEBUG
#    if !defined(TARGET_DEBUG)
#        define TARGET_DEBUG
#    endif
#    define D_DEBUG
#    define D_ASSERT
#elif defined(NDEBUG)
#    if !defined(TARGET_DEBUG) && !defined(TARGET_RELEASE) && !defined(TARGET_FINAL)
#        define TARGET_RELEASE
#    endif
#else
#    if !defined(TARGET_DEBUG) && !defined(TARGET_RELEASE) && !defined(TARGET_FINAL)
#        define TARGET_RELEASE
#    endif
#endif

    // ------------------------------------------------------------------------
    // Check if the configuration is debug, release or final

// Check if the configuration is debug, release or final
#if !defined(TARGET_DEV) && !defined(TARGET_RETAIL) && !defined(TARGET_TEST) && !defined(TARGET_PROFILE)
#    define TARGET_DEV 1
#endif
#if defined(NDEBUG) && !defined(_DEBUG)
#    if !defined(TARGET_RELEASE) && !defined(TARGET_FINAL)
#        define TARGET_RELEASE 1
#    endif
#else
#    if !defined(TARGET_DEBUG) && !defined(TARGET_FINAL)
#        define TARGET_DEBUG 1
#    endif
#endif

    // ------------------------------------------------------------------------
    // By default, GCC defines NULL as ((void*)0), which is the
    // C definition. This causes all sort of problems for C++ code, so it is
    // worked around by undefining NULL.

#if defined(NULL)
#    undef NULL
#endif

    // ------------------------------------------------------------------------
    // Define the NULL pointer. This is normally defined in <stddef.h>, but we
    // don't want to force a global dependency on that header, so the definition
    // is duplicated here.

#if defined(__cplusplus)
#    define NULL 0
#else
#    define NULL ((void*)0)
#endif

// ------------------------------------------------------------------------
// We need to test this after we potentially include stddef.h, otherwise we
// would have put this into the compilertraits header.
#if !defined(CC_COMPILER_HAS_INTTYPES) && (!defined(_MSC_VER) || (_MSC_VER > 1500)) && (defined(CC_COMPILER_IS_C99) || defined(D_S8_MIN) || defined(CC_COMPILER_HAS_C99_TYPES) || defined(_SN_STDINT_H))
#    define CC_COMPILER_HAS_INTTYPES
#endif

    // ------------------------------------------------------------------------
    typedef signed char    s8;     // 8 bit signed integer
    typedef signed short   s16;    // 16 bit signed integer
    typedef signed int     s32;    // 32 bit signed integer. This works for both 32 bit and 64 bit platforms, as we assume the LP64 is followed.
    typedef signed char    i8;     // 8 bit signed integer
    typedef signed short   i16;    // 16 bit signed integer
    typedef signed int     i32;    // 32 bit signed integer. This works for both 32 bit and 64 bit platforms, as we assume the LP64 is followed.
    typedef unsigned char  u8;     // 8 bit unsigned integer
    typedef unsigned short u16;    // 16 bit unsigned integer
    typedef unsigned int   u32;    // 32 bit unsigned integer. This works for both 32 bit and 64 bit platforms, as we assume the LP64 is followed.
    typedef s8             sbyte;  // 8 bit signed integer
    typedef u8             byte;   // 8 bit unsigned integer

// According to the C98/99 standard, FLT_EVAL_METHOD defines control the
// width used for floating point _t types.
#if defined(_MSC_VER) && _MSC_VER >= 1800

#elif defined(FLT_EVAL_METHOD)
#    if (FLT_EVAL_METHOD == 0)
    typedef float  f32;
    typedef double f64;
#    elif (FLT_EVAL_METHOD == 1)
    typedef double f32;
    typedef double f64;
#    elif (FLT_EVAL_METHOD == 2)
    typedef long double f32;
    typedef long double f64;
#    endif
#endif

#ifndef FLT_EVAL_METHOD
#    define FLT_EVAL_METHOD 0
    typedef float  f32;
    typedef double f64;
#endif

#if defined(CC_COMPILER_MSVC)
    typedef signed __int64   s64;
    typedef unsigned __int64 u64;
#else
#    if defined(CC_PLATFORM_OSX)
    typedef signed long   s64;
    typedef unsigned long u64;
    static_assert(sizeof(s64) == 8, "s64 must be 64 bits");
    static_assert(sizeof(u64) == 8, "s64 must be 64 bits");
#    else
    typedef signed long long   s64;
    typedef unsigned long long u64;
    static_assert(sizeof(s64) == 8, "s64 must be 64 bits");
    static_assert(sizeof(u64) == 8, "s64 must be 64 bits");
#    endif
#endif
    
    typedef s64 i64;

#if (CC_PLATFORM_PTR_SIZE == 4)
    typedef s32 ptr_t;
    typedef s32 ptrdiff_t;
    typedef u32 uptr_t;
    typedef s32 int_t;
    typedef u32 uint_t;
    typedef u32 size_t;
    typedef s32 ssize_t;
#elif (CC_PLATFORM_PTR_SIZE == 8)
    typedef s64 ptr_t;
    typedef s64 ptrdiff_t;
    typedef u64 uptr_t;
    typedef s64 int_t;
    typedef u64 uint_t;
    typedef u64 size_t;
    typedef s64 ssize_t;
#endif

    struct u16x4
    {
        u16 v[4];
    };

    struct u16x2
    {
        u16 v[2];
    };

    struct u32x2
    {
        u32 v[2];
    };

    struct f32x2
    {
        f32 v[2];
    };

    struct f32x3
    {
        f32 v[3];
    };

    struct f32x4
    {
        f32 v[4];
    };

    struct f64x2
    {
        f64 v[2];
    };

    struct f64x3
    {
        f64 v[3];
    };

    struct f64x4
    {
        f64 v[4];
    };

    //==============================================================================
    // ASCII, UTF 8, UTF 16, UTF 32
    typedef char uchar;
    typedef u8   uchar8;
    typedef u16  uchar16;
    typedef u32  uchar32;

    // ------------------------------------------------------------------------
    // macros for declaring constants in a portable way.
    //
    // e.g. s64  x = D_S64(1234567812345678);
    // e.g. s64  x = D_S64(0x1111111122222222);
    // e.g. u64  x = D_U64(0x1111111122222222);
    //
    // Microsoft VC++'s definitions of D_S8/D_U8/D_S16/D_U16 are like so:
    //    #define D_S8(x)    (x)
    //    #define D_S16(x)   (x)
    //    #define D_U8(x)    (x)
    //    #define D_U16(x)   (x)
    // To consider: undefine Microsoft's and use the casting versions below.
    // ------------------------------------------------------------------------

#ifndef D_INTEGER_CONSTANT_DEFINED  // If the user hasn't already defined these...
#    define D_INTEGER_CONSTANT_DEFINED

#    ifndef D_S8
#        define D_CONSTANT_S8(x) s8(x)  // For the majority of compilers and platforms, long is 32 bits and long long is 64 bits.
#    endif
#    ifndef D_U8
#        define D_CONSTANT_U8(x) u8(x)
#        define D_NILL_U8        D_CONSTANT_U8(0xff)
#    endif
#    ifndef D_S16
#        define D_CONSTANT_S16(x) s16(x)
#    endif
#    ifndef D_U16
#        define D_CONSTANT_U16(x) u16(x)  // Possibly we should make this be u16(x##u). Let's see how compilers react before changing this.
#        define D_NILL_U16        D_CONSTANT_U16(0xffff)
#    endif
#    ifndef D_S32
#        define D_CONSTANT_S32(x) x##L
#    endif
#    ifndef D_U32
#        define D_CONSTANT_U32(x) x##UL
#        define D_NILL_U32        D_CONSTANT_U32(0xffffffff)
#    endif
#    ifndef D_S64
#        define D_CONSTANT_S64(x) (s64)x##LL  // The way to deal with this is to compare ULONG_MAX to 0xffffffff and if not equal, then remove the L.
#    endif
#    ifndef D_U64
#        define D_CONSTANT_U64(x) x##ULL  // We need to follow a similar approach for LL.
#        define D_NILL_U64        D_CONSTANT_U64(0xffffffffffffffff)
#    endif
#    ifndef D_UINTMAX
#        define D_UINTMAX(x) D_U64(x)
#    endif
#endif

// ------------------------------------------------------------------------
// type sizes
#ifndef D_INTEGER_MAX_DEFINED  // If the user hasn't already defined these...
#    define D_INTEGER_MAX_DEFINED

// The value must be 2^(n-1)-1
#    ifndef D_S8_MAX
#        define D_S8_MAX 127
#    endif
#    ifndef D_S16_MAX
#        define D_S16_MAX 32767
#    endif
#    ifndef D_S32_MAX
#        define D_S32_MAX 2147483647
#    endif
#    ifndef D_S64_MAX
#        define D_S64_MAX D_S64(9223372036854775807)
#    endif
#    ifndef D_INTMAX_MAX
#        define D_INTMAX_MAX D_S64_MAX
#    endif
#    ifndef D_INTPTR_MAX
#        if CC_PLATFORM_PTR_SIZE == 4
#            define D_INTPTR_MAX D_S32_MAX
#        else
#            define D_INTPTR_MAX D_S64_MAX
#        endif
#    endif

// The value must be either -2^(n-1) or 1-2(n-1).
#    ifndef D_S8_MIN
#        define D_S8_MIN -128
#    endif
#    ifndef D_S16_MIN
#        define D_S16_MIN -32768
#    endif
#    ifndef D_S32_MIN
#        define D_S32_MIN (-D_S32_MAX - 1)  // -2147483648
#    endif
#    ifndef D_S64_MIN
#        define D_S64_MIN (-D_S64_MAX - 1)  // -9223372036854775808
#    endif
#    ifndef D_INTMAX_MIN
#        define D_INTMAX_MIN D_S64_MIN
#    endif
#    ifndef D_INTPTR_MIN
#        if CC_PLATFORM_PTR_SIZE == 4
#            define D_INTPTR_MIN D_S32_MIN
#        else
#            define D_INTPTR_MIN D_S64_MIN
#        endif
#    endif

// The value must be 2^n-1
#    ifndef D_U8_MAX
#        define D_U8_MAX 0xffU  // 255
#    endif
#    ifndef D_U16_MAX
#        define D_U16_MAX 0xffffU  // 65535
#    endif
#    ifndef D_U32_MAX
#        define D_U32_MAX D_U32(0xffffffff)  // 4294967295
#    endif
#    ifndef D_U64_MAX
#        define D_U64_MAX D_U64(0xffffffffffffffff)  // 18446744073709551615
#    endif
#    ifndef D_UINTMAX_MAX
#        define D_UINTMAX_MAX D_U64_MAX
#    endif
#    ifndef D_UINTPTR_MAX
#        if CC_PLATFORM_PTR_SIZE == 4
#            define D_UINTPTR_MAX D_U32_MAX
#        else
#            define D_UINTPTR_MAX D_U64_MAX
#        endif
#    endif
#endif

    //==============================================================================
    // Min/Max values
    //==============================================================================

    const u8 cU8Min = (u8)0x00;   // minimum value of a u8.
    const u8 cU8Max = (u8)0xFF;   // maximum value of a u8.
    const s8 cS8Min = (s8)-0x80;  // minimum value of a s8.
    const s8 cS8Max = (s8)0x7F;   // maximum value of a s8.

    const u16 cU16Min = (u16)0x0000;   // minimum value of a u16
    const u16 cU16Max = (u16)0xFFFF;   // maximum value of a u16.
    const s16 cS16Min = (s16)-0x8000;  // minimum value of a s16.
    const s16 cS16Max = (s16)0x7FFF;   // maximum value of a s16.

    const u32 cU32Min = (u32)0x00000000;  // minimum value of a u32.
    const u32 cU32Max = (u32)0xFFFFFFFF;  // maximum value of a u32.
    const s32 cS32Min = (s32)0x80000000;  // minimum value of a s32.
    const s32 cS32Max = (s32)0x7FFFFFFF;  // maximum value of a s32.

    const u64 cU64Min = (u64)0x0000000000000000ull;  // minimum value of a u64.
    const u64 cU64Max = (u64)0xFFFFFFFFFFFFFFFFull;  // maximum value of a u64.
    const s64 cS64Min = (s64)0x8000000000000000ll;   // minimum value of a s64.
    const s64 cS64Max = (s64)0x7FFFFFFFFFFFFFFFll;   // maximum value of a s64.

    const f32 cF32Min = (f32)1.175494351e-38f;  // minimum value of a f32.
    const f32 cF32Max = (f32)3.402823466e+38f;  // maximum value of a f32.
    const f32 cF32Eps = (f32)0.0001f;           // default epsilon generally good to check values in the range [0 - 1], normalisations, dot products and such.

    const f64 cF64Min = (f64)2.2250738585072014e-308;  // minimum value of a f64.
    const f64 cF64Max = (f64)1.7976931348623158e+308;  // maximum value of a f64.

    //==============================================================================
    // KB, MB, GB, TB values
    //==============================================================================
    const u64 cKB = (u64)1024;
    const u64 cMB = (u64)1024 * 1024;
    const u64 cGB = (u64)1024 * 1024 * 1024;
    const u64 cTB = (u64)1024 * 1024 * 1024 * 1024;

#if defined(CC_COMPILER_HAS_INTTYPES) && (!defined(CC_COMPILER_MSVC) || (defined(CC_COMPILER_MSVC) && CC_COMPILER_VERSION >= 1800))
#    define CC_COMPILER_HAS_C99_FORMAT_MACROS
#endif

#ifndef CC_COMPILER_HAS_C99_FORMAT_MACROS
    // ------------------------------------------------------------------------
    // sized printf and scanf format specifiers
    // See the C99 standard, section 7.8.1 -- Macros for format specifiers.
    //
    // The C99 standard specifies that inttypes.h only define printf/scanf
    // format macros if __STDC_FORMAT_MACROS is defined before #including
    // inttypes.h. For consistency, we define both __STDC_FORMAT_MACROS and
    // the printf format specifiers here. We also skip the "least/most"
    // variations of these specifiers, as we've decided to do so with
    // basic types.
    //
    // For 64 bit systems, we assume the LP64 standard is followed
    // (as opposed to ILP64, etc.) For 32 bit systems, we assume the
    // ILP32 standard is followed. See:
    //    http://www.opengroup.org/public/tech/aspen/lp64_wp.htm
    // for information about this. Thus, on both 32 and 64 bit platforms,
    // %l refers to 32 bit data while %ll refers to 64 bit data.

#    ifndef __STDC_FORMAT_MACROS
#        define __STDC_FORMAT_MACROS
#    endif

#    if defined(CC_COMPILER_MSVC)  // VC++ 7.1+ understands long long as a data type but doesn't accept %ll as a printf specifier.
#        define CC_PRI_64_LENGTH_SPECIFIER "I64"
#        define CC_SCN_64_LENGTH_SPECIFIER "I64"
#    else
#        define CC_PRI_64_LENGTH_SPECIFIER "ll"
#        define CC_SCN_64_LENGTH_SPECIFIER "ll"
#    endif  // It turns out that some platforms use %q to represent a 64 bit value, but these are not relevant to us at this time.

// Printf format specifiers
#    if defined(CC_COMPILER_IS_C99) || defined(CC_COMPILER_GNUC)
#        define PRId8 "hhd"
#        define PRIi8 "hhi"
#        define PRIo8 "hho"
#        define PRIu8 "hhu"
#        define PRIx8 "hhx"
#        define PRIX8 "hhX"
#    else                  // VC++, Borland, etc. which have no way to specify 8 bit values other than %c.
#        define PRId8 "c"  // This may not work properly but it at least will not crash. Try using 16 bit versions instead.
#        define PRIi8 "c"  //  "
#        define PRIo8 "o"  //  "
#        define PRIu8 "u"  //  "
#        define PRIx8 "x"  //  "
#        define PRIX8 "X"  //  "
#    endif

#    define PRId16 "hd"
#    define PRIi16 "hi"
#    define PRIo16 "ho"
#    define PRIu16 "hu"
#    define PRIx16 "hx"
#    define PRIX16 "hX"

#    define PRId32 "d"  // This works for both 32 bit and 64 bit systems, as we assume LP64 conventions.
#    define PRIi32 "i"
#    define PRIo32 "o"
#    define PRIu32 "u"
#    define PRIx32 "x"
#    define PRIX32 "X"

#    define PRId64 CC_PRI_64_LENGTH_SPECIFIER "d"
#    define PRIi64 CC_PRI_64_LENGTH_SPECIFIER "i"
#    define PRIo64 CC_PRI_64_LENGTH_SPECIFIER "o"
#    define PRIu64 CC_PRI_64_LENGTH_SPECIFIER "u"
#    define PRIx64 CC_PRI_64_LENGTH_SPECIFIER "x"
#    define PRIX64 CC_PRI_64_LENGTH_SPECIFIER "X"

#    if (CC_PLATFORM_PTR_SIZE == 4)
#        define PRIdPTR PRId32  // Usage of pointer values will generate warnings with
#        define PRIiPTR PRIi32  // some compilers because they are defined in terms of
#        define PRIoPTR PRIo32  // integers. However, you can't simply use "p" because
#        define PRIuPTR PRIu32  // 'p' is interpreted in a specific and often different
#        define PRIxPTR PRIx32  // way by the library.
#        define PRIXPTR PRIX32
#    elif (CC_PLATFORM_PTR_SIZE == 8)
#        define PRIdPTR PRId64
#        define PRIiPTR PRIi64
#        define PRIoPTR PRIo64
#        define PRIuPTR PRIu64
#        define PRIxPTR PRIx64
#        define PRIXPTR PRIX64
#    endif

// Scanf format specifiers
#    if defined(CC_COMPILER_IS_C99) || defined(CC_COMPILER_GNUC)
#        define SCNd8 "hhd"
#        define SCNi8 "hhi"
#        define SCNo8 "hho"
#        define SCNu8 "hhu"
#        define SCNx8 "hhx"
#    else                  // VC++, Borland, etc. which have no way to specify 8 bit values other than %c.
#        define SCNd8 "c"  // This will not work properly but it at least will not crash. Try using 16 bit versions instead.
#        define SCNi8 "c"  //  "
#        define SCNo8 "c"  //  "
#        define SCNu8 "c"  //  "
#        define SCNx8 "c"  //  "
#    endif

#    define SCNd16 "hd"
#    define SCNi16 "hi"
#    define SCNo16 "ho"
#    define SCNu16 "hu"
#    define SCNx16 "hx"

#    define SCNd32 "d"  // This works for both 32 bit and 64 bit systems, as we assume LP64 conventions.
#    define SCNi32 "i"
#    define SCNo32 "o"
#    define SCNu32 "u"
#    define SCNx32 "x"

#    define SCNd64 CC_SCN_64_LENGTH_SPECIFIER "d"
#    define SCNi64 CC_SCN_64_LENGTH_SPECIFIER "i"
#    define SCNo64 CC_SCN_64_LENGTH_SPECIFIER "o"
#    define SCNu64 CC_SCN_64_LENGTH_SPECIFIER "u"
#    define SCNx64 CC_SCN_64_LENGTH_SPECIFIER "x"

#    if defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION >= 1900)
#        define SCNdPTR PRIdPTR
#        define SCNiPTR PRIiPTR
#        define SCNoPTR PRIoPTR
#        define SCNuPTR PRIuPTR
#        define SCNxPTR PRIxPTR
#    elif (CC_PLATFORM_PTR_SIZE == 4)
#        define SCNdPTR SCNd32  // Usage of pointer values will generate warnings with
#        define SCNiPTR SCNi32  // some compilers because they are defined in terms of
#        define SCNoPTR SCNo32  // integers. However, you can't simply use "p" because
#        define SCNuPTR SCNu32  // 'p' is interpreted in a specific and often different
#        define SCNxPTR SCNx32  // way by the library.
#    elif (CC_PLATFORM_PTR_SIZE == 8)
#        define SCNdPTR SCNd64
#        define SCNiPTR SCNi64
#        define SCNoPTR SCNo64
#        define SCNuPTR SCNu64
#        define SCNxPTR SCNx64
#    endif
#endif

// ------------------------------------------------------------------------
// intptr_t / uintptr_t
// Integer type guaranteed to be big enough to hold
// a native pointer ( intptr_t is defined in STDDEF.H )
//
#if !defined(_INTPTR_T_DEFINED) && !defined(_intptr_t_defined) && !defined(CC_COMPILER_HAS_C99_TYPES)
#    if (CC_PLATFORM_PTR_SIZE == 4)
    typedef s32 intptr_t;
#    elif (CC_PLATFORM_PTR_SIZE == 8)
    typedef s64 intptr_t;
#    endif

#    define _intptr_t_defined
#    define _INTPTR_T_DEFINED
#endif

#if !defined(_UINTPTR_T_DEFINED) && !defined(_uintptr_t_defined) && !defined(CC_COMPILER_HAS_C99_TYPES)
#    if (CC_PLATFORM_PTR_SIZE == 4)
    typedef u32 uintptr_t;
#    elif (CC_PLATFORM_PTR_SIZE == 8)
    typedef u64 uintptr_t;
#    endif

#    define _uintptr_t_defined
#    define _UINTPTR_T_DEFINED
#endif

#if !defined(CC_COMPILER_HAS_INTTYPES)
#    ifndef INTMAX_T_DEFINED
#        define INTMAX_T_DEFINED

    // At this time, all supported compilers have s64 as the max
    // integer type. Some compilers support a 128 bit integer type,
    // but in some cases it is not a true int128_t but rather a
    // crippled data type. Also, it turns out that Unix 64 bit ABIs
    // require that intmax_t be s64 and nothing larger. So we
    // play it safe here and set intmax_t to s64, even though
    // an int128_t type may exist.

    typedef s64 intmax_t;
    typedef u64 uintmax_t;
#    endif
#endif

    // ------------------------------------------------------------------------
    // char8_t  -- Guaranteed to be equal to the compiler's char data type.
    //             Some compilers implement char8_t as unsigned, though char
    //             is usually set to be signed.
    //
    // char16_t -- This is set to be an unsigned 16 bit value. If the compiler
    //             has wchar_t as an unsigned 16 bit value, then char16_t is
    //             set to be the same thing as wchar_t in order to allow the
    //             user to use char16_t with standard wchar_t functions.
    //
    // char32_t -- This is set to be an unsigned 32 bit value. If the compiler
    //             has wchar_t as an unsigned 32 bit value, then char32_t is
    //             set to be the same thing as wchar_t in order to allow the
    //             user to use char32_t with standard wchar_t functions.
    //
    // CC_CHAR8_UNIQUE
    // CC_CHAR16_NATIVE
    // CC_CHAR32_NATIVE
    // CC_WCHAR_UNIQUE
    //
    // VS2010 unilaterally defines char16_t and char32_t in its yvals.h header
    // unless _HAS_CHAR16_T_LANGUAGE_SUPPORT or _CHAR16T are defined.
    // However, VS2010 does not support the C++0x u"" and U"" string literals,
    // which makes its definition of char16_t and char32_t somewhat useless.
    // Until VC++ supports string literals, the build system should define
    // _CHAR16T and let CCore define char16_t and CC_CHAR16.
    //
    // GCC defines char16_t and char32_t in the C compiler in -std=gnu99 mode,
    // as __CHAR16_TYPE__ and __CHAR32_TYPE__, and for the C++ compiler
    // in -std=c++0x and -std=gnu++0x modes, as char16_t and char32_t too.
    //
    // The CC_WCHAR_UNIQUE symbol is defined to 1 if wchar_t is distinct from
    // char8_t, char16_t, and char32_t, and defined to 0 if not. In some cases,
    // if the compiler does not support char16_t/char32_t, one of these two types
    // is typically a typedef or define of wchar_t. For compilers that support
    // the C++11 unicode character types often overloads must be provided to
    // support existing code that passes a wide char string to a function that
    // takes a unicode string.
    //
    // The CC_CHAR8_UNIQUE symbol is defined to 1 if char8_t is distinct type
    // from char in the type system, and defined to 0 if otherwise.

#if !defined(CC_CHAR16_NATIVE)
// To do: Change this to be based on CC_COMPILER_NO_NEW_CHARACTER_TYPES.
#    if defined(_MSC_VER) && (_MSC_VER >= 1600) && defined(_HAS_CHAR16_T_LANGUAGE_SUPPORT) && _HAS_CHAR16_T_LANGUAGE_SUPPORT  // VS2010+
#        define CC_CHAR16_NATIVE 1
#    elif defined(CC_COMPILER_CLANG) && defined(CC_COMPILER_CPP11_ENABLED)
#        if __has_feature(cxx_unicode_literals)
#            define CC_CHAR16_NATIVE 1
#        elif (CC_COMPILER_VERSION >= 300) && !(defined(CC_PLATFORM_IPHONE) || defined(CC_PLATFORM_OSX))
#            define CC_CHAR16_NATIVE 1
#        elif defined(CC_PLATFORM_APPLE)
#            define CC_CHAR16_NATIVE 1
#        else
#            define CC_CHAR16_NATIVE 0
#        endif
#    elif defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 404) && defined(__CHAR16_TYPE__) && defined(CC_COMPILER_CPP11_ENABLED)  // EDG 4.4+.
#        define CC_CHAR16_NATIVE 1
#    elif defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4004) && !defined(CC_COMPILER_EDG) && (defined(CC_COMPILER_CPP11_ENABLED) || defined(__STDC_VERSION__))  // g++ (C++ compiler) 4.4+ with -std=c++0x or gcc (C compiler) 4.4+ with -std=gnu99
#        define CC_CHAR16_NATIVE 1
#    else
#        define CC_CHAR16_NATIVE 0
#    endif
#endif

#if !defined(CC_CHAR32_NATIVE)  // Microsoft currently ties char32_t language support to char16_t language support. So we use CHAR16_T here.
// To do: Change this to be based on CC_COMPILER_NO_NEW_CHARACTER_TYPES.
#    if defined(_MSC_VER) && (_MSC_VER >= 1600) && defined(_HAS_CHAR16_T_LANGUAGE_SUPPORT) && _HAS_CHAR16_T_LANGUAGE_SUPPORT  // VS2010+
#        define CC_CHAR32_NATIVE 1
#    elif defined(CC_COMPILER_CLANG) && defined(CC_COMPILER_CPP11_ENABLED)
#        if __has_feature(cxx_unicode_literals)
#            define CC_CHAR32_NATIVE 1
#        elif (CC_COMPILER_VERSION >= 300) && !(defined(CC_PLATFORM_IPHONE) || defined(CC_PLATFORM_OSX))
#            define CC_CHAR32_NATIVE 1
#        elif defined(CC_PLATFORM_APPLE)
#            define CC_CHAR32_NATIVE 1
#        else
#            define CC_CHAR32_NATIVE 0
#        endif
#    elif defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 404) && defined(__CHAR32_TYPE__) && defined(CC_COMPILER_CPP11_ENABLED)  // EDG 4.4+.
#        define CC_CHAR32_NATIVE 1
#    elif defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4004) && !defined(CC_COMPILER_EDG) && (defined(CC_COMPILER_CPP11_ENABLED) || defined(__STDC_VERSION__))  // g++ (C++ compiler) 4.4+ with -std=c++0x or gcc (C compiler) 4.4+ with -std=gnu99
#        define CC_CHAR32_NATIVE 1
#    else
#        define CC_CHAR32_NATIVE 0
#    endif
#endif

#if CC_CHAR16_NATIVE || CC_CHAR32_NATIVE
#    define CC_WCHAR_UNIQUE 1
#else
#    define CC_WCHAR_UNIQUE 0
#endif

// CC_CHAR8_UNIQUE
//
// Check for char8_t support in the cpp type system. Moving forward from c++20,
// the char8_t type allows users to overload function for character encoding.
//
// CC_CHAR8_UNIQUE is 1 when the type is a unique in the type system and
// can there be used as a valid overload. CC_CHAR8_UNIQUE is 0 otherwise.
//
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2018/p0482r6.html
//
#ifdef __cpp_char8_t
#    define CHAR8_T_DEFINED
#    define CC_CHAR8_UNIQUE 1
#else
#    define CC_CHAR8_UNIQUE 0
#endif

#ifndef CHAR8_T_DEFINED  // If the user hasn't already defined these...
#    define CHAR8_T_DEFINED
#    if defined(CC_PLATFORM_APPLE)
#        define char8_t char  // The Apple debugger is too stupid to realize char8_t is typedef'd to char, so we #define it.
#    else
    typedef char char8_t;
#    endif

#    if CC_CHAR16_NATIVE
// In C++, char16_t and char32_t are already defined by the compiler.
// In MS C, char16_t and char32_t are already defined by the compiler/standard library.
// In GCC C, __CHAR16_TYPE__ and __CHAR32_TYPE__ are defined instead, and we must define char16_t and char32_t from these.
#        if defined(__GNUC__) && !defined(__GXX_EXPERIMENTAL_CXX0X__) && defined(__CHAR16_TYPE__)  // If using GCC and compiling in C...
    typedef __CHAR16_TYPE__ char16_t;
    typedef __CHAR32_TYPE__ char32_t;
#        endif
#    elif (CC_WCHAR_SIZE == 2)
#        if (defined(_MSC_VER) && (_MSC_VER >= 1600))  // if VS2010+ or using platforms that use Dinkumware under a compiler that doesn't natively support C++11 char16_t.
#            if !defined(_CHAR16T)
#                define _CHAR16T
#            endif
#            if !defined(_HAS_CHAR16_T_LANGUAGE_SUPPORT) || !_HAS_CHAR16_T_LANGUAGE_SUPPORT
    typedef wchar_t char16_t;
    typedef u32 char32_t;
#            endif
#        else
    typedef wchar_t char16_t;
    typedef u32 char32_t;
#        endif
#    else
    typedef u16 char16_t;
#        if defined(__cplusplus)
    typedef wchar_t char32_t;
#        else
    typedef u32 char32_t;
#        endif
#    endif
#endif

// CHAR8_MIN, CHAR8_MAX, etc.
//
#define CC_LIMITS_DIGITS_S(T)  ((sizeof(T) * 8) - 1)
#define CC_LIMITS_DIGITS_U(T)  ((sizeof(T) * 8))
#define CC_LIMITS_DIGITS(T)    ((CC_LIMITS_IS_SIGNED(T) ? CC_LIMITS_DIGITS_S(T) : CC_LIMITS_DIGITS_U(T)))
#define CC_LIMITS_IS_SIGNED(T) ((T)(-1) < 0)
#define CC_LIMITS_MIN_S(T)     ((T)((T)1 << CC_LIMITS_DIGITS_S(T)))
#define CC_LIMITS_MIN_U(T)     ((T)0)
#define CC_LIMITS_MIN(T)       ((CC_LIMITS_IS_SIGNED(T) ? CC_LIMITS_MIN_S(T) : CC_LIMITS_MIN_U(T)))
#define CC_LIMITS_MAX_S(T)     ((T)(((((T)1 << (CC_LIMITS_DIGITS(T) - 1)) - 1) << 1) + 1))
#define CC_LIMITS_MAX_U(T)     ((T) ~(T)0)
#define CC_LIMITS_MAX(T)       ((CC_LIMITS_IS_SIGNED(T) ? CC_LIMITS_MAX_S(T) : CC_LIMITS_MAX_U(T)))

#if !defined(CHAR8_MIN)
#    define CHAR8_MIN CC_LIMITS_MIN(char8_t)
#endif

#if !defined(CHAR8_MAX)
#    define CHAR8_MAX CC_LIMITS_MAX(char8_t)
#endif

#if !defined(CHAR16_MIN)
#    define CHAR16_MIN CC_LIMITS_MIN(char16_t)
#endif

#if !defined(CHAR16_MAX)
#    define CHAR16_MAX CC_LIMITS_MAX(char16_t)
#endif

#if !defined(CHAR32_MIN)
#    define CHAR32_MIN CC_LIMITS_MIN(char32_t)
#endif

#if !defined(CHAR32_MAX)
#    define CHAR32_MAX CC_LIMITS_MAX(char32_t)
#endif

// CC_CHAR8 / CC_CHAR16 / CC_CHAR32 / CC_WCHAR
//
// Supports usage of portable string constants.
//
// Example usage:
//     const char16_t* str = CC_CHAR16("Hello world");
//     const char32_t* str = CC_CHAR32("Hello world");
//     const char16_t  c   = CC_CHAR16('\x3001');
//     const char32_t  c   = CC_CHAR32('\x3001');
//
#ifndef CC_CHAR8
#    if CC_CHAR8_UNIQUE
#        define CC_CHAR8(s) u8##s
#    else
#        define CC_CHAR8(s) s
#    endif
#endif

#ifndef CC_WCHAR
#    define CC_WCHAR_(s) L##s
#    define CC_WCHAR(s)  CC_WCHAR_(s)
#endif

#ifndef CC_CHAR16
#    if CC_CHAR16_NATIVE && !defined(_MSC_VER)  // Microsoft doesn't support char16_t string literals.
#        define CC_CHAR16_(s) u##s
#        define CC_CHAR16(s)  CC_CHAR16_(s)
#    elif (CC_WCHAR_SIZE == 2)
#        if defined(_MSC_VER) && (_MSC_VER >= 1900) && defined(__cplusplus)  // VS2015 supports u"" string literals.
#            define CC_CHAR16_(s) u##s
#            define CC_CHAR16(s)  CC_CHAR16_(s)
#        else
#            define CC_CHAR16_(s) L##s
#            define CC_CHAR16(s)  CC_CHAR16_(s)
#        endif
#    else
    // #define CC_CHAR16(s) // Impossible to implement efficiently.
#    endif
#endif

#ifndef CC_CHAR32
#    if CC_CHAR32_NATIVE && !defined(_MSC_VER)  // Microsoft doesn't support char32_t string literals.
#        define CC_CHAR32_(s) U##s
#        define CC_CHAR32(s)  CC_CHAR32_(s)
#    elif (CC_WCHAR_SIZE == 2)
#        if defined(_MSC_VER) && (_MSC_VER >= 1900) && defined(__cplusplus)  // VS2015 supports u"" string literals.
#            define CC_CHAR32_(s) U##s
#            define CC_CHAR32(s)  CC_CHAR32_(s)
#        else
    // #define CC_CHAR32(s) // Impossible to implement.
#        endif
#    elif (CC_WCHAR_SIZE == 4)
#        define CC_CHAR32_(s) L##s
#        define CC_CHAR32(s)  CC_CHAR32_(s)
#    else
#        error Unexpected size of wchar_t
#    endif
#endif

// ------------------------------------------------------------------------
// DArrayCount
//
// Returns the count of items in a built-in C array. This is a common technique
// which is often used to help properly calculate the number of items in an
// array at runtime in order to prevent overruns, etc.
//
// Example usage:
//     int array[75];
//     size_t arrayCount = CCArrayCount(array); // arrayCount is 75.
//
#if defined(CC_COMPILER_NO_CONSTEXPR)
#    ifndef DArrayCount
#        define DArrayCount(x) (sizeof(x) / sizeof(x[0]))
#    endif
#else
    // This C++11 version is a little smarter than the macro version above;
    // it can tell the difference between arrays and pointers. Other simpler
    // templated versions have failed in various subtle ways.

    template <typename T, size_t N>
    char (&CCArraySizeHelper(T (&x)[N]))[N];

    template <typename T, size_t N>
    char (&CCArraySizeHelper(T (&&x)[N]))[N];

#    define DARRAYSIZE(x) (sizeof(CCArraySizeHelper(x)))
#endif

// ------------------------------------------------------------------------
// static_assert
//
// C++11 static_assert (a.k.a. compile-time assert).
//
// Specification:
//     void static_assert(bool const_expression, const char* description);
//
// Example usage:
//     static_assert(sizeof(int) == 4, "int must be 32 bits");
//
#if defined(_MSC_VER) && (_MSC_VER >= 1600) && defined(__cplusplus)
    // static_assert is defined by the compiler for both C and C++.
#elif !defined(__cplusplus) && defined(CC_PLATFORM_ANDROID) && ((defined(__STDC_VERSION__) && __STDC_VERSION__ < 201100L) || !defined(__STDC_VERSION__))
// AndroidNDK does not support static_assert despite claiming it's a C11 compiler
#    define NEED_CUSTOM_STATIC_ASSERT
#elif defined(__clang__) && defined(__cplusplus)
// We need to separate these checks on a new line, as the pre-processor on other compilers will fail on the _has_feature macros
#    if !(__has_feature(cxx_static_assert) || __has_extension(cxx_static_assert))
#        define NEED_CUSTOM_STATIC_ASSERT
#    endif
#elif defined(__GNUC__) && (defined(__GXX_EXPERIMENTAL_CXX0X__) || (defined(__cplusplus) && (__cplusplus >= 201103L)))
    // static_assert is defined by the compiler.
#elif defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401) && defined(CC_COMPILER_CPP11_ENABLED)
    // static_assert is defined by the compiler.
#elif !defined(__cplusplus) && defined(__GLIBC__) && defined(__USE_ISOC11)
    // static_assert is defined by the compiler.
#elif !defined(__cplusplus) && defined(__STDC_VERSION__) && __STDC_VERSION__ >= 201100L
    // static_assert is defined by the compiler.
#else
#    define NEED_CUSTOM_STATIC_ASSERT
#endif

#ifdef NEED_CUSTOM_STATIC_ASSERT
#    ifdef __GNUC__
// On GCC the 'unused' attribute can be used to indicate a typedef is not actually used
// (such as in the static_assert implementation below).  New versions of GCC generate
// warnings for unused typedefs in function/method scopes.
#        define CC_STATIC_ASSERT_UNUSED_ATTRIBUTE __attribute__((unused))
#    else
#        define CC_STATIC_ASSERT_UNUSED_ATTRIBUTE
#    endif

#    define CC_STATIC_ASSERT_TOKEN_PASTE(a, b)        a##b
#    define CC_STATIC_ASSERT_CONCATENATE_HELPER(a, b) CC_STATIC_ASSERT_TOKEN_PASTE(a, b)

#    if defined(__COUNTER__)  // If this extension is available, which allows multiple statements per line...
#        define static_assert(expression, description) typedef char CC_STATIC_ASSERT_CONCATENATE_HELPER(compileTimeAssert, __COUNTER__)[((expression) != 0) ? 1 : -1] CC_STATIC_ASSERT_UNUSED_ATTRIBUTE
#    else
#        define static_assert(expression, description) typedef char CC_STATIC_ASSERT_CONCATENATE_HELPER(compileTimeAssert, __LINE__)[((expression) != 0) ? 1 : -1] CC_STATIC_ASSERT_UNUSED_ATTRIBUTE
#    endif

#    undef NEED_CUSTOM_STATIC_ASSERT
#endif

    // ------------------------------------------------------------------------
    // CC_IS_ENABLED
    //
    // CC_IS_ENABLED is intended to be used for detecting if compile time features are enabled or disabled.
    //
    // It has some advantages over using a standard #if or #ifdef tests:
    //	1) Fails to compile when given numeric macro values, valid options are strictly CC_ENABLED or CC_DISABLED.
    //	2) Fails to compile when given an undefined macro value rather than disabling by default
    //	3) Fails to compile when the passed macro is defined but has no value
    //
    // To use this macro, the calling code should create a define for the feature to enable or disable. This define
    // must be set to either CC_ENABLED or CC_DISABLED, any other value will result in a compile error.
    //
    // Example usage:
    //
    //      // CC_ENABLED or CC_DISABLED.
    //      #define USER_FEATURE_DEFINE CC_ENABLED
    //
    //      #if CC_IS_ENABLED(USER_FEATURE_DEFINE)
    //          // USER_FEATURE_DEFINE is enabled
    //      #else
    //          // USER_FEATURE_DEFINE is disabled
    //      #endif
    //

    // clang-format off
    #define CC_ENABLED  64-
    #define CC_DISABLED 32-

    #define CC_IS_ENABLED(x) (32 == 32 * 64 / ((x 0) * (((x 0) == 32 ? 1 : 0) + ((x 0) == 64 ? 1 : 0))))
    #define CC_IS_DISABLED(x) (64 == 32 * 64 / ((x 0) * (((x 0) == 32 ? 1 : 0) + ((x 0) == 64 ? 1 : 0))))
    // clang-format on

};  // namespace ncore

#endif

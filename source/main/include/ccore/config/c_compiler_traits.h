#ifndef __CCORE_TARGET_COMPILER_TRAITS_V2_H__
#define __CCORE_TARGET_COMPILER_TRAITS_V2_H__
#pragma once

namespace ncore
{
    // ---------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------
    //  COMPILER TRAITS
    // ---------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------
    // ---------------------------------------------------------------------------------------------------------------------

    // Currently supported defines include:
    //    CC_PREPROCESSOR_JOIN
    //
    //    CC_COMPILER_IS_ANSIC
    //    CC_COMPILER_IS_C99
    //    CC_COMPILER_IS_C11
    //    CC_COMPILER_HAS_C99_TYPES
    //    CC_COMPILER_IS_CPLUSPLUS
    //    CC_COMPILER_MANAGED_CPP
    //    CC_COMPILER_INTMAX_SIZE
    //    CC_OFFSETOF
    //    CC_SIZEOF_MEMBER
    //
    //    CC_ALIGN_OF()
    //    CC_ALIGN_MAX_STATIC / CC_ALIGN_MAX_AUTOMATIC
    //    CC_ALIGN() / CC_PREFIX_ALIGN() / CC_POSTFIX_ALIGN()
    //    CC_ALIGNED()
    //    CC_PACKED()
    //
    //    CC_LIKELY()
    //    CC_UNLIKELY()
    //    CC_INIT_PRIORITY()
    //    CC_MAY_ALIAS()
    //    CC_ASSUME()
    //    CC_ANALYSIS_ASSUME()
    //    CC_PURE
    //    CC_WEAK
    //    CC_UNUSED()
    //    CC_EMPTY()
    //
    //    CC_WCHAR_T_NON_NATIVE
    //    CC_WCHAR_SIZE = <n bytes>
    //
    //    CC_RESTRICT
    //    CC_DEPRECATED   / CC_PREFIX_DEPRECATED   / CC_POSTFIX_DEPRECATED
    //    CC_FORCE_INLINE /
    //    CC_NO_INLINE    /
    //    CC_NO_VTABLE    / CC_CLASS_NO_VTABLE     / CC_STRUCT_NO_VTABLE
    //    CC_PASCAL
    //    CC_PASCAL_FUNC()
    //    CC_SSE = [0 | 1]
    //    CC_IMPORT
    //    CC_EXPORT
    //    CC_PRAGMA_ONCE_SUPPORTED
    //    CC_INCLUDE_ONCE
    //    CC_OVERRIDE
    //    CC_FINAL
    //    CC_SEALED
    //    CC_ABSTRACT
    //    CC_CONSTEXPR / CC_CONSTEXPR_OR_CONST
    //    CC_CONSTEXPR_IF
    //    CC_EXTERN_TEMPLATE
    //    CC_NOEXCEPT
    //    CC_NORETURN
    //    CC_CARRIES_DEPENDENCY
    //    CC_NON_COPYABLE / struct EANonCopyable
    //    CC_OPTIMIZE_OFF / CC_OPTIMIZE_ON
    //    CC_SIGNED_RIGHT_SHIFT_IS_UNSIGNED
    //
    //    CC_DISABLE_VC_WARNING    / CC_RESTORE_VC_WARNING / CC_DISABLE_ALL_VC_WARNINGS / CC_RESTORE_ALL_VC_WARNINGS
    //    CC_DISABLE_GCC_WARNING   / CC_RESTORE_GCC_WARNING
    //    CC_DISABLE_CLANG_WARNING / CC_RESTORE_CLANG_WARNING
    //    CC_DISABLE_SN_WARNING    / CC_RESTORE_SN_WARNING / CC_DISABLE_ALL_SN_WARNINGS / CC_RESTORE_ALL_SN_WARNINGS
    //    CC_DISABLE_GHS_WARNING   / CC_RESTORE_GHS_WARNING
    //    CC_DISABLE_EDG_WARNING   / CC_RESTORE_EDG_WARNING
    //    CC_DISABLE_CW_WARNING    / CC_RESTORE_CW_WARNING
    //
    //    CC_DISABLE_DEFAULT_CTOR
    //    CC_DISABLE_COPY_CTOR
    //    CC_DISABLE_MOVE_CTOR
    //    CC_DISABLE_ASSIGNMENT_OPERATOR
    //    CC_DISABLE_MOVE_OPERATOR
    //
    //  Todo:
    //    Find a way to reliably detect wchar_t size at preprocessor time and
    //    implement it below for CC_WCHAR_SIZE.
    //

// Metrowerks uses #defines in its core C header files to define
// the kind of information we need below (e.g. C99 compatibility)

// Determine if this compiler is ANSI C compliant and if it is C99 compliant.
#if defined(__STDC__)
#    define CC_COMPILER_IS_ANSIC 1  // The compiler claims to be ANSI C

// Is the compiler a C99 compiler or equivalent?
// From ISO/IEC 9899:1999:
//    6.10.8 Predefined macro names
//    __STDC_VERSION__ The integer constant 199901L. (150)
//
//    150) This macro was not specified in ISO/IEC 9899:1990 and was
//    specified as 199409L in ISO/IEC 9899/AMD1:1995. The intention
//    is that this will remain an integer constant of type long int
//    that is increased with each revision of this International Standard.
//
#    if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901L)
#        define CC_COMPILER_IS_C99 1
#    endif

// Is the compiler a C11 compiler?
// From ISO/IEC 9899:2011:
//   Page 176, 6.10.8.1 (Predefined macro names) :
//   __STDC_VERSION__ The integer constant 201112L. (178)
//
#    if defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 201112L)
#        define CC_COMPILER_IS_C11 1
#    endif
#endif

// Some compilers (e.g. GCC) define __USE_ISOC99 if they are not
// strictly C99 compilers (or are simply C++ compilers) but are set
// to use C99 functionality. Metrowerks defines _MSL_C99 as 1 in
// this case, but 0 otherwise.
#if (defined(__USE_ISOC99) || (defined(_MSL_C99) && (_MSL_C99 == 1))) && !defined(CC_COMPILER_IS_C99)
#    define CC_COMPILER_IS_C99 1
#endif

// Metrowerks defines C99 types (e.g. intptr_t) instrinsically when in C99 mode (-lang C99 on the command line).
#if (defined(_MSL_C99) && (_MSL_C99 == 1))
#    define CC_COMPILER_HAS_C99_TYPES 1
#endif

#if defined(__GNUC__)
#    if (((__GNUC__ * 100) + __GNUC_MINOR__) >= 302)  // Also, GCC defines _HAS_C9X.
#        define CC_COMPILER_HAS_C99_TYPES 1           // The compiler is not necessarily a C99 compiler, but it defines C99 types.

    // #ifndef __STDC_LIMIT_MACROS
    //     #define __STDC_LIMIT_MACROS 1
    // #endif

    // #ifndef __STDC_CONSTANT_MACROS
    //     #define __STDC_CONSTANT_MACROS 1  // This tells the GCC compiler that we want it to use its native C99 types.
    // #endif
#    endif
#endif

#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#    define CC_COMPILER_HAS_C99_TYPES 1
#endif

#ifdef __cplusplus
#    define CC_COMPILER_IS_CPLUSPLUS 1
#endif

// ------------------------------------------------------------------------
// CC_PREPROCESSOR_JOIN
//
// This macro joins the two arguments together, even when one of
// the arguments is itself a macro (see 16.3.1 in C++98 standard).
// This is often used to create a unique name with __LINE__.
//
// For example, this declaration:
//    char CC_PREPROCESSOR_JOIN(unique_, __LINE__);
// expands to this:
//    char unique_73;
//
// Note that all versions of MSVC++ up to at least version 7.1
// fail to properly compile macros that use __LINE__ in them
// when the "program database for edit and continue" option
// is enabled. The result is that __LINE__ gets converted to
// something like __LINE__(Var+37).
//
#ifndef CC_PREPROCESSOR_JOIN
#    define CC_PREPROCESSOR_JOIN(a, b)  CC_PREPROCESSOR_JOIN1(a, b)
#    define CC_PREPROCESSOR_JOIN1(a, b) CC_PREPROCESSOR_JOIN2(a, b)
#    define CC_PREPROCESSOR_JOIN2(a, b) a##b
#endif

// ------------------------------------------------------------------------
// CC_STRINGIFY
//
// Example usage:
//     printf("Line: %s", CC_STRINGIFY(__LINE__));
//
#ifndef CC_STRINGIFY
#    define CC_STRINGIFY(x)     CC_STRINGIFYIMPL(x)
#    define CC_STRINGIFYIMPL(x) #x
#endif

// ------------------------------------------------------------------------
// CC_IDENTITY
//
#ifndef CC_IDENTITY
#    define CC_IDENTITY(x) x
#endif

// ------------------------------------------------------------------------
// CC_COMPILER_MANAGED_CPP
// Defined if this is being compiled with Managed C++ extensions
#ifdef CC_COMPILER_MSVC
#    if CC_COMPILER_VERSION >= 1300
#        ifdef _MANAGED
#            define CC_COMPILER_MANAGED_CPP 1
#        endif
#    endif
#endif

// ------------------------------------------------------------------------
// CC_COMPILER_INTMAX_SIZE
//
// This is related to the concept of intmax_t uintmax_t, but is available
// in preprocessor form as opposed to compile-time form. At compile-time
// you can use intmax_t and uintmax_t to use the actual types.
//
#if defined(__GNUC__) && defined(__x86_64__)
#    define CC_COMPILER_INTMAX_SIZE 16  // intmax_t is __int128_t (GCC extension) and is 16 bytes.
#else
#    define CC_COMPILER_INTMAX_SIZE 8  // intmax_t is int64_t and is 8 bytes.
#endif

// ------------------------------------------------------------------------
// CC_LPAREN / CC_RPAREN / CC_COMMA / CC_SEMI
//
// These are used for using special characters in macro-using expressions.
// Note that this macro intentionally uses (), as in some cases it can't
// work unless it does.
//
// Example usage:
//     int x = SOME_MACRO(SomeTemplate<int CC_COMMA() int EACOMMA() char>);
//
#ifndef CC_LPAREN
#    define CC_LPAREN() (
#endif
#ifndef CC_RPAREN
#    define CC_RPAREN() )
#endif
#ifndef CC_COMMA
#    define CC_COMMA() ,
#endif
#ifndef CC_SEMI
#    define CC_SEMI() ;
#endif

// ------------------------------------------------------------------------
// CC_OFFSETOF
// Implements a portable version of the non-standard offsetof macro.
//
// The offsetof macro is guaranteed to only work with POD types. However, we wish to use
// it for non-POD types but where we know that offsetof will still work for the cases
// in which we use it. GCC unilaterally gives a warning when using offsetof with a non-POD,
// even if the given usage happens to work. So we make a workaround version of offsetof
// here for GCC which has the same effect but tricks the compiler into not issuing the warning.
// The 65536 does the compiler fooling; the reinterpret_cast prevents the possibility of
// an overloaded operator& for the class getting in the way.
//
// Example usage:
//     struct A{ int x; int y; };
//     size_t n = CC_OFFSETOF(A, y);
//
#if defined(__GNUC__)  // We can't use GCC 4's __builtin_offsetof because it mistakenly complains about non-PODs that are really PODs.
#    define CC_OFFSETOF(struct_, member_) ((size_t)(((uintptr_t)&reinterpret_cast<const volatile char&>((((struct_*)65536)->member_))) - 65536))
#else
#    define CC_OFFSETOF(struct_, member_) offsetof(struct_, member_)
#endif

// ------------------------------------------------------------------------
// CC_SIZEOF_MEMBER
// Implements a portable way to determine the size of a member.
//
// The CC_SIZEOF_MEMBER simply returns the size of a member within a class or struct; member
// access rules still apply. We offer two approaches depending on the compiler's support for non-static member
// initializers although most C++11 compilers support this.
//
// Example usage:
//     struct A{ int x; int y; };
//     size_t n = CC_SIZEOF_MEMBER(A, y);
//
#ifndef CC_COMPILER_NO_EXTENDED_SIZEOF
#    define CC_SIZEOF_MEMBER(struct_, member_) (sizeof(struct_::member_))
#else
#    define CC_SIZEOF_MEMBER(struct_, member_) (sizeof(((struct_*)0)->member_))
#endif

    // ------------------------------------------------------------------------
    // alignment expressions
    //
    // Here we define
    //    CC_ALIGN_OF(type)         // Returns size_t.
    //    CC_ALIGN_MAX_STATIC       // The max align value that the compiler will respect for CC_ALIGN for static data (global and static variables). Some compilers allow high values, some allow no more than 8. CC_ALIGN_MIN is assumed to be 1.
    //    CC_ALIGN_MAX_AUTOMATIC    // The max align value for automatic variables (variables declared as local to a function).
    //    CC_ALIGN(n)               // Used as a prefix. n is byte alignment, with being a power of two. Most of the time you can use this and avoid using CC_PREFIX_ALIGN/CC_POSTFIX_ALIGN.
    //    CC_ALIGNED(t, v, n)       // Type, variable, alignment. Used to align an instance. You should need this only for unusual compilers.
    //    CC_PACKED                 // Specifies that the given structure be packed (and not have its members aligned).
    //
    // Also we define the following for rare cases that it's needed.
    //    CC_PREFIX_ALIGN(n)        // n is byte alignment, with being a power of two. You should need this only for unusual compilers.
    //    CC_POSTFIX_ALIGN(n)       // Valid values for n are 1, 2, 4, 8, etc. You should need this only for unusual compilers.
    //
    // Example usage:
    //    size_t x = CC_ALIGN_OF(int);                                  Non-aligned equivalents.        Meaning
    //    CC_PREFIX_ALIGN(8) int x = 5;                                 int x = 5;                      Align x on 8 for compilers that require prefix attributes. Can just use CC_ALIGN instead.
    //    CC_ALIGN(8) int x;                                            int x;                          Align x on 8 for compilers that allow prefix attributes.
    //    int x CC_POSTFIX_ALIGN(8);                                    int x;                          Align x on 8 for compilers that require postfix attributes.
    //    int x CC_POSTFIX_ALIGN(8) = 5;                                int x = 5;                      Align x on 8 for compilers that require postfix attributes.
    //    int x CC_POSTFIX_ALIGN(8)(5);                                 int x(5);                       Align x on 8 for compilers that require postfix attributes.
    //    struct CC_PREFIX_ALIGN(8) X { int x; } CC_POSTFIX_ALIGN(8);   struct X { int x; };            Define X as a struct which is aligned on 8 when used.
    //    CC_ALIGNED(int, x, 8) = 5;                                    int x = 5;                      Align x on 8.
    //    CC_ALIGNED(int, x, 16)(5);                                    int x(5);                       Align x on 16.
    //    CC_ALIGNED(int, x[3], 16);                                    int x[3];                       Align x array on 16.
    //    CC_ALIGNED(int, x[3], 16) = { 1, 2, 3 };                      int x[3] = { 1, 2, 3 };         Align x array on 16.
    //    int x[3] CC_PACKED;                                           int x[3];                       Pack the 3 ints of the x array. GCC doesn't seem to support packing of int arrays.
    //    struct CC_ALIGN(32) X { int x; int y; };                      struct X { int x; };            Define A as a struct which is aligned on 32 when used.
    //    CC_ALIGN(32) struct X { int x; int y; } Z;                    struct X { int x; } Z;          Define A as a struct, and align the instance Z on 32.
    //    struct X { int x CC_PACKED; int y CC_PACKED; };               struct X { int x; int y; };     Pack the x and y members of struct X.
    //    struct X { int x; int y; } CC_PACKED;                         struct X { int x; int y; };     Pack the members of struct X.
    //    typedef CC_ALIGNED(int, int16, 16); int16 n16;                typedef int int16; int16 n16;   Define int16 as an int which is aligned on 16.
    //    typedef CC_ALIGNED(X, X16, 16); X16 x16;                      typedef X X16; X16 x16;         Define X16 as an X which is aligned on 16.

#if !defined(CC_ALIGN_MAX)         // If the user hasn't globally set an alternative value...
#    if defined(CC_PROCESSOR_ARM)  // ARM compilers in general tend to limit automatic variables to 8 or less.
#        define CC_ALIGN_MAX_STATIC    1048576
#        define CC_ALIGN_MAX_AUTOMATIC 1  // Typically they support only built-in natural aligment types (both arm-eabi and apple-abi).
#    elif defined(CC_PLATFORM_APPLE)
#        define CC_ALIGN_MAX_STATIC    1048576
#        define CC_ALIGN_MAX_AUTOMATIC 16
#    else
#        define CC_ALIGN_MAX_STATIC    1048576  // Arbitrarily high value. What is the actual max?
#        define CC_ALIGN_MAX_AUTOMATIC 1048576
#    endif
#endif

// EDG intends to be compatible with GCC but has a bug whereby it
// fails to support calling a constructor in an aligned declaration when
// using postfix alignment attributes. Prefix works for alignment, but does not align
// the size like postfix does.  Prefix also fails on templates.  So gcc style post fix
// is still used, but the user will need to use CC_POSTFIX_ALIGN before the constructor parameters.
#if defined(__GNUC__) && (__GNUC__ < 3)
#    define CC_ALIGN_OF(type) ((size_t)__alignof__(type))
#    define CC_ALIGN(n)
#    define CC_PREFIX_ALIGN(n)
#    define CC_POSTFIX_ALIGN(n)                    __attribute__((aligned(n)))
#    define CC_ALIGNED(variable_type, variable, n) variable_type variable __attribute__((aligned(n)))
#    define CC_PACKED                              __attribute__((packed))

// GCC 3.x+, IBM, and clang support prefix attributes.
#elif (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__xlC__) || defined(__clang__)
#    define CC_ALIGN_OF(type) ((size_t)__alignof__(type))
#    define CC_ALIGN(n)       __attribute__((aligned(n)))
#    define CC_PREFIX_ALIGN(n)
#    define CC_POSTFIX_ALIGN(n)                    __attribute__((aligned(n)))
#    define CC_ALIGNED(variable_type, variable, n) variable_type variable __attribute__((aligned(n)))
#    define CC_PACKED                              __attribute__((packed))

// Metrowerks supports prefix attributes.
// Metrowerks does not support packed alignment attributes.
#elif defined(CC_COMPILER_INTEL) || defined(CS_UNDEFINED_STRING) || (defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION >= 1300))
#    define CC_ALIGN_OF(type)  ((size_t)__alignof(type))
#    define CC_ALIGN(n)        __declspec(align(n))
#    define CC_PREFIX_ALIGN(n) CC_ALIGN(n)
#    define CC_POSTFIX_ALIGN(n)
#    define CC_ALIGNED(variable_type, variable, n) CC_ALIGN(n) variable_type variable
#    define CC_PACKED  // See CC_PRAGMA_PACK_VC for an alternative.

// Arm brand compiler
#elif defined(CC_COMPILER_ARM)
#    define CC_ALIGN_OF(type)  ((size_t)__ALIGNOF__(type))
#    define CC_ALIGN(n)        __align(n)
#    define CC_PREFIX_ALIGN(n) __align(n)
#    define CC_POSTFIX_ALIGN(n)
#    define CC_ALIGNED(variable_type, variable, n) __align(n) variable_type variable
#    define CC_PACKED                              __packed

// Unusual compilers
// There is nothing we can do about some of these. This is not as bad a problem as it seems.
// If the given platform/compiler doesn't support alignment specifications, then it's somewhat
// likely that alignment doesn't matter for that platform. Otherwise they would have defined
// functionality to manipulate alignment.
#else
#    define CC_ALIGN(n)
#    define CC_PREFIX_ALIGN(n)
#    define CC_POSTFIX_ALIGN(n)
#    define CC_ALIGNED(variable_type, variable, n) variable_type variable
#    define CC_PACKED
#    define CC_ALIGN_OF(type) ((size_t)__ALIGNOF__(type))

#endif

// CC_PRAGMA_PACK_VC
//
// Wraps #pragma pack in a way that allows for cleaner code.
//
// Example usage:
//    CC_PRAGMA_PACK_VC(push, 1)
//    struct X{ char c; int i; };
//    CC_PRAGMA_PACK_VC(pop)
//
#if !defined(CC_PRAGMA_PACK_VC)
#    if defined(CC_COMPILER_MSVC)
#        define CC_PRAGMA_PACK_VC(...) __pragma(pack(__VA_ARGS__))
#    elif !defined(CC_COMPILER_NO_VARIADIC_MACROS)
#        define CC_PRAGMA_PACK_VC(...)
#    else
    // No support. However, all compilers of significance to us support variadic macros.
#    endif
#endif

// ------------------------------------------------------------------------
// CC_LIKELY / CC_UNLIKELY
//
// Defined as a macro which gives a hint to the compiler for branch
// prediction. GCC gives you the ability to manually give a hint to
// the compiler about the result of a comparison, though it's often
// best to compile shipping code with profiling feedback under both
// GCC (-fprofile-arcs) and VC++ (/LTCG:PGO, etc.). However, there
// are times when you feel very sure that a boolean expression will
// usually evaluate to either true or false and can help the compiler
// by using an explicity directive...
//
// Example usage:
//    if(CC_LIKELY(a == 0)) // Tell the compiler that a will usually equal 0.
//       { ... }
//
// Example usage:
//    if(CC_UNLIKELY(a == 0)) // Tell the compiler that a will usually not equal 0.
//       { ... }
//
#ifndef CC_LIKELY
#    if (defined(__GNUC__) && (__GNUC__ >= 3)) || defined(__clang__)
#        if defined(__cplusplus)
#            define CC_LIKELY(x)   __builtin_expect(!!(x), true)
#            define CC_UNLIKELY(x) __builtin_expect(!!(x), false)
#        else
#            define CC_LIKELY(x)   __builtin_expect(!!(x), 1)
#            define CC_UNLIKELY(x) __builtin_expect(!!(x), 0)
#        endif
#    else
#        define CC_LIKELY(x)   (x)
#        define CC_UNLIKELY(x) (x)
#    endif
#endif

// ------------------------------------------------------------------------
// CC_HAS_INCLUDE_AVAILABLE
//
// Used to guard against the CC_HAS_INCLUDE() macro on compilers that do not
// support said feature.
//
// Example usage:
//
// #if CC_HAS_INCLUDE_AVAILABLE
//     #if CC_HAS_INCLUDE("myinclude.h")
//         #include "myinclude.h"
//     #endif
// #endif
#if !defined(CC_HAS_INCLUDE_AVAILABLE)
#    if CC_COMPILER_CPP17_ENABLED || CC_COMPILER_CLANG || CC_COMPILER_GNUC
#        define CC_HAS_INCLUDE_AVAILABLE 1
#    else
#        define CC_HAS_INCLUDE_AVAILABLE 0
#    endif
#endif

    // ------------------------------------------------------------------------
    // CC_HAS_INCLUDE
    //
    // May be used in #if and #elif expressions to test for the existence
    // of the header referenced in the operand. If possible it evaluates to a
    // non-zero value and zero otherwise. The operand is the same form as the file
    // in a #include directive.
    //
    // Example usage:
    //
    // #if CC_HAS_INCLUDE("myinclude.h")
    //     #include "myinclude.h"
    // #endif
    //
    // #if CC_HAS_INCLUDE(<myinclude.h>)
    //     #include <myinclude.h>
    // #endif

#if !defined(CC_HAS_INCLUDE)
#    if CC_COMPILER_CPP17_ENABLED
#        define CC_HAS_INCLUDE(x) __has_include(x)
#    elif CC_COMPILER_CLANG
#        define CC_HAS_INCLUDE(x) __has_include(x)
#    elif CC_COMPILER_GNUC
#        define CC_HAS_INCLUDE(x) __has_include(x)
#    endif
#endif

// ------------------------------------------------------------------------
// CC_INIT_PRIORITY_AVAILABLE
//
// This value is either not defined, or defined to 1.
// Defines if the GCC attribute init_priority is supported by the compiler.
//
#if !defined(CC_INIT_PRIORITY_AVAILABLE)
#    if defined(__GNUC__) && !defined(__EDG__)  // EDG typically #defines __GNUC__ but doesn't implement init_priority.
#        define CC_INIT_PRIORITY_AVAILABLE 1
#    elif defined(__clang__)
#        define CC_INIT_PRIORITY_AVAILABLE 1  // Clang implements init_priority
#    endif
#endif

// ------------------------------------------------------------------------
// CC_INIT_PRIORITY
//
// This is simply a wrapper for the GCC init_priority attribute that allows
// multiplatform code to be easier to read. This attribute doesn't apply
// to VC++ because VC++ uses file-level pragmas to control init ordering.
//
// Example usage:
//     SomeClass gSomeClass CC_INIT_PRIORITY(2000);
//
#if !defined(CC_INIT_PRIORITY)
#    if defined(CC_INIT_PRIORITY_AVAILABLE)
#        define CC_INIT_PRIORITY(x) __attribute__((init_priority(x)))
#    else
#        define CC_INIT_PRIORITY(x)
#    endif
#endif

// ------------------------------------------------------------------------
// CC_ASSUME
//
// This acts the same as the VC++ __assume directive and is implemented
// simply as a wrapper around it to allow portable usage of it and to take
// advantage of it if and when it appears in other compilers.
//
// Example usage:
//    void Function(int a) {
//       switch(a) {
//          case 1:
//             DoSomething(1);
//             break;
//          case 2:
//             DoSomething(-1);
//             break;
//          default:
//             CC_ASSUME(0); // This tells the optimizer that the default cannot be reached.
//       }
//    }
//
#ifndef CC_ASSUME
#    if defined(_MSC_VER) && (_MSC_VER >= 1300)  // If VC7.0 and later
#        define CC_ASSUME(x) __assume(x)
#    else
#        define CC_ASSUME(x)
#    endif
#endif

// ------------------------------------------------------------------------
// CC_ANALYSIS_ASSUME
//
// This acts the same as the VC++ __analysis_assume directive and is implemented
// simply as a wrapper around it to allow portable usage of it and to take
// advantage of it if and when it appears in other compilers.
//
// Example usage:
//    char Function(char* p) {
//       CC_ANALYSIS_ASSUME(p != NULL);
//       return *p;
//    }
//
#ifndef CC_ANALYSIS_ASSUME
#    if defined(_MSC_VER) && (_MSC_VER >= 1300)                 // If VC7.0 and later
#        define CC_ANALYSIS_ASSUME(x) __analysis_assume(!!(x))  // !! because that allows for convertible-to-bool in addition to bool.
#    else
#        define CC_ANALYSIS_ASSUME(x)
#    endif
#endif

// ------------------------------------------------------------------------
// CC_DISABLE_VC_WARNING / CC_RESTORE_VC_WARNING
//
// Disable and re-enable warning(s) within code.
// This is simply a wrapper for VC++ #pragma warning(disable: nnnn) for the
// purpose of making code easier to read due to avoiding nested compiler ifdefs
// directly in code.
//
// Example usage:
//     CC_DISABLE_VC_WARNING(4127 3244)
//     <code>
//     CC_RESTORE_VC_WARNING()
//
#ifndef CC_DISABLE_VC_WARNING
#    if defined(_MSC_VER)
#        define CC_DISABLE_VC_WARNING(w) __pragma(warning(push)) __pragma(warning(disable : w))
#    else
#        define CC_DISABLE_VC_WARNING(w)
#    endif
#endif

#ifndef CC_RESTORE_VC_WARNING
#    if defined(_MSC_VER)
#        define CC_RESTORE_VC_WARNING() __pragma(warning(pop))
#    else
#        define CC_RESTORE_VC_WARNING()
#    endif
#endif

// ------------------------------------------------------------------------
// CC_ENABLE_VC_WARNING_AS_ERROR / CC_DISABLE_VC_WARNING_AS_ERROR
//
// Disable and re-enable treating a warning as error within code.
// This is simply a wrapper for VC++ #pragma warning(error: nnnn) for the
// purpose of making code easier to read due to avoiding nested compiler ifdefs
// directly in code.
//
// Example usage:
//     CC_ENABLE_VC_WARNING_AS_ERROR(4996)
//     <code>
//     CC_DISABLE_VC_WARNING_AS_ERROR()
//
#ifndef CC_ENABLE_VC_WARNING_AS_ERROR
#    if defined(_MSC_VER)
#        define CC_ENABLE_VC_WARNING_AS_ERROR(w) __pragma(warning(push)) __pragma(warning(error : w))
#    else
#        define CC_ENABLE_VC_WARNING_AS_ERROR(w)
#    endif
#endif

#ifndef CC_DISABLE_VC_WARNING_AS_ERROR
#    if defined(_MSC_VER)
#        define CC_DISABLE_VC_WARNING_AS_ERROR() __pragma(warning(pop))
#    else
#        define CC_DISABLE_VC_WARNING_AS_ERROR()
#    endif
#endif

// ------------------------------------------------------------------------
// CC_DISABLE_GCC_WARNING / CC_RESTORE_GCC_WARNING
//
// Example usage:
//     // Only one warning can be ignored per statement, due to how GCC works.
//     CC_DISABLE_GCC_WARNING(-Wuninitialized)
//     CC_DISABLE_GCC_WARNING(-Wunused)
//     <code>
//     CC_RESTORE_GCC_WARNING()
//     CC_RESTORE_GCC_WARNING()
//
#ifndef CC_DISABLE_GCC_WARNING
#    if defined(CC_COMPILER_GNUC)
#        define CC_GCCWHELP0(x) #x
#        define CC_GCCWHELP1(x) CC_GCCWHELP0(GCC diagnostic ignored x)
#        define CC_GCCWHELP2(x) CC_GCCWHELP1(#x)
#    endif

#    if defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4006)  // Can't test directly for __GNUC__ because some compilers lie.
#        define CC_DISABLE_GCC_WARNING(w) _Pragma("GCC diagnostic push") _Pragma(CC_GCCWHELP2(w))
#    elif defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4004)
#        define CC_DISABLE_GCC_WARNING(w) _Pragma(CC_GCCWHELP2(w))
#    else
#        define CC_DISABLE_GCC_WARNING(w)
#    endif
#endif

#ifndef CC_RESTORE_GCC_WARNING
#    if defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4006)
#        define CC_RESTORE_GCC_WARNING() _Pragma("GCC diagnostic pop")
#    else
#        define CC_RESTORE_GCC_WARNING()
#    endif
#endif

// ------------------------------------------------------------------------
// CC_DISABLE_ALL_GCC_WARNINGS / CC_RESTORE_ALL_GCC_WARNINGS
//
// This isn't possible except via using _Pragma("GCC system_header"), though
// that has some limitations in how it works. Another means is to manually
// disable individual warnings within a GCC diagnostic push statement.
// GCC doesn't have as many warnings as VC++ and EDG and so this may be feasible.
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// CC_ENABLE_GCC_WARNING_AS_ERROR / CC_DISABLE_GCC_WARNING_AS_ERROR
//
// Example usage:
//     // Only one warning can be treated as an error per statement, due to how GCC works.
//     CC_ENABLE_GCC_WARNING_AS_ERROR(-Wuninitialized)
//     CC_ENABLE_GCC_WARNING_AS_ERROR(-Wunused)
//     <code>
//     CC_DISABLE_GCC_WARNING_AS_ERROR()
//     CC_DISABLE_GCC_WARNING_AS_ERROR()
//
#ifndef CC_ENABLE_GCC_WARNING_AS_ERROR
#    if defined(CC_COMPILER_GNUC)
#        define CC_GCCWERRORHELP0(x) #x
#        define CC_GCCWERRORHELP1(x) CC_GCCWERRORHELP0(GCC diagnostic error x)
#        define CC_GCCWERRORHELP2(x) CC_GCCWERRORHELP1(#x)
#    endif

#    if defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4006)  // Can't test directly for __GNUC__ because some compilers lie.
#        define CC_ENABLE_GCC_WARNING_AS_ERROR(w) _Pragma("GCC diagnostic push") _Pragma(CC_GCCWERRORHELP2(w))
#    elif defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4004)
#        define CC_DISABLE_GCC_WARNING(w) _Pragma(CC_GCCWERRORHELP2(w))
#    else
#        define CC_DISABLE_GCC_WARNING(w)
#    endif
#endif

#ifndef CC_DISABLE_GCC_WARNING_AS_ERROR
#    if defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4006)
#        define CC_DISABLE_GCC_WARNING_AS_ERROR() _Pragma("GCC diagnostic pop")
#    else
#        define CC_DISABLE_GCC_WARNING_AS_ERROR()
#    endif
#endif

// ------------------------------------------------------------------------
// CC_DISABLE_CLANG_WARNING / CC_RESTORE_CLANG_WARNING
//
// Example usage:
//     // Only one warning can be ignored per statement, due to how clang works.
//     CC_DISABLE_CLANG_WARNING(-Wuninitialized)
//     CC_DISABLE_CLANG_WARNING(-Wunused)
//     <code>
//     CC_RESTORE_CLANG_WARNING()
//     CC_RESTORE_CLANG_WARNING()
//
#ifndef CC_DISABLE_CLANG_WARNING
#    if defined(CC_COMPILER_CLANG) || defined(CC_COMPILER_CLANG_CL)
#        define CC_CLANGWHELP0(x) #x
#        define CC_CLANGWHELP1(x) CC_CLANGWHELP0(clang diagnostic ignored x)
#        define CC_CLANGWHELP2(x) CC_CLANGWHELP1(#x)

#        define CC_DISABLE_CLANG_WARNING(w) _Pragma("clang diagnostic push") _Pragma(CC_CLANGWHELP2(-Wunknown - warning - option)) _Pragma(CC_CLANGWHELP2(w))
#    else
#        define CC_DISABLE_CLANG_WARNING(w)
#    endif
#endif

#ifndef CC_RESTORE_CLANG_WARNING
#    if defined(CC_COMPILER_CLANG) || defined(CC_COMPILER_CLANG_CL)
#        define CC_RESTORE_CLANG_WARNING() _Pragma("clang diagnostic pop")
#    else
#        define CC_RESTORE_CLANG_WARNING()
#    endif
#endif

// ------------------------------------------------------------------------
// CC_DISABLE_ALL_CLANG_WARNINGS / CC_RESTORE_ALL_CLANG_WARNINGS
//
// The situation for clang is the same as for GCC. See above.
// ------------------------------------------------------------------------

// ------------------------------------------------------------------------
// CC_ENABLE_CLANG_WARNING_AS_ERROR / CC_DISABLE_CLANG_WARNING_AS_ERROR
//
// Example usage:
//     // Only one warning can be treated as an error per statement, due to how clang works.
//     CC_ENABLE_CLANG_WARNING_AS_ERROR(-Wuninitialized)
//     CC_ENABLE_CLANG_WARNING_AS_ERROR(-Wunused)
//     <code>
//     CC_DISABLE_CLANG_WARNING_AS_ERROR()
//     CC_DISABLE_CLANG_WARNING_AS_ERROR()
//
#ifndef CC_ENABLE_CLANG_WARNING_AS_ERROR
#    if defined(CC_COMPILER_CLANG) || defined(CC_COMPILER_CLANG_CL)
#        define CC_CLANGWERRORHELP0(x) #x
#        define CC_CLANGWERRORHELP1(x) CC_CLANGWERRORHELP0(clang diagnostic error x)
#        define CC_CLANGWERRORHELP2(x) CC_CLANGWERRORHELP1(#x)

#        define CC_ENABLE_CLANG_WARNING_AS_ERROR(w) _Pragma("clang diagnostic push") _Pragma(CC_CLANGWERRORHELP2(w))
#    else
#        define CC_DISABLE_CLANG_WARNING(w)
#    endif
#endif

#ifndef CC_DISABLE_CLANG_WARNING_AS_ERROR
#    if defined(CC_COMPILER_CLANG) || defined(CC_COMPILER_CLANG_CL)
#        define CC_DISABLE_CLANG_WARNING_AS_ERROR() _Pragma("clang diagnostic pop")
#    else
#        define CC_DISABLE_CLANG_WARNING_AS_ERROR()
#    endif
#endif

// ------------------------------------------------------------------------
// CC_DISABLE_EDG_WARNING / CC_RESTORE_EDG_WARNING
//
// Example usage:
//     // Currently we are limited to one warning per line.
//     CC_DISABLE_EDG_WARNING(193)
//     CC_DISABLE_EDG_WARNING(236)
//     <code>
//     CC_RESTORE_EDG_WARNING()
//     CC_RESTORE_EDG_WARNING()
//
#ifndef CC_DISABLE_EDG_WARNING
    // EDG-based compilers are inconsistent in how the implement warning pragmas.
#    if defined(CC_COMPILER_EDG) && !defined(CC_COMPILER_INTEL) && !defined(CC_COMPILER_RVCT)
#        define CC_EDGWHELP0(x) #x
#        define CC_EDGWHELP1(x) CC_EDGWHELP0(diag_suppress x)

#        define CC_DISABLE_EDG_WARNING(w) _Pragma("control %push diag") _Pragma(CC_EDGWHELP1(w))
#    else
#        define CC_DISABLE_EDG_WARNING(w)
#    endif
#endif

#ifndef CC_RESTORE_EDG_WARNING
#    if defined(CC_COMPILER_EDG) && !defined(CC_COMPILER_INTEL) && !defined(CC_COMPILER_RVCT)
#        define CC_RESTORE_EDG_WARNING() _Pragma("control %pop diag")
#    else
#        define CC_RESTORE_EDG_WARNING()
#    endif
#endif

// ------------------------------------------------------------------------
// CC_DISABLE_ALL_EDG_WARNINGS / CC_RESTORE_ALL_EDG_WARNINGS
//
// #ifndef CC_DISABLE_ALL_EDG_WARNINGS
//    #if defined(CC_COMPILER_EDG) && !defined(CC_COMPILER_SN)
//        #define CC_DISABLE_ALL_EDG_WARNINGS(w)  \_
//            _Pragma("_________")
//    #else
//        #define CC_DISABLE_ALL_EDG_WARNINGS(w)
//    #endif
// #endif
//
// #ifndef CC_RESTORE_ALL_EDG_WARNINGS
//    #if defined(CC_COMPILER_EDG) && !defined(CC_COMPILER_SN)
//        #define CC_RESTORE_ALL_EDG_WARNINGS()   \_
//            _Pragma("_________")
//    #else
//        #define CC_RESTORE_ALL_EDG_WARNINGS()
//    #endif
// #endif

// ------------------------------------------------------------------------
// CC_DISABLE_CW_WARNING / CC_RESTORE_CW_WARNING
//
// Note that this macro can only control warnings via numbers and not by
// names. The reason for this is that the compiler's syntax for such
// warnings is not the same as for numbers.
//
// Example usage:
//     // Currently we are limited to one warning per line and must also specify the warning in the restore macro.
//     CC_DISABLE_CW_WARNING(10317)
//     CC_DISABLE_CW_WARNING(10324)
//     <code>
//     CC_RESTORE_CW_WARNING(10317)
//     CC_RESTORE_CW_WARNING(10324)
//
#ifndef CC_DISABLE_CW_WARNING
#    define CC_DISABLE_CW_WARNING(w)
#endif

#ifndef CC_RESTORE_CW_WARNING

#    define CC_RESTORE_CW_WARNING(w)

#endif

// ------------------------------------------------------------------------
// CC_DISABLE_ALL_CW_WARNINGS / CC_RESTORE_ALL_CW_WARNINGS
//
#ifndef CC_DISABLE_ALL_CW_WARNINGS
#    define CC_DISABLE_ALL_CW_WARNINGS()

#endif

#ifndef CC_RESTORE_ALL_CW_WARNINGS
#    define CC_RESTORE_ALL_CW_WARNINGS()
#endif

// ------------------------------------------------------------------------
// CC_WEAK
// CC_WEAK_SUPPORTED -- defined as 0 or 1.
//
// GCC
// The weak attribute causes the declaration to be emitted as a weak
// symbol rather than a global. This is primarily useful in defining
// library functions which can be overridden in user code, though it
// can also be used with non-function declarations.
//
// VC++
// At link time, if multiple definitions of a COMDAT are seen, the linker
// picks one and discards the rest. If the linker option /OPT:REF
// is selected, then COMDAT elimination will occur to remove all the
// unreferenced data items in the linker output.
//
// Example usage:
//    CC_WEAK void Function();
//
#ifndef CC_WEAK
#    if defined(_MSC_VER) && (_MSC_VER >= 1300)  // If VC7.0 and later
#        define CC_WEAK           __declspec(selectany)
#        define CC_WEAK_SUPPORTED 1
#    elif defined(_MSC_VER) || (defined(__GNUC__) && defined(__CYGWIN__))
#        define CC_WEAK
#        define CC_WEAK_SUPPORTED 0
#    elif defined(CC_COMPILER_ARM)  // Arm brand compiler for ARM CPU
#        define CC_WEAK           __weak
#        define CC_WEAK_SUPPORTED 1
#    else  // GCC and IBM compilers, others.
#        define CC_WEAK           __attribute__((weak))
#        define CC_WEAK_SUPPORTED 1
#    endif
#endif

// ------------------------------------------------------------------------
// CC_UNUSED
//
// Makes compiler warnings about unused variables go away.
//
// Example usage:
//    void Function(int x)
//    {
//        int y;
//        CC_UNUSED(x);
//        CC_UNUSED(y);
//    }
//
#ifndef CC_UNUSED
    // The EDG solution below is pretty weak and needs to be augmented or replaced.
// It can't handle the C language, is limited to places where template declarations
// can be used, and requires the type x to be usable as a functions reference argument.
#    if defined(__cplusplus) && defined(__EDG__)
    template <typename T>
    inline void CCoreUnused(T const volatile& x)
    {
        (void)x;
    }
#        define CC_UNUSED(x) CCoreUnused(x)
#    else
#        define CC_UNUSED(x) (void)x
#    endif
#endif

// ------------------------------------------------------------------------
// CC__FUNCTION__
//
// Provides a consistent way to get the current function name as a macro
// like the __FILE__ and __LINE__ macros work. The C99 standard specifies
// that __func__ be provided by the compiler, but most compilers don't yet
// follow that convention. However, many compilers have an alternative.
//
// We also define CC_CURRENT_FUNCTION_SUPPORTED for when it is not possible
// to have CC__FUNCTION__ work as expected.
//
// Defined inside a function because otherwise the macro might not be
// defined and code below might not compile. This happens with some
// compilers.
//
#ifndef CC__FUNCTION__
#    if defined __GNUC__ || (defined __ICC && __ICC >= 600)
#        define CC__FUNCTION__ __PRETTY_FUNCTION__
#    elif defined(__FUNCSIG__)
#        define CC__FUNCTION__ __FUNCSIG__
#    elif (defined __INTEL_COMPILER && __INTEL_COMPILER >= 600) || (defined __IBMCPP__ && __IBMCPP__ >= 500) || (defined CS_UNDEFINED_STRING && CS_UNDEFINED_STRING >= 0x4200)
#        define CC__FUNCTION__ __FUNCTION__
#    elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901
#        define CC__FUNCTION__ __func__
#    else
#        define CC__FUNCTION__ "(unknown function)"
#    endif
#endif

// ------------------------------------------------------------------------
// wchar_t
// Here we define:
//    CC_WCHAR_T_NON_NATIVE
//    CC_WCHAR_SIZE = <sizeof(wchar_t)>
//
#ifndef CC_WCHAR_T_NON_NATIVE
// Compilers that always implement wchar_t as native include:
//     COMEAU, new SN, and other EDG-based compilers.
//     GCC
//     Borland
//     SunPro
//     IBM Visual Age
#    if defined(CC_COMPILER_INTEL)
#        if (CC_COMPILER_VERSION < 700)
#            define CC_WCHAR_T_NON_NATIVE 1
#        else
#            if (!defined(_WCHAR_T_DEFINED) && !defined(_WCHAR_T))
#                define CC_WCHAR_T_NON_NATIVE 1
#            endif
#        endif
#    elif defined(CC_COMPILER_MSVC) || (defined(CC_COMPILER_CLANG) && defined(CC_PLATFORM_WINDOWS))
#        ifndef _NATIVE_WCHAR_T_DEFINED
#            define CC_WCHAR_T_NON_NATIVE 1
#        endif
#    elif defined(__EDG_VERSION__) && (!defined(_WCHAR_T) && (__EDG_VERSION__ < 400))
// EDG prior to v4 uses _WCHAR_T to indicate if wchar_t is native. v4+ may define something else, but we're not currently aware of it.
#        define CC_WCHAR_T_NON_NATIVE 1
#    endif
#endif

#ifndef CC_WCHAR_SIZE           // If the user hasn't specified that it is a given size...
#    if defined(__WCHAR_MAX__)  // GCC defines this for most platforms.
#        if (__WCHAR_MAX__ == 2147483647) || (__WCHAR_MAX__ == 4294967295)
#            define CC_WCHAR_SIZE 4
#        elif (__WCHAR_MAX__ == 32767) || (__WCHAR_MAX__ == 65535)
#            define CC_WCHAR_SIZE 2
#        elif (__WCHAR_MAX__ == 127) || (__WCHAR_MAX__ == 255)
#            define CC_WCHAR_SIZE 1
#        else
#            define CC_WCHAR_SIZE 4
#        endif
#    elif defined(WCHAR_MAX)  // The SN and Arm compilers define this.
#        if (WCHAR_MAX == 2147483647) || (WCHAR_MAX == 4294967295)
#            define CC_WCHAR_SIZE 4
#        elif (WCHAR_MAX == 32767) || (WCHAR_MAX == 65535)
#            define CC_WCHAR_SIZE 2
#        elif (WCHAR_MAX == 127) || (WCHAR_MAX == 255)
#            define CC_WCHAR_SIZE 1
#        else
#            define CC_WCHAR_SIZE 4
#        endif
#    elif defined(__WCHAR_BIT)  // Green Hills (and other versions of EDG?) uses this.
#        if (__WCHAR_BIT == 16)
#            define CC_WCHAR_SIZE 2
#        elif (__WCHAR_BIT == 32)
#            define CC_WCHAR_SIZE 4
#        elif (__WCHAR_BIT == 8)
#            define CC_WCHAR_SIZE 1
#        else
#            define CC_WCHAR_SIZE 4
#        endif
#    elif defined(_WCMAX)  // The SN and Arm compilers define this.
#        if (_WCMAX == 2147483647) || (_WCMAX == 4294967295)
#            define CC_WCHAR_SIZE 4
#        elif (_WCMAX == 32767) || (_WCMAX == 65535)
#            define CC_WCHAR_SIZE 2
#        elif (_WCMAX == 127) || (_WCMAX == 255)
#            define CC_WCHAR_SIZE 1
#        else
#            define CC_WCHAR_SIZE 4
#        endif
#    elif defined(CC_PLATFORM_UNIX)
    // It is standard on Unix to have wchar_t be int32_t or uint32_t.
// All versions of GNUC default to a 32 bit wchar_t, but EA has used
// the -fshort-wchar GCC command line option to force it to 16 bit.
// If you know that the compiler is set to use a wchar_t of other than
// the default, you need to manually define CC_WCHAR_SIZE for the build.
#        define CC_WCHAR_SIZE 4
#    else
    // It is standard on Windows to have wchar_t be uint16_t.  GCC
// defines wchar_t as int by default.  Electronic Arts has
// standardized on wchar_t being an unsigned 16 bit value on all
// console platforms. Given that there is currently no known way to
// tell at preprocessor time what the size of wchar_t is, we declare
// it to be 2, as this is the Electronic Arts standard. If you have
// CC_WCHAR_SIZE != sizeof(wchar_t), then your code might not be
// broken, but it also won't work with wchar libraries and data from
// other parts of EA. Under GCC, you can force wchar_t to two bytes
// with the -fshort-wchar compiler argument.
#        define CC_WCHAR_SIZE 2
#    endif
#endif

// ------------------------------------------------------------------------
// CC_RESTRICT
//
// The C99 standard defines a new keyword, restrict, which allows for the
// improvement of code generation regarding memory usage. Compilers can
// generate significantly faster code when you are able to use restrict.
//
// Example usage:
//    void DoSomething(char* CC_RESTRICT p1, char* CC_RESTRICT p2);
//
#ifndef CC_RESTRICT
#    if defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION >= 1400)  // If VC8 (VS2005) or later...
#        define CC_RESTRICT __restrict
#    elif defined(CC_COMPILER_CLANG)
#        define CC_RESTRICT __restrict
#    elif defined(CC_COMPILER_GNUC)     // Includes GCC and other compilers emulating GCC.
#        define CC_RESTRICT __restrict  // GCC defines 'restrict' (as opposed to __restrict) in C99 mode only.
#    elif defined(CC_COMPILER_ARM)
#        define CC_RESTRICT __restrict
#    elif defined(CC_COMPILER_IS_C99)
#        define CC_RESTRICT restrict
#    else
    // If the compiler didn't support restricted pointers, defining CC_RESTRICT
// away would result in compiling and running fine but you just wouldn't
// the same level of optimization. On the other hand, all the major compilers
// support restricted pointers.
#        define CC_RESTRICT
#    endif
#endif

    // ------------------------------------------------------------------------
    // CC_DEPRECATED            // Used as a prefix.
    // CC_DEPRECATED_MESSAGE    // Used as a prefix and provides a deprecation message.
    //
    // Example usage:
    //    CC_DEPRECATED void Function();
    //    CC_DEPRECATED_MESSAGE("Use 1.0v API instead") void Function();
    //

#ifndef CC_DEPRECATED
#    if defined(CC_COMPILER_CPP14_ENABLED)
#        define CC_DEPRECATED [[deprecated]]
#    elif defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION > 1300)  // If VC7 (VS2003) or later...
#        define CC_DEPRECATED __declspec(deprecated)
#    elif defined(CC_COMPILER_MSVC)
#        define CC_DEPRECATED
#    else
#        define CC_DEPRECATED __attribute__((deprecated))
#    endif
#endif

#ifndef CC_DEPRECATED_MESSAGE
#    if defined(CC_COMPILER_CPP14_ENABLED)
#        define CC_DEPRECATED_MESSAGE(msg) [[deprecated(#msg)]]
#    else
    // Compiler does not support depreaction messages, explicitly drop the msg but still mark the function as deprecated
#        define CC_DEPRECATED_MESSAGE(msg) CC_DEPRECATED
#    endif
#endif

// ------------------------------------------------------------------------
// CC_INLINE
//
// Example usage:
//     CC_INLINE void Foo() { ..... }
//
#ifndef CC_INLINE
#    if defined(CC_COMPILER_MSVC)
#        define CC_INLINE inline
#        define D_INLINE  inline
#    elif defined(CC_COMPILER_GNUC) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 301) || defined(CC_COMPILER_CLANG)
#        if defined(__cplusplus)
#            define CC_INLINE inline
#            define D_INLINE  inline
#        else
#            define CC_INLINE __inline__
#            define D_INLINE  __inline__
#        endif
#    else
#        if defined(__cplusplus)
#            define CC_INLINE inline
#            define D_INLINE  inline
#        else
#            define CC_INLINE __inline
#            define D_INLINE  __inline
#        endif
#    endif
#endif

// ------------------------------------------------------------------------
// CC_FORCE_INLINE              // Used as a prefix.
//
// Example usage:
//     CC_FORCE_INLINE void Foo();                                // Implementation elsewhere.
//
// Note that when the prefix version of this function is used, it replaces
// the regular C++ 'inline' statement. Thus you should not use both the
// C++ inline statement and this macro with the same function declaration.
//
// To force inline usage under GCC 3.1+, you use this:
//    inline void Foo() __attribute__((always_inline));
//       or
//    inline __attribute__((always_inline)) void Foo();
//
// The CodeWarrior compiler doesn't have the concept of forcing inlining per function.
//
#ifndef CC_FORCE_INLINE
#    if defined(CC_COMPILER_MSVC)
#        define CC_FORCE_INLINE __forceinline
#    elif defined(CC_COMPILER_GNUC) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 301) || defined(CC_COMPILER_CLANG)
#        if defined(__cplusplus)
#            define CC_FORCE_INLINE inline __attribute__((always_inline))
#        else
#            define CC_FORCE_INLINE __inline__ __attribute__((always_inline))
#        endif
#    else
#        if defined(__cplusplus)
#            define CC_FORCE_INLINE inline
#        else
#            define CC_FORCE_INLINE __inline
#        endif
#    endif
#endif

// ------------------------------------------------------------------------
// CC_FORCE_INLINE_LAMBDA
//
// CC_FORCE_INLINE_LAMBDA is used to force inline a call to a lambda when possible.
// Force inlining a lambda can be useful to reduce overhead in situations where a lambda may
// may only be called once, or inlining allows the compiler to apply other optimizations that wouldn't
// otherwise be possible.
//
// The ability to force inline a lambda is currently only available on a subset of compilers.
//
// Example usage:
//
//		auto lambdaFunction = []() CC_FORCE_INLINE_LAMBDA
//		{
//		};
//
#ifndef CC_FORCE_INLINE_LAMBDA
#    if defined(CC_COMPILER_GNUC) || defined(CC_COMPILER_CLANG)
#        define CC_FORCE_INLINE_LAMBDA __attribute__((always_inline))
#    else
#        define CC_FORCE_INLINE_LAMBDA
#    endif
#endif

// ------------------------------------------------------------------------
// CC_NO_INLINE             // Used as a prefix.
//
// Example usage:
//     CC_NO_INLINE        void Foo();                       // Implementation elsewhere.
//
// That this declaration is incompatbile with C++ 'inline' and any
// variant of CC_FORCE_INLINE.
//
// To disable inline usage under VC++ priof to VS2005, you need to use this:
//    #pragma inline_depth(0) // Disable inlining.
//    void Foo() { ... }
//    #pragma inline_depth()  // Restore to default.
//
// Since there is no easy way to disable inlining on a function-by-function
// basis in VC++ prior to VS2005, the best strategy is to write platform-specific
// #ifdefs in the code or to disable inlining for a given module and enable
// functions individually with CC_FORCE_INLINE.
//
#ifndef CC_NO_INLINE
#    if defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION >= 1400)  // If VC8 (VS2005) or later...
#        define CC_NO_INLINE __declspec(noinline)
#    elif defined(CC_COMPILER_MSVC)
#        define CC_NO_INLINE
#    else
#        define CC_NO_INLINE __attribute__((noinline))
#    endif
#endif

// ------------------------------------------------------------------------
// CC_NO_VTABLE
//
// Example usage:
//     class CC_NO_VTABLE X {
//        virtual void InterfaceFunction();
//     };
//
//     CC_CLASS_NO_VTABLE(X) {
//        virtual void InterfaceFunction();
//     };
//
#ifdef CC_COMPILER_MSVC
#    define CC_NO_VTABLE           __declspec(novtable)
#    define CC_CLASS_NO_VTABLE(x)  class __declspec(novtable) x
#    define CC_STRUCT_NO_VTABLE(x) struct __declspec(novtable) x
#else
#    define CC_NO_VTABLE
#    define CC_CLASS_NO_VTABLE(x)  class x
#    define CC_STRUCT_NO_VTABLE(x) struct x
#endif

// ------------------------------------------------------------------------
// CC_SSE
// Visual C Processor Packs define _MSC_FULL_VER and are needed for SSE
// Intel C also has SSE support.
// CC_SSE is used to select FPU or SSE versions in hw_select.inl
//
// CC_SSE defines the level of SSE support:
//  0 indicates no SSE support
//  1 indicates SSE1 is supported
//  2 indicates SSE2 is supported
//  3 indicates SSE3 (or greater) is supported
//
// Note: SSE support beyond SSE3 can't be properly represented as a single
// version number.  Instead users should use specific SSE defines (e.g.
// CC_SSE4_2) to detect what specific support is available.  CC_SSE being
// equal to 3 really only indicates that SSE3 or greater is supported.
#ifndef CC_SSE
#    if defined(CC_COMPILER_GNUC) || defined(CC_COMPILER_CLANG)
#        if defined(__SSE3__)
#            define CC_SSE 3
#        elif defined(__SSE2__)
#            define CC_SSE 2
#        elif defined(__SSE__) && __SSE__
#            define CC_SSE 1
#        else
#            define CC_SSE 0
#        endif
#    elif (defined(CC_SSE3) && CC_SSE3) || defined CC_PLATFORM_XBOXONE || defined CS_UNDEFINED_STRING
#        define CC_SSE 3
#    elif defined(CC_SSE2) && CC_SSE2
#        define CC_SSE 2
#    elif defined(CC_PROCESSOR_X86) && defined(_MSC_FULL_VER) && !defined(__NOSSE__) && defined(_M_IX86_FP)
#        define CC_SSE _M_IX86_FP
#    elif defined(CC_PROCESSOR_X86) && defined(CC_COMPILER_INTEL) && !defined(__NOSSE__)
#        define CC_SSE 1
#    elif defined(CC_PROCESSOR_X86_64)
    // All x64 processors support SSE2 or higher
#        define CC_SSE 2
#    else
#        define CC_SSE 0
#    endif
#endif

// ------------------------------------------------------------------------
// We define separate defines for SSE support beyond SSE1.  These defines
// are particularly useful for detecting SSE4.x features since there isn't
// a single concept of SSE4.
//
// The following SSE defines are always defined.  0 indicates the
// feature/level of SSE is not supported, and 1 indicates support is
// available.
#ifndef CC_SSE2
#    if CC_SSE >= 2
#        define CC_SSE2 1
#    else
#        define CC_SSE2 0
#    endif
#endif
#ifndef CC_SSE3
#    if CC_SSE >= 3
#        define CC_SSE3 1
#    else
#        define CC_SSE3 0
#    endif
#endif
#ifndef CC_SSSE3
#    if defined __SSSE3__ || defined CC_PLATFORM_XBOXONE || defined CS_UNDEFINED_STRING
#        define CC_SSSE3 1
#    else
#        define CC_SSSE3 0
#    endif
#endif
#ifndef CC_SSE4_1
#    if defined __SSE4_1__ || defined CC_PLATFORM_XBOXONE || defined CS_UNDEFINED_STRING
#        define CC_SSE4_1 1
#    else
#        define CC_SSE4_1 0
#    endif
#endif
#ifndef CC_SSE4_2
#    if defined __SSE4_2__ || defined CC_PLATFORM_XBOXONE || defined CS_UNDEFINED_STRING
#        define CC_SSE4_2 1
#    else
#        define CC_SSE4_2 0
#    endif
#endif
#ifndef CC_SSE4A
#    if defined __SSE4A__ || defined CC_PLATFORM_XBOXONE || defined CS_UNDEFINED_STRING
#        define CC_SSE4A 1
#    else
#        define CC_SSE4A 0
#    endif
#endif

// ------------------------------------------------------------------------
// CC_AVX
// CC_AVX may be used to determine if Advanced Vector Extensions are available for the target architecture
//
// CC_AVX defines the level of AVX support:
//  0 indicates no AVX support
//  1 indicates AVX1 is supported
//  2 indicates AVX2 is supported
#ifndef CC_AVX
#    if defined __AVX2__
#        define CC_AVX 2
#    elif defined __AVX__ || defined CC_PLATFORM_XBOXONE || defined CS_UNDEFINED_STRING
#        define CC_AVX 1
#    else
#        define CC_AVX 0
#    endif
#endif
#ifndef CC_AVX2
#    if CC_AVX >= 2
#        define CC_AVX2 1
#    else
#        define CC_AVX2 0
#    endif
#endif

// CC_FP16C may be used to determine the existence of float <-> half conversion operations on an x86 CPU.
// (For example to determine if _mm_cvtph_ps or _mm_cvtps_ph could be used.)
#ifndef CC_FP16C
#    if defined __F16C__ || defined CC_PLATFORM_XBOXONE || defined CS_UNDEFINED_STRING
#        define CC_FP16C 1
#    else
#        define CC_FP16C 0
#    endif
#endif

// CC_FP128 may be used to determine if __float128 is a supported type for use. This type is enabled by a GCC extension (_GLIBCXX_USE_FLOAT128)
// but has support by some implementations of clang (__FLOAT128__)
// PS4 does not support __float128 as of SDK 5.500 https://ps4.siedev.net/resources/documents/SDK/5.500/CPU_Compiler_ABI-Overview/0003.html
#ifndef CC_FP128
#    if (defined __FLOAT128__ || defined _GLIBCXX_USE_FLOAT128) && !defined(CC_PLATFORM_SONY)
#        define CC_FP128 1
#    else
#        define CC_FP128 0
#    endif
#endif

// ------------------------------------------------------------------------
// CC_ABM
// CC_ABM may be used to determine if Advanced Bit Manipulation sets are available for the target architecture (POPCNT, LZCNT)
//
#ifndef CC_ABM
#    if defined(__ABM__) || defined(CC_PLATFORM_XBOXONE) || defined(CC_PLATFORM_SONY) || defined(CS_UNDEFINED_STRING)
#        define CC_ABM 1
#    else
#        define CC_ABM 0
#    endif
#endif

// ------------------------------------------------------------------------
// CC_NEON
// CC_NEON may be used to determine if NEON is supported.
#ifndef CC_NEON
#    if defined(__ARM_NEON__) || defined(__ARM_NEON)
#        define CC_NEON 1
#    else
#        define CC_NEON 0
#    endif
#endif

// ------------------------------------------------------------------------
// CC_BMI
// CC_BMI may be used to determine if Bit Manipulation Instruction sets are available for the target architecture
//
// CC_BMI defines the level of BMI support:
//  0 indicates no BMI support
//  1 indicates BMI1 is supported
//  2 indicates BMI2 is supported
#ifndef CC_BMI
#    if defined(__BMI2__)
#        define CC_BMI 2
#    elif defined(__BMI__) || defined(CC_PLATFORM_XBOXONE) || defined(CS_UNDEFINED_STRING)
#        define CC_BMI 1
#    else
#        define CC_BMI 0
#    endif
#endif
#ifndef CC_BMI2
#    if CC_BMI >= 2
#        define CC_BMI2 1
#    else
#        define CC_BMI2 0
#    endif
#endif

// ------------------------------------------------------------------------
// CC_FMA3
// CC_FMA3 may be used to determine if Fused Multiply Add operations are available for the target architecture
// __FMA__ is defined only by GCC, Clang, and ICC; MSVC only defines __AVX__ and __AVX2__
// FMA3 was introduced alongside AVX2 on Intel Haswell
// All AMD processors support FMA3 if AVX2 is also supported
//
// CC_FMA3 defines the level of FMA3 support:
//  0 indicates no FMA3 support
//  1 indicates FMA3 is supported
#ifndef CC_FMA3
#    if defined(__FMA__) || CC_AVX2 >= 1
#        define CC_FMA3 1
#    else
#        define CC_FMA3 0
#    endif
#endif

// ------------------------------------------------------------------------
// CC_TBM
// CC_TBM may be used to determine if Trailing Bit Manipulation instructions are available for the target architecture
#ifndef CC_TBM
#    if defined(__TBM__)
#        define CC_TBM 1
#    else
#        define CC_TBM 0
#    endif
#endif

// ------------------------------------------------------------------------
// CC_IMPORT
// import declaration specification
// specifies that the declared symbol is imported from another dynamic library.
#ifndef CC_IMPORT
#    if defined(CC_COMPILER_MSVC)
#        define CC_IMPORT __declspec(dllimport)
#    else
#        define CC_IMPORT
#    endif
#endif

// ------------------------------------------------------------------------
// CC_EXPORT
// export declaration specification
// specifies that the declared symbol is exported from the current dynamic library.
// this is not the same as the C++ export keyword. The C++ export keyword has been
// removed from the language as of C++11.
#ifndef CC_EXPORT
#    if defined(CC_COMPILER_MSVC)
#        define CC_EXPORT __declspec(dllexport)
#    else
#        define CC_EXPORT
#    endif
#endif

// ------------------------------------------------------------------------
// CC_PRAGMA_ONCE_SUPPORTED
//
// This is a wrapper for the #pragma once preprocessor directive.
// It allows for some compilers (in particular VC++) to implement signifcantly
// faster include file preprocessing. #pragma once can be used to replace
// header include guards or to augment them. However, #pragma once isn't
// necessarily supported by all compilers and isn't guaranteed to be so in
// the future, so using #pragma once to replace traditional include guards
// is not strictly portable. Note that a direct #define for #pragma once is
// impossible with VC++, due to limitations, but can be done with other
// compilers/preprocessors via _Pragma("once").
//
// Example usage (which includes traditional header guards for portability):
//    #ifndef SOMEPACKAGE_SOMEHEADER_H
//    #define SOMEPACKAGE_SOMEHEADER_H
//
//    #if defined(CC_PRAGMA_ONCE_SUPPORTED)
//        #pragma once
//    #endif
//
//    <user code>
//
//    #endif
//
#if defined(_MSC_VER) || defined(__GNUC__) || defined(__EDG__) || defined(__APPLE__)
#    define CC_PRAGMA_ONCE_SUPPORTED 1
#    define USE_PRAGMA_ONCE
#endif

// ------------------------------------------------------------------------
// CC_INCLUDE_ONCE
//
// Example usage (which includes traditional header guards for portability):
//    #ifndef SOMEPACKAGE_SOMEHEADER_H
//    #define SOMEPACKAGE_SOMEHEADER_H
//
//    CC_INCLUDE_ONCE
//
//    <user code>
//
//    #endif
//
#if defined(CC_PRAGMA_ONCE_SUPPORTED)
#    if defined(_MSC_VER)
#        define CC_INCLUDE_ONCE __pragma(once)
#    else
#        define CC_INCLUDE_ONCE  // _Pragma("once")   It turns out that _Pragma("once") isn't supported by many compilers.
#    endif
#endif

// ------------------------------------------------------------------------
// CC_OVERRIDE
//
// C++11 override
// See http://msdn.microsoft.com/en-us/library/jj678987.aspx for more information.
// You can use CC_FINAL_OVERRIDE to combine usage of CC_OVERRIDE and CC_FINAL in a single statement.
//
// Example usage:
//        struct B     { virtual void f(int); };
//        struct D : B { void f(int) CC_OVERRIDE; };
//
#ifndef CC_OVERRIDE
#    if defined(CC_COMPILER_NO_OVERRIDE)
#        define CC_OVERRIDE
#    else
#        define CC_OVERRIDE override
#    endif
#endif

// ------------------------------------------------------------------------
// CC_FINAL
//
// Portably wraps the C++11 final specifier.
// See http://msdn.microsoft.com/en-us/library/jj678985.aspx for more information.
// You can use CC_FINAL_OVERRIDE to combine usage of CC_OVERRIDE and CC_FINAL in a single statement.
//
// Example usage:
//     struct B { virtual void f() CC_FINAL; };
//
#ifndef CC_FINAL
#    if defined(CC_COMPILER_NO_INHERITANCE_FINAL)
#        define CC_FINAL
#    elif (defined(_MSC_VER) && (CC_COMPILER_VERSION < 1700))  // Pre-VS2012
#        define CC_FINAL sealed
#    else
#        define CC_FINAL final
#    endif
#endif

// ------------------------------------------------------------------------
// CC_FINAL_OVERRIDE
//
// Portably wraps the C++11 override final specifiers combined.
//
// Example usage:
//     struct A            { virtual void f(); };
//     struct B : public A { virtual void f() CC_FINAL_OVERRIDE; };
//
#ifndef CC_FINAL_OVERRIDE
#    define CC_FINAL_OVERRIDE CC_OVERRIDE CC_FINAL
#endif

// ------------------------------------------------------------------------
// CC_CONSTEXPR
// CC_CONSTEXPR_OR_CONST
//
// Portable wrapper for C++11's 'constexpr' support.
//
// See http://www.cprogramming.com/c++11/c++11-compile-time-processing-with-constexpr.html for more information.
// Example usage:
//     CC_CONSTEXPR int GetValue() { return 37; }
//     CC_CONSTEXPR_OR_CONST double gValue = std::sin(kTwoPi);
//
#if !defined(CC_CONSTEXPR)
#    if defined(CC_COMPILER_NO_CONSTEXPR)
#        define CC_CONSTEXPR
#    else
#        define CC_CONSTEXPR constexpr
#    endif
#endif

#if !defined(CC_CONSTEXPR_OR_CONST)
#    if defined(CC_COMPILER_NO_CONSTEXPR)
#        define CC_CONSTEXPR_OR_CONST const
#    else
#        define CC_CONSTEXPR_OR_CONST constexpr
#    endif
#endif

// ------------------------------------------------------------------------
// CC_CONSTEXPR_IF
//
// Portable wrapper for C++17's 'constexpr if' support.
//
// https://en.cppreference.com/w/cpp/language/if
//
// Example usage:
//
// CC_CONSTEXPR_IF(eastl::is_copy_constructible_v<T>)
// 	{ ... }
//
#if !defined(CC_CONSTEXPR_IF)
#    if defined(CC_COMPILER_NO_CONSTEXPR_IF)
#        define CC_CONSTEXPR_IF(predicate) if ((predicate))
#    else
#        define CC_CONSTEXPR_IF(predicate) if constexpr ((predicate))
#    endif
#endif

// ------------------------------------------------------------------------
// CC_EXTERN_TEMPLATE
//
// Portable wrapper for C++11's 'extern template' support.
//
// Example usage:
//     CC_EXTERN_TEMPLATE(class basic_string<char>);
//
#if !defined(CC_EXTERN_TEMPLATE)
#    if defined(CC_COMPILER_NO_EXTERN_TEMPLATE)
#        define CC_EXTERN_TEMPLATE(declaration)
#    else
#        define CC_EXTERN_TEMPLATE(declaration) extern template declaration
#    endif
#endif

// ------------------------------------------------------------------------
// CC_NOEXCEPT
// CC_NOEXCEPT_IF(predicate)
// CC_NOEXCEPT_EXPR(expression)
//
// Portable wrapper for C++11 noexcept
// http://en.cppreference.com/w/cpp/language/noexcept
// http://en.cppreference.com/w/cpp/language/noexcept_spec
//
// Example usage:
//     CC_NOEXCEPT
//     CC_NOEXCEPT_IF(predicate)
//     CC_NOEXCEPT_EXPR(expression)
//
//     This function never throws an exception.
//     void DoNothing() CC_NOEXCEPT
//         { }
//
//     This function throws an exception of T::T() throws an exception.
//     template <class T>
//     void DoNothing() CC_NOEXCEPT_IF(CC_NOEXCEPT_EXPR(T()))
//         { T t; }
//
#if !defined(CC_NOEXCEPT)
#    if defined(CC_COMPILER_NO_NOEXCEPT)
#        define CC_NOEXCEPT
#        define CC_NOEXCEPT_IF(predicate)
#        define CC_NOEXCEPT_EXPR(expression) false
#    else
#        define CC_NOEXCEPT                  noexcept
#        define CC_NOEXCEPT_IF(predicate)    noexcept((predicate))
#        define CC_NOEXCEPT_EXPR(expression) noexcept((expression))
#    endif
#endif

// ------------------------------------------------------------------------
// CC_NORETURN
//
// Wraps the C++11 noreturn attribute. See CC_COMPILER_NO_NORETURN
// http://en.cppreference.com/w/cpp/language/attributes
// http://msdn.microsoft.com/en-us/library/k6ktzx3s%28v=vs.80%29.aspx
// http://blog.aaronballman.com/2011/09/understanding-attributes/
//
// Example usage:
//     CC_NORETURN void SomeFunction()
//         { throw "error"; }
//
#if !defined(CC_NORETURN)
#    if defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION >= 1300)  // VS2003 (VC7) and later
#        define CC_NORETURN __declspec(noreturn)
#    elif defined(CC_COMPILER_NO_NORETURN)
#        define CC_NORETURN
#    else
#        define CC_NORETURN [[noreturn]]
#    endif
#endif

// ------------------------------------------------------------------------
// CC_CARRIES_DEPENDENCY
//
// Wraps the C++11 carries_dependency attribute
// http://en.cppreference.com/w/cpp/language/attributes
// http://blog.aaronballman.com/2011/09/understanding-attributes/
//
// Example usage:
//     CC_CARRIES_DEPENDENCY int* SomeFunction()
//         { return &mX; }
//
//
#if !defined(CC_CARRIES_DEPENDENCY)
#    if defined(CC_COMPILER_NO_CARRIES_DEPENDENCY)
#        define CC_CARRIES_DEPENDENCY
#    else
#        define CC_CARRIES_DEPENDENCY [[carries_dependency]]
#    endif
#endif

// ------------------------------------------------------------------------
// CC_FALLTHROUGH
//
// [[fallthrough] is a C++17 standard attribute that appears in switch
// statements to indicate that the fallthrough from the previous case in the
// switch statement is intentially and not a bug.
//
// http://en.cppreference.com/w/cpp/language/attributes
//
// Example usage:
// 		void f(int n)
// 		{
// 			switch(n)
// 			{
// 				case 1:
// 				DoCase1();
// 				// Compiler may generate a warning for fallthrough behaviour
//
// 				case 2:
// 				DoCase2();
//
// 				CC_FALLTHROUGH;
// 				case 3:
// 				DoCase3();
// 			}
// 		}
//
#if !defined(CC_FALLTHROUGH)
#    if defined(CC_COMPILER_NO_FALLTHROUGH)
#        define CC_FALLTHROUGH
#    else
#        define CC_FALLTHROUGH [[fallthrough]]
#    endif
#endif

// ------------------------------------------------------------------------
// CC_NODISCARD
//
// [[nodiscard]] is a C++17 standard attribute that can be applied to a
// function declaration, enum, or class declaration.  If a any of the list
// previously are returned from a function (without the user explicitly
// casting to void) the addition of the [[nodiscard]] attribute encourages
// the compiler to generate a warning about the user discarding the return
// value. This is a useful practice to encourage client code to check API
// error codes.
//
// http://en.cppreference.com/w/cpp/language/attributes
//
// Example usage:
//
//     CC_NODISCARD int baz() { return 42; }
//
//     void foo()
//     {
//         baz(); // warning: ignoring return value of function declared with 'nodiscard' attribute
//     }
//
#if !defined(CC_NODISCARD)
#    if defined(CC_COMPILER_NO_NODISCARD)
#        define CC_NODISCARD
#    else
#        define CC_NODISCARD [[nodiscard]]
#    endif
#endif

// ------------------------------------------------------------------------
// CC_MAYBE_UNUSED
//
// [[maybe_unused]] is a C++17 standard attribute that suppresses warnings
// on unused entities that are declared as maybe_unused.
//
// http://en.cppreference.com/w/cpp/language/attributes
//
// Example usage:
//    void foo(CC_MAYBE_UNUSED int i)
//    {
//        assert(i == 42);  // warning suppressed when asserts disabled.
//    }
//
#if !defined(CC_MAYBE_UNUSED)
#    if defined(CC_COMPILER_NO_MAYBE_UNUSED)
#        define CC_MAYBE_UNUSED
#    else
#        define CC_MAYBE_UNUSED [[maybe_unused]]
#    endif
#endif

// ------------------------------------------------------------------------
// CC_NO_UBSAN
//
// The LLVM/Clang undefined behaviour sanitizer will not analyse a function tagged with the following attribute.
//
// https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html#disabling-instrumentation-with-attribute-no-sanitize-undefined
//
// Example usage:
//     CC_NO_UBSAN int SomeFunction() { ... }
//
#ifndef CC_NO_UBSAN
#    if defined(CC_COMPILER_CLANG)
#        define CC_NO_UBSAN __attribute__((no_sanitize("undefined")))
#    else
#        define CC_NO_UBSAN
#    endif
#endif

// ------------------------------------------------------------------------
// CC_NO_ASAN
//
// The LLVM/Clang address sanitizer will not analyse a function tagged with the following attribute.
//
// https://clang.llvm.org/docs/AddressSanitizer.html#disabling-instrumentation-with-attribute-no-sanitize-address
//
// Example usage:
//     CC_NO_ASAN int SomeFunction() { ... }
//
#ifndef CC_NO_ASAN
#    if defined(CC_COMPILER_CLANG)
#        define CC_NO_ASAN __attribute__((no_sanitize("address")))
#    else
#        define CC_NO_ASAN
#    endif
#endif

// ------------------------------------------------------------------------
// CC_ASAN_ENABLED
//
// Defined as 0 or 1. It's value depends on the compile environment.
// Specifies whether the code is being built with Clang's Address Sanitizer.
//
#if defined(__has_feature)
#    if __has_feature(address_sanitizer)
#        define CC_ASAN_ENABLED 1
#    else
#        define CC_ASAN_ENABLED 0
#    endif
#else
#    define CC_ASAN_ENABLED 0
#endif

// ------------------------------------------------------------------------
// CC_NON_COPYABLE
//
// This macro defines as a class as not being copy-constructable
// or assignable. This is useful for preventing class instances
// from being passed to functions by value, is useful for preventing
// compiler warnings by some compilers about the inability to
// auto-generate a copy constructor and assignment, and is useful
// for simply declaring in the interface that copy semantics are
// not supported by the class. Your class needs to have at least a
// default constructor when using this macro.
//
// Beware that this class works by declaring a private: section of
// the class in the case of compilers that don't support C++11 deleted
// functions.
//
// Note: With some pre-C++11 compilers (e.g. Green Hills), you may need
//       to manually define an instances of the hidden functions, even
//       though they are not used.
//
// Example usage:
//    class Widget {
//       Widget();
//       . . .
//       CC_NON_COPYABLE(Widget)
//    };
//
#if !defined(CC_NON_COPYABLE)
#    if defined(CC_COMPILER_NO_DELETED_FUNCTIONS)
#        define CC_NON_COPYABLE(ClassName)                                                      \
        private:                                                                                \
            CC_DISABLE_VC_WARNING(4822); /* local class member function does not have a body	*/ \
            ClassName(const ClassName&);                                                        \
            void operator=(const ClassName&);                                                   \
            CC_RESTORE_VC_WARNING();
#    else
#        define CC_NON_COPYABLE(ClassName)                                                      \
            CC_DISABLE_VC_WARNING(4822); /* local class member function does not have a body	*/ \
            ClassName(const ClassName&)      = delete;                                          \
            void operator=(const ClassName&) = delete;                                          \
            CC_RESTORE_VC_WARNING();
#    endif
#endif

// ------------------------------------------------------------------------
// CC_FUNCTION_DELETE
//
// Semi-portable way of specifying a deleted function which allows for
// cleaner code in class declarations.
//
// Example usage:
//
//  class Example
//  {
//  private: // For portability with pre-C++11 compilers, make the function private.
//      void foo() CC_FUNCTION_DELETE;
//  };
//
// Note: CC_FUNCTION_DELETE'd functions should be private to prevent the
// functions from being called even when the compiler does not support
// deleted functions. Some compilers (e.g. Green Hills) that don't support
// C++11 deleted functions can require that you define the function,
// which you can do in the associated source file for the class.
//
#if defined(CC_COMPILER_NO_DELETED_FUNCTIONS)
#    define CC_FUNCTION_DELETE
#else
#    define CC_FUNCTION_DELETE = delete
#endif

// ------------------------------------------------------------------------
// CC_DISABLE_DEFAULT_CTOR
//
// Disables the compiler generated default constructor. This macro is
// provided to improve portability and clarify intent of code.
//
// Example usage:
//
//  class Example
//  {
//  private:
//      CC_DISABLE_DEFAULT_CTOR(Example);
//  };
//
#define CC_DISABLE_DEFAULT_CTOR(ClassName) ClassName() CC_FUNCTION_DELETE

// ------------------------------------------------------------------------
// CC_DISABLE_COPY_CTOR
//
// Disables the compiler generated copy constructor. This macro is
// provided to improve portability and clarify intent of code.
//
// Example usage:
//
//  class Example
//  {
//  private:
//      CC_DISABLE_COPY_CTOR(Example);
//  };
//
#define CC_DISABLE_COPY_CTOR(ClassName) ClassName(const ClassName&) CC_FUNCTION_DELETE

// ------------------------------------------------------------------------
// CC_DISABLE_MOVE_CTOR
//
// Disables the compiler generated move constructor. This macro is
// provided to improve portability and clarify intent of code.
//
// Example usage:
//
//  class Example
//  {
//  private:
//      CC_DISABLE_MOVE_CTOR(Example);
//  };
//
#define CC_DISABLE_MOVE_CTOR(ClassName) ClassName(ClassName&&) CC_FUNCTION_DELETE

// ------------------------------------------------------------------------
// CC_DISABLE_ASSIGNMENT_OPERATOR
//
// Disables the compiler generated assignment operator. This macro is
// provided to improve portability and clarify intent of code.
//
// Example usage:
//
//  class Example
//  {
//  private:
//      CC_DISABLE_ASSIGNMENT_OPERATOR(Example);
//  };
//
#define CC_DISABLE_ASSIGNMENT_OPERATOR(ClassName) ClassName& operator=(const ClassName&) CC_FUNCTION_DELETE

// ------------------------------------------------------------------------
// CC_DISABLE_MOVE_OPERATOR
//
// Disables the compiler generated move operator. This macro is
// provided to improve portability and clarify intent of code.
//
// Example usage:
//
//  class Example
//  {
//  private:
//      CC_DISABLE_MOVE_OPERATOR(Example);
//  };
//
#define CC_DISABLE_MOVE_OPERATOR(ClassName) ClassName& operator=(ClassName&&) CC_FUNCTION_DELETE

// ------------------------------------------------------------------------
// CC_OPTIMIZE_OFF / CC_OPTIMIZE_ON
//
// Implements portable inline optimization enabling/disabling.
// Usage of these macros must be in order OFF then ON. This is
// because the OFF macro pushes a set of settings and the ON
// macro pops them. The nesting of OFF/ON sets (e.g. OFF, OFF, ON, ON)
// is not guaranteed to work on all platforms.
//
// This is often used to allow debugging of some code that's
// otherwise compiled with undebuggable optimizations. It's also
// useful for working around compiler code generation problems
// that occur in optimized builds.
//
// Some compilers (e.g. VC++) don't allow doing this within a function and
// so the usage must be outside a function, as with the example below.
// GCC on x86 appears to have some problem with argument passing when
// using CC_OPTIMIZE_OFF in optimized builds.
//
// Example usage:
//     // Disable optimizations for SomeFunction.
//     CC_OPTIMIZE_OFF()
//     void SomeFunction()
//     {
//         ...
//     }
//     CC_OPTIMIZE_ON()
//
#if !defined(CC_OPTIMIZE_OFF)
#    if defined(CC_COMPILER_MSVC)
#        define CC_OPTIMIZE_OFF() __pragma(optimize("", off))
#    elif defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION > 4004) && (defined(__i386__) || defined(__x86_64__))
// GCC 4.4+ - Seems to work only on x86/Linux so far. However, GCC 4.4 itself appears broken and screws up parameter passing conventions.
#        define CC_OPTIMIZE_OFF() _Pragma("GCC push_options") _Pragma("GCC optimize 0")
#    elif defined(CC_COMPILER_CLANG) && (!defined(CC_PLATFORM_ANDROID) || (CC_COMPILER_VERSION >= 380))
#        define CC_OPTIMIZE_OFF()                         \
            CC_DISABLE_CLANG_WARNING(-Wunknown - pragmas) \
            _Pragma("clang optimize off") CC_RESTORE_CLANG_WARNING()
#    else
#        define CC_OPTIMIZE_OFF()
#    endif
#endif

#if !defined(CC_OPTIMIZE_ON)
#    if defined(CC_COMPILER_MSVC)
#        define CC_OPTIMIZE_ON() __pragma(optimize("", on))
#    elif defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION > 4004) && (defined(__i386__) || defined(__x86_64__))
// GCC 4.4+ - Seems to work only on x86/Linux so far. However, GCC 4.4 itself appears broken and screws up parameter passing conventions.
#        define CC_OPTIMIZE_ON() _Pragma("GCC pop_options")
#    elif defined(CC_COMPILER_CLANG) && (!defined(CC_PLATFORM_ANDROID) || (CC_COMPILER_VERSION >= 380))
#        define CC_OPTIMIZE_ON()                          \
            CC_DISABLE_CLANG_WARNING(-Wunknown - pragmas) \
            _Pragma("clang optimize on") CC_RESTORE_CLANG_WARNING()
#    else
#        define CC_OPTIMIZE_ON()
#    endif
#endif

};  // namespace ncore

#endif

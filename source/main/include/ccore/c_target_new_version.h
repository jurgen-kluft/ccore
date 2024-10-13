#ifndef __CCORE_TARGET_V2_H__
#define __CCORE_TARGET_V2_H__
#pragma once

namespace ncore
{
    // Hardware enumeration for TARGET_PLATFORM
    namespace eplatform
    {
        enum enum_value
        {
            PLATFORM_NONE             = 0,
            PLATFORM_PC               = (0x0001 << 0),
            PLATFORM_MAC              = (0x0001 << 1),
            PLATFORM_LINUX            = (0x0001 << 2),
            PLATFORM_ANDROID          = 0,
            PLATFORM_APPLE            = 0,
            PLATFORM_IPHONE           = 0,
            PLATFORM_IPHONE_SIMULATOR = 0,
            PLATFORM_OSX              = 0,
            PLATFORM_LINUX            = 0,
            PLATFORM_SAMSUNG_TV       = 0,
            PLATFORM_WINDOWS          = 0,
            PLATFORM_WIN32            = 0,
            PLATFORM_WIN64            = 0,
            PLATFORM_WINDOWS_PHONE    = 0,
            PLATFORM_WINRT            = 0,
            PLATFORM_SUN              = 0,
            PLATFORM_LRB              = 0,  // (Larrabee)
            PLATFORM_POSIX            = 0,  // (pseudo-platform; may be defined with another platform like PLATFORM_LINUX, PLATFORM_UNIX, PLATFORM_QNX)
            PLATFORM_UNIX             = 0,  // (pseudo-platform; may be defined with another platform like PLATFORM_LINUX)
            PLATFORM_CYGWIN           = 0,  // (pseudo-platform; may be defined with another platform like PLATFORM_LINUX)
            PLATFORM_MINGW            = 0,  // (pseudo-platform; may be defined with another platform like PLATFORM_WINDOWS)
            PLATFORM_MICROSOFT        = 0,  // (pseudo-platform; may be defined with another platform like PLATFORM_WINDOWS)
            PLATFORM_ALL              = TARGET_PLATFORM_PC | TARGET_PLATFORM_MAC | TARGET_PLATFORM_LINUX,
        };

        const char* ToString(enum_value platform);

    };  // namespace eplatform

    // Available configurations
    namespace econfig
    {
        enum enum_value
        {
            CONFIG_NONE    = 0,
            CONFIG_DEBUG   = (0x0100 << 0),
            CONFIG_RELEASE = (0x0100 << 1),
            CONFIG_FINAL   = (0x0100 << 2),
            CONFIG_ALL     = CONFIG_DEBUG | CONFIG_RELEASE | CONFIG_FINAL,
        };

        const char* ToString(enum_value config);

    }  // namespace econfig

    // Available build types
    namespace ebuild
    {
        enum enum_value
        {
            BUILD_NONE    = 0,
            BUILD_DEV     = (0x1000 << 0),
            BUILD_TEST    = (0x1000 << 1),
            BUILD_PROFILE = (0x1000 << 2),
            BUILD_RETAIL  = (0x1000 << 3),
            BUILD_ALL     = BUILD_DEV | BUILD_RETAIL | BUILD_TEST,
        };

        const char* ToString(enum_value build);

    }  // namespace ebuild

    // Platform define emanations from this file:
    // #define CC_PLATFORM_ANDROID
    // #define CC_PLATFORM_APPLE
    // #define CC_PLATFORM_IPHONE
    // #define CC_PLATFORM_IPHONE_SIMULATOR
    // #define CC_PLATFORM_OSX
    // #define CC_PLATFORM_LINUX
    // #define CC_PLATFORM_SAMSUNG_TV
    // #define CC_PLATFORM_WINDOWS
    // #define CC_PLATFORM_WIN32
    // #define CC_PLATFORM_WIN64
    // #define CC_PLATFORM_WINDOWS_PHONE
    // #define CC_PLATFORM_WINRT
    // #define CC_PLATFORM_SUN
    // #define CC_PLATFORM_LRB       (Larrabee)
    // #define CC_PLATFORM_POSIX     (pseudo-platform; defined along with e.g. CC_PLATFORM_LINUX, CC_PLATFORM_UNIX, CC_PLATFORM_QNX)
    // #define CC_PLATFORM_UNIX      (pseudo-platform; defined along with e.g. CC_PLATFORM_LINUX)
    // #define CC_PLATFORM_CYGWIN    (pseudo-platform; defined along with e.g. CC_PLATFORM_LINUX)
    // #define CC_PLATFORM_MINGW     (pseudo-platform; defined along with e.g. CC_PLATFORM_WINDOWS)
    // #define CC_PLATFORM_MICROSOFT (pseudo-platform; defined along with e.g. CC_PLATFORM_WINDOWS)

    // More emanations from this file:
    // #define CC_PLATFORM_NAME = <string>
    // #define CC_PLATFORM_DESCRIPTION = <string>
    // #define CC_PROCESSOR_XXX
    // #define CC_MISALIGNED_SUPPORT_LEVEL=0|1|2
    // #define CC_SYSTEM_LITTLE_ENDIAN | CC_SYSTEM_BIG_ENDIAN
    // #define CC_ASM_STYLE_ATT | CC_ASM_STYLE_INTEL | CC_ASM_STYLE_MOTOROLA
    // #define CC_PLATFORM_PTR_SIZE = <integer size in bytes>
    // #define CC_PLATFORM_WORD_SIZE = <integer size in bytes>
    // #define CC_CACHE_LINE_SIZE = <integer size in bytes>

    // #define CC_PLATFORM_MOBILE
    //   CC_PLATFORM_MOBILE is a peer to CC_PLATORM_DESKTOP and CC_PLATFORM_CONSOLE. Their definition is qualitative rather
    //   than quantitative, and refers to the general (usually weaker) capabilities of the machine. Mobile devices have a
    //   similar set of weaknesses that are useful to generally categorize. The primary motivation is to avoid code that
    //   tests for multiple mobile platforms on a line and needs to be updated every time we get a new one.
    //   For example, mobile platforms tend to have weaker ARM processors, don't have full multiple processor support,
    //   are hand-held, don't have mice (though may have touch screens or basic cursor controls), have writable solid
    //   state permanent storage. Production user code shouldn't have too many expectations about the meaning of this define.
    //
    // define CC_PLATFORM_DESKTOP
    //   This is similar to CC_PLATFORM_MOBILE in its qualitative nature and refers to platforms that are powerful.
    //   For example, they nearly always have virtual memory, mapped memory, hundreds of GB of writable disk storage,
    //   TCP/IP network connections, mice, keyboards, 512+ MB of RAM, multiprocessing, multiple display support.
    //   Production user code shouldn't have too many expectations about the meaning of this define.
    //
    // define CC_PLATFORM_CONSOLE
    //   This is similar to CC_PLATFORM_MOBILE in its qualitative nature and refers to platforms that are consoles.
    //   This means platforms that are connected to TVs, are fairly powerful (especially graphics-wise), are tightly
    //   controlled by vendors, tend not to have mapped memory, tend to have TCP/IP, don't have multiple process support
    //   though they might have multiple CPUs, support TV output only. Production user code shouldn't have too many
    //   expectations about the meaning of this define.

// Cygwin
// This is a pseudo-platform which will be defined along with CC_PLATFORM_LINUX when
// using the Cygwin build environment.
#if defined(__CYGWIN__)
#    define CC_PLATFORM_CYGWIN  1
#    define CC_PLATFORM_DESKTOP 1
#endif

// MinGW
// This is a pseudo-platform which will be defined along with CC_PLATFORM_WINDOWS when
// using the MinGW Windows build environment.
#if defined(__MINGW32__) || defined(__MINGW64__)
#    define CC_PLATFORM_MINGW   1
#    define CC_PLATFORM_DESKTOP 1
#endif

#if defined(CC_PLATFORM_LRB) || defined(__LRB__) || (defined(__EDG__) && defined(__ICC) && defined(__x86_64__))
#    undef CC_PLATFORM_LRB
#    define CC_PLATFORM_LRB         1
#    define CC_PLATFORM_NAME        "Larrabee"
#    define CC_PLATFORM_DESCRIPTION "Larrabee on LRB1"
#    define CC_PROCESSOR_X86_64     1
#    if defined(BYTE_ORDER) && (BYTE_ORDER == 4321)
#        define CC_SYSTEM_BIG_ENDIAN 1
#    else
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#    endif
#    define CC_PROCESSOR_LRB    1
#    define CC_PROCESSOR_LRB1   1  // Larrabee version 1
#    define CC_ASM_STYLE_ATT    1  // Both types of asm style
#    define CC_ASM_STYLE_INTEL  1  // are supported.
#    define CC_PLATFORM_DESKTOP 1

// Android (Google phone OS)
#elif defined(CC_PLATFORM_ANDROID) || defined(__ANDROID__)
#    undef CC_PLATFORM_ANDROID
#    define CC_PLATFORM_ANDROID 1
#    define CC_PLATFORM_LINUX   1
#    define CC_PLATFORM_UNIX    1
#    define CC_PLATFORM_POSIX   1
#    define CC_PLATFORM_NAME    "Android"
#    define CC_ASM_STYLE_ATT    1
#    if defined(__arm__)
#        define CC_ABI_ARM_LINUX        1  // a.k.a. "ARM eabi"
#        define CC_PROCESSOR_ARM32      1
#        define CC_PLATFORM_DESCRIPTION "Android on ARM"
#    elif defined(__aarch64__)
#        define CC_PROCESSOR_ARM64      1
#        define CC_PLATFORM_DESCRIPTION "Android on ARM64"
#    elif defined(__i386__)
#        define CC_PROCESSOR_X86        1
#        define CC_PLATFORM_DESCRIPTION "Android on x86"
#    elif defined(__x86_64)
#        define CC_PROCESSOR_X86_64     1
#        define CC_PLATFORM_DESCRIPTION "Android on x64"
#    else
#        error Unknown processor
#    endif
#    if !defined(CC_SYSTEM_BIG_ENDIAN) && !defined(CC_SYSTEM_LITTLE_ENDIAN)
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#    endif
#    define CC_PLATFORM_MOBILE 1

// Samsung SMART TV - a Linux-based smart TV
#elif defined(CC_PLATFORM_SAMSUNG_TV)
#    undef CC_PLATFORM_SAMSUNG_TV
#    define CC_PLATFORM_SAMSUNG_TV  1
#    define CC_PLATFORM_LINUX       1
#    define CC_PLATFORM_UNIX        1
#    define CC_PLATFORM_POSIX       1
#    define CC_PLATFORM_NAME        "SamsungTV"
#    define CC_PLATFORM_DESCRIPTION "Samsung SMART TV on ARM"
#    define CC_ASM_STYLE_ATT        1
#    define CC_SYSTEM_LITTLE_ENDIAN 1
#    define CC_PROCESSOR_ARM32      1
#    define CC_ABI_ARM_LINUX        1  // a.k.a. "ARM eabi"
#    define CC_PROCESSOR_ARM7       1

#elif defined(__APPLE__) && __APPLE__
#    include <TargetConditionals.h>

// Apple family of operating systems.
#    define CC_PLATFORM_APPLE
#    define CC_PLATFORM_POSIX 1

// iPhone
// TARGET_OS_IPHONE will be undefined on an unknown compiler, and will be defined on gcc.
#    if defined(CC_PLATFORM_IPHONE) || defined(__IPHONE__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR)
#        undef CC_PLATFORM_IPHONE
#        define CC_PLATFORM_IPHONE         1
#        define CC_PLATFORM_NAME           "iPhone"
#        define CC_ASM_STYLE_ATT           1
#        define CC_POSIX_THREADS_AVAILABLE 1
#        if defined(__arm__)
#            define CC_ABI_ARM_APPLE        1
#            define CC_PROCESSOR_ARM32      1
#            define CC_SYSTEM_LITTLE_ENDIAN 1
#            define CC_PLATFORM_DESCRIPTION "iPhone on ARM"
#        elif defined(__aarch64__) || defined(__AARCH64)
#            define CC_ABI_ARM64_APPLE      1
#            define CC_PROCESSOR_ARM64      1
#            define CC_SYSTEM_LITTLE_ENDIAN 1
#            define CC_PLATFORM_DESCRIPTION "iPhone on ARM64"
#        elif defined(__i386__)
#            define CC_PLATFORM_IPHONE_SIMULATOR 1
#            define CC_PROCESSOR_X86             1
#            define CC_SYSTEM_LITTLE_ENDIAN      1
#            define CC_PLATFORM_DESCRIPTION      "iPhone simulator on x86"
#        elif defined(__x86_64) || defined(__amd64)
#            define CC_PROCESSOR_X86_64     1
#            define CC_SYSTEM_LITTLE_ENDIAN 1
#            define CC_PLATFORM_DESCRIPTION "iPhone simulator on x64"
#        else
#            error Unknown processor
#        endif
#        define CC_PLATFORM_MOBILE 1

// Macintosh OSX
// TARGET_OS_MAC is defined by the Metrowerks and older AppleC compilers.
// Howerver, TARGET_OS_MAC is defined to be 1 in all cases.
// __i386__ and __intel__ are defined by the GCC compiler.
// __dest_os is defined by the Metrowerks compiler.
// __MACH__ is defined by the Metrowerks and GCC compilers.
// powerc and __powerc are defined by the Metrowerks and GCC compilers.
#    elif defined(CC_PLATFORM_OSX) || defined(__MACH__) || (defined(__MSL__) && (__dest_os == __mac_os_x))
#        undef CC_PLATFORM_OSX
#        define CC_PLATFORM_OSX   1
#        define CC_PLATFORM_UNIX  1
#        define CC_PLATFORM_POSIX 1
       // #define CC_PLATFORM_BSD 1           We don't currently define this. OSX has some BSD history but a lot of the API is different.
#        define CC_PLATFORM_NAME  "OSX"
#        if defined(__i386__) || defined(__intel__)
#            define CC_PROCESSOR_X86        1
#            define CC_SYSTEM_LITTLE_ENDIAN 1
#            define CC_PLATFORM_DESCRIPTION "OSX on x86"
#        elif defined(__x86_64) || defined(__amd64)
#            define CC_PROCESSOR_X86_64     1
#            define CC_SYSTEM_LITTLE_ENDIAN 1
#            define CC_PLATFORM_DESCRIPTION "OSX on x64"
#        elif defined(__arm__)
#            define CC_ABI_ARM_APPLE        1
#            define CC_PROCESSOR_ARM32      1
#            define CC_SYSTEM_LITTLE_ENDIAN 1
#            define CC_PLATFORM_DESCRIPTION "OSX on ARM"
#        elif defined(__aarch64__) || defined(__AARCH64)
#            define CC_ABI_ARM64_APPLE      1
#            define CC_PROCESSOR_ARM64      1
#            define CC_SYSTEM_LITTLE_ENDIAN 1
#            define CC_PLATFORM_DESCRIPTION "OSX on ARM64"
#        elif defined(__POWERPC64__) || defined(__powerpc64__)
#            define CC_PROCESSOR_POWERPC    1
#            define CC_PROCESSOR_POWERPC_64 1
#            define CC_SYSTEM_BIG_ENDIAN    1
#            define CC_PLATFORM_DESCRIPTION "OSX on PowerPC 64"
#        elif defined(__POWERPC__) || defined(__powerpc__)
#            define CC_PROCESSOR_POWERPC    1
#            define CC_PROCESSOR_POWERPC_32 1
#            define CC_SYSTEM_BIG_ENDIAN    1
#            define CC_PLATFORM_DESCRIPTION "OSX on PowerPC"
#        else
#            error Unknown processor
#        endif
#        if defined(__GNUC__)
#            define CC_ASM_STYLE_ATT 1
#        else
#            define CC_ASM_STYLE_MOTOROLA 1
#        endif
#        define CC_PLATFORM_DESKTOP 1
#    else
#        error Unknown Apple Platform
#    endif

// Linux
// __linux and __linux__ are defined by the GCC and Borland compiler.
// __i386__ and __intel__ are defined by the GCC compiler.
// __i386__ is defined by the Metrowerks compiler.
// _M_IX86 is defined by the Borland compiler.
// __sparc__ is defined by the GCC compiler.
// __powerpc__ is defined by the GCC compiler.
// __ARM_EABI__ is defined by GCC on an ARM v6l (Raspberry Pi 1)
// __ARM_ARCH_7A__ is defined by GCC on an ARM v7l (Raspberry Pi 2)
#elif defined(CC_PLATFORM_LINUX) || (defined(__linux) || defined(__linux__))
#    undef CC_PLATFORM_LINUX
#    define CC_PLATFORM_LINUX 1
#    define CC_PLATFORM_UNIX  1
#    define CC_PLATFORM_POSIX 1
#    define CC_PLATFORM_NAME  "Linux"
#    if defined(__i386__) || defined(__intel__) || defined(_M_IX86)
#        define CC_PROCESSOR_X86        1
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#        define CC_PLATFORM_DESCRIPTION "Linux on x86"
#    elif defined(__ARM_ARCH_7A__) || defined(__ARM_EABI__)
#        define CC_ABI_ARM_LINUX        1
#        define CC_PROCESSOR_ARM32      1
#        define CC_PLATFORM_DESCRIPTION "Linux on ARM 6/7 32-bits"
#    elif defined(__aarch64__) || defined(__AARCH64)
#        define CC_PROCESSOR_ARM64      1
#        define CC_PLATFORM_DESCRIPTION "Linux on ARM64"
#    elif defined(__x86_64__)
#        define CC_PROCESSOR_X86_64     1
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#        define CC_PLATFORM_DESCRIPTION "Linux on x64"
#    elif defined(__powerpc64__)
#        define CC_PROCESSOR_POWERPC    1
#        define CC_PROCESSOR_POWERPC_64 1
#        define CC_SYSTEM_BIG_ENDIAN    1
#        define CC_PLATFORM_DESCRIPTION "Linux on PowerPC 64"
#    elif defined(__powerpc__)
#        define CC_PROCESSOR_POWERPC    1
#        define CC_PROCESSOR_POWERPC_32 1
#        define CC_SYSTEM_BIG_ENDIAN    1
#        define CC_PLATFORM_DESCRIPTION "Linux on PowerPC"
#    else
#        error Unknown processor
#        error Unknown endianness
#    endif
#    if defined(__GNUC__)
#        define CC_ASM_STYLE_ATT 1
#    endif
#    define CC_PLATFORM_DESKTOP 1

#elif defined(CC_PLATFORM_BSD) || (defined(__BSD__) || defined(__FreeBSD__))
#    undef CC_PLATFORM_BSD
#    define CC_PLATFORM_BSD   1
#    define CC_PLATFORM_UNIX  1
#    define CC_PLATFORM_POSIX 1  // BSD's posix complaince is not identical to Linux's
#    define CC_PLATFORM_NAME  "BSD Unix"
#    if defined(__i386__) || defined(__intel__)
#        define CC_PROCESSOR_X86        1
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#        define CC_PLATFORM_DESCRIPTION "BSD on x86"
#    elif defined(__x86_64__)
#        define CC_PROCESSOR_X86_64     1
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#        define CC_PLATFORM_DESCRIPTION "BSD on x64"
#    elif defined(__powerpc64__)
#        define CC_PROCESSOR_POWERPC    1
#        define CC_PROCESSOR_POWERPC_64 1
#        define CC_SYSTEM_BIG_ENDIAN    1
#        define CC_PLATFORM_DESCRIPTION "BSD on PowerPC 64"
#    elif defined(__powerpc__)
#        define CC_PROCESSOR_POWERPC    1
#        define CC_PROCESSOR_POWERPC_32 1
#        define CC_SYSTEM_BIG_ENDIAN    1
#        define CC_PLATFORM_DESCRIPTION "BSD on PowerPC"
#    else
#        error Unknown processor
#        error Unknown endianness
#    endif
#    if !defined(CC_PLATFORM_FREEBSD) && defined(__FreeBSD__)
#        define CC_PLATFORM_FREEBSD 1  // This is a variation of BSD.
#    endif
#    if defined(__GNUC__)
#        define CC_ASM_STYLE_ATT 1
#    endif
#    define CC_PLATFORM_DESKTOP 1

#elif defined(CC_PLATFORM_WINDOWS_PHONE)
#    undef CC_PLATFORM_WINDOWS_PHONE
#    define CC_PLATFORM_WINDOWS_PHONE 1
#    define CC_PLATFORM_NAME          "Windows Phone"
#    if defined(_M_AMD64) || defined(_AMD64_) || defined(__x86_64__)
#        define CC_PROCESSOR_X86_64     1
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#        define CC_PLATFORM_DESCRIPTION "Windows Phone on x64"
#    elif defined(_M_IX86) || defined(_X86_)
#        define CC_PROCESSOR_X86        1
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#        define CC_PLATFORM_DESCRIPTION "Windows Phone on X86"
#    elif defined(_M_ARM)
#        define CC_ABI_ARM_WINCE        1
#        define CC_PROCESSOR_ARM32      1
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#        define CC_PLATFORM_DESCRIPTION "Windows Phone on ARM"
#    else  // Possibly other Windows Phone variants
#        error Unknown processor
#        error Unknown endianness
#    endif
#    define CC_PLATFORM_MICROSOFT        1

// WINAPI_FAMILY defines - mirrored from winapifamily.h
#    define CC_WINAPI_FAMILY_APP         1
#    define CC_WINAPI_FAMILY_DESKTOP_APP 2
#    define CC_WINAPI_FAMILY_PHONE_APP   3

#    if defined(WINAPI_FAMILY)
#        include <winapifamily.h>
#        if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
#            define CC_WINAPI_FAMILY CC_WINAPI_FAMILY_PHONE_APP
#        else
#            error Unsupported WINAPI_FAMILY for Windows Phone
#        endif
#    else
#        error WINAPI_FAMILY should always be defined on Windows Phone.
#    endif

// Macro to determine if a partition is enabled.
#    define CC_WINAPI_FAMILY_PARTITION(Partition) (Partition)

// Enable the appropriate partitions for the current family
#    if CC_WINAPI_FAMILY == CC_WINAPI_FAMILY_PHONE_APP
#        define CC_WINAPI_PARTITION_CORE  1
#        define CC_WINAPI_PARTITION_PHONE 1
#        define CC_WINAPI_PARTITION_APP   1
#    else
#        error Unsupported WINAPI_FAMILY for Windows Phone
#    endif

// Windows
// _WIN32 is defined by the VC++, Intel and GCC compilers.
// _WIN64 is defined by the VC++, Intel and GCC compilers.
// __WIN32__ is defined by the Borland compiler.
// __INTEL__ is defined by the Metrowerks compiler.
// _M_IX86, _M_AMD64 and _M_IA64 are defined by the VC++, Intel, and Borland compilers.
// _X86_, _AMD64_, and _IA64_ are defined by the Metrowerks compiler.
// _M_ARM is defined by the VC++ compiler.
#elif (defined(CC_PLATFORM_WINDOWS) || (defined(_WIN32) || defined(__WIN32__) || defined(_WIN64))) && !defined(CS_UNDEFINED_STRING)
#    undef CC_PLATFORM_WINDOWS
#    define CC_PLATFORM_WINDOWS 1
#    define CC_PLATFORM_NAME    "Windows"
#    ifdef _WIN64  // VC++ defines both _WIN32 and _WIN64 when compiling for Win64.
#        define CC_PLATFORM_WIN64 1
#    else
#        define CC_PLATFORM_WIN32 1
#    endif
#    if defined(_M_AMD64) || defined(_AMD64_) || defined(__x86_64__)
#        define CC_PROCESSOR_X86_64     1
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#        define CC_PLATFORM_DESCRIPTION "Windows on x64"
#    elif defined(_M_IX86) || defined(_X86_)
#        define CC_PROCESSOR_X86        1
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#        define CC_PLATFORM_DESCRIPTION "Windows on X86"
#    elif defined(_M_IA64) || defined(_IA64_)
#        define CC_PROCESSOR_IA64       1
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#        define CC_PLATFORM_DESCRIPTION "Windows on IA-64"
#    elif defined(_M_ARM)
#        define CC_ABI_ARM_WINCE        1
#        define CC_PROCESSOR_ARM32      1
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#        define CC_PLATFORM_DESCRIPTION "Windows on ARM"
#    elif defined(_M_ARM64)
#        define CC_PROCESSOR_ARM64      1
#        define CC_SYSTEM_LITTLE_ENDIAN 1
#        define CC_PLATFORM_DESCRIPTION "Windows on ARM64"
#    else  // Possibly other Windows CE variants
#        error Unknown processor
#        error Unknown endianness
#    endif
#    if defined(__GNUC__)
#        define CC_ASM_STYLE_ATT 1
#    elif defined(_MSC_VER) || defined(__BORLANDC__) || defined(__ICL)
#        define CC_ASM_STYLE_INTEL 1
#    endif
#    define CC_PLATFORM_DESKTOP   1
#    define CC_PLATFORM_MICROSOFT 1

#    if defined(_KERNEL_MODE)
#        define CC_PLATFORM_WINDOWS_KERNEL 1
#    endif

// WINAPI_FAMILY defines to support Windows 8 Metro Apps - mirroring winapifamily.h in the Windows 8 SDK
#    define CC_WINAPI_FAMILY_APP         1000
#    define CC_WINAPI_FAMILY_DESKTOP_APP 1001
#    define CC_WINAPI_FAMILY_GAMES       1006

#    if defined(WINAPI_FAMILY)
#        if defined(_MSC_VER)
#            pragma warning(push, 0)
#        endif
#        include <winapifamily.h>
#        if defined(_MSC_VER)
#            pragma warning(pop)
#        endif
#        if defined(WINAPI_FAMILY_DESKTOP_APP) && WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
#            define CC_WINAPI_FAMILY CC_WINAPI_FAMILY_DESKTOP_APP
#        elif defined(WINAPI_FAMILY_APP) && WINAPI_FAMILY == WINAPI_FAMILY_APP
#            define CC_WINAPI_FAMILY CC_WINAPI_FAMILY_APP
#        elif defined(WINAPI_FAMILY_GAMES) && WINAPI_FAMILY == WINAPI_FAMILY_GAMES
#            define CC_WINAPI_FAMILY CC_WINAPI_FAMILY_GAMES
#        else
#            error Unsupported WINAPI_FAMILY
#        endif
#    else
#        define CC_WINAPI_FAMILY CC_WINAPI_FAMILY_DESKTOP_APP
#    endif

#    define CC_WINAPI_PARTITION_DESKTOP 1
#    define CC_WINAPI_PARTITION_APP     1
#    define CC_WINAPI_PARTITION_GAMES   (CC_WINAPI_FAMILY == CC_WINAPI_FAMILY_GAMES)

#    define CC_WINAPI_FAMILY_PARTITION(Partition) (Partition)

// CC_PLATFORM_WINRT
// This is a subset of Windows which is used for tablets and the "Metro" (restricted) Windows user interface.
// WinRT doesn't doesn't have access to the Windows "desktop" API, but WinRT can nevertheless run on
// desktop computers in addition to tablets. The Windows Phone API is a subset of WinRT and is not included
// in it due to it being only a part of the API.
#    if defined(__cplusplus_winrt)
#        define CC_PLATFORM_WINRT 1
#    endif

// Sun (Solaris)
// __SUNPRO_CC is defined by the Sun compiler.
// __sun is defined by the GCC compiler.
// __i386 is defined by the Sun and GCC compilers.
// __sparc is defined by the Sun and GCC compilers.
#else
#    error Unknown platform
#    error Unknown processor
#    error Unknown endianness
#endif

#ifndef CC_PROCESSOR_ARM
#    if defined(CC_PROCESSOR_ARM32) || defined(CC_PROCESSOR_ARM64) || defined(CC_PROCESSOR_ARM7)
#        define CC_PROCESSOR_ARM
#    endif
#endif

// CC_PLATFORM_PTR_SIZE
// Platform pointer size; same as sizeof(void*).
// This is not the same as sizeof(int), as int is usually 32 bits on
// even 64 bit platforms.
//
// _WIN64 is defined by Win64 compilers, such as VC++.
// _M_IA64 is defined by VC++ and Intel compilers for IA64 processors.
// __LP64__ is defined by HP compilers for the LP64 standard.
// _LP64 is defined by the GCC and Sun compilers for the LP64 standard.
// __ia64__ is defined by the GCC compiler for IA64 processors.
// __arch64__ is defined by the Sparc compiler for 64 bit processors.
// __mips64__ is defined by the GCC compiler for MIPS processors.
// __powerpc64__ is defined by the GCC compiler for PowerPC processors.
// __64BIT__ is defined by the AIX compiler for 64 bit processors.
// __sizeof_ptr is defined by the ARM compiler (armcc, armcpp).
//
#ifndef CC_PLATFORM_PTR_SIZE
#    if defined(__WORDSIZE)  // Defined by some variations of GCC.
#        define CC_PLATFORM_PTR_SIZE ((__WORDSIZE) / 8)
#    elif defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(_M_IA64) || defined(__ia64__) || defined(__arch64__) || defined(__aarch64__) || defined(__mips64__) || defined(__64BIT__) || defined(__Ptr_Is_64)
#        define CC_PLATFORM_PTR_SIZE 8
#    elif defined(__CC_ARM) && (__sizeof_ptr == 8)
#        define CC_PLATFORM_PTR_SIZE 8
#    else
#        define CC_PLATFORM_PTR_SIZE 4
#    endif
#endif

// CC_PLATFORM_WORD_SIZE
// This defines the size of a machine word. This will be the same as
// the size of registers on the machine but not necessarily the same
// as the size of pointers on the machine. A number of 64 bit platforms
// have 64 bit registers but 32 bit pointers.
//
#ifndef CC_PLATFORM_WORD_SIZE
#    define CC_PLATFORM_WORD_SIZE CC_PLATFORM_PTR_SIZE
#endif

// CC_PLATFORM_MIN_MALLOC_ALIGNMENT
// This defines the minimal alignment that the platform's malloc
// implementation will return. This should be used when writing custom
// allocators to ensure that the alignment matches that of malloc
#ifndef CC_PLATFORM_MIN_MALLOC_ALIGNMENT
#    if defined(CC_PLATFORM_APPLE)
#        define CC_PLATFORM_MIN_MALLOC_ALIGNMENT 16
#    elif defined(CC_PLATFORM_ANDROID) && defined(CC_PROCESSOR_ARM)
#        define CC_PLATFORM_MIN_MALLOC_ALIGNMENT 8
#    elif defined(CC_PLATFORM_ANDROID) && defined(CC_PROCESSOR_X86_64)
#        define CC_PLATFORM_MIN_MALLOC_ALIGNMENT 8
#    else
#        define CC_PLATFORM_MIN_MALLOC_ALIGNMENT (CC_PLATFORM_PTR_SIZE * 2)
#    endif
#endif

// CC_MISALIGNED_SUPPORT_LEVEL
// Specifies if the processor can read and write built-in types that aren't
// naturally aligned.
//    0 - not supported. Likely causes an exception.
//    1 - supported but slow.
//    2 - supported and fast.
//
#ifndef CC_MISALIGNED_SUPPORT_LEVEL
#    if defined(CC_PROCESSOR_X86_64)
#        define CC_MISALIGNED_SUPPORT_LEVEL 2
#    else
#        define CC_MISALIGNED_SUPPORT_LEVEL 0
#    endif
#endif

// Macro to determine if a Windows API partition is enabled. Always false on non Microsoft platforms.
#if !defined(CC_WINAPI_FAMILY_PARTITION)
#    define CC_WINAPI_FAMILY_PARTITION(Partition) (0)
#endif

    // CC_CACHE_LINE_SIZE
    // Specifies the cache line size broken down by compile target.
    // This the expected best guess values for the targets that we can make at compilation time.

#ifndef CC_CACHE_LINE_SIZE
#    if defined(CC_PROCESSOR_X86)
#        define CC_CACHE_LINE_SIZE 32  // This is the minimum possible value.
#    elif defined(CC_PROCESSOR_X86_64)
#        define CC_CACHE_LINE_SIZE 64  // This is the minimum possible value
#    elif defined(CC_PROCESSOR_ARM32)
#        define CC_CACHE_LINE_SIZE 32  // This varies between implementations and is usually 32 or 64.
#    elif defined(CC_PROCESSOR_ARM64)
#        define CC_CACHE_LINE_SIZE 64  // Cache line Cortex-A8  (64 bytes) (assumption at this stage)
#    elif (CC_PLATFORM_WORD_SIZE == 4)
#        define CC_CACHE_LINE_SIZE 32  // This is the minimum possible value
#    else
#        define CC_CACHE_LINE_SIZE 64  // This is the minimum possible value
#    endif
#endif

    // -----------------------------------------------------------------------------
    //  COMPILER
    // -----------------------------------------------------------------------------
    //  Currently supported defines include:
    //      CC_COMPILER_GNUC
    //      CC_COMPILER_ARM
    //      CC_COMPILER_EDG
    //      CC_COMPILER_SN
    //      CC_COMPILER_MSVC
    //      CC_COMPILER_METROWERKS
    //      CC_COMPILER_INTEL
    //      CC_COMPILER_BORLANDC
    //      CC_COMPILER_IBM
    //      CC_COMPILER_QNX
    //      CC_COMPILER_GREEN_HILLS
    //      CC_COMPILER_CLANG
    //      CC_COMPILER_CLANG_CL
    //
    //      CC_COMPILER_VERSION = <integer>
    //      CC_COMPILER_NAME = <string>
    //      CC_COMPILER_STRING = <string>
    //
    //   C++98/03 functionality
    //      CC_COMPILER_NO_STATIC_CONSTANTS
    //      CC_COMPILER_NO_TEMPLATE_SPECIALIZATION
    //      CC_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION
    //      CC_COMPILER_NO_MEMBER_TEMPLATES
    //      CC_COMPILER_NO_MEMBER_TEMPLATE_SPECIALIZATION
    //      CC_COMPILER_NO_TEMPLATE_TEMPLATES
    //      CC_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS
    //      CC_COMPILER_NO_VOID_RETURNS
    //      CC_COMPILER_NO_COVARIANT_RETURN_TYPE
    //      CC_COMPILER_NO_DEDUCED_TYPENAME
    //      CC_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP
    //      CC_COMPILER_NO_EXCEPTION_STD_NAMESPACE
    //      CC_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS
    //      CC_COMPILER_NO_RTTI
    //      CC_COMPILER_NO_EXCEPTIONS
    //      CC_COMPILER_NO_UNWIND
    //      CC_COMPILER_NO_STANDARD_CPP_LIBRARY
    //      CC_COMPILER_NO_STATIC_VARIABLE_INIT
    //      CC_COMPILER_NO_STATIC_FUNCTION_INIT
    //      CC_COMPILER_NO_VARIADIC_MACROS
    //
    //   C++11 functionality
    //      CC_COMPILER_NO_RVALUE_REFERENCES
    //      CC_COMPILER_NO_EXTERN_TEMPLATE
    //      CC_COMPILER_NO_CONSTEXPR
    //      CC_COMPILER_NO_OVERRIDE
    //      CC_COMPILER_NO_INHERITANCE_FINAL
    //      CC_COMPILER_NO_NULLPTR
    //      CC_COMPILER_NO_AUTO
    //      CC_COMPILER_NO_DECLTYPE
    //      CC_COMPILER_NO_DEFAULTED_FUNCTIONS
    //      CC_COMPILER_NO_DELETED_FUNCTIONS
    //      CC_COMPILER_NO_LAMBDA_EXPRESSIONS
    //      CC_COMPILER_NO_TRAILING_RETURN_TYPES
    //      CC_COMPILER_NO_STRONGLY_TYPED_ENUMS
    //      CC_COMPILER_NO_FORWARD_DECLARED_ENUMS
    //      CC_COMPILER_NO_VARIADIC_TEMPLATES
    //      CC_COMPILER_NO_TEMPLATE_ALIASES
    //      CC_COMPILER_NO_INITIALIZER_LISTS
    //      CC_COMPILER_NO_NORETURN
    //      CC_COMPILER_NO_CARRIES_DEPENDENCY
    //      CC_COMPILER_NO_FALLTHROUGH
    //      CC_COMPILER_NO_NODISCARD
    //      CC_COMPILER_NO_MAYBE_UNUSED
    //      CC_COMPILER_NO_NONSTATIC_MEMBER_INITIALIZERS
    //      CC_COMPILER_NO_RIGHT_ANGLE_BRACKETS
    //      CC_COMPILER_NO_ALIGNOF
    //      CC_COMPILER_NO_ALIGNAS
    //      CC_COMPILER_NO_DELEGATING_CONSTRUCTORS
    //      CC_COMPILER_NO_INHERITING_CONSTRUCTORS
    //      CC_COMPILER_NO_USER_DEFINED_LITERALS
    //      CC_COMPILER_NO_STANDARD_LAYOUT_TYPES
    //      CC_COMPILER_NO_EXTENDED_SIZEOF
    //      CC_COMPILER_NO_INLINE_NAMESPACES
    //      CC_COMPILER_NO_UNRESTRICTED_UNIONS
    //      CC_COMPILER_NO_EXPLICIT_CONVERSION_OPERATORS
    //      CC_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
    //      CC_COMPILER_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
    //      CC_COMPILER_NO_NOEXCEPT
    //      CC_COMPILER_NO_RAW_LITERALS
    //      CC_COMPILER_NO_UNICODE_STRING_LITERALS
    //      CC_COMPILER_NO_NEW_CHARACTER_TYPES
    //      CC_COMPILER_NO_UNICODE_CHAR_NAME_LITERALS
    //      CC_COMPILER_NO_UNIFIED_INITIALIZATION_SYNTAX
    //      CC_COMPILER_NO_EXTENDED_FRIEND_DECLARATIONS
    //
    //   C++14 functionality
    //      CC_COMPILER_NO_VARIABLE_TEMPLATES
    //
    //   C++17 functionality
    //      CC_COMPILER_NO_INLINE_VARIABLES
    //      CC_COMPILER_NO_ALIGNED_NEW
    //
    //   C++20 functionality
    //      CC_COMPILER_NO_DESIGNATED_INITIALIZERS
    //
    // -----------------------------------------------------------------------------
    //
    //  Supplemental documentation
    //      CC_COMPILER_NO_STATIC_CONSTANTS
    //          Code such as this is legal, but some compilers fail to compile it:
    //              struct A{ static const a = 1; };
    //
    //      CC_COMPILER_NO_TEMPLATE_SPECIALIZATION
    //          Some compilers fail to allow template specialization, such as with this:
    //              template<class U> void DoSomething(U u);
    //              void DoSomething(int x);
    //
    //      CC_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION
    //          Some compilers fail to allow partial template specialization, such as with this:
    //              template <class T, class Allocator> class vector{ };         // Primary templated class.
    //              template <class Allocator> class vector<bool, Allocator>{ }; // Partially specialized version.
    //
    //      CC_COMPILER_NO_MEMBER_TEMPLATES
    //          Some compilers fail to allow member template functions such as this:
    //              struct A{ template<class U> void DoSomething(U u); };
    //
    //      CC_COMPILER_NO_MEMBER_TEMPLATE_SPECIALIZATION
    //          Some compilers fail to allow member template specialization, such as with this:
    //              struct A{
    //                  template<class U> void DoSomething(U u);
    //                  void DoSomething(int x);
    //              };
    //
    //      CC_COMPILER_NO_TEMPLATE_TEMPLATES
    //          Code such as this is legal:
    //              template<typename T, template<typename> class U>
    //              U<T> SomeFunction(const U<T> x) { return x.DoSomething(); }
    //
    //      CC_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS
    //          Some compilers fail to compile templated friends, as with this:
    //              struct A{ template<class U> friend class SomeFriend; };
    //          This is described in the C++ Standard at 14.5.3.
    //
    //      CC_COMPILER_NO_VOID_RETURNS
    //           This is legal C++:
    //               void DoNothing1(){ };
    //               void DoNothing2(){ return DoNothing1(); }
    //
    //      CC_COMPILER_NO_COVARIANT_RETURN_TYPE
    //          See the C++ standard sec 10.3,p5.
    //
    //      CC_COMPILER_NO_DEDUCED_TYPENAME
    //          Some compilers don't support the use of 'typename' for
    //          dependent types in deduced contexts, as with this:
    //              template <class T> void Function(T, typename T::type);
    //
    //      CC_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP
    //          Also known as Koenig lookup. Basically, if you have a function
    //          that is a namespace and you call that function without prefixing
    //          it with the namespace the compiler should look at any arguments
    //          you pass to that function call and search their namespace *first*
    //          to see if the given function exists there.
    //
    //      CC_COMPILER_NO_EXCEPTION_STD_NAMESPACE
    //          <exception> is in namespace std. Some std libraries fail to
    //          put the contents of <exception> in namespace std. The following
    //          code should normally be legal:
    //              void Function(){ std::terminate(); }
    //
    //      CC_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS
    //          Some compilers fail to execute DoSomething() properly, though they
    //          succeed in compiling it, as with this:
    //              template <int i>
    //              bool DoSomething(int j){ return i == j; };
    //              DoSomething<1>(2);
    //
    //      CC_COMPILER_NO_EXCEPTIONS
    //          The compiler is configured to disallow the use of try/throw/catch
    //          syntax (often to improve performance). Use of such syntax in this
    //          case will cause a compilation error.
    //
    //      CC_COMPILER_NO_UNWIND
    //          The compiler is configured to allow the use of try/throw/catch
    //          syntax and behaviour but disables the generation of stack unwinding
    //          code for responding to exceptions (often to improve performance).
    //
    // ---------------------------------------------------------------------------*/

// Note: This is used to generate the CC_COMPILER_STRING macros
#ifndef INTERNAL_STRINGIZE
#    define INTERNAL_STRINGIZE(x) INTERNAL_PRIMITIVE_STRINGIZE(x)
#endif
#ifndef INTERNAL_PRIMITIVE_STRINGIZE
#    define INTERNAL_PRIMITIVE_STRINGIZE(x) #x
#endif

// CC_COMPILER_HAS_FEATURE
#ifndef CC_COMPILER_HAS_FEATURE
#    if defined(__clang__)
#        define CC_COMPILER_HAS_FEATURE(x) __has_feature(x)
#    else
#        define CC_COMPILER_HAS_FEATURE(x) 0
#    endif
#endif

// CC_COMPILER_HAS_BUILTIN
#ifndef CC_COMPILER_HAS_BUILTIN
#    if defined(__clang__)
#        define CC_COMPILER_HAS_BUILTIN(x) __has_builtin(x)
#    else
#        define CC_COMPILER_HAS_BUILTIN(x) 0
#    endif
#endif

// EDG (EDG compiler front-end, used by other compilers such as SN)
#if defined(__EDG_VERSION__)
#    define CC_COMPILER_EDG 1

#    if defined(_MSC_VER)
#        define CC_COMPILER_EDG_VC_MODE 1
#    endif
#    if defined(__GNUC__)
#        define CC_COMPILER_EDG_GCC_MODE 1
#    endif
#endif

// CC_COMPILER_CPP11_ENABLED
//
// Defined as 1 if the compiler has its available C++11 support enabled, else undefined.
// This does not mean that all of C++11 or any particular feature of C++11 is supported
// by the compiler. It means that whatever C++11 support the compiler has is enabled.
// This also includes existing and older compilers that still identify C++11 as C++0x.
//
// We cannot use (__cplusplus >= 201103L) alone because some compiler vendors have
// decided to not define __cplusplus like thus until they have fully completed their
// C++11 support.
//
#if !defined(CC_COMPILER_CPP11_ENABLED) && defined(__cplusplus)
#    if (__cplusplus >= 201103L)  // Clang and GCC defines this like so in C++11 mode.
#        define CC_COMPILER_CPP11_ENABLED 1
#    elif defined(__GNUC__) && defined(__GXX_EXPERIMENTAL_CXX0X__)
#        define CC_COMPILER_CPP11_ENABLED 1
#    elif defined(_MSC_VER) && _MSC_VER >= 1600  // Microsoft unilaterally enables its C++11 support; there is no way to disable it.
#        define CC_COMPILER_CPP11_ENABLED 1
#    elif defined(__EDG_VERSION__)  // && ???
       // To do: Is there a generic way to determine this?
#    endif
#endif

// CC_COMPILER_CPP14_ENABLED
//
// Defined as 1 if the compiler has its available C++14 support enabled, else undefined.
// This does not mean that all of C++14 or any particular feature of C++14 is supported
// by the compiler. It means that whatever C++14 support the compiler has is enabled.
//
// We cannot use (__cplusplus >= 201402L) alone because some compiler vendors have
// decided to not define __cplusplus like thus until they have fully completed their
// C++14 support.
#if !defined(CC_COMPILER_CPP14_ENABLED) && defined(__cplusplus)
#    if (__cplusplus >= 201402L)  // Clang and GCC defines this like so in C++14 mode.
#        define CC_COMPILER_CPP14_ENABLED 1
#    elif defined(_MSC_VER) && (_MSC_VER >= 1900)  // VS2015+
#        define CC_COMPILER_CPP14_ENABLED 1
#    endif
#endif

// CC_COMPILER_CPP17_ENABLED
//
// Defined as 1 if the compiler has its available C++17 support enabled, else undefined.
// This does not mean that all of C++17 or any particular feature of C++17 is supported
// by the compiler. It means that whatever C++17 support the compiler has is enabled.
//
// We cannot use (__cplusplus >= 201703L) alone because some compiler vendors have
// decided to not define __cplusplus like thus until they have fully completed their
// C++17 support.
#if !defined(CC_COMPILER_CPP17_ENABLED) && defined(__cplusplus)
#    if (__cplusplus >= 201703L)
#        define CC_COMPILER_CPP17_ENABLED 1
#    elif defined(_MSVC_LANG) && (_MSVC_LANG >= 201703L)  // C++17+
#        define CC_COMPILER_CPP17_ENABLED 1
#    endif
#endif

// CC_COMPILER_CPP20_ENABLED
//
// Defined as 1 if the compiler has its available C++20 support enabled, else undefined.
// This does not mean that all of C++20 or any particular feature of C++20 is supported
// by the compiler. It means that whatever C++20 support the compiler has is enabled.
//
// We cannot use (__cplusplus >= 202002L) alone because some compiler vendors have
// decided to not define __cplusplus like thus until they have fully completed their
// C++20 support.
#if !defined(CC_COMPILER_CPP20_ENABLED) && defined(__cplusplus)
#    if (__cplusplus >= 202002L)
#        define CC_COMPILER_CPP20_ENABLED 1
#    elif defined(_MSVC_LANG) && (_MSVC_LANG >= 202003L)  // C++20+
#        define CC_COMPILER_CPP20_ENABLED 1
#    endif
#endif

#if defined(__ARMCC_VERSION)
// Note that this refers to the ARM RVCT compiler (armcc or armcpp), but there
// are other compilers that target ARM processors, such as GCC and Microsoft VC++.
// If you want to detect compiling for the ARM processor, check for CC_PROCESSOR_ARM
// being defined.
// This compiler is also identified by defined(__CC_ARM) || defined(__ARMCC__).
#    define CC_COMPILER_RVCT    1
#    define CC_COMPILER_ARM     1
#    define CC_COMPILER_VERSION __ARMCC_VERSION
#    define CC_COMPILER_NAME    "RVCT"
    // #define CC_COMPILER_STRING (defined below)

// Clang's GCC-compatible driver.
#elif defined(__clang__) && !defined(_MSC_VER)
#    define CC_COMPILER_CLANG   1
#    define CC_COMPILER_VERSION (__clang_major__ * 100 + __clang_minor__)
#    define CC_COMPILER_NAME    "clang"
#    define CC_COMPILER_STRING  CC_COMPILER_NAME __clang_version__

// GCC (a.k.a. GNUC)
#elif defined(__GNUC__)  // GCC compilers exist for many platforms.
#    define CC_COMPILER_GNUC    1
#    define CC_COMPILER_VERSION (__GNUC__ * 1000 + __GNUC_MINOR__)
#    define CC_COMPILER_NAME    "GCC"
#    define CC_COMPILER_STRING  CC_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE(__GNUC__) "." INTERNAL_STRINGIZE(__GNUC_MINOR__)

#    if (__GNUC__ == 2) && (__GNUC_MINOR__ < 95)  // If GCC < 2.95...
#        define CC_COMPILER_NO_MEMBER_TEMPLATES 1
#    endif
#    if (__GNUC__ == 2) && (__GNUC_MINOR__ <= 97)  // If GCC <= 2.97...
#        define CC_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1
#    endif
#    if (__GNUC__ == 3) && ((__GNUC_MINOR__ == 1) || (__GNUC_MINOR__ == 2))  // If GCC 3.1 or 3.2 (but not pre 3.1 or post 3.2)...
#        define CC_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS 1
#    endif

// Borland C++
#elif defined(__BORLANDC__)
#    define CC_COMPILER_BORLANDC 1
#    define CC_COMPILER_VERSION  __BORLANDC__
#    define CC_COMPILER_NAME     "Borland C"
       // #define CC_COMPILER_STRING (defined below)

#    if (__BORLANDC__ <= 0x0550)  // If Borland C++ Builder 4 and 5...
#        define CC_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS 1
#    endif
#    if (__BORLANDC__ >= 0x561) && (__BORLANDC__ < 0x600)
#        define CC_COMPILER_NO_MEMBER_FUNCTION_SPECIALIZATION 1
#    endif

// Intel C++
// The Intel Windows compiler masquerades as VC++ and defines _MSC_VER.
// The Intel compiler is based on the EDG compiler front-end.
#elif defined(__ICL) || defined(__ICC)
#    define CC_COMPILER_INTEL 1

// Should we enable the following? We probably should do so since enabling it does a lot more good than harm
// for users. The Intel Windows compiler does a pretty good job of emulating VC++ and so the user would likely
// have to handle few special cases where the Intel compiler doesn't emulate VC++ correctly.
#    if defined(_MSC_VER)
#        define CC_COMPILER_MSVC      1
#        define CC_COMPILER_MICROSOFT 1
#    endif

// Should we enable the following? This isn't as clear because as of this writing we don't know if the Intel
// compiler truly emulates GCC well enough that enabling this does more good than harm.
#    if defined(__GNUC__)
#        define CC_COMPILER_GNUC 1
#    endif

#    if defined(__ICL)
#        define CC_COMPILER_VERSION __ICL
#    elif defined(__ICC)
#        define CC_COMPILER_VERSION __ICC
#    endif
#    define CC_COMPILER_NAME "Intel C++"
#    if defined(_MSC_VER)
#        define CC_COMPILER_STRING CC_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE(CC_COMPILER_VERSION) ", EDG version " INTERNAL_STRINGIZE(__EDG_VERSION__) ", VC++ version " INTERNAL_STRINGIZE(_MSC_VER)
#    elif defined(__GNUC__)
#        define CC_COMPILER_STRING CC_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE(CC_COMPILER_VERSION) ", EDG version " INTERNAL_STRINGIZE(__EDG_VERSION__) ", GCC version " INTERNAL_STRINGIZE(__GNUC__)
#    else
#        define CC_COMPILER_STRING CC_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE(CC_COMPILER_VERSION) ", EDG version " INTERNAL_STRINGIZE(__EDG_VERSION__)
#    endif

#elif defined(_MSC_VER)
#    define CC_COMPILER_MSVC      1
#    define CC_COMPILER_MICROSOFT 1
#    define CC_COMPILER_VERSION   _MSC_VER
#    define CC_COMPILER_NAME      "Microsoft Visual C++"
       // #define CC_COMPILER_STRING (defined below)

#    if defined(__clang__)
       // Clang's MSVC-compatible driver.
#        define CC_COMPILER_CLANG_CL 1
#    endif

#    define CC_STANDARD_LIBRARY_MSVC      1
#    define CC_STANDARD_LIBRARY_MICROSOFT 1

#    if (_MSC_VER <= 1200)  // If VC6.x and earlier...
#        if (_MSC_VER < 1200)
#            define CC_COMPILER_MSVCOLD 1
#        else
#            define CC_COMPILER_MSVC6 1
#        endif

#        if (_MSC_VER < 1200)  // If VC5.x or earlier...
#            define CC_COMPILER_NO_TEMPLATE_SPECIALIZATION 1
#        endif
#        define CC_COMPILER_NO_EXPLICIT_FUNCTION_TEMPLATE_ARGUMENTS 1  // The compiler compiles this OK, but executes it wrong. Fixed in VC7.0
#        define CC_COMPILER_NO_VOID_RETURNS                         1  // The compiler fails to compile such cases. Fixed in VC7.0
#        define CC_COMPILER_NO_EXCEPTION_STD_NAMESPACE              1  // The compiler fails to compile such cases. Fixed in VC7.0
#        define CC_COMPILER_NO_DEDUCED_TYPENAME                     1  // The compiler fails to compile such cases. Fixed in VC7.0
#        define CC_COMPILER_NO_STATIC_CONSTANTS                     1  // The compiler fails to compile such cases. Fixed in VC7.0
#        define CC_COMPILER_NO_COVARIANT_RETURN_TYPE                1  // The compiler fails to compile such cases. Fixed in VC7.1
#        define CC_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP            1  // The compiler compiles this OK, but executes it wrong. Fixed in VC7.1
#        define CC_COMPILER_NO_TEMPLATE_TEMPLATES                   1  // The compiler fails to compile such cases. Fixed in VC7.1
#        define CC_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION      1  // The compiler fails to compile such cases. Fixed in VC7.1
#        define CC_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS              1  // The compiler fails to compile such cases. Fixed in VC7.1
       // #define CC_COMPILER_NO_MEMBER_TEMPLATES 1                       // VC6.x supports member templates properly 95% of the time. So do we flag the remaining 5%?
       // #define CC_COMPILER_NO_MEMBER_TEMPLATE_SPECIALIZATION 1         // VC6.x supports member templates properly 95% of the time. So do we flag the remaining 5%?

#    elif (_MSC_VER <= 1300)  // If VC7.0 and earlier...
#        define CC_COMPILER_MSVC7 1

#        define CC_COMPILER_NO_COVARIANT_RETURN_TYPE           1  // The compiler fails to compile such cases. Fixed in VC7.1
#        define CC_COMPILER_NO_ARGUMENT_DEPENDENT_LOOKUP       1  // The compiler compiles this OK, but executes it wrong. Fixed in VC7.1
#        define CC_COMPILER_NO_TEMPLATE_TEMPLATES              1  // The compiler fails to compile such cases. Fixed in VC7.1
#        define CC_COMPILER_NO_TEMPLATE_PARTIAL_SPECIALIZATION 1  // The compiler fails to compile such cases. Fixed in VC7.1
#        define CC_COMPILER_NO_MEMBER_TEMPLATE_FRIENDS         1  // The compiler fails to compile such cases. Fixed in VC7.1
#        define CC_COMPILER_NO_MEMBER_FUNCTION_SPECIALIZATION  1  // This is the case only for VC7.0 and not VC6 or VC7.1+. Fixed in VC7.1
       // #define CC_COMPILER_NO_MEMBER_TEMPLATES 1                       // VC7.0 supports member templates properly 95% of the time. So do we flag the remaining 5%?

#    elif (_MSC_VER < 1400)  // VS2003       _MSC_VER of 1300 means VC7 (VS2003)
       // The VC7.1 and later compiler is fairly close to the C++ standard
// and thus has no compiler limitations that we are concerned about.
#        define CC_COMPILER_MSVC
#        define CC_COMPILER_MSVC_VERSION 2003
#        define CC_COMPILER_MSVC7_2003   1
#        define CC_COMPILER_MSVC7_1      1

#    elif (_MSC_VER < 1500)  // VS2005       _MSC_VER of 1400 means VC8 (VS2005)
#        define CC_COMPILER_MSVC
#        define CC_COMPILER_MSVC_VERSION 2005
#        define CC_COMPILER_MSVC8_2005   1
#        define CC_COMPILER_MSVC8_0      1

#    elif (_MSC_VER < 1600)  // VS2008.      _MSC_VER of 1500 means VC9 (VS2008)
#        define CC_COMPILER_MSVC
#        define CC_COMPILER_MSVC_VERSION 2008
#        define CC_COMPILER_MSVC9_2008   1
#        define CC_COMPILER_MSVC9_0      1

#    elif (_MSC_VER < 1700)  // VS2010       _MSC_VER of 1600 means VC10 (VS2010)
#        define CC_COMPILER_MSVC
#        define CC_COMPILER_MSVC_VERSION 2010
#        define CC_COMPILER_MSVC_2010    1
#        define CC_COMPILER_MSVC10_0     1

#    elif (_MSC_VER < 1800)  // VS2012       _MSC_VER of 1700 means VS2011/VS2012
#        define CC_COMPILER_MSVC
#        define CC_COMPILER_MSVC_VERSION 2012
#        define CC_COMPILER_MSVC_2011    1  // Microsoft changed the name to VS2012 before shipping, despite referring to it as VS2011 up to just a few weeks before shipping.
#        define CC_COMPILER_MSVC11_0     1
#        define CC_COMPILER_MSVC_2012    1
#        define CC_COMPILER_MSVC12_0     1

#    elif (_MSC_VER < 1900)  // VS2013       _MSC_VER of 1800 means VS2013
#        define CC_COMPILER_MSVC
#        define CC_COMPILER_MSVC_VERSION 2013
#        define CC_COMPILER_MSVC_2013    1
#        define CC_COMPILER_MSVC13_0     1

#    elif (_MSC_VER < 1911)  // VS2015       _MSC_VER of 1900 means VS2015
#        define CC_COMPILER_MSVC
#        define CC_COMPILER_MSVC_VERSION 2015
#        define CC_COMPILER_MSVC_2015    1
#        define CC_COMPILER_MSVC14_0     1

#    elif (_MSC_VER < 1920)  // VS2017       _MSC_VER of 1910 means VS2017
#        define CC_COMPILER_MSVC
#        define CC_COMPILER_MSVC_VERSION 2017
#        define CC_COMPILER_MSVC_2017    1
#        define CC_COMPILER_MSVC15_0     1
#    elif (_MSC_VER < 1930)  // VS2019       _MSC_VER of 1920 means VS2019
#        define CC_COMPILER_MSVC
#        define CC_COMPILER_MSVC_VERSION 2019
#        define CC_COMPILER_MSVC_2019    1
#        define CC_COMPILER_MSVC16_0     1
#    elif (_MSC_VER < 1940)  // VS2022       _MSC_VER of 1930 means VS2022
#        define CC_COMPILER_MSVC
#        define CC_COMPILER_MSVC_VERSION 2022
#        define CC_COMPILER_MSVC_2022    1
#        define CC_COMPILER_MSVC17_0     1
#    else
#        define CC_COMPILER_MSVC
#        define CC_COMPILER_MSVC_VERSION 2022
#        define CC_COMPILER_MSVC_2022    1
#        define CC_COMPILER_MSVC17_0     1
#    endif
// IBM
#elif defined(__xlC__)
#    define CC_COMPILER_IBM     1
#    define CC_COMPILER_NAME    "IBM XL C"
#    define CC_COMPILER_VERSION __xlC__
#    define CC_COMPILER_STRING  "IBM XL C compiler, version " INTERNAL_STRINGIZE(__xlC__)

// Unknown
#else  // Else the compiler is unknown

#    define CC_COMPILER_VERSION 0
#    define CC_COMPILER_NAME    "Unknown"

#endif

#ifndef CC_COMPILER_STRING
#    define CC_COMPILER_STRING CC_COMPILER_NAME " compiler, version " INTERNAL_STRINGIZE(CC_COMPILER_VERSION)
#endif

// CC_COMPILER_NO_RTTI
//
// If CC_COMPILER_NO_RTTI is defined, then RTTI (run-time type information)
// is not available (possibly due to being disabled by the user).
//
#if defined(__EDG_VERSION__) && !defined(__RTTI)
#    define CC_COMPILER_NO_RTTI 1
#elif defined(__clang__) && !CC_COMPILER_HAS_FEATURE(cxx_rtti)
#    define CC_COMPILER_NO_RTTI 1
#elif defined(__IBMCPP__) && !defined(__RTTI_ALL__)
#    define CC_COMPILER_NO_RTTI 1
#elif defined(__GXX_ABI_VERSION) && !defined(__GXX_RTTI)
#    define CC_COMPILER_NO_RTTI 1
#elif defined(_MSC_VER) && !defined(_CPPRTTI)
#    define CC_COMPILER_NO_RTTI 1
#elif defined(__ARMCC_VERSION) && defined(__TARGET_CPU_MPCORE) && !defined(__RTTI)
#    define CC_COMPILER_NO_RTTI 1
#endif

// CC_COMPILER_NO_EXCEPTIONS / CC_COMPILER_NO_UNWIND
//
// If CC_COMPILER_NO_EXCEPTIONS is defined, then the compiler is
// configured to not recognize C++ exception-handling statements
// such as try/catch/throw. Thus, when CC_COMPILER_NO_EXCEPTIONS is
// defined, code that attempts to use exception handling statements
// will usually cause a compilation error. If is often desirable
// for projects to disable exception handling because exception
// handling causes extra code and/or data generation which might
// not be needed, especially if it is known that exceptions won't
// be happening. When writing code that is to be portable between
// systems of which some enable exception handling while others
// don't, check for CC_COMPILER_NO_EXCEPTIONS being defined.
//
#if !defined(CC_COMPILER_NO_EXCEPTIONS) && !defined(CC_COMPILER_NO_UNWIND)
#    if defined(CC_COMPILER_GNUC) && defined(_NO_EX)  // GCC on some platforms defines _NO_EX when exceptions are disabled.
#        define CC_COMPILER_NO_EXCEPTIONS 1

#    elif (defined(CC_COMPILER_CLANG) || defined(CC_COMPILER_GNUC) || defined(CC_COMPILER_INTEL) || defined(CC_COMPILER_RVCT)) && !defined(__EXCEPTIONS)  // GCC and most EDG-based compilers define __EXCEPTIONS when exception handling is enabled.
#        define CC_COMPILER_NO_EXCEPTIONS 1

#    elif (defined(CC_COMPILER_MSVC)) && !defined(_CPPUNWIND)
#        define CC_COMPILER_NO_UNWIND 1

#    endif  // CC_COMPILER_NO_EXCEPTIONS / CC_COMPILER_NO_UNWIND
#endif      // !defined(CC_COMPILER_NO_EXCEPTIONS) && !defined(CC_COMPILER_NO_UNWIND)

// ------------------------------------------------------------------------
// CC_DISABLE_ALL_VC_WARNINGS / CC_RESTORE_ALL_VC_WARNINGS
//
// Disable and re-enable all warning(s) within code.
//
// Example usage:
//     CC_DISABLE_ALL_VC_WARNINGS()
//     <code>
//     CC_RESTORE_ALL_VC_WARNINGS()
//
// This is duplicated from EABase's eacompilertraits.h
#ifndef CC_DISABLE_ALL_VC_WARNINGS
#    if defined(_MSC_VER)
#        define CC_DISABLE_ALL_VC_WARNINGS() __pragma(warning(push, 0)) __pragma(warning(disable : 4244 4265 4267 4350 4472 4509 4548 4623 4710 4985 6320 4755 4625 4626 4702))  // Some warnings need to be explicitly called out.
#    else
#        define CC_DISABLE_ALL_VC_WARNINGS()
#    endif
#endif

// This is duplicated from EABase's eacompilertraits.h
#ifndef CC_RESTORE_ALL_VC_WARNINGS
#    if defined(_MSC_VER)
#        define CC_RESTORE_ALL_VC_WARNINGS() __pragma(warning(pop))
#    else
#        define CC_RESTORE_ALL_VC_WARNINGS()
#    endif
#endif

// CC_COMPILER_NO_ALIGNED_NEW
//
//
#if !defined(CC_COMPILER_NO_ALIGNED_NEW)
#    if defined(_HAS_ALIGNED_NEW) && _HAS_ALIGNED_NEW  // VS2017 15.5 Preview
                                                       // supported.
#    elif defined(CC_COMPILER_CPP17_ENABLED)
       // supported.
#    else
#        define CC_COMPILER_NO_ALIGNED_NEW 1
#    endif
#endif

// CC_COMPILER_NO_STANDARD_CPP_LIBRARY
//
// If defined, then the compiler doesn't provide a Standard C++ library.
//
#if defined(CC_PLATFORM_ANDROID)
    // Disabled because EA's eaconfig/android_config/android_sdk packages currently
    // don't support linking STL libraries. Perhaps we can figure out what linker arguments
    // are needed for an app so we can manually specify them and then re-enable this code.
    // #include <android/api-level.h>
    //
    // #if (__ANDROID_API__ < 9) // Earlier versions of Android provide no std C++ STL implementation.
#    define CC_COMPILER_NO_STANDARD_CPP_LIBRARY 1
    // #endif
#endif

// CC_COMPILER_NO_STATIC_VARIABLE_INIT
//
// If defined, it means that global or static C++ variables will be
// constructed. Not all compiler/platorm combinations support this.
// User code that needs to be portable must avoid having C++ variables
// that construct before main.
//
// #if defined(CC_PLATFORM_MOBILE)
//    #define CC_COMPILER_NO_STATIC_VARIABLE_INIT 1
// #endif

// CC_COMPILER_NO_STATIC_FUNCTION_INIT
//
// If defined, it means that functions marked as startup functions
// (e.g. __attribute__((constructor)) in GCC) are supported. It may
// be that some compiler/platform combinations don't support this.
//
// #if defined(XXX) // So far, all compiler/platforms we use support this.
//    #define CC_COMPILER_NO_STATIC_VARIABLE_INIT 1
// #endif

// CC_COMPILER_NO_VARIADIC_MACROS
//
// If defined, the compiler doesn't support C99/C++11 variadic macros.
// With a variadic macro, you can do this:
//     #define MY_PRINTF(format, ...) printf(format, __VA_ARGS__)
//
#if !defined(CC_COMPILER_NO_VARIADIC_MACROS)
#    if defined(_MSC_VER) && (_MSC_VER < 1500)  // If earlier than VS2008..
#        define CC_COMPILER_NO_VARIADIC_MACROS 1
#    elif defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__)) < 401  // If earlier than GCC 4.1..
#        define CC_COMPILER_NO_VARIADIC_MACROS 1
#    elif defined(CC_COMPILER_EDG)  // Includes other compilers
       // variadic macros are supported
#    endif
#endif

// CC_COMPILER_NO_RVALUE_REFERENCES
//
// If defined, the compiler doesn't fully support C++11 rvalue reference semantics.
// This applies to the compiler only and not the Standard Library in use with the compiler,
// which is required by the Standard to have some support itself.
//
#if !defined(CC_COMPILER_NO_RVALUE_REFERENCES)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (_MSC_VER >= 1600)                 // VS2010+
                                                                                                      // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403)  // EDG 4.3+.
       // supported. Earlier EDG supported a subset of rvalue references. Implicit move constructors and assignment operators aren't supported until EDG 4.5.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && CC_COMPILER_HAS_FEATURE(cxx_rvalue_references)
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4005)  // GCC 4.5+
       // supported.
#    else
#        define CC_COMPILER_NO_RVALUE_REFERENCES 1
#    endif
#endif

// CC_COMPILER_NO_EXTERN_TEMPLATE
//
// If defined, the compiler doesn't support C++11 extern template.
// With extern templates, you can do this:
//     extern template void DoSomething(KnownType u);
//
#if !defined(CC_COMPILER_NO_EXTERN_TEMPLATE)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (_MSC_VER >= 1700)                 // VS2012+...
                                                                                                      // Extern template is supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)  // EDG 4.1+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && defined(__apple_build_version__) && (CC_COMPILER_VERSION >= 401)
       // Extern template is supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && !defined(__apple_build_version__)  // Clang other than Apple's Clang
       // Extern template is supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4006)       // GCC 4.6+
       // Extern template is supported.
#    else
#        define CC_COMPILER_NO_EXTERN_TEMPLATE 1
#    endif
#endif

// CC_COMPILER_NO_CONSTEXPR
//
// Refers to C++11 = constexpr (const expression) declarations.
//
#if !defined(CC_COMPILER_NO_CONSTEXPR)
#    if defined(CC_COMPILER_CPP11_ENABLED) && (defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1900))    // VS2015+... Not present in VC++ up to and including VS2013.
                                                                                                      // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 406)  // EDG 4.6+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && CC_COMPILER_HAS_FEATURE(cxx_constexpr)
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4006)  // GCC 4.6+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1900)  // VS 2015+
       // supported.
#    else
#        define CC_COMPILER_NO_CONSTEXPR 1
#    endif
#endif

// CC_COMPILER_NO_CONSTEXPR_IF
//
// Refers to C++17 = constexpr if(const expression) conditionals.
//
#if !defined(CC_COMPILER_NO_CONSTEXPR_IF)
#    if defined(CC_COMPILER_CPP17_ENABLED) && (defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1911))  // VS2017 15.3+
                                                                                                    // supported.
#    elif defined(CC_COMPILER_CPP17_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 309)  // Clang 3.9+
       // supported.
#    elif defined(CC_COMPILER_CPP17_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 7000)  // GCC 7+
       // supported.
#    else
#        define CC_COMPILER_NO_CONSTEXPR_IF 1
#    endif
#endif

// CC_COMPILER_NO_OVERRIDE
//
// Refers to the C++11 override specifier.
//
#ifndef CC_COMPILER_NO_OVERRIDE
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION > 1600)     // VC++ > VS2010, even without C++11 support. VS2010 does support override, however will generate warnings due to the keyword being 'non-standard'
                                                                                                    // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)  // Clang 2.9+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4007)  // GCC 4.7+
       // supported.
#    else
#        define CC_COMPILER_NO_OVERRIDE 1
#    endif
#endif

// CC_COMPILER_NO_INHERITANCE_FINAL
//
// Refers to the C++11 final specifier.
//
#ifndef CC_COMPILER_NO_INHERITANCE_FINAL
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1500)    // VS2008+, even without C++11 support.
                                                                                                    // supported, though you need to use CC_INHERITANCE_FINAL for it to work with VS versions prior to 2012.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)  // Clang 2.9+
       // supported
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4007)  // GCC 4.7+
       // supported
#    else
#        define CC_COMPILER_NO_INHERITANCE_FINAL 1
#    endif
#endif

// CC_COMPILER_NO_AUTO
//
// Refers to C++11 auto.
//
#if !defined(CC_COMPILER_NO_AUTO)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1600)      // VS2010+
                                                                                                      // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)  // EDG 4.1+.
       // supported with the exception of the usage of braced initializer lists as of EDG 4.3.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)    // Clang 2.9+, including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4004)    // GCC 4.4+
       // supported.
#    else
#        define CC_COMPILER_NO_AUTO 1
#    endif
#endif

// CC_COMPILER_NO_NULLPTR
//
// Refers to C++11 nullptr (which is a built in type). std::nullptr_t is defined in C++11 <cstddef>.
// Note that <EABase/nullptr.h> implements a portable nullptr implementation.
//
#if !defined(CC_COMPILER_NO_NULLPTR)
#    if (defined(_MSC_VER) && (_MSC_VER >= 1600)) && defined(CC_COMPILER_CPP11_ENABLED)
    // supported
#    elif defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4006) && defined(CC_COMPILER_CPP11_ENABLED)
       // supported
#    elif defined(__clang__) && defined(CC_COMPILER_CPP11_ENABLED)
       // supported
#    elif defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403) && defined(CC_COMPILER_CPP11_ENABLED)
       // supported
#    else
#        define CC_COMPILER_NO_NULLPTR 1
#    endif
#endif

// CC_COMPILER_NO_DECLTYPE
//
// Refers to C++11 decltype.
//
#if !defined(CC_COMPILER_NO_DECLTYPE)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1600)      // VS2010+
                                                                                                      // supported, though VS2010 doesn't support the spec completely as specified in the final standard.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)  // EDG 4.1+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)    // Clang 2.9+, including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4003)    // GCC 4.3+
       // supported.
#    else
#        define CC_COMPILER_NO_DECLTYPE 1
#    endif
#endif

// CC_COMPILER_NO_DEFAULTED_FUNCTIONS
// CC_COMPILER_NO_DELETED_FUNCTIONS
//
// Refers to C++11 = default and = delete function declarations.
//
#if !defined(CC_COMPILER_NO_DEFAULTED_FUNCTIONS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1800)      // VS2013+
                                                                                                      // supported, but as of VS2013 it isn't supported for defaulted move constructors and move assignment operators.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)  // EDG 4.1+.
       // supported, but as of EDG 4.3 it isn't supported for defaulted move constructors and move assignment operators until EDG 4.5.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 300)    // Clang 3.0+, including Apple's Clang
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4004)    // GCC 4.4+
       // supported.
#    else
       // VC++ doesn't support it as of VS2012.
#        define CC_COMPILER_NO_DEFAULTED_FUNCTIONS 1
#    endif
#endif

#if !defined(CC_COMPILER_NO_DELETED_FUNCTIONS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1800)      // VS2013+
                                                                                                      // supported, but as of VS2013 it isn't supported for defaulted move constructors and move assignment operators.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)  // EDG 4.1+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)    // Clang 2.9+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4004)    // GCC 4.4+
       // supported.
#    else
       // VC++ doesn't support it as of VS2012.
#        define CC_COMPILER_NO_DELETED_FUNCTIONS 1
#    endif
#endif

// CC_COMPILER_NO_LAMBDA_EXPRESSIONS
//
// Refers to C++11 lambda expressions.
//
#if !defined(CC_COMPILER_NO_LAMBDA_EXPRESSIONS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1600)      // VS2010+
                                                                                                      // supported, though VS2010 doesn't support the spec completely as specified in the final standard.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)  // EDG 4.1+.
       // supported. However, converting lambdas to function pointers is not supported until EDG 4.5.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 301) && !defined(__apple_build_version__)  // Clang 3.1+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4004)                                       // GCC 4.4+
       // supported.
#    else
#        define CC_COMPILER_NO_LAMBDA_EXPRESSIONS 1
#    endif
#endif

// CC_COMPILER_NO_TRAILING_RETURN_TYPES
//
// Refers to C++11 trailing-return-type. Also sometimes referred to as "incomplete return type".
//
#if !defined(CC_COMPILER_NO_TRAILING_RETURN_TYPES)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1600)      // VS2010+
                                                                                                      // supported, though VS2010 doesn't support the spec completely as specified in the final standard.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402)  // EDG 4.2+.
       // supported. However, use of "this" in trailing return types is not supported untiil EDG 4.4
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 301) && !defined(__apple_build_version__)  // Clang 3.1+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4004)                                       // GCC 4.4+
       // supported.
#    else
#        define CC_COMPILER_NO_TRAILING_RETURN_TYPES 1
#    endif
#endif

// CC_COMPILER_NO_STRONGLY_TYPED_ENUMS
//
// Refers to C++11 strongly typed enums, which includes enum classes and sized enums. Doesn't include forward-declared enums.
//
#if !defined(CC_COMPILER_NO_STRONGLY_TYPED_ENUMS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1700)      // VS2012+
                                                                                                      // supported. A subset of this is actually supported by VS2010.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 400)  // EDG 4.0+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)    // Clang 2.9+, including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4004)    // GCC 4.4+
       // supported.
#    else
#        define CC_COMPILER_NO_STRONGLY_TYPED_ENUMS 1
#    endif
#endif

// CC_COMPILER_NO_FORWARD_DECLARED_ENUMS
//
// Refers to C++11 forward declared enums.
//
#if !defined(CC_COMPILER_NO_FORWARD_DECLARED_ENUMS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1700)      // VS2012+
                                                                                                      // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)  // EDG 4.5+.
       // supported. EDG 4.3 supports basic forward-declared enums, but not forward-declared strongly typed enums.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 301) && !defined(__apple_build_version__)  // Clang 3.1+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4006)                                       // GCC 4.6+
       // supported.
#    else
#        define CC_COMPILER_NO_FORWARD_DECLARED_ENUMS 1
#    endif
#endif

// CC_COMPILER_NO_VARIADIC_TEMPLATES
//
// Refers to C++11 variadic templates.
//
#if !defined(CC_COMPILER_NO_VARIADIC_TEMPLATES)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1800)      // VS2013+.
                                                                                                      // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (_MSC_FULL_VER == 170051025)     // VS2012 November Preview for Windows only.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403)  // EDG 4.3+.
       // supported, though 4.1 has partial support for variadic templates.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)    // Clang 2.9+, including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4004)    // GCC 4.4+
       // supported, though GCC 4.3 has partial support for variadic templates.
#    else
#        define CC_COMPILER_NO_VARIADIC_TEMPLATES 1
#    endif
#endif

// CC_COMPILER_NO_TEMPLATE_ALIASES
//
// Refers to C++11 alias templates.
// Example alias template usage:
//     template <typename T>
//     using Dictionary = std::map<int, T>;
//
//     Dictionary<int> StringIntDictionary;
//
#if !defined(CC_COMPILER_NO_TEMPLATE_ALIASES)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1800)  // VS2013+.
                                                                                                  // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402)                                     // EDG 4.2+.
       // supported, though 4.1 has partial support for variadic templates.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 300) && !defined(__apple_build_version__)  // Clang 3.0+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4007)                                       // GCC 4.7+
       // supported, though GCC 4.3 has partial support for variadic templates.
#    else
#        define CC_COMPILER_NO_TEMPLATE_ALIASES 1
#    endif
#endif

// CC_COMPILER_NO_VARIABLE_TEMPLATES
//
// Refers to C++14 variable templates.
// Example variable template usage:
//     template<class T>
//     constexpr T pi = T(3.1415926535897932385);
//
#if !defined(CC_COMPILER_NO_VARIABLE_TEMPLATES)
#    if defined(_MSC_VER) && (_MSC_FULL_VER >= 190023918)                                                                                // VS2015 Update 2 and above.
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP14_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 304) && !defined(__apple_build_version__)  // Clang 3.4+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP14_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 5000)                                       // GCC 5+
       // supported.
#    elif !defined(CC_COMPILER_CPP14_ENABLED)
#        define CC_COMPILER_NO_VARIABLE_TEMPLATES 1
#    endif
#endif

// CC_COMPILER_NO_INLINE_VARIABLES
//
// Refers to C++17 inline variables that allows the definition of variables in header files
//
// Example usage:
//    struct Foo
//    {
//        static inline constexpr int kConstant = 42;  // no out of class definition
//    };
//
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2015/n4424.pdf
// http://en.cppreference.com/w/cpp/language/inline
//
#if !defined(CC_COMPILER_NO_INLINE_VARIABLES)
#    define CC_COMPILER_NO_INLINE_VARIABLES 1
#endif

// CC_COMPILER_NO_INITIALIZER_LISTS
//
// Refers to C++11 initializer lists.
// This refers to the compiler support for this and not the Standard Library support (std::initializer_list).
//
#if !defined(CC_COMPILER_NO_INITIALIZER_LISTS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1800)      // VS2013+.
                                                                                                      // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (_MSC_FULL_VER == 170051025)     // VS2012 November Preview for Windows only.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)  // EDG 4.5+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 301) && !defined(__apple_build_version__)  // Clang 3.1+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4004)                                       // GCC 4.4+
       // supported, though GCC 4.3 has partial support for it.
#    else
#        define CC_COMPILER_NO_INITIALIZER_LISTS 1
#    endif
#endif

// CC_COMPILER_NO_NORETURN
//
// Refers to C++11 declaration attribute: noreturn.
// http://en.cppreference.com/w/cpp/language/attributes
// http://blog.aaronballman.com/2011/09/understanding-attributes/
//
#if !defined(CC_COMPILER_NO_NORETURN)
#    if defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION >= 1300)                                    // VS2003+
                                                                                                      // supported via __declspec(noreturn). You need to use that or CC_NORETURN. VC++ up to VS2013 doesn't support any C++11 attribute types.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402)  // EDG 4.2+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 300) && !defined(__apple_build_version__)  // Clang 3.0+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4008)                                       // GCC 4.8+
       // supported.
#    else
#        define CC_COMPILER_NO_NORETURN 1
#    endif
#endif

// CC_COMPILER_NO_CARRIES_DEPENDENCY
//
// Refers to C++11 declaration attribute: carries_dependency.
// http://en.cppreference.com/w/cpp/language/attributes
// http://blog.aaronballman.com/2011/09/understanding-attributes/
//
#if !defined(CC_COMPILER_NO_CARRIES_DEPENDENCY)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)     // Apple clang 4.1+
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402)                                     // EDG 4.2+.
       // supported; stricter than other compilers in its usage.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 300) && !defined(__apple_build_version__)  // Clang 3.0+, not including Apple's Clang.
       // supported.
// Currently GNUC doesn't appear to support this attribute.
// #elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4008)                        // GCC 4.8+
//    // supported.
#    else
#        define CC_COMPILER_NO_CARRIES_DEPENDENCY 1
#    endif
#endif

// CC_COMPILER_NO_FALLTHROUGH
//
// Refers to C++17 declaration attribute: fallthrough.
// http://en.cppreference.com/w/cpp/language/attributes
//
#if !defined(CC_COMPILER_NO_FALLTHROUGH)
#    if defined(CC_COMPILER_CPP17_ENABLED)
    // supported.
#    else
#        define CC_COMPILER_NO_FALLTHROUGH 1
#    endif
#endif

// CC_COMPILER_NO_NODISCARD
//
// Refers to C++17 declaration attribute: nodiscard.
// http://en.cppreference.com/w/cpp/language/attributes
//
#if !defined(CC_COMPILER_NO_NODISCARD)
#    if defined(CC_COMPILER_CPP17_ENABLED)
    // supported.
#    else
#        define CC_COMPILER_NO_NODISCARD 1
#    endif
#endif

// CC_COMPILER_NO_MAYBE_UNUSED
//
// Refers to C++17 declaration attribute: maybe_unused.
// http://en.cppreference.com/w/cpp/language/attributes
//
#if !defined(CC_COMPILER_NO_MAYBE_UNUSED)
#    if defined(CC_COMPILER_CPP17_ENABLED)
    // supported.
#    elif defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION >= 1912)  // VS2017 15.3+
       // supported.
#    else
#        define CC_COMPILER_NO_MAYBE_UNUSED 1
#    endif
#endif

// CC_COMPILER_NO_STRUCTURED_BINDING
//
// Indicates if target compiler supports the C++17 "structured binding" language feature.
// https://en.cppreference.com/w/cpp/language/structured_binding
//
//
#if !defined(CC_COMPILER_NO_STRUCTURED_BINDING)
#    if defined(CC_COMPILER_CPP17_ENABLED)
    // supported.
#    elif defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION >= 1912)  // VS2017 15.3+
       // supported.
#    else
#        define CC_COMPILER_NO_STRUCTURED_BINDING 1
#    endif
#endif

// CC_COMPILER_NO_DESIGNATED_INITIALIZERS
//
// Indicates the target compiler supports the C++20 "designated initializer" language feature.
// https://en.cppreference.com/w/cpp/language/aggregate_initialization
//
// Example:
//   struct A { int x; int y; };
//   A a = { .y = 42, .x = 1 };
//
#if !defined(CC_COMPILER_NO_DESIGNATED_INITIALIZERS)
#    if defined(CC_COMPILER_CPP20_ENABLED)
    // supported.
#    else
#        define CC_COMPILER_NO_DESIGNATED_INITIALIZERS 1
#    endif
#endif

// CC_COMPILER_NO_NONSTATIC_MEMBER_INITIALIZERS
//
// Refers to C++11 declaration attribute: carries_dependency.
// http://www.open-std.org/JTC1/SC22/WG21/docs/papers/2008/n2756.htm
//
#if !defined(CC_COMPILER_NO_NONSTATIC_MEMBER_INITIALIZERS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1800)                                         // VS2013+.
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)   // Apple clang 4.1+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 300) && !defined(__apple_build_version__)  // Clang 3.0+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4007)                                       // GCC 4.7+
       // supported.
#    else
#        define CC_COMPILER_NO_NONSTATIC_MEMBER_INITIALIZERS 1
#    endif
#endif

// CC_COMPILER_NO_RIGHT_ANGLE_BRACKETS
//
// Defines if the compiler supports >> (as opposed to > >) in template
// declarations such as typedef eastl::list<eastl::list<int>> ListList;
//
#if !defined(CC_COMPILER_NO_RIGHT_ANGLE_BRACKETS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1600)      // VS2010+
                                                                                                      // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)  // EDG 4.1+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)    // Clang 2.9+, including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4003)    // GCC 4.3+
       // supported.
#    else
#        define CC_COMPILER_NO_RIGHT_ANGLE_BRACKETS 1
#    endif
#endif

// CC_COMPILER_NO_ALIGNOF
//
// Refers specifically to C++11 alignof and not old compiler extensions such as __alignof__().
// However, EABase provides a portable CC_ALIGN_OF which works for all compilers.
//
#if !defined(CC_COMPILER_NO_ALIGNOF)
// Not supported by VC++ as of VS2013, though CC_ALIGN_OF is supported on all coompilers as an alternative.
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)    // Clang 2.9+, including Apple's Clang.
                                                                                                    // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4005)  // GCC 4.5+
       // supported.
#    else
#        define CC_COMPILER_NO_ALIGNOF 1
#    endif
#endif

// CC_COMPILER_NO_ALIGNAS
//
// Refers to C++11 alignas.
//
#if !defined(CC_COMPILER_NO_ALIGNAS)
// Not supported by VC++ as of VS2013.
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)     // Apple clang 4.1+
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 300) && !defined(__apple_build_version__)  // Clang 3.0+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4008)                                       // GCC 4.8+
       // supported.
#    else
#        define CC_COMPILER_NO_ALIGNAS 1
#    endif
#endif

// CC_COMPILER_NO_DELEGATING_CONSTRUCTORS
//
// Refers to C++11 constructor delegation.
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2006/n1986.pdf
// https://www.ibm.com/developerworks/mydeveloperworks/blogs/5894415f-be62-4bc0-81c5-3956e82276f3/entry/c_0x_delegating_constructors
//
#if !defined(CC_COMPILER_NO_DELEGATING_CONSTRUCTORS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1800)                                         // VS2013+.
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 407)                                     // EDG 4.7+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)   // Apple clang 4.1+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 300) && !defined(__apple_build_version__)  // Clang 3.0+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4007)                                       // GCC 4.7+
       // supported.
#    else
#        define CC_COMPILER_NO_DELEGATING_CONSTRUCTORS 1
#    endif
#endif

// CC_COMPILER_NO_INHERITING_CONSTRUCTORS
//
// Refers to C++11 constructor inheritance via 'using'.
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2540.htm
//
#if !defined(CC_COMPILER_NO_INHERITING_CONSTRUCTORS)
// Not supported by VC++ as of VS2013.
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && CC_COMPILER_HAS_FEATURE(cxx_inheriting_constructors)  // Clang
                                                                                                                          // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4008)                        // GCC 4.8+
       // supported.
#    else
#        define CC_COMPILER_NO_INHERITING_CONSTRUCTORS 1
#    endif
#endif

// CC_COMPILER_NO_USER_DEFINED_LITERALS
//
// http://en.cppreference.com/w/cpp/language/user_literal
// http://stackoverflow.com/questions/237804/what-new-capabilities-do-user-defined-literals-add-to-c
//
#if !defined(CC_COMPILER_NO_USER_DEFINED_LITERALS)
// Not supported by VC++ as of VS2013.
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)     // Apple clang 4.1+
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 301) && !defined(__apple_build_version__)  // Clang 3.1+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4007)                                       // GCC 4.7+
       // supported.
#    else
#        define CC_COMPILER_NO_USER_DEFINED_LITERALS 1
#    endif
#endif

// CC_COMPILER_NO_STANDARD_LAYOUT_TYPES
//     a.k.a. POD relaxation
//     http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2342.htm
//
#if !defined(CC_COMPILER_NO_STANDARD_LAYOUT_TYPES)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1700)                                         // VS2012+
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)   // Apple clang 4.1+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 300) && !defined(__apple_build_version__)  // Clang 3.0+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4005)                                       // GCC 4.5+
       // supported.
#    else
#        define CC_COMPILER_NO_STANDARD_LAYOUT_TYPES 1
#    endif
#endif

// CC_COMPILER_NO_EXTENDED_SIZEOF
//
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2253.html
// Allows you to do this: sizeof(SomeClass::mSomeMember)
//
#if !defined(CC_COMPILER_NO_EXTENDED_SIZEOF)
// Not supported by VC++ as of VS2013.
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)  // Apple clang 4.1+
                                                                                                                                      // supported.
// Versions of EDG prior to 4.5 only support extended sizeof in non-member functions. Full support was added in 4.5
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)                                     // EDG 4.5+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 301) && !defined(__apple_build_version__)  // Clang 3.1+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4005)                                       // GCC 4.5+
       // supported.
#    else
#        define CC_COMPILER_NO_EXTENDED_SIZEOF 1
#    endif
#endif

// CC_COMPILER_NO_INLINE_NAMESPACES
//
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2535.htm
// http://blog.aaronballman.com/2011/07/inline-namespaces/
//
#if !defined(CC_COMPILER_NO_INLINE_NAMESPACES)
// Not supported by VC++ as of VS2013.
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)  // EDG 4.5+.
                                                                                                    // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)  // Clang 2.9+, including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4004)  // GCC 4.4+
       // supported.
#    else
#        define CC_COMPILER_NO_INLINE_NAMESPACES 1
#    endif
#endif

// CC_COMPILER_NO_UNRESTRICTED_UNIONS
//
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2544.pdf
//
#if !defined(CC_COMPILER_NO_UNRESTRICTED_UNIONS)
// Not supported by VC++ as of VS2013.
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 406)                                       // EDG 4.6+.
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)   // Apple clang 4.1+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 301) && !defined(__apple_build_version__)  // Clang 3.1+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4006)                                       // GCC 4.6+
       // supported.
#    else
#        define CC_COMPILER_NO_UNRESTRICTED_UNIONS 1
#    endif
#endif

// CC_COMPILER_NO_EXPLICIT_CONVERSION_OPERATORS
//
// http://en.wikipedia.org/wiki/C%2B%2B11#Explicit_conversion_operators
//
#if !defined(CC_COMPILER_NO_EXPLICIT_CONVERSION_OPERATORS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1800)                                         // VS2013+.
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (_MSC_FULL_VER == 170051025)                                        // VS2012 November Preview for Windows only.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 404)                                     // EDG 4.4+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)   // Apple clang 4.1+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 300) && !defined(__apple_build_version__)  // Clang 3.0+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4005)                                       // GCC 4.5+
       // supported.
#    else
#        define CC_COMPILER_NO_EXPLICIT_CONVERSION_OPERATORS 1
#    endif
#endif

// CC_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS
//
// The compiler does not support default template arguments for function templates.
// http://stackoverflow.com/questions/2447458/default-template-arguments-for-function-templates
//
#if !defined(CC_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1800)      // VS2013+.
                                                                                                      // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 403)  // EDG 4.4+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)    // Clang 2.9+, including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4003)    // GCC 4.3+
       // supported.
#    else
#        define CC_COMPILER_NO_FUNCTION_TEMPLATE_DEFAULT_ARGS 1
#    endif
#endif

// CC_COMPILER_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS
//
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2008/n2657.htm
// http://stackoverflow.com/questions/5751977/local-type-as-template-arguments-in-c
//
#if !defined(CC_COMPILER_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1600)  // VS2010+
// supported.
#        if (CC_COMPILER_VERSION < 1700)     // VS2010 generates a warning, but the C++ language now allows it.
#            pragma warning(disable : 4836)  // nonstandard extension used: local types or unnamed types cannot be used as template arguments.
#        endif
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 402)  // EDG 4.2+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)    // Clang 2.9+, including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4005)    // GCC 4.5+
       // supported.
#    else
#        define CC_COMPILER_NO_LOCAL_CLASS_TEMPLATE_PARAMETERS 1
#    endif
#endif

// CC_COMPILER_NO_NOEXCEPT
//
// C++11 noexcept
// http://en.cppreference.com/w/cpp/language/attributes
// http://en.cppreference.com/w/cpp/language/noexcept
//
#if !defined(CC_COMPILER_NO_NOEXCEPT)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1900)                                         // VS2014+
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)   // Apple clang 4.1+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 405)                                     // EDG 4.5+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 300) && !defined(__apple_build_version__)  // Clang 3.0+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4006)                                       // GCC 4.6+
       // supported.
#    else
#        define CC_COMPILER_NO_NOEXCEPT 1
#    endif
#endif

// CC_COMPILER_NO_RAW_LITERALS
//
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2442.htm
// http://en.wikipedia.org/wiki/C%2B%2B11#New_string_literals
//
#if !defined(CC_COMPILER_NO_RAW_LITERALS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1800)                                         // VS2013+.
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 407)                                     // EDG 4.7+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)   // Apple clang 4.1+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 300) && !defined(__apple_build_version__)  // Clang 3.0+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4005)                                       // GCC 4.5+
       // supported.
#    else
#        define CC_COMPILER_NO_RAW_LITERALS 1
#    endif
#endif

// CC_COMPILER_NO_UNICODE_STRING_LITERALS
//
// http://en.wikipedia.org/wiki/C%2B%2B11#New_string_literals
//
#if !defined(CC_COMPILER_NO_UNICODE_STRING_LITERALS)
// Not supported by VC++ as of VS2013.
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 407)                                       // EDG 4.7+.
                                                                                                                                         // supported. It's not clear if it's v4.4 or v4.7 that adds this support.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)   // Apple clang 4.1+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 300) && !defined(__apple_build_version__)  // Clang 3.0+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4004)                                       // GCC 4.4+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 407)                                     // EDG 4.7+.
       // supported. It's not clear if it's v4.4 or v4.7 that adds this support.
#    else
#        define CC_COMPILER_NO_UNICODE_STRING_LITERALS 1
#    endif
#endif

// CC_COMPILER_NO_NEW_CHARACTER_TYPES
//
// Refers to char16_t and char32_t as true native types (and not something simply typedef'd from uint16_t and uint32_t).
// http://en.cppreference.com/w/cpp/language/types
//
#if !defined(CC_COMPILER_NO_NEW_CHARACTER_TYPES)
#    if defined(CC_COMPILER_NO_UNICODE_STRING_LITERALS)  // Some compilers have had support for char16_t prior to support for u"", but it's not useful to have the former without the latter.
#        define CC_COMPILER_NO_NEW_CHARACTER_TYPES 1
#    endif
#endif

// CC_COMPILER_NO_UNICODE_CHAR_NAME_LITERALS
//
// C++ 11 relaxed \u\U sequences in strings.
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2007/n2170.html
//
#if !defined(CC_COMPILER_NO_UNICODE_CHAR_NAME_LITERALS)
    // VC++ up till at least VS2013 supports \u and \U but supports them wrong with respect to the C++11 Standard.

#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)     // Apple clang 4.1+
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 301) && !defined(__apple_build_version__)  // Clang 3.1+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4005)                                       // GCC 4.5+
       // supported.
#    else
#        define CC_COMPILER_NO_UNICODE_CHAR_NAME_LITERALS 1
#    endif
#endif

// CC_COMPILER_NO_UNIFIED_INITIALIZATION_SYNTAX
//
// http://en.wikipedia.org/wiki/C%2B%2B11#Uniform_initialization
//
#if !defined(CC_COMPILER_NO_UNIFIED_INITIALIZATION_SYNTAX)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1800)                                         // VS2013+.
                                                                                                                                         // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 401) && defined(__apple_build_version__)   // Apple clang 4.1+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 301) && !defined(__apple_build_version__)  // Clang 3.1+, not including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4004)                                       // GCC 4.4+
       // supported.
#    else
#        define CC_COMPILER_NO_UNIFIED_INITIALIZATION_SYNTAX 1
#    endif
#endif

// CC_COMPILER_NO_EXTENDED_FRIEND_DECLARATIONS
//
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2005/n1791.pdf
//
#if !defined(CC_COMPILER_NO_EXTENDED_FRIEND_DECLARATIONS)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1600)      // VS2010+
                                                                                                      // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__EDG_VERSION__) && (__EDG_VERSION__ >= 401)  // EDG 4.1+.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && (CC_COMPILER_VERSION >= 209)    // Clang 2.9+, including Apple's Clang.
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4007)    // GCC 4.7+
       // supported.
#    else
#        define CC_COMPILER_NO_EXTENDED_FRIEND_DECLARATIONS 1
#    endif
#endif

// CC_COMPILER_NO_THREAD_LOCAL
//
// Refers specifically to C++ thread_local, which is like compiler __thread implementations except
// that it also supports non-trivial classes (e.g. with ctors). CC_COMPILER_NO_THREAD_LOCAL refers
// specifically to full C++11 thread_local support. The EAThread package provides a wrapper for
// __thread via CC_THREAD_LOCAL (which unfortunately sounds like C++ thread_local).
//
// https://en.cppreference.com/w/cpp/keyword/thread_local
//
#if !defined(CC_COMPILER_NO_THREAD_LOCAL)
#    if defined(CC_COMPILER_CPP11_ENABLED) && defined(__clang__) && CC_COMPILER_HAS_FEATURE(cxx_thread_local)
    // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(_MSC_VER) && (CC_COMPILER_VERSION >= 1900)  // VS2015+
       // supported.
#    elif defined(CC_COMPILER_CPP11_ENABLED) && defined(__GNUC__) && (CC_COMPILER_VERSION >= 4008)  // GCC 4.8+
       // supported.
#    else
#        define CC_COMPILER_NO_THREAD_LOCAL 1
#    endif
#endif

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
    //    CC_FORCE_INLINE / CC_PREFIX_FORCE_INLINE / CC_POSTFIX_FORCE_INLINE
    //    CC_NO_INLINE    / CC_PREFIX_NO_INLINE    / CC_POSTFIX_NO_INLINE
    //    CC_NO_VTABLE    / CC_CLASS_NO_VTABLE     / CC_STRUCT_NO_VTABLE
    //    CC_PASCAL
    //    CC_PASCAL_FUNC()
    //    CC_SSE = [0 | 1]
    //    CC_IMPORT
    //    CC_EXPORT
    //    CC_PRAGMA_ONCE_SUPPORTED
    //    CC_ONCE
    //    CC_OVERRIDE
    //    CC_INHERITANCE_FINAL
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
    //  Todo:
    //    Find out how to support CC_PASCAL and CC_PASCAL_FUNC for systems in
    //    which it hasn't yet been found out for.

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

#        ifndef __STDC_LIMIT_MACROS
#            define __STDC_LIMIT_MACROS 1
#        endif

#        ifndef __STDC_CONSTANT_MACROS
#            define __STDC_CONSTANT_MACROS 1  // This tells the GCC compiler that we want it to use its native C99 types.
#        endif
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

#else  // Unusual compilers
       // There is nothing we can do about some of these. This is not as bad a problem as it seems.
// If the given platform/compiler doesn't support alignment specifications, then it's somewhat
// likely that alignment doesn't matter for that platform. Otherwise they would have defined
// functionality to manipulate alignment.
#    define CC_ALIGN(n)
#    define CC_PREFIX_ALIGN(n)
#    define CC_POSTFIX_ALIGN(n)
#    define CC_ALIGNED(variable_type, variable, n) variable_type variable
#    define CC_PACKED

#    ifdef __cplusplus
    template <typename T>
    struct EAAlignOf1
    {
        enum
        {
            s     = sizeof(T),
            value = s ^ (s & (s - 1))
        };
    };
    template <typename T>
    struct EAAlignOf2;
    template <int size_diff>
    struct helper
    {
        template <typename T>
        struct Val
        {
            enum
            {
                value = size_diff
            };
        };
    };
    template <>
    struct helper<0>
    {
        template <typename T>
        struct Val
        {
            enum
            {
                value = EAAlignOf2<T>::value
            };
        };
    };
    template <typename T>
    struct EAAlignOf2
    {
        struct Big
        {
            T    x;
            char c;
        };
        enum
        {
            diff  = sizeof(Big) - sizeof(T),
            value = helper<diff>::template Val<Big>::value
        };
    };
    template <typename T>
    struct EAAlignof3
    {
        enum
        {
            x     = EAAlignOf2<T>::value,
            y     = EAAlignOf1<T>::value,
            value = x < y ? x : y
        };
    };
#        define CC_ALIGN_OF(type) ((size_t)EAAlignof3<type>::value)

#    else
       // C implementation of CC_ALIGN_OF
// This implementation works for most cases, but doesn't directly work
// for types such as function pointer declarations. To work with those
// types you need to typedef the type and then use the typedef in CC_ALIGN_OF.
#        define CC_ALIGN_OF(type) \
            ((size_t)offsetof(    \
              struct {            \
                  char c;         \
                  type m;         \
              },                  \
              m))
#    endif
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
// CC_INIT_SEG_AVAILABLE
//
//
#if !defined(CC_INIT_SEG_AVAILABLE)
#    if defined(_MSC_VER)
#        define CC_INIT_SEG_AVAILABLE 1
#    endif
#endif

// ------------------------------------------------------------------------
// CC_INIT_SEG
//
// Specifies a keyword or code section that affects the order in which startup code is executed.
//
// https://docs.microsoft.com/en-us/cpp/preprocessor/init-seg?view=vs-2019
//
// Example:
// 		CC_INIT_SEG(compiler) MyType gMyTypeGlobal;
// 		CC_INIT_SEG("my_section") MyOtherType gMyOtherTypeGlobal;
//
#if !defined(CC_INIT_SEG)
#    if defined(CC_INIT_SEG_AVAILABLE)
#        define CC_INIT_SEG(x) __pragma(warning(push)) __pragma(warning(disable : 4074)) __pragma(warning(disable : 4075)) __pragma(init_seg(x)) __pragma(warning(pop))
#    else
#        define CC_INIT_SEG(x)
#    endif
#endif

// ------------------------------------------------------------------------
// CC_MAY_ALIAS_AVAILABLE
//
// Defined as 0, 1, or 2.
// Defines if the GCC attribute may_alias is supported by the compiler.
// Consists of a value 0 (unsupported, shouldn't be used), 1 (some support),
// or 2 (full proper support).
//
#ifndef CC_MAY_ALIAS_AVAILABLE
#    if defined(__GNUC__) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 303)
#        if !defined(__EDG__)  // define it as 1 while defining GCC's support as 2.
#            define CC_MAY_ALIAS_AVAILABLE 2
#        else
#            define CC_MAY_ALIAS_AVAILABLE 0
#        endif
#    else
#        define CC_MAY_ALIAS_AVAILABLE 0
#    endif
#endif

// CC_MAY_ALIAS
//
// Defined as a macro that wraps the GCC may_alias attribute. This attribute
// has no significance for VC++ because VC++ doesn't support the concept of
// strict aliasing. Users should avoid writing code that breaks strict
// aliasing rules; CC_MAY_ALIAS is for cases with no alternative.
//
// Example usage:
//    void* CC_MAY_ALIAS gPtr = NULL;
//
// Example usage:
//    typedef void* CC_MAY_ALIAS pvoid_may_alias;
//    pvoid_may_alias gPtr = NULL;
//
#if CC_MAY_ALIAS_AVAILABLE
#    define CC_MAY_ALIAS __attribute__((__may_alias__))
#else
#    define CC_MAY_ALIAS
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
#        define EAGCCWHELP0(x) #x
#        define EAGCCWHELP1(x) EAGCCWHELP0(GCC diagnostic ignored x)
#        define EAGCCWHELP2(x) EAGCCWHELP1(#x)
#    endif

#    if defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4006)  // Can't test directly for __GNUC__ because some compilers lie.
#        define CC_DISABLE_GCC_WARNING(w) _Pragma("GCC diagnostic push") _Pragma(EAGCCWHELP2(w))
#    elif defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4004)
#        define CC_DISABLE_GCC_WARNING(w) _Pragma(EAGCCWHELP2(w))
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
#        define EAGCCWERRORHELP0(x) #x
#        define EAGCCWERRORHELP1(x) EAGCCWERRORHELP0(GCC diagnostic error x)
#        define EAGCCWERRORHELP2(x) EAGCCWERRORHELP1(#x)
#    endif

#    if defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4006)  // Can't test directly for __GNUC__ because some compilers lie.
#        define CC_ENABLE_GCC_WARNING_AS_ERROR(w) _Pragma("GCC diagnostic push") _Pragma(EAGCCWERRORHELP2(w))
#    elif defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION >= 4004)
#        define CC_DISABLE_GCC_WARNING(w) _Pragma(EAGCCWERRORHELP2(w))
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
#        define EACLANGWHELP0(x) #x
#        define EACLANGWHELP1(x) EACLANGWHELP0(clang diagnostic ignored x)
#        define EACLANGWHELP2(x) EACLANGWHELP1(#x)

#        define CC_DISABLE_CLANG_WARNING(w) _Pragma("clang diagnostic push") _Pragma(EACLANGWHELP2(-Wunknown - warning - option)) _Pragma(EACLANGWHELP2(w))
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
#        define EACLANGWERRORHELP0(x) #x
#        define EACLANGWERRORHELP1(x) EACLANGWERRORHELP0(clang diagnostic error x)
#        define EACLANGWERRORHELP2(x) EACLANGWERRORHELP1(#x)

#        define CC_ENABLE_CLANG_WARNING_AS_ERROR(w) _Pragma("clang diagnostic push") _Pragma(EACLANGWERRORHELP2(w))
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
// CC_DISABLE_SN_WARNING / CC_RESTORE_SN_WARNING
//
// Note that we define this macro specifically for the SN compiler instead of
// having a generic one for EDG-based compilers. The reason for this is that
// while SN is indeed based on EDG, SN has different warning value mappings
// and thus warning 1234 for SN is not the same as 1234 for all other EDG compilers.
//
// Example usage:
//     // Currently we are limited to one warning per line.
//     CC_DISABLE_SN_WARNING(1787)
//     CC_DISABLE_SN_WARNING(552)
//     <code>
//     CC_RESTORE_SN_WARNING()
//     CC_RESTORE_SN_WARNING()
//
#ifndef CC_DISABLE_SN_WARNING
#    define CC_DISABLE_SN_WARNING(w)
#endif

#ifndef CC_RESTORE_SN_WARNING
#    define CC_RESTORE_SN_WARNING()
#endif

// ------------------------------------------------------------------------
// CC_DISABLE_ALL_SN_WARNINGS / CC_RESTORE_ALL_SN_WARNINGS
//
// Example usage:
//     CC_DISABLE_ALL_SN_WARNINGS()
//     <code>
//     CC_RESTORE_ALL_SN_WARNINGS()
//
#ifndef CC_DISABLE_ALL_SN_WARNINGS
#    define CC_DISABLE_ALL_SN_WARNINGS()
#endif

#ifndef CC_RESTORE_ALL_SN_WARNINGS
#    define CC_RESTORE_ALL_SN_WARNINGS()
#endif

// ------------------------------------------------------------------------
// CC_DISABLE_GHS_WARNING / CC_RESTORE_GHS_WARNING
//
// Disable warnings from the Green Hills compiler.
//
// Example usage:
//     CC_DISABLE_GHS_WARNING(193)
//     CC_DISABLE_GHS_WARNING(236, 5323)
//     <code>
//     CC_RESTORE_GHS_WARNING()
//     CC_RESTORE_GHS_WARNING()
//
#ifndef CC_DISABLE_GHS_WARNING
#    define CC_DISABLE_GHS_WARNING(w)
#endif

#ifndef CC_RESTORE_GHS_WARNING
#    define CC_RESTORE_GHS_WARNING()
#endif

// ------------------------------------------------------------------------
// CC_DISABLE_ALL_GHS_WARNINGS / CC_RESTORE_ALL_GHS_WARNINGS
//
// #ifndef CC_DISABLE_ALL_GHS_WARNINGS
//     #if defined(CC_COMPILER_GREEN_HILLS)
//         #define CC_DISABLE_ALL_GHS_WARNINGS(w)  \_
//             _Pragma("_________")
//     #else
//         #define CC_DISABLE_ALL_GHS_WARNINGS(w)
//     #endif
// #endif
//
// #ifndef CC_RESTORE_ALL_GHS_WARNINGS
//     #if defined(CC_COMPILER_GREEN_HILLS)
//         #define CC_RESTORE_ALL_GHS_WARNINGS()   \_
//             _Pragma("_________")
//     #else
//         #define CC_RESTORE_ALL_GHS_WARNINGS()
//     #endif
// #endif

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
#        define EAEDGWHELP0(x) #x
#        define EAEDGWHELP1(x) EAEDGWHELP0(diag_suppress x)

#        define CC_DISABLE_EDG_WARNING(w) _Pragma("control %push diag") _Pragma(EAEDGWHELP1(w))
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
// CC_PURE
//
// This acts the same as the GCC __attribute__ ((pure)) directive and is
// implemented simply as a wrapper around it to allow portable usage of
// it and to take advantage of it if and when it appears in other compilers.
//
// A "pure" function is one that has no effects except its return value and
// its return value is a function of only the function's parameters or
// non-volatile global variables. Any parameter or global variable access
// must be read-only. Loop optimization and subexpression elimination can be
// applied to such functions. A common example is strlen(): Given identical
// inputs, the function's return value (its only effect) is invariant across
// multiple invocations and thus can be pulled out of a loop and called but once.
//
// Example usage:
//    CC_PURE void Function();
//
#ifndef CC_PURE
#    if defined(CC_COMPILER_GNUC)
#        define CC_PURE __attribute__((pure))
#    elif defined(CC_COMPILER_ARM)  // Arm brand compiler for ARM CPU
#        define CC_PURE __pure
#    else
#        define CC_PURE
#    endif
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
    inline void EABaseUnused(T const volatile& x)
    {
        (void)x;
    }
#        define CC_UNUSED(x) EABaseUnused(x)
#    else
#        define CC_UNUSED(x) (void)x
#    endif
#endif

// ------------------------------------------------------------------------
// CC_EMPTY
//
// Allows for a null statement, usually for the purpose of avoiding compiler warnings.
//
// Example usage:
//    #ifdef CC_DEBUG
//        #define MyDebugPrintf(x, y) printf(x, y)
//    #else
//        #define MyDebugPrintf(x, y)  CC_EMPTY
//    #endif
//
#ifndef CC_EMPTY
#    define CC_EMPTY (void)0
#endif

// ------------------------------------------------------------------------
// CC_CURRENT_FUNCTION
//
// Provides a consistent way to get the current function name as a macro
// like the __FILE__ and __LINE__ macros work. The C99 standard specifies
// that __func__ be provided by the compiler, but most compilers don't yet
// follow that convention. However, many compilers have an alternative.
//
// We also define CC_CURRENT_FUNCTION_SUPPORTED for when it is not possible
// to have CC_CURRENT_FUNCTION work as expected.
//
// Defined inside a function because otherwise the macro might not be
// defined and code below might not compile. This happens with some
// compilers.
//
#ifndef CC_CURRENT_FUNCTION
#    if defined __GNUC__ || (defined __ICC && __ICC >= 600)
#        define CC_CURRENT_FUNCTION __PRETTY_FUNCTION__
#    elif defined(__FUNCSIG__)
#        define CC_CURRENT_FUNCTION __FUNCSIG__
#    elif (defined __INTEL_COMPILER && __INTEL_COMPILER >= 600) || (defined __IBMCPP__ && __IBMCPP__ >= 500) || (defined CS_UNDEFINED_STRING && CS_UNDEFINED_STRING >= 0x4200)
#        define CC_CURRENT_FUNCTION __FUNCTION__
#    elif defined __STDC_VERSION__ && __STDC_VERSION__ >= 199901
#        define CC_CURRENT_FUNCTION __func__
#    else
#        define CC_CURRENT_FUNCTION "(unknown function)"
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
#    elif defined(__EDG_VERSION__) && (!defined(_WCHAR_T) && (__EDG_VERSION__ < 400))  // EDG prior to v4 uses _WCHAR_T to indicate if wchar_t is native. v4+ may define something else, but we're not currently aware of it.
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
    // CC_PREFIX_DEPRECATED     // You should need this only for unusual compilers.
    // CC_POSTFIX_DEPRECATED    // You should need this only for unusual compilers.
    // CC_DEPRECATED_MESSAGE    // Used as a prefix and provides a deprecation message.
    //
    // Example usage:
    //    CC_DEPRECATED void Function();
    //    CC_DEPRECATED_MESSAGE("Use 1.0v API instead") void Function();
    //
    // or for maximum portability:
    //    CC_PREFIX_DEPRECATED void Function() CC_POSTFIX_DEPRECATED;
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

#ifndef CC_PREFIX_DEPRECATED
#    if defined(CC_COMPILER_CPP14_ENABLED)
#        define CC_PREFIX_DEPRECATED [[deprecated]]
#        define CC_POSTFIX_DEPRECATED
#    elif defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION > 1300)  // If VC7 (VS2003) or later...
#        define CC_PREFIX_DEPRECATED __declspec(deprecated)
#        define CC_POSTFIX_DEPRECATED
#    elif defined(CC_COMPILER_MSVC)
#        define CC_PREFIX_DEPRECATED
#        define CC_POSTFIX_DEPRECATED
#    else
#        define CC_PREFIX_DEPRECATED
#        define CC_POSTFIX_DEPRECATED __attribute__((deprecated))
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
// CC_FORCE_INLINE              // Used as a prefix.
// CC_PREFIX_FORCE_INLINE       // You should need this only for unusual compilers.
// CC_POSTFIX_FORCE_INLINE      // You should need this only for unusual compilers.
//
// Example usage:
//     CC_FORCE_INLINE void Foo();                                // Implementation elsewhere.
//     CC_PREFIX_FORCE_INLINE void Foo() CC_POSTFIX_FORCE_INLINE; // Implementation elsewhere.
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

#if defined(CC_COMPILER_GNUC) && (((__GNUC__ * 100) + __GNUC_MINOR__) >= 301) || defined(CC_COMPILER_CLANG)
#    define CC_PREFIX_FORCE_INLINE  inline
#    define CC_POSTFIX_FORCE_INLINE __attribute__((always_inline))
#else
#    define CC_PREFIX_FORCE_INLINE inline
#    define CC_POSTFIX_FORCE_INLINE
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
// CC_PREFIX_NO_INLINE      // You should need this only for unusual compilers.
// CC_POSTFIX_NO_INLINE     // You should need this only for unusual compilers.
//
// Example usage:
//     CC_NO_INLINE        void Foo();                       // Implementation elsewhere.
//     CC_PREFIX_NO_INLINE void Foo() CC_POSTFIX_NO_INLINE;  // Implementation elsewhere.
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

#if defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION >= 1400)  // If VC8 (VS2005) or later...
#    define CC_PREFIX_NO_INLINE __declspec(noinline)
#    define CC_POSTFIX_NO_INLINE
#elif defined(CC_COMPILER_MSVC)
#    define CC_PREFIX_NO_INLINE
#    define CC_POSTFIX_NO_INLINE
#else
#    define CC_PREFIX_NO_INLINE
#    define CC_POSTFIX_NO_INLINE __attribute__((noinline))
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
// CC_PASCAL
//
// Also known on PC platforms as stdcall.
// This convention causes the compiler to assume that the called function
// will pop off the stack space used to pass arguments, unless it takes a
// variable number of arguments.
//
// Example usage:
//    this:
//       void DoNothing(int x);
//       void DoNothing(int x){}
//    would be written as this:
//       void CC_PASCAL_FUNC(DoNothing(int x));
//       void CC_PASCAL_FUNC(DoNothing(int x)){}
//
#ifndef CC_PASCAL
#    if defined(CC_COMPILER_MSVC)
#        define CC_PASCAL __stdcall
#    elif defined(CC_COMPILER_GNUC) && defined(CC_PROCESSOR_X86)
#        define CC_PASCAL __attribute__((stdcall))
#    else
       // Some compilers simply don't support pascal calling convention.
// As a result, there isn't an issue here, since the specification of
// pascal calling convention is for the purpose of disambiguating the
// calling convention that is applied.
#        define CC_PASCAL
#    endif
#endif

#ifndef CC_PASCAL_FUNC
#    if defined(CC_COMPILER_MSVC)
#        define CC_PASCAL_FUNC(funcname_and_paramlist) __stdcall funcname_and_paramlist
#    elif defined(CC_COMPILER_GNUC) && defined(CC_PROCESSOR_X86)
#        define CC_PASCAL_FUNC(funcname_and_paramlist) __attribute__((stdcall)) funcname_and_paramlist
#    else
#        define CC_PASCAL_FUNC(funcname_and_paramlist) funcname_and_paramlist
#    endif
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
#endif

// ------------------------------------------------------------------------
// CC_ONCE
//
// Example usage (which includes traditional header guards for portability):
//    #ifndef SOMEPACKAGE_SOMEHEADER_H
//    #define SOMEPACKAGE_SOMEHEADER_H
//
//    CC_ONCE()
//
//    <user code>
//
//    #endif
//
#if defined(CC_PRAGMA_ONCE_SUPPORTED)
#    if defined(_MSC_VER)
#        define CC_ONCE() __pragma(once)
#    else
#        define CC_ONCE()  // _Pragma("once")   It turns out that _Pragma("once") isn't supported by many compilers.
#    endif
#endif

// ------------------------------------------------------------------------
// CC_OVERRIDE
//
// C++11 override
// See http://msdn.microsoft.com/en-us/library/jj678987.aspx for more information.
// You can use CC_FINAL_OVERRIDE to combine usage of CC_OVERRIDE and CC_INHERITANCE_FINAL in a single statement.
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
// CC_INHERITANCE_FINAL
//
// Portably wraps the C++11 final specifier.
// See http://msdn.microsoft.com/en-us/library/jj678985.aspx for more information.
// You can use CC_FINAL_OVERRIDE to combine usage of CC_OVERRIDE and CC_INHERITANCE_FINAL in a single statement.
// This is not called CC_FINAL because that term is used within EA to denote debug/release/final builds.
//
// Example usage:
//     struct B { virtual void f() CC_INHERITANCE_FINAL; };
//
#ifndef CC_INHERITANCE_FINAL
#    if defined(CC_COMPILER_NO_INHERITANCE_FINAL)
#        define CC_INHERITANCE_FINAL
#    elif (defined(_MSC_VER) && (CC_COMPILER_VERSION < 1700))  // Pre-VS2012
#        define CC_INHERITANCE_FINAL sealed
#    else
#        define CC_INHERITANCE_FINAL final
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
#    define CC_FINAL_OVERRIDE CC_OVERRIDE CC_INHERITANCE_FINAL
#endif

// ------------------------------------------------------------------------
// CC_SEALED
//
// This is deprecated, as the C++11 Standard has final (CC_INHERITANCE_FINAL) instead.
// See http://msdn.microsoft.com/en-us/library/0w2w91tf.aspx for more information.
// Example usage:
//     struct B { virtual void f() CC_SEALED; };
//
#ifndef CC_SEALED
#    if defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION >= 1400)  // VS2005 (VC8) and later
#        define CC_SEALED sealed
#    else
#        define CC_SEALED
#    endif
#endif

// ------------------------------------------------------------------------
// CC_ABSTRACT
//
// This is a Microsoft language extension.
// See http://msdn.microsoft.com/en-us/library/b0z6b513.aspx for more information.
// Example usage:
//     struct X CC_ABSTRACT { virtual void f(){} };
//
#ifndef CC_ABSTRACT
#    if defined(CC_COMPILER_MSVC) && (CC_COMPILER_VERSION >= 1400)  // VS2005 (VC8) and later
#        define CC_ABSTRACT abstract
#    else
#        define CC_ABSTRACT
#    endif
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
#        define CC_NON_COPYABLE(EAClass_)                                                       \
        private:                                                                                \
            CC_DISABLE_VC_WARNING(4822); /* local class member function does not have a body	*/ \
            EAClass_(const EAClass_&);                                                          \
            void operator=(const EAClass_&);                                                    \
            CC_RESTORE_VC_WARNING();
#    else
#        define CC_NON_COPYABLE(EAClass_)                                                       \
            CC_DISABLE_VC_WARNING(4822); /* local class member function does not have a body	*/ \
            EAClass_(const EAClass_&)       = delete;                                           \
            void operator=(const EAClass_&) = delete;                                           \
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
// EANonCopyable
//
// Declares a class as not supporting copy construction or assignment.
// May be more reliable with some situations that CC_NON_COPYABLE alone,
// though it may result in more code generation.
//
// Note that VC++ will generate warning C4625 and C4626 if you use EANonCopyable
// and you are compiling with /W4 and /Wall. There is no resolution but
// to redelare CC_NON_COPYABLE in your subclass or disable the warnings with
// code like this:
//     CC_DISABLE_VC_WARNING(4625 4626)
//     ...
//     CC_RESTORE_VC_WARNING()
//
// Example usage:
//     struct Widget : EANonCopyable {
//        . . .
//     };
//
#ifdef __cplusplus
    struct EANonCopyable
    {
#    if defined(CC_COMPILER_NO_DEFAULTED_FUNCTIONS) || defined(__EDG__)
        // EDG doesn't appear to behave properly for the case of defaulted constructors;
        // it generates a mistaken warning about missing default constructors.
        EANonCopyable() {}   // Putting {} here has the downside that it allows a class to create itself,
        ~EANonCopyable() {}  // but avoids linker errors that can occur with some compilers (e.g. Green Hills).
#    else
        EANonCopyable()  = default;
        ~EANonCopyable() = default;
#    endif

        CC_NON_COPYABLE(EANonCopyable)
    };
#endif

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
#    elif defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION > 4004) && (defined(__i386__) || defined(__x86_64__))  // GCC 4.4+ - Seems to work only on x86/Linux so far. However, GCC 4.4 itself appears broken and screws up parameter passing conventions.
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
#    elif defined(CC_COMPILER_GNUC) && (CC_COMPILER_VERSION > 4004) && (defined(__i386__) || defined(__x86_64__))  // GCC 4.4+ - Seems to work only on x86/Linux so far. However, GCC 4.4 itself appears broken and screws up parameter passing conventions.
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

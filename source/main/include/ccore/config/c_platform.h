#ifndef __CCORE_TARGET_PLATFORM_V2_H__
#define __CCORE_TARGET_PLATFORM_V2_H__
#pragma once

namespace ncore
{
    // Platform define emanations from this file:
    // #define CC_PLATFORM_ANDROID
    // #define CC_PLATFORM_APPLE
    // #define CC_PLATFORM_IPHONE
    // #define CC_PLATFORM_IPHONE_SIMULATOR
    // #define CC_PLATFORM_OSX
    // #define CC_PLATFORM_MAC
    // #define CC_PLATFORM_LINUX
    // #define CC_PLATFORM_SAMSUNG_TV
    // #define CC_PLATFORM_WINDOWS
    // #define CC_PLATFORM_PC
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
    #define CC_PLATFORM_CYGWIN  1
    #define CC_PLATFORM_DESKTOP 1
#endif

// MinGW
// This is a pseudo-platform which will be defined along with CC_PLATFORM_WINDOWS when
// using the MinGW Windows build environment.
#if defined(__MINGW32__) || defined(__MINGW64__)
    #define CC_PLATFORM_MINGW   1
    #define CC_PLATFORM_DESKTOP 1
#endif

#if defined(CC_PLATFORM_LRB) || defined(__LRB__) || (defined(__EDG__) && defined(__ICC) && defined(__x86_64__))
    #undef CC_PLATFORM_LRB
    #define CC_PLATFORM_DESKTOP     1
    #define CC_PLATFORM_LRB         1
    #define CC_PLATFORM_NAME        "Larrabee"
    #define CC_PLATFORM_DESCRIPTION "Larrabee on LRB1"
    #define CC_PROCESSOR_X86_64     1
    #if defined(BYTE_ORDER) && (BYTE_ORDER == 4321)
        #define CC_SYSTEM_BIG_ENDIAN 1
    #else
        #define CC_SYSTEM_LITTLE_ENDIAN 1
    #endif
    #define CC_PROCESSOR_LRB   1
    #define CC_PROCESSOR_LRB1  1  // Larrabee version 1
    #define CC_ASM_STYLE_ATT   1  // Both types of asm style
    #define CC_ASM_STYLE_INTEL 1  // are supported.

// Android (Google phone OS)
#elif defined(CC_PLATFORM_ANDROID) || defined(__ANDROID__)
    #undef CC_PLATFORM_ANDROID
    #define CC_PLATFORM_MOBILE  1
    #define CC_PLATFORM_ANDROID 1
    #define CC_PLATFORM_LINUX   1
    #define CC_PLATFORM_UNIX    1
    #define CC_PLATFORM_POSIX   1
    #define CC_PLATFORM_NAME    "Android"
    #define CC_ASM_STYLE_ATT    1
    #if defined(__arm__)
        #define CC_ABI_ARM_LINUX        1  // a.k.a. "ARM eabi"
        #define CC_PROCESSOR_ARM32      1
        #define CC_PLATFORM_DESCRIPTION "Android on ARM"
    #elif defined(__aarch64__)
        #define CC_PROCESSOR_ARM64      1
        #define CC_PLATFORM_DESCRIPTION "Android on ARM64"
    #elif defined(__i386__)
        #define CC_PROCESSOR_X86        1
        #define CC_PLATFORM_DESCRIPTION "Android on x86"
    #elif defined(__x86_64)
        #define CC_PROCESSOR_X86_64     1
        #define CC_PLATFORM_DESCRIPTION "Android on x64"
    #else
        #error Unknown processor
    #endif
    #if !defined(CC_SYSTEM_BIG_ENDIAN) && !defined(CC_SYSTEM_LITTLE_ENDIAN)
        #define CC_SYSTEM_LITTLE_ENDIAN 1
    #endif

// Samsung SMART TV - a Linux-based smart TV
#elif defined(CC_PLATFORM_SAMSUNG_TV)
    #undef CC_PLATFORM_SAMSUNG_TV
    #define CC_PLATFORM_SAMSUNG_TV  1
    #define CC_PLATFORM_LINUX       1
    #define CC_PLATFORM_UNIX        1
    #define CC_PLATFORM_POSIX       1
    #define CC_PLATFORM_NAME        "SamsungTV"
    #define CC_PLATFORM_DESCRIPTION "Samsung SMART TV on ARM"
    #define CC_ASM_STYLE_ATT        1
    #define CC_SYSTEM_LITTLE_ENDIAN 1
    #define CC_PROCESSOR_ARM32      1
    #define CC_ABI_ARM_LINUX        1  // a.k.a. "ARM eabi"
    #define CC_PROCESSOR_ARM7       1

#elif defined(__APPLE__) && __APPLE__
    #include <TargetConditionals.h>

    // Apple family of operating systems.
    #define CC_PLATFORM_APPLE
    #define CC_PLATFORM_POSIX 1

    // iPhone
    // TARGET_OS_IPHONE will be undefined on an unknown compiler, and will be defined on gcc.
    #if defined(CC_PLATFORM_IPHONE) || defined(__IPHONE__) || (defined(TARGET_OS_IPHONE) && TARGET_OS_IPHONE) || (defined(TARGET_IPHONE_SIMULATOR) && TARGET_IPHONE_SIMULATOR)
        #undef CC_PLATFORM_IPHONE
        #define CC_PLATFORM_MOBILE         1
        #define CC_PLATFORM_IPHONE         1
        #define CC_PLATFORM_NAME           "iPhone"
        #define CC_ASM_STYLE_ATT           1
        #define CC_POSIX_THREADS_AVAILABLE 1
        #if defined(__arm__)
            #define CC_ABI_ARM_APPLE        1
            #define CC_PROCESSOR_ARM32      1
            #define CC_SYSTEM_LITTLE_ENDIAN 1
            #define CC_PLATFORM_DESCRIPTION "iPhone on ARM"
        #elif defined(__aarch64__) || defined(__AARCH64)
            #define CC_ABI_ARM64_APPLE      1
            #define CC_PROCESSOR_ARM64      1
            #define CC_SYSTEM_LITTLE_ENDIAN 1
            #define CC_PLATFORM_DESCRIPTION "iPhone on ARM64"
        #elif defined(__i386__)
            #define CC_PLATFORM_IPHONE_SIMULATOR 1
            #define CC_PROCESSOR_X86             1
            #define CC_SYSTEM_LITTLE_ENDIAN      1
            #define CC_PLATFORM_DESCRIPTION      "iPhone simulator on x86"
        #elif defined(__x86_64) || defined(__amd64)
            #define CC_PROCESSOR_X86_64     1
            #define CC_SYSTEM_LITTLE_ENDIAN 1
            #define CC_PLATFORM_DESCRIPTION "iPhone simulator on x64"
        #else
            #error Unknown processor
        #endif

    // Macintosh OSX
    // TARGET_OS_MAC is defined by the Metrowerks and older AppleC compilers.
    // Howerver, TARGET_OS_MAC is defined to be 1 in all cases.
    // __i386__ and __intel__ are defined by the GCC compiler.
    // __dest_os is defined by the Metrowerks compiler.
    // __MACH__ is defined by the Metrowerks and GCC compilers.
    // powerc and __powerc are defined by the Metrowerks and GCC compilers.
    #elif defined(CC_PLATFORM_OSX) || defined(__MACH__) || (defined(__MSL__) && (__dest_os == __mac_os_x))
        #undef CC_PLATFORM_OSX
        #define CC_PLATFORM_OSX     1
        #define TARGET_MAC          1
        #define CC_PLATFORM_DESKTOP 1
        #define CC_PLATFORM_UNIX    1
        #define CC_PLATFORM_POSIX   1
            // #define CC_PLATFORM_BSD 1           We don't currently define this. OSX has some BSD history but a lot of the API is different.
        #define CC_PLATFORM_NAME    "OSX"
        #if defined(__i386__) || defined(__intel__)
            #define CC_PROCESSOR_X86        1
            #define CC_SYSTEM_LITTLE_ENDIAN 1
            #define CC_PLATFORM_DESCRIPTION "OSX on x86"
        #elif defined(__x86_64) || defined(__amd64)
            #define CC_PROCESSOR_X86_64     1
            #define CC_SYSTEM_LITTLE_ENDIAN 1
            #define CC_PLATFORM_DESCRIPTION "OSX on x64"
        #elif defined(__arm__)
            #define CC_ABI_ARM_APPLE        1
            #define CC_PROCESSOR_ARM32      1
            #define CC_SYSTEM_LITTLE_ENDIAN 1
            #define CC_PLATFORM_DESCRIPTION "OSX on ARM"
        #elif defined(__aarch64__) || defined(__AARCH64)
            #define CC_ABI_ARM64_APPLE      1
            #define CC_PROCESSOR_ARM64      1
            #define CC_SYSTEM_LITTLE_ENDIAN 1
            #define CC_PLATFORM_DESCRIPTION "OSX on ARM64"
        #elif defined(__POWERPC64__) || defined(__powerpc64__)
            #define CC_PROCESSOR_POWERPC    1
            #define CC_PROCESSOR_POWERPC_64 1
            #define CC_SYSTEM_BIG_ENDIAN    1
            #define CC_PLATFORM_DESCRIPTION "OSX on PowerPC 64"
        #elif defined(__POWERPC__) || defined(__powerpc__)
            #define CC_PROCESSOR_POWERPC    1
            #define CC_PROCESSOR_POWERPC_32 1
            #define CC_SYSTEM_BIG_ENDIAN    1
            #define CC_PLATFORM_DESCRIPTION "OSX on PowerPC"
        #else
            #error Unknown processor
        #endif
        #if defined(__GNUC__)
            #define CC_ASM_STYLE_ATT 1
        #else
            #define CC_ASM_STYLE_MOTOROLA 1
        #endif
    #else
        #error Unknown Apple Platform
    #endif

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
    #undef CC_PLATFORM_LINUX
    #define CC_PLATFORM_DESKTOP 1
    #define CC_PLATFORM_LINUX   1
    #define TARGET_LINUX        1
    #define CC_PLATFORM_UNIX    1
    #define CC_PLATFORM_POSIX   1
    #define CC_PLATFORM_NAME    "Linux"
    #if defined(__i386__) || defined(__intel__) || defined(_M_IX86)
        #define CC_PROCESSOR_X86        1
        #define CC_SYSTEM_LITTLE_ENDIAN 1
        #define CC_PLATFORM_DESCRIPTION "Linux on x86"
    #elif defined(__ARM_ARCH_7A__) || defined(__ARM_EABI__)
        #define CC_ABI_ARM_LINUX        1
        #define CC_PROCESSOR_ARM32      1
        #define CC_PLATFORM_DESCRIPTION "Linux on ARM 6/7 32-bits"
    #elif defined(__aarch64__) || defined(__AARCH64)
        #define CC_PROCESSOR_ARM64      1
        #define CC_PLATFORM_DESCRIPTION "Linux on ARM64"
    #elif defined(__x86_64__)
        #define CC_PROCESSOR_X86_64     1
        #define CC_SYSTEM_LITTLE_ENDIAN 1
        #define CC_PLATFORM_DESCRIPTION "Linux on x64"
    #elif defined(__powerpc64__)
        #define CC_PROCESSOR_POWERPC    1
        #define CC_PROCESSOR_POWERPC_64 1
        #define CC_SYSTEM_BIG_ENDIAN    1
        #define CC_PLATFORM_DESCRIPTION "Linux on PowerPC 64"
    #elif defined(__powerpc__)
        #define CC_PROCESSOR_POWERPC    1
        #define CC_PROCESSOR_POWERPC_32 1
        #define CC_SYSTEM_BIG_ENDIAN    1
        #define CC_PLATFORM_DESCRIPTION "Linux on PowerPC"
    #else
        #error Unknown processor
        #error Unknown endianness
    #endif
    #if defined(__GNUC__)
        #define CC_ASM_STYLE_ATT 1
    #endif

#elif defined(CC_PLATFORM_BSD) || (defined(__BSD__) || defined(__FreeBSD__))
    #undef CC_PLATFORM_BSD
    #define CC_PLATFORM_DESKTOP 1
    #define CC_PLATFORM_BSD     1
    #define CC_PLATFORM_UNIX    1
    #define CC_PLATFORM_POSIX   1  // BSD's posix complaince is not identical to Linux's
    #define CC_PLATFORM_NAME    "BSD Unix"
    #if defined(__i386__) || defined(__intel__)
        #define CC_PROCESSOR_X86        1
        #define CC_SYSTEM_LITTLE_ENDIAN 1
        #define CC_PLATFORM_DESCRIPTION "BSD on x86"
    #elif defined(__x86_64__)
        #define CC_PROCESSOR_X86_64     1
        #define CC_SYSTEM_LITTLE_ENDIAN 1
        #define CC_PLATFORM_DESCRIPTION "BSD on x64"
    #elif defined(__powerpc64__)
        #define CC_PROCESSOR_POWERPC    1
        #define CC_PROCESSOR_POWERPC_64 1
        #define CC_SYSTEM_BIG_ENDIAN    1
        #define CC_PLATFORM_DESCRIPTION "BSD on PowerPC 64"
    #elif defined(__powerpc__)
        #define CC_PROCESSOR_POWERPC    1
        #define CC_PROCESSOR_POWERPC_32 1
        #define CC_SYSTEM_BIG_ENDIAN    1
        #define CC_PLATFORM_DESCRIPTION "BSD on PowerPC"
    #else
        #error Unknown processor
        #error Unknown endianness
    #endif
    #if !defined(CC_PLATFORM_FREEBSD) && defined(__FreeBSD__)
        #define CC_PLATFORM_FREEBSD 1  // This is a variation of BSD.
    #endif
    #if defined(__GNUC__)
        #define CC_ASM_STYLE_ATT 1
    #endif

#elif defined(CC_PLATFORM_WINDOWS_PHONE)
    #undef CC_PLATFORM_WINDOWS_PHONE
    #define CC_PLATFORM_WINDOWS_PHONE 1
    #define CC_PLATFORM_NAME          "Windows Phone"
    #if defined(_M_AMD64) || defined(_AMD64_) || defined(__x86_64__)
        #define CC_PROCESSOR_X86_64     1
        #define CC_SYSTEM_LITTLE_ENDIAN 1
        #define CC_PLATFORM_DESCRIPTION "Windows Phone on x64"
    #elif defined(_M_IX86) || defined(_X86_)
        #define CC_PROCESSOR_X86        1
        #define CC_SYSTEM_LITTLE_ENDIAN 1
        #define CC_PLATFORM_DESCRIPTION "Windows Phone on X86"
    #elif defined(_M_ARM)
        #define CC_ABI_ARM_WINCE        1
        #define CC_PROCESSOR_ARM32      1
        #define CC_SYSTEM_LITTLE_ENDIAN 1
        #define CC_PLATFORM_DESCRIPTION "Windows Phone on ARM"
    #else  // Possibly other Windows Phone variants
        #error Unknown processor
        #error Unknown endianness
    #endif
    #define CC_PLATFORM_MICROSOFT        1

    // WINAPI_FAMILY defines - mirrored from winapifamily.h
    #define CC_WINAPI_FAMILY_APP         1
    #define CC_WINAPI_FAMILY_DESKTOP_APP 2
    #define CC_WINAPI_FAMILY_PHONE_APP   3

    #if defined(WINAPI_FAMILY)
        #include <winapifamily.h>
        #if WINAPI_FAMILY == WINAPI_FAMILY_PHONE_APP
            #define CC_WINAPI_FAMILY CC_WINAPI_FAMILY_PHONE_APP
        #else
            #error Unsupported WINAPI_FAMILY for Windows Phone
        #endif
    #else
        #error WINAPI_FAMILY should always be defined on Windows Phone.
    #endif

    // Macro to determine if a partition is enabled.
    #define CC_WINAPI_FAMILY_PARTITION(Partition) (Partition)

    // Enable the appropriate partitions for the current family
    #if CC_WINAPI_FAMILY == CC_WINAPI_FAMILY_PHONE_APP
        #define CC_WINAPI_PARTITION_CORE  1
        #define CC_WINAPI_PARTITION_PHONE 1
        #define CC_WINAPI_PARTITION_APP   1
    #else
        #error Unsupported WINAPI_FAMILY for Windows Phone
    #endif

// Windows
// _WIN32 is defined by the VC++, Intel and GCC compilers.
// _WIN64 is defined by the VC++, Intel and GCC compilers.
// __WIN32__ is defined by the Borland compiler.
// __INTEL__ is defined by the Metrowerks compiler.
// _M_IX86, _M_AMD64 and _M_IA64 are defined by the VC++, Intel, and Borland compilers.
// _X86_, _AMD64_, and _IA64_ are defined by the Metrowerks compiler.
// _M_ARM is defined by the VC++ compiler.
#elif (defined(CC_PLATFORM_WINDOWS) || (defined(_WIN32) || defined(__WIN32__) || defined(_WIN64))) && !defined(CS_UNDEFINED_STRING)
    #undef CC_PLATFORM_WINDOWS
    #define CC_PLATFORM_DESKTOP   1
    #define CC_PLATFORM_MICROSOFT 1
    #define CC_PLATFORM_WINDOWS   1
    #define CC_PLATFORM_PC        1
    #define TARGET_PC             1
    #define CC_PLATFORM_NAME      "Windows"
    #ifdef _WIN64  // VC++ defines both _WIN32 and _WIN64 when compiling for Win64.
        #define CC_PLATFORM_WIN64 1
    #else
        #define CC_PLATFORM_WIN32 1
    #endif
    #if defined(_M_AMD64) || defined(_AMD64_) || defined(__x86_64__)
        #define CC_PROCESSOR_X86_64     1
        #define CC_SYSTEM_LITTLE_ENDIAN 1
        #define CC_PLATFORM_DESCRIPTION "Windows on x64"
    #elif defined(_M_IX86) || defined(_X86_)
        #define CC_PROCESSOR_X86        1
        #define CC_SYSTEM_LITTLE_ENDIAN 1
        #define CC_PLATFORM_DESCRIPTION "Windows on X86"
    #elif defined(_M_IA64) || defined(_IA64_)
        #define CC_PROCESSOR_IA64       1
        #define CC_SYSTEM_LITTLE_ENDIAN 1
        #define CC_PLATFORM_DESCRIPTION "Windows on IA-64"
    #elif defined(_M_ARM)
        #define CC_ABI_ARM_WINCE        1
        #define CC_PROCESSOR_ARM32      1
        #define CC_SYSTEM_LITTLE_ENDIAN 1
        #define CC_PLATFORM_DESCRIPTION "Windows on ARM"
    #elif defined(_M_ARM64)
        #define CC_PROCESSOR_ARM64      1
        #define CC_SYSTEM_LITTLE_ENDIAN 1
        #define CC_PLATFORM_DESCRIPTION "Windows on ARM64"
    #else  // Possibly other Windows CE variants
        #error Unknown processor
        #error Unknown endianness
    #endif
    #if defined(__GNUC__)
        #define CC_ASM_STYLE_ATT 1
    #elif defined(_MSC_VER) || defined(__BORLANDC__) || defined(__ICL)
        #define CC_ASM_STYLE_INTEL 1
    #endif

    #if defined(_KERNEL_MODE)
        #define CC_PLATFORM_WINDOWS_KERNEL 1
    #endif

    // WINAPI_FAMILY defines to support Windows 8 Metro Apps - mirroring winapifamily.h in the Windows 8 SDK
    #define CC_WINAPI_FAMILY_APP         1000
    #define CC_WINAPI_FAMILY_DESKTOP_APP 1001
    #define CC_WINAPI_FAMILY_GAMES       1006

    #if defined(WINAPI_FAMILY)
        #if defined(_MSC_VER)
            #pragma warning(push, 0)
        #endif
        #include <winapifamily.h>
        #if defined(_MSC_VER)
            #pragma warning(pop)
        #endif
        #if defined(WINAPI_FAMILY_DESKTOP_APP) && WINAPI_FAMILY == WINAPI_FAMILY_DESKTOP_APP
            #define CC_WINAPI_FAMILY CC_WINAPI_FAMILY_DESKTOP_APP
        #elif defined(WINAPI_FAMILY_APP) && WINAPI_FAMILY == WINAPI_FAMILY_APP
            #define CC_WINAPI_FAMILY CC_WINAPI_FAMILY_APP
        #elif defined(WINAPI_FAMILY_GAMES) && WINAPI_FAMILY == WINAPI_FAMILY_GAMES
            #define CC_WINAPI_FAMILY CC_WINAPI_FAMILY_GAMES
        #else
            #error Unsupported WINAPI_FAMILY
        #endif
    #else
        #define CC_WINAPI_FAMILY CC_WINAPI_FAMILY_DESKTOP_APP
    #endif

    #define CC_WINAPI_PARTITION_DESKTOP 1
    #define CC_WINAPI_PARTITION_APP     1
    #define CC_WINAPI_PARTITION_GAMES   (CC_WINAPI_FAMILY == CC_WINAPI_FAMILY_GAMES)

    #define CC_WINAPI_FAMILY_PARTITION(Partition) (Partition)

    // CC_PLATFORM_WINRT
    // This is a subset of Windows which is used for tablets and the "Metro" (restricted) Windows user interface.
    // WinRT doesn't doesn't have access to the Windows "desktop" API, but WinRT can nevertheless run on
    // desktop computers in addition to tablets. The Windows Phone API is a subset of WinRT and is not included
    // in it due to it being only a part of the API.
    #if defined(__cplusplus_winrt)
        #define CC_PLATFORM_WINRT 1
    #endif

// Sun (Solaris)
// __SUNPRO_CC is defined by the Sun compiler.
// __sun is defined by the GCC compiler.
// __i386 is defined by the Sun and GCC compilers.
// __sparc is defined by the Sun and GCC compilers.
#else
    #error Unknown platform
    #error Unknown processor
    #error Unknown endianness
#endif

#ifndef CC_PROCESSOR_ARM
    #if defined(CC_PROCESSOR_ARM32) || defined(CC_PROCESSOR_ARM64) || defined(CC_PROCESSOR_ARM7)
        #define CC_PROCESSOR_ARM
    #endif
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
    #if defined(__WORDSIZE)  // Defined by some variations of GCC.
        #define CC_PLATFORM_PTR_SIZE ((__WORDSIZE) / 8)
    #elif defined(_WIN64) || defined(__LP64__) || defined(_LP64) || defined(_M_IA64) || defined(__ia64__) || defined(__arch64__) || defined(__aarch64__) || defined(__mips64__) || defined(__64BIT__) || defined(__Ptr_Is_64)
        #define CC_PLATFORM_PTR_SIZE 8
    #elif defined(__CC_ARM) && (__sizeof_ptr == 8)
        #define CC_PLATFORM_PTR_SIZE 8
    #else
        #define CC_PLATFORM_PTR_SIZE 4
    #endif
#endif

// CC_PLATFORM_WORD_SIZE
// This defines the size of a machine word. This will be the same as
// the size of registers on the machine but not necessarily the same
// as the size of pointers on the machine. A number of 64 bit platforms
// have 64 bit registers but 32 bit pointers.
//
#ifndef CC_PLATFORM_WORD_SIZE
    #define CC_PLATFORM_WORD_SIZE CC_PLATFORM_PTR_SIZE
#endif

// CC_PLATFORM_MIN_MALLOC_ALIGNMENT
// This defines the minimal alignment that the platform's malloc
// implementation will return. This should be used when writing custom
// allocators to ensure that the alignment matches that of malloc
#ifndef CC_PLATFORM_MIN_MALLOC_ALIGNMENT
    #if defined(CC_PLATFORM_APPLE)
        #define CC_PLATFORM_MIN_MALLOC_ALIGNMENT 16
    #elif defined(CC_PLATFORM_ANDROID) && defined(CC_PROCESSOR_ARM)
        #define CC_PLATFORM_MIN_MALLOC_ALIGNMENT 8
    #elif defined(CC_PLATFORM_ANDROID) && defined(CC_PROCESSOR_X86_64)
        #define CC_PLATFORM_MIN_MALLOC_ALIGNMENT 8
    #else
        #define CC_PLATFORM_MIN_MALLOC_ALIGNMENT (CC_PLATFORM_PTR_SIZE * 2)
    #endif
#endif

// CC_MISALIGNED_SUPPORT_LEVEL
// Specifies if the processor can read and write built-in types that aren't
// naturally aligned.
//    0 - not supported. Likely causes an exception.
//    1 - supported but slow.
//    2 - supported and fast.
//
#ifndef CC_MISALIGNED_SUPPORT_LEVEL
    #if defined(CC_PROCESSOR_X86_64)
        #define CC_MISALIGNED_SUPPORT_LEVEL 2
    #else
        #define CC_MISALIGNED_SUPPORT_LEVEL 0
    #endif
#endif

// Macro to determine if a Windows API partition is enabled. Always false on non Microsoft platforms.
#if !defined(CC_WINAPI_FAMILY_PARTITION)
    #define CC_WINAPI_FAMILY_PARTITION(Partition) (0)
#endif

    // CC_CACHE_LINE_SIZE
    // Specifies the cache line size broken down by compile target.
    // This the expected best guess values for the targets that we can make at compilation time.

#ifndef CC_CACHE_LINE_SIZE
    #if defined(CC_PROCESSOR_X86)
        #define CC_CACHE_LINE_SIZE 32  // This is the minimum possible value.
    #elif defined(CC_PROCESSOR_X86_64)
        #define CC_CACHE_LINE_SIZE 64  // This is the minimum possible value
    #elif defined(CC_PROCESSOR_ARM32)
        #define CC_CACHE_LINE_SIZE 32  // This varies between implementations and is usually 32 or 64.
    #elif defined(CC_PROCESSOR_ARM64)
        #define CC_CACHE_LINE_SIZE 64  // Cache line Cortex-A8  (64 bytes) (assumption at this stage)
    #elif (CC_PLATFORM_WORD_SIZE == 4)
        #define CC_CACHE_LINE_SIZE 32  // This is the minimum possible value
    #else
        #define CC_CACHE_LINE_SIZE 64  // This is the minimum possible value
    #endif
#endif

#ifdef CC_SYSTEM_LITTLE_ENDIAN
    #define D_LITTLE_ENDIAN
#elif defined(CC_SYSTEM_BIG_ENDIAN)
    #define D_BIG_ENDIAN
#endif

};  // namespace ncore

#endif

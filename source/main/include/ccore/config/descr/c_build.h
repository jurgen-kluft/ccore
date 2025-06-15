#ifndef __CCORE_TARGET_BUILD_DESCRIPTION_H__
#define __CCORE_TARGET_BUILD_DESCRIPTION_H__
#pragma once

#include "ccore/config/c_platform.h"

//  generate string containing build description

#ifdef TARGET_PLATFORM_STR
#    undef TARGET_PLATFORM_STR
#endif

#ifdef CC_PLATFORM_OSX
#    define TARGET_PLATFORM_STR "MACOS"
#elif defined(CC_PLATFORM_WINDOWS)
#    define TARGET_PLATFORM_STR "PC"
#elif defined(CC_PLATFORM_LINUX)
#    define TARGET_PLATFORM_STR "LINUX"
#endif

#if defined(TARGET_TEST)
#    define TARGET_VARIANT_STR "_TEST"
#elif defined(TARGET_DEV)
#    define TARGET_VARIANT_STR "_DEV"
#elif defined(TARGET_PROFILE)
#    define TARGET_VARIANT_STR "_PROFILE"
#elif defined(TARGET_RETAIL)
#    define TARGET_VARIANT_STR "_RETAIL"
#else
#    define TARGET_VARIANT_STR ""
#endif

#if defined(TARGET_DEBUG)
#        define TARGET_CONFIG_FULL_STR TARGET_VARIANT_STR "DEBUG"
#elif defined(TARGET_RELEASE)
#        define TARGET_CONFIG_FULL_STR TARGET_VARIANT_STR "RELEASE"
#elif defined(TARGET_FINAL)
#    define TARGET_CONFIG_FULL_STR TARGET_VARIANT_STR "FINAL"
#else
#    define TARGET_CONFIG_FULL_STR TARGET_VARIANT_STR "UNKNOWN"
#endif

#if !defined(TARGET_HW_STR)
#    if defined(TARGET_DEVKIT)
#        define TARGET_HW_STR "_DEVKIT"
#    endif
#    if defined(TARGET_RETAIL)
#        define TARGET_HW_STR "_RETAIL"
#    endif
#    if defined(TARGET_CLIENT)
#        define TARGET_HW_STR "_CLIENT"
#    endif
#    if !defined(TARGET_HW_STR)
#        define TARGET_HW_STR ""
#    endif
#endif

#if defined(TARGET_32BIT)
#    define TARGET_BITS_STR "_32"
#elif defined(TARGET_64BIT)
#    define TARGET_BITS_STR "_64"
#else
#    define TARGET_BITS_STR "_64"
#endif

#define TARGET_FULL_DESCR_STR TARGET_PLATFORM_STR TARGET_HW_STR TARGET_CONFIG_FULL_STR TARGET_BITS_STR

#endif

#ifndef __CCORE_TARGET_BUILD_DESCRIPTION_H__
#define __CCORE_TARGET_BUILD_DESCRIPTION_H__
#pragma once

#include "ccore/config/c_platform.h"

//  generate string containing the target description
//
//  CC_PLATFORM_PC + TARGET_DEBUG +

#ifdef TARGET_PLATFORM_STR
    #undef TARGET_PLATFORM_STR
#endif

#if defined(CC_PLATFORM_MAC)
    #define TARGET_PLATFORM_STR "MACOS"
#elif defined(CC_PLATFORM_PC)
    #define TARGET_PLATFORM_STR "PC"
#elif defined(CC_PLATFORM_LINUX)
    #define TARGET_PLATFORM_STR "LINUX"
#endif

#if defined(TARGET_DEBUG)
    #if defined(TARGET_TEST)
        #define TARGET_CONFIG_SHORT_STR "_TB"
    #else
        #define TARGET_CONFIG_SHORT_STR "_B"
    #endif
#endif
#if defined(TARGET_RELEASE)
    #if defined(TARGET_TEST)
        #define TARGET_CONFIG_SHORT_STR "_TR"
    #else
        #define TARGET_CONFIG_SHORT_STR "_R"
    #endif
#endif
#if defined(TARGET_FINAL)
    #define TARGET_CONFIG_SHORT_STR "_F"
#endif

#if defined(TARGET_DEBUG)
    #if defined(TARGET_TEST)
        #define TARGET_CONFIG_FULL_STR "_TESTDEBUG"
    #else
        #define TARGET_CONFIG_FULL_STR "_DEBUG"
    #endif
#endif
#if defined(TARGET_RELEASE)
    #if defined(TARGET_TEST)
        #define TARGET_CONFIG_FULL_STR "_TESTRELEASE"
    #else
        #define TARGET_CONFIG_FULL_STR "_RELEASE"
    #endif
#endif
#if defined(TARGET_FINAL)
    #define TARGET_CONFIG_FULL_STR "_FINAL"
#endif

#if defined(TARGET_DEVKIT)
    #define TARGET_HW_STR "_DEVKIT"
#endif
#if defined(TARGET_RETAIL)
    #define TARGET_HW_STR "_RETAIL"
#endif
#if defined(TARGET_CLIENT)
    #define TARGET_HW_STR "_CLIENT"
#endif
#if !defined(TARGET_HW_STR)
    #define TARGET_HW_STR ""
#endif

#if defined(TARGET_32BIT)
    #define TARGET_BIT_STR "_32"
#elif defined(TARGET_64BIT)
    #define TARGET_BIT_STR "_64"
#else
    #define TARGET_BIT_STR "_64"
#endif

#define TARGET_FULL_DESCR_STR TARGET_PLATFORM_STR TARGET_HW_STR TARGET_EDITOR_STR TARGET_CONFIG_FULL_STR TARGET_BIT_STR

#endif
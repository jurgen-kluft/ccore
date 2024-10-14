#ifndef __CCORE_TARGET_TARGET_DESCRIPTION_V2_H__
#define __CCORE_TARGET_TARGET_DESCRIPTION_V2_H__
#pragma once

#include "ccore/config/c_platform.h"

namespace ncore
{
   // Hardware enumeration for TARGET_PLATFORM
    namespace eplatform
    {
        enum enum_value
        {
            PLATFORM_NONE             = 0,
            PLATFORM_PC               = (1 << 0),
            PLATFORM_MAC              = (1 << 1),
            PLATFORM_LINUX            = (1 << 2),
            PLATFORM_ANDROID          = (1 << 3),
            PLATFORM_APPLE            = (1 << 4),
            PLATFORM_IPHONE           = (1 << 5),
            PLATFORM_IPHONE_SIMULATOR = (1 << 6),
            PLATFORM_OSX              = (1 << 7),
            PLATFORM_LINUX            = (1 << 8),
            PLATFORM_SAMSUNG_TV       = (1 << 9),
            PLATFORM_WINDOWS          = (1 << 10),
            PLATFORM_WIN32            = (1 << 11),
            PLATFORM_WIN64            = (1 << 12),
            PLATFORM_WINDOWS_PHONE    = (1 << 13),
            PLATFORM_WINRT            = (1 << 14),
            PLATFORM_SUN              = (1 << 15),
            PLATFORM_LRB              = (1 << 16),  // (Larrabee)
            PLATFORM_POSIX            = (1 << 20),  // (pseudo-platform; may be defined with another platform like PLATFORM_LINUX, PLATFORM_UNIX, PLATFORM_QNX)
            PLATFORM_UNIX             = (1 << 21),  // (pseudo-platform; may be defined with another platform like PLATFORM_LINUX)
            PLATFORM_CYGWIN           = (1 << 22),  // (pseudo-platform; may be defined with another platform like PLATFORM_LINUX)
            PLATFORM_MINGW            = (1 << 23),  // (pseudo-platform; may be defined with another platform like PLATFORM_WINDOWS)
            PLATFORM_MICROSOFT        = (1 << 24),  // (pseudo-platform; may be defined with another platform like PLATFORM_WINDOWS)
            PLATFORM_ALL              = PLATFORM_PC | PLATFORM_MAC | PLATFORM_LINUX,
        };

        const char* ToString(enum_value platform);

    };  // namespace eplatform

    // Available configurations
    namespace econfig
    {
        enum enum_value
        {
            CONFIG_NONE    = 0,
            CONFIG_DEBUG   = (1 << 0),
            CONFIG_RELEASE = (1 << 1),
            CONFIG_FINAL   = (1 << 2),
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
            BUILD_DEV     = (1 << 0),
            BUILD_TEST    = (1 << 1),
            BUILD_PROFILE = (1 << 2),
            BUILD_RETAIL  = (1 << 3),
            BUILD_ALL     = BUILD_DEV | BUILD_TEST | BUILD_PROFILE | BUILD_RETAIL,
        };

        const char* ToString(enum_value build);

    }  // namespace ebuild
}

#endif
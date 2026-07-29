#ifndef __CCORE_BYTES_H__
#define __CCORE_BYTES_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    inline u8 g_low_byte(u16 v) { return (u8)(v & 0xFF); }
    inline u8 g_high_byte(u16 v) { return (u8)((v >> 8) & 0xFF); }

    inline u16 g_low_word(u32 v) { return (u16)(v & 0xFFFF); }
    inline u16 g_high_word(u32 v) { return (u16)((v >> 16) & 0xFFFF); }

    inline u32 g_low_dword(u64 v) { return (u32)(v & 0xFFFFFFFF); }
    inline u32 g_high_dword(u64 v) { return (u32)((v >> 32) & 0xFFFFFFFF); }
};  // namespace ncore

#endif  // __CCORE_BYTES_H__

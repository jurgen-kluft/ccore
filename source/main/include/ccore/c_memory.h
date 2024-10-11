#ifndef __CCORE_MEMORY_STD_H__
#define __CCORE_MEMORY_STD_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_debug.h"

namespace ncore
{
    ///@description:	Interface/Utility class for MemSet, MemClear, MemCopy, MemMove
    ///@note:			In DEBUG these functions should perform sanity checks on the source and destination memory blocks
    namespace nmem
    {
        void*       memset(void* inDest, u32 inValue, int_t inLength);
        void*       memcpy(void* inDest, const void* inSrc, int_t inLength);
        s32         memcmp(const void* inLHS, const void* inRHS, int_t inLength);
        void        memswap(void* inLHS, void* inRHS, int_t inLength);
        void*       memmove(void* inDest, const void* inSrc, int_t inLength);
        inline void memclr(void* inDest, int_t inLength) { memset(inDest, 0, inLength); }

        ///@name Pointer arithmetic
        inline void* ptr_add(void* ptr, int_t size) { return (void*)((u8*)ptr + size); }
        inline void* ptr_add_clamp(void* ptr, int_t size, void* lower, void* upper)
        {
            void* p = (void*)((u8*)ptr + size);
            if (p < lower)
                p = lower;
            else if (p > upper)
                p = upper;
            return p;
        }
        inline void* ptr_align(void* ptr, u32 alignment) { return (void*)(((ptr_t)ptr + (ptr_t)(alignment - 1)) & ~((ptr_t)(alignment - 1))); }
        inline s64   ptr_diff(void* ptr, void* other) { return (s64)((u8*)other - (u8*)ptr); }
        inline bool  ptr_is_aligned(void* ptr, u32 alignment) { return ((ptr_t)ptr & (ptr_t)(alignment - 1)) == 0; }

        ///@name Conversion
        inline s64 toKb(s64 inNumBytes) { return (inNumBytes + (s64)512) / (s64)1024; }
        inline s64 toMb(s64 inNumBytes) { return (inNumBytes + (s64)(512 * 1024)) / (s64)((s64)1024 * 1024); }
        inline s64 toGb(s64 inNumBytes) { return (inNumBytes + (s64)(512 * 1024 * 1024)) / (s64)(1024 * 1024 * 1024); }
    };  // namespace nmem

    template <class T>
    inline void g_swap(T& a, T& b)
    {
        const T c(a);
        a = b;
        b = c;
    }

    inline void g_memset(void* inBlock, u32 inValue, int_t inLength) { nmem::memset(inBlock, inValue, inLength); }
    inline void g_memcpy(void* inDest, const void* inSource, int_t inLength) { nmem::memcpy(inDest, inSource, inLength); }
    inline s32  g_memcmp(const void* inLHS, const void* inRHS, int_t inLength) { return nmem::memcmp(inLHS, inRHS, inLength); }
    inline void g_memswap(void* inLHS, void* inRHS, int_t inLength) { nmem::memswap(inLHS, inRHS, inLength); }

    // Set memory block <inBlock> of length <inLength> to zero
    inline void g_memclr(void* inBlock, int_t inLength) { g_memset(inBlock, 0, inLength); }
    inline void g_memzero(void* inBlock, int_t inLength) { g_memset(inBlock, 0, inLength); }
    // Copy <inLength> aligned 128-bit integers from <inSource> to <inDest>
    inline void* g_memmove(void* inDest, const void* inSource, int_t inLength) { return nmem::memmove(inDest, inSource, inLength); }
    inline void  g_memcopy(void* inDest, const void* inSource, int_t inLength) { g_memcpy(inDest, inSource, inLength); }
    // Compare memory block <inBlock1> and <inBlock2> of length <inLength>, return <0 if inBlock1<inBlock2, >0 if inBlock1>inBlock2 and 0 if inBlock1==inBlock2
    inline s32 g_memcompare(const void* inBlock1, const void* inBlock2, int_t inLength) { return g_memcmp(inBlock1, inBlock2, inLength); }
    // Return the result of the bitwise compare between memory block <inBlock1> and <inBlock2> of length <inLength>, return <0 if inBlock1<inBlock2, >0 if inBlock1>inBlock2 and 0 if inBlock1==inBlock2
    inline bool g_memequal(const void* inBlock1, const void* inBlock2, int_t inLength) { return (g_memcompare(inBlock1, inBlock2, inLength) == 0); }

}  // namespace ncore
#endif  // __C_MEMORY_STD_H__

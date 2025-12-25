#include "ccore/c_target.h"
#include "ccore/c_allocator.h"
#include "ccore/c_memory.h"
#include "ccore/c_runes.h"
#include "ccore/c_arena.h"

namespace ncore
{
    void* g_reallocate(alloc_t* alloc, void* ptr, u32 size, u32 new_size)
    {
        void*     newptr  = alloc->allocate(new_size, sizeof(void*));
        u32 const minsize = size < new_size ? size : new_size;
        g_memory_aligned4_copy(newptr, ptr, minsize);
        alloc->deallocate(ptr);
        return newptr;
    }

    char* g_duplicate_string(alloc_t* alloc, const char* str)
    {
        const u32 len = ascii::strlen(str) + 1;
        char*     cp  = g_allocate_memory<char>(alloc, len);
        g_memory_aligned4_copy(cp, str, len);
        return cp;
    }

    void g_deallocate_string(alloc_t* alloc, const char*& str)
    {
        if (str != nullptr)
        {
            alloc->deallocate((void*)str);
            str = nullptr;
        }
    }

};  // namespace ncore

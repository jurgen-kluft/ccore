#include "ccore/c_memory.h"

// TODO
// - Investigate the use of madvise(MADV_FREE) to decommit memory on Mac, madvise(MADV_DONTNEED) on Linux, and VirtualAlloc(MEM_RESET)

#if defined(TARGET_PC)
    #define WIN32_LEAN_AND_MEAN
    #include <windows.h>

    #include "ccore/c_arena.h"
    #include "ccore/c_math.h"
    #include "ccore/c_memory.h"

namespace ncore
{
    static u32 s_page_size = 0;
    u32        v_alloc_get_page_size()
    {
        if (s_page_size == 0)
        {
            SYSTEM_INFO sys_info;
            GetSystemInfo(&sys_info);
            s_page_size = sys_info.dwPageSize;
        }
        return s_page_size;
    }

    void* v_alloc_reserve(uint_t size)
    {
        // Reserve a certain amount of address space
        return VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_NOACCESS);
    }

    bool v_alloc_commit(void* addr, uint_t size)
    {
        void* result = VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE);
        return result != nullptr;
    }

    bool v_alloc_decommit(void* addr, uint_t size)
    {
        // VirtualFree(base_addr + 1MB, size, MEM_DECOMMIT);
        /*
            "The VirtualFree function can decommit a range of pages that are in
            different states, some committed and some uncommitted. This means
            that you can decommit a range of pages without first determining
            the state of each page."
        */
        BOOL success = VirtualFree(addr, (DWORD)size, MEM_DECOMMIT);
        return success ? true : false;
    }

    bool v_alloc_release(void* addr, uint_t size)
    {
        (void)size;
        return VirtualFree(addr, 0, MEM_RELEASE);
    }
}  // namespace ncore

#elif defined(TARGET_LINUX) || defined(TARGET_MAC)

    #include <unistd.h>
    #include <sys/mman.h>

    #include "ccore/c_arena.h"
    #include "ccore/c_math.h"
    #include "ccore/c_memory.h"

namespace ncore
{
    static u32 s_page_size = 0;
    u32        v_alloc_get_page_size()
    {
        if (s_page_size == 0)
            s_page_size = (u32)sysconf(_SC_PAGESIZE);
        return s_page_size;
    }

    void* v_alloc_reserve(uint_t size)
    {
        void* ptr = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
        return ptr == MAP_FAILED ? nullptr : ptr;
    }

    bool v_alloc_commit(void* addr, uint_t size)
    {
        const s32 result = mprotect(addr, size, PROT_READ | PROT_WRITE);
        return result == 0;
    }

    bool v_alloc_decommit(void* addr, uint_t extra_size)
    {
        s32 result = madvise(addr, extra_size, MADV_DONTNEED);
        if (result == 0)
        {
            result = mprotect(addr, extra_size, PROT_NONE);
        }
        return result == 0;
    }

    bool v_alloc_release(void* addr, uint_t size)
    {
        // munmap returns 0 on success
        return munmap(addr, size) == 0;
    }
}  // namespace ncore

#else

    #include "ccore/c_arena.h"
    #include "ccore/c_math.h"
    #include "ccore/c_memory.h"

namespace ncore
{
    u32   v_alloc_get_page_size() { return 4 * cKB; }
    void* v_alloc_reserve(uint_t size)
    {
        CC_UNUSED(size);
        return nullptr;
    }
    bool v_alloc_commit(void* addr, uint_t size)
    {
        CC_UNUSED(addr);
        CC_UNUSED(size);
        return false;
    }
    bool v_alloc_decommit(void* addr, uint_t extra_size)
    {
        CC_UNUSED(addr);
        CC_UNUSED(extra_size);
        return false;
    }
    bool v_alloc_release(void* addr, uint_t size)
    {
        CC_UNUSED(addr);
        CC_UNUSED(size);
        return false;
    }
}  // namespace ncore

#endif

namespace ncore
{
    static u8 s_page_size_shift = 0;
    u8        v_alloc_get_page_size_shift()
    {
        if (s_page_size_shift == 0)
        {
            const u32 page_size = v_alloc_get_page_size();
            s_page_size_shift   = (u8)math::ilog2(page_size);
        }
        return s_page_size_shift;
    }
}  // namespace ncore

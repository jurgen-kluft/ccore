#include "ccore/c_binmap1.h"

// TODO
// - Investigate the use of madvise(MADV_FREE) to decommit memory on Mac, madvise(MADV_DONTNEED) on Linux, and VirtualAlloc(MEM_RESET)

#if defined(TARGET_PC)
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>

#    include "ccore/c_arena.h"
#    include "ccore/c_math.h"
#    include "ccore/c_memory.h"

namespace ncore
{
    static s32 s_page_size = 0;
    s32        v_alloc_get_page_size()
    {
        if (s_page_size == 0)
        {
            SYSTEM_INFO sys_info;
            GetSystemInfo(&sys_info);
            s_page_size = sys_info.dwPageSize;
        }
        return s_page_size;
    }

    void *v_alloc_reserve(int_t size)
    {
        // Reserve a certain amount of address space
        return VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_NOACCESS);
    }

    bool v_alloc_commit(void *addr, int_t size)
    {
        void *result = VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE);
        return result != nullptr;
    }

    bool v_alloc_decommit(void *addr, int_t size)
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

    bool v_alloc_release(void *addr, int_t size)
    {
        (void)size;
        return VirtualFree(addr, 0, MEM_RELEASE);
    }
}  // namespace ncore

#elif defined(TARGET_LINUX) || defined(TARGET_MAC)

#    include <unistd.h>
#    include <sys/mman.h>

#    include "ccore/c_arena.h"
#    include "ccore/c_math.h"
#    include "ccore/c_memory.h"

namespace ncore
{
    static s32 s_page_size = 0;
    s32        v_alloc_get_page_size()
    {
        if (s_page_size == 0)
            s_page_size = sysconf(_SC_PAGESIZE);
        return s_page_size;
    }

    void* v_alloc_reserve(int_t size)
    {
        void* ptr = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
        return ptr == MAP_FAILED ? nullptr : ptr;
    }

    bool v_alloc_commit(void* addr, int_t size)
    {
        const s32 result = mprotect(addr, size, PROT_READ | PROT_WRITE);
        return result == 0;
    }

    bool v_alloc_decommit(void* addr, int_t extra_size)
    {
        s32 result = madvise(addr, extra_size, MADV_DONTNEED);
        if (result == 0)
        {
            result = mprotect(addr, extra_size, PROT_NONE);
        }
        return result == 0;
    }

    bool v_alloc_release(void* addr, int_t size)
    {
        // munmap returns 0 on success
        return munmap(addr, size) == 0;
    }
}  // namespace ncore

#else

#    include "ccore/c_arena.h"
#    include "ccore/c_math.h"
#    include "ccore/c_memory.h"

namespace ncore
{
    s32   v_alloc_get_page_size() { return 4 * cKB; }
    void *v_alloc_reserve(int_t size)
    {
        CC_UNUSED(size);
        return nullptr;
    }
    bool v_alloc_commit(void *addr, int_t size)
    {
        CC_UNUSED(addr);
        CC_UNUSED(size);
        return false;
    }
    bool v_alloc_decommit(void *addr, int_t extra_size)
    {
        CC_UNUSED(addr);
        CC_UNUSED(extra_size);
        return false;
    }
    bool v_alloc_release(void *addr, int_t size)
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
            const s32 page_size = v_alloc_get_page_size();
            s_page_size_shift   = (u8)math::ilog2(page_size);
        }
        return s_page_size_shift;
    }
}  // namespace ncore

#if defined(TARGET_MAC) || defined(TARGET_LINUX) || defined(TARGET_PC)

namespace ncore
{
    namespace narena
    {
        enum ownership_t
        {
            ARENA_IS_OWNER  = 0,
            ARENA_NOT_OWNER = 1
        };

        arena_t *s_new_arena(int_t _header_size, int_t _reserve_size, int_t _commit_size)
        {
            const s32   page_size       = v_alloc_get_page_size();
            const u8    page_size_shift = v_alloc_get_page_size_shift();
            const int_t reserved_size   = math::alignUp(_reserve_size, (int_t)page_size);
            const int_t header_size     = math::alignUp(_header_size, (int_t)page_size);
            const int_t commit_size     = math::alignUp(_commit_size, (int_t)page_size);
            const int_t base_size       = header_size;

            byte *base_address = (byte *)v_alloc_reserve(reserved_size + base_size);
            if (base_address == nullptr)
                return nullptr;

            const bool result = v_alloc_commit(base_address, header_size + commit_size);
            if (!result)
            {
                v_alloc_release(base_address, reserved_size + base_size);
                return nullptr;
            }

            arena_t *arena           = (arena_t *)base_address;
            arena->m_base            = base_address + base_size;
            arena->m_pos             = 0;
            arena->m_header_pages    = (s32)(header_size >> page_size_shift);
            arena->m_reserved_pages  = (s32)(reserved_size >> page_size_shift);
            arena->m_committed_pages = (s32)(commit_size >> page_size_shift);
            arena->m_page_size_shift = page_size_shift;
            arena->m_ownership       = ARENA_IS_OWNER;
            arena->m_padding1        = 0;
            return arena;
        }

        arena_t *new_arena(int_t reserve_size, int_t commit_size)
        {
            arena_t *arena = s_new_arena(sizeof(arena_t), reserve_size, commit_size);
            return arena;
        }

        arena_t *init_arena(void *base, int_t reserve_size, int_t commit_size)
        {
            const u8    page_size_shift = v_alloc_get_page_size_shift();
            const u32   page_size       = (u32)1u << page_size_shift;
            const int_t header_size     = page_size;
            arena_t    *ar              = (arena_t *)base;
            if (!v_alloc_commit(base, header_size + commit_size))
                return nullptr;

            ar->m_base            = (byte *)base + header_size;
            ar->m_pos             = 0;
            ar->m_header_pages    = 1;
            ar->m_reserved_pages  = (s32)(reserve_size >> page_size_shift);
            ar->m_committed_pages = (s32)(commit_size >> page_size_shift);
            ar->m_page_size_shift = page_size_shift;
            ar->m_ownership       = ARENA_NOT_OWNER;
            ar->m_padding1        = 0;

            return ar;
        }

        void init_arena(arena_t *ar, void *base, int_t reserve_size, int_t commit_size)
        {
            const u8 page_size_shift = v_alloc_get_page_size_shift();
            ar->m_base               = (byte *)base;
            ar->m_pos                = 0;
            ar->m_header_pages       = 0;
            ar->m_reserved_pages     = (s32)(reserve_size >> page_size_shift);
            ar->m_committed_pages    = (s32)(commit_size >> page_size_shift);
            ar->m_page_size_shift    = page_size_shift;
            ar->m_ownership          = ARENA_NOT_OWNER;
            ar->m_padding1           = 0;
        }

        bool commit(arena_t *ar, int_t committed_size_in_bytes)
        {
            const s32 want_committed_pages = math::max((s32)(math::alignUp(committed_size_in_bytes, (int_t)1 << ar->m_page_size_shift) >> ar->m_page_size_shift), 1);
            const s32 total_reserved_pages = ar->m_reserved_pages;
            const s8  page_size_shift      = ar->m_page_size_shift;

            byte *base_address            = base_ptr(ar);
            s32   current_committed_pages = ar->m_committed_pages;

            if (want_committed_pages > current_committed_pages)
            {
                const int_t extra_needed_pages = (int_t)(want_committed_pages - current_committed_pages);
                if ((current_committed_pages + extra_needed_pages) > total_reserved_pages)
                    return current_committed_pages;
                const bool result = v_alloc_commit(base_address + ((int_t)current_committed_pages << page_size_shift), extra_needed_pages << page_size_shift);
                if (!result)
                    return current_committed_pages;
                current_committed_pages += (s32)extra_needed_pages;

                const bool success    = (current_committed_pages == want_committed_pages);
                ar->m_committed_pages = current_committed_pages;
                return success;
            }

            // don't run the logic to shrink
            // else if (want_committed_pages < current_committed_pages)
            // {
            //     // decommit the extra pages
            //     const int_t decommit_size_in_bytes = ((int_t)current_committed_pages - want_committed_pages) << page_size_shift;
            //     byte       *decommit_start         = base_address + (want_committed_pages << page_size_shift);
            //     if (!v_alloc_decommit(decommit_start, decommit_size_in_bytes))
            //         return current_committed_pages;
            //     current_committed_pages = (s32)want_committed_pages;
            // }

            return true;
        }

        // commits (allocate) size number of bytes and possibly grows the committed region.
        // returns a pointer to the allocated memory or nullptr if allocation failed.
        void *alloc(arena_t *ar, int_t size_in_bytes)
        {
            if (size_in_bytes == 0)
                return nullptr;  // we will consider this an error

            // When allocating, will our pointer stay within our committed region, if not we
            // need to see if we can commit the needed extra pages.
            const u32 need_committed_pages = (u32)(math::alignUp(ar->m_pos + size_in_bytes, (int_t)1 << ar->m_page_size_shift) >> ar->m_page_size_shift);
            if (need_committed_pages > ar->m_committed_pages)
            {
                if (need_committed_pages > ar->m_reserved_pages)
                    return nullptr;
                const bool result = v_alloc_commit(base_ptr(ar) + ((int_t)ar->m_committed_pages << ar->m_page_size_shift), (need_committed_pages - ar->m_committed_pages) << ar->m_page_size_shift);
                if (!result)
                    return nullptr;
                ar->m_committed_pages = need_committed_pages;
            }
            void *ptr = base_ptr(ar) + ar->m_pos;
            ar->m_pos += size_in_bytes;
            return ptr;
        }

        void *alloc(arena_t *ar, int_t size, u32 align)
        {
            if (size == 0)
                return nullptr;

            // ensure alignment is a power of two
            ASSERTS(math::ispo2(align), "Error: alignment value should be a power of 2");

            // align the position to the requested alignment
            const int_t aligning = (math::alignUp(ar->m_pos, (int_t)align) - ar->m_pos);

            // we let our core commit function handle the rest
            void *ptr = alloc(ar, size + aligning);

            // out of memory ?
            if (ptr == nullptr)
                return nullptr;

            // align the pointer to the given alignment
            ptr = (void *)((byte *)ptr + aligning);
            return ptr;
        }

        void *alloc_and_zero(arena_t *ar, int_t size)
        {
            void *ptr = alloc(ar, size);
            nmem::memset(ptr, 0, size);
            return ptr;
        }

        void *alloc_and_zero(arena_t *ar, int_t size, u32 alignment)
        {
            void *ptr = alloc(ar, size, alignment);
            nmem::memset(ptr, 0, size);
            return ptr;
        }

        void *alloc_and_fill(arena_t *ar, int_t size, u32 fill)
        {
            void *ptr = alloc(ar, size);
            nmem::memset(ptr, fill, size);
            return ptr;
        }

        void *alloc_and_fill(arena_t *ar, int_t size, u32 alignment, u32 fill)
        {
            void *ptr = alloc(ar, size, alignment);
            nmem::memset(ptr, fill, size);
            return ptr;
        }

        // save the address of the current allocation point
        void *current_address(arena_t *ar) { return (void *)(base_ptr(ar) + ar->m_pos); }

        // restore the arena to a saved address
        void restore_address(arena_t *ar, void *ptr)
        {
            ASSERT(ptr >= base_ptr(ar));
            const int_t position = (int_t)((byte *)ptr - base_ptr(ar));
            ASSERT(position >= 0 && position < (ar->m_committed_pages << ar->m_page_size_shift));
#    ifdef TARGET_DEBUG
            // clear the memory that is being 'freed' for debug purposes
            if ((ar->m_pos - position) > 0)
                nmem::memset(base_ptr(ar) + position, 0xFEFEFEFE, ar->m_pos - position);
#    endif
            ar->m_pos = position;
        }

        void shrink(arena_t *ar)
        {
            // ensure current used memory is aligned up to page size
            const int_t used_bytes           = math::alignUp(ar->m_pos, ((int_t)1 << ar->m_page_size_shift));
            const u32   used_committed_pages = (u32)(used_bytes >> ar->m_page_size_shift);

            if (used_committed_pages < ar->m_committed_pages)
            {
                byte       *decommit_start         = base_ptr(ar) + used_bytes;
                const int_t decommit_size_in_bytes = (int_t)(ar->m_committed_pages - used_committed_pages) << ar->m_page_size_shift;
                if (decommit_size_in_bytes > 0)
                {
                    if (v_alloc_decommit(decommit_start, decommit_size_in_bytes))
                    {
                        ar->m_committed_pages = used_committed_pages;
                    }
                }
            }
        }

        void reset(arena_t *ar) { ar->m_pos = 0; }

        bool destroy(arena_t *&ar)
        {
            if (ar == nullptr)
                return false;

            if (ar->m_ownership == ARENA_NOT_OWNER)
            {
                if (ar->m_header_pages > 0)
                {
                    // decommit (header region + committed region) at once since they are contiguous
                    v_alloc_decommit((byte *)ar, ((ar->m_header_pages + ar->m_committed_pages) << ar->m_page_size_shift));
                }
                else
                {
                    // arena is not part of the virtual memory, so we just decommit the committed region if there is
                    // one, and we don't have to worry about the header region since it's not part of the virtual memory
                    if (ar->m_committed_pages > 0)
                    {
                        v_alloc_decommit(base_ptr(ar), (ar->m_committed_pages << ar->m_page_size_shift));
                    }
                }
                ar = nullptr;
                return true;  // we don't own the virtual memory, so we just nullify the pointer
            }

            // decommit all committed pages and release the reserved region
            // check if the header and base have a gap in committed memory
            byte       *reserved_address      = (byte *)ar;
            const int_t reserved_address_size = (base_ptr(ar) + ((int_t)ar->m_reserved_pages << ar->m_page_size_shift)) - reserved_address;
            if (reserved_address + (ar->m_header_pages << ar->m_page_size_shift) < base_ptr(ar))
            {
                // decommit header region
                v_alloc_decommit(reserved_address, (ar->m_header_pages << ar->m_page_size_shift));
                // decommit committed region
                v_alloc_decommit(base_ptr(ar), (ar->m_committed_pages << ar->m_page_size_shift));
            }
            else
            {
                // decommit all at once
                v_alloc_decommit(reserved_address, ((ar->m_committed_pages + ar->m_header_pages) << ar->m_page_size_shift));
            }

            // release the reserved address space
            v_alloc_release(reserved_address, reserved_address_size);

            ar = nullptr;
            return true;
        }

    }  // namespace narena
}  // namespace ncore

#else

namespace ncore
{
    namespace narena
    {
        arena_t* create(int_t reserve_size, int_t commit_size)
        {
            CC_UNUSED(reserve_size);
            CC_UNUSED(commit_size);
            return nullptr;
        }

        bool commit(arena_t* ar, int_t committed_size_in_bytes)
        {
            CC_UNUSED(ar);
            CC_UNUSED(committed_size_in_bytes);
            return false;
        }

        bool commit_from_address(arena_t* ar, void* address, int_t size_in_bytes)
        {
            CC_UNUSED(ar);
            CC_UNUSED(address);
            CC_UNUSED(size_in_bytes);
            return false;
        }

        // commits (allocate) size number of bytes and possibly grows the committed region.
        // returns a pointer to the allocated memory or nullptr if allocation failed.
        void* alloc(arena_t* ar, int_t size)
        {
            CC_UNUSED(ar);
            CC_UNUSED(size);
            return nullptr;  // we will consider this an error
        }

        void* alloc(arena_t* ar, int_t size, s32 alignment)
        {
            CC_UNUSED(ar);
            CC_UNUSED(size);
            CC_UNUSED(alignment);
            return nullptr;
        }

        void* alloc_and_zero(arena_t* ar, int_t size)
        {
            CC_UNUSED(ar);
            CC_UNUSED(size);
            return nullptr;
        }

        void* alloc_and_zero(arena_t* ar, int_t size, s32 alignment)
        {
            CC_UNUSED(ar);
            CC_UNUSED(size);
            CC_UNUSED(alignment);
            return nullptr;
        }

        void* alloc_and_fill(arena_t* ar, int_t size, u32 fill)
        {
            CC_UNUSED(ar);
            CC_UNUSED(size);
            CC_UNUSED(fill);
            return nullptr;
        }

        void* alloc_and_fill(arena_t* ar, int_t size, u32 alignment, u32 fill)
        {
            CC_UNUSED(ar);
            CC_UNUSED(size);
            CC_UNUSED(alignment);
            CC_UNUSED(fill);
            return nullptr;
        }

        void* current_address(arena_t* ar)
        {
            CC_UNUSED(ar);
            return nullptr;
        }

        void restore_address(arena_t* ar, void* ptr)
        {
            CC_UNUSED(ar);
            CC_UNUSED(ptr);
        }

        void shrink(arena_t* ar) { CC_UNUSED(ar); }
        void reset(arena_t* ar) { CC_UNUSED(ar); }
        bool destroy(arena_t* ar)
        {
            CC_UNUSED(ar);
            return false;
        }
    }  // namespace narena
}  // namespace ncore

#endif

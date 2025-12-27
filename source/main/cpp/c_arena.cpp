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
        // VirtualFree(base_addr + 1MB, MEM_DECOMMIT, size);
        /*
            "The VirtualFree function can decommit a range of pages that are in
            different states, some committed and some uncommitted. This means
            that you can decommit a range of pages without first determining
            the state of each page."
        */
        BOOL success = VirtualFree(addr, MEM_DECOMMIT, (DWORD)size);
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

#if defined(TARGET_MAC) || defined(TARGET_LINUX) || defined(TARGET_PC)

namespace ncore
{
    namespace narena
    {
        arena_t *sNewArena(int_t _header_size, int_t _reserve_size, int_t _commit_size, s8 _arena_alignment_shift)
        {
            const s32   page_size       = v_alloc_get_page_size();
            const s8    page_size_shift = math::ilog2(page_size);
            const int_t reserved_size   = math::alignUp(_reserve_size, (int_t)page_size);
            const int_t header_size     = math::alignUp(_header_size, (int_t)page_size);
            const int_t alignment_size  = math::alignUp(header_size, 1 << _arena_alignment_shift);
            const int_t commit_size     = math::alignUp(_commit_size, (int_t)page_size);
            const int_t base_size       = math::max(header_size, alignment_size);

            byte *base_address = (byte *)v_alloc_reserve(reserved_size + base_size);
            if (base_address == nullptr)
                return nullptr;

            if (header_size == base_size)
            {
                // Header and Alignment don't generate a gap, can commit in one go
                const bool result = v_alloc_commit(base_address, header_size + commit_size);
                if (!result)
                {
                    v_alloc_release(base_address, reserved_size + base_size);
                    return nullptr;
                }
            }
            else
            {
                // A gap exists between Header and the Alignment, commit separately, instead of wasting memory
                {
                    const bool result = v_alloc_commit(base_address, header_size);
                    if (!result)
                    {
                        v_alloc_release(base_address, reserved_size + base_size);
                        return nullptr;
                    }
                }
                {
                    const bool result = v_alloc_commit(base_address + alignment_size, commit_size);
                    if (!result)
                    {
                        v_alloc_release(base_address, reserved_size + base_size);
                        return nullptr;
                    }
                }
            }

            arena_t *arena           = (arena_t *)base_address;
            arena->m_base            = base_address + base_size;
            arena->m_pos             = 0;
            arena->m_header_pages    = (s32)(header_size >> page_size_shift);
            arena->m_reserved_pages  = (s32)(reserved_size >> page_size_shift);
            arena->m_committed_pages = (s32)(commit_size >> page_size_shift);
            arena->m_page_size_shift = page_size_shift;
            arena->m_alignment_shift = _arena_alignment_shift;

            return arena;
        }

        arena_t *new_arena(int_t reserve_size, int_t commit_size, s8 arena_alignment_shift)
        {
            arena_t *arena = sNewArena(sizeof(arena_t), reserve_size, commit_size, arena_alignment_shift);
            return arena;
        }

        bool commit(arena_t *ar, int_t committed_size_in_bytes)
        {
            const s32 want_committed_pages = math::max((s32)(math::alignUp(committed_size_in_bytes, (int_t)1 << ar->m_page_size_shift) >> ar->m_page_size_shift), 1);
            const s32 total_reserved_pages = ar->m_reserved_pages;
            const s8  page_size_shift      = ar->m_page_size_shift;

            byte *base_address            = base(ar);
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
            }
            else if (want_committed_pages < current_committed_pages)
            {
                // decommit the extra pages
                const int_t decommit_size_in_bytes = ((int_t)current_committed_pages - want_committed_pages) << page_size_shift;
                byte       *decommit_start         = base_address + (want_committed_pages << page_size_shift);
                if (!v_alloc_decommit(decommit_start, decommit_size_in_bytes))
                    return current_committed_pages;
                current_committed_pages = (s32)want_committed_pages;
            }

            const bool success    = (current_committed_pages == want_committed_pages);
            ar->m_committed_pages = current_committed_pages;
            return success;
        }

        // commits (allocate) size number of bytes and possibly grows the committed region.
        // returns a pointer to the allocated memory or nullptr if allocation failed.
        void *alloc(arena_t *ar, int_t size)
        {
            if (size == 0)
                return nullptr;  // we will consider this an error

            // align the size to the minimum alignment for this arena
            const int_t size_in_bytes = math::alignUp(size, narena::alignment(ar));

            // When allocating, will our pointer stay within our committed region, if not we
            // need to see if we can commit the needed extra pages.
            const u32 need_committed_pages = (u32)(math::alignUp(ar->m_pos + size_in_bytes, (int_t)1 << ar->m_page_size_shift) >> ar->m_page_size_shift);
            if (need_committed_pages > ar->m_committed_pages)
            {
                if (need_committed_pages > ar->m_reserved_pages)
                    return nullptr;
                const bool result = v_alloc_commit(base(ar) + ((int_t)ar->m_committed_pages << ar->m_page_size_shift), (need_committed_pages - ar->m_committed_pages) << ar->m_page_size_shift);
                if (!result)
                    return nullptr;
                ar->m_committed_pages = need_committed_pages;
            }
            void *ptr = base(ar) + ar->m_pos;
            ar->m_pos += size_in_bytes;
            return ptr;
        }

        void *alloc(arena_t *ar, int_t size, u32 align)
        {
            if (size == 0)
                return nullptr;

            align = math::max(align, narena::alignment(ar));

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
        void *current_address(arena_t *ar) { return (void *)(base(ar) + ar->m_pos); }

        // restore the arena to a saved address
        void restore_address(arena_t *ar, void *ptr)
        {
            ASSERT(ptr >= base(ar));
            const int_t position = (int_t)((byte *)ptr - base(ar));
            ASSERT(position >= 0 && position < (ar->m_committed_pages << ar->m_page_size_shift));
#    ifdef TARGET_DEBUG
            // clear the memory that is being 'freed' for debug purposes
            if ((ar->m_pos - position) > 0)
                nmem::memset(base(ar) + position, 0xFEFEFEFE, ar->m_pos - position);
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
                byte       *decommit_start         = base(ar) + used_bytes;
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

            ASSERT(ar->m_header_pages > 0);

            // decommit all committed pages and release the reserved region
            // check if the header and base have a gap in committed memory
            byte       *reserved_address      = (byte *)ar;
            const int_t reserved_address_size = (ar->m_base + ((int_t)ar->m_reserved_pages << ar->m_page_size_shift)) - reserved_address;
            if (reserved_address + (ar->m_header_pages << ar->m_page_size_shift) < ar->m_base)
            {
                // decommit header region
                v_alloc_decommit(reserved_address, (ar->m_header_pages << ar->m_page_size_shift));
                // decommit committed region
                v_alloc_decommit(ar->m_base, (ar->m_committed_pages << ar->m_page_size_shift));
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

        struct stack_t
        {
            arena_t *m_arena;
            s32      m_cur_depth;
            s32      m_max_depth;
            void   **m_stack;
        };

        stack_t *new_stack(int_t reserve_size, int_t commit_size, s32 max_depth)
        {
            ASSERT(max_depth <= 1024 && max_depth > 0);

            const s32 header_size = (s32)(sizeof(arena_t) + sizeof(stack_t) + (sizeof(void *) * max_depth));

            arena_t *arena = sNewArena(header_size, reserve_size, commit_size, 3);

            stack_t *stack     = (stack_t *)(arena + 1);
            stack->m_arena     = arena;
            stack->m_cur_depth = 0;
            stack->m_max_depth = max_depth;
            stack->m_stack     = (void **)(stack + 1);

            return stack;
        }

        void destroy(stack_t *&s)
        {
            if (s != nullptr && s->m_arena != nullptr)
            {
                narena::destroy(s->m_arena);
                s = nullptr;
            }
        }

        arena_t *push_stack(stack_t *s)
        {
            if (s->m_cur_depth >= s->m_max_depth)
                return nullptr;

            s->m_stack[s->m_cur_depth] = narena::current_address(s->m_arena);
            s->m_cur_depth++;

            return s->m_arena;
        }

        bool pop_stack(stack_t *s)
        {
            if (s->m_cur_depth <= 0)
                return false;

            s->m_cur_depth--;
            void *addr = s->m_stack[s->m_cur_depth];
            narena::restore_address(s->m_arena, addr);

            return true;
        }

        struct region_t
        {
            arena_t *m_arena;
            arena_t *m_arena_array;
            s16      m_num_arenas;
        };

        region_t *new_region(int_t region_reserve_size, int_t arena_reserve_size, s8 arena_alignment_shift, u16 num_arenas)
        {
            // What is sane for the amount of requested arenas?
            ASSERT(num_arenas <= 256 && num_arenas > 0);

            // Align the requested region size to page size
            const s32 page_size            = v_alloc_get_page_size();
            const s8  page_size_shift      = math::ilog2(page_size);
            const s32 region_reserve_pages = (s32)(math::alignUp(region_reserve_size, (int_t)1 << page_size_shift) >> page_size_shift);

            // Align the header size to pages
            const s32 header_size  = (s32)(sizeof(arena_t) + sizeof(region_t) + (sizeof(arena_t) * num_arenas));
            const s32 header_pages = (s32)(math::alignUp(header_size, (int_t)1 << page_size_shift) >> page_size_shift);
            arena_t  *arena        = sNewArena((int_t)header_pages << page_size_shift, (int_t)region_reserve_pages << page_size_shift, 0, arena_alignment_shift);

            // Align the arena reserve size to page size
            const u32 arena_reserve_pages = (u32)(math::alignUp(arena_reserve_size, (int_t)1 << arena->m_page_size_shift) >> arena->m_page_size_shift);

            // Does this compute? arena->m_reserved_pages should be enough to hold all the arenas
            if (arena == nullptr || (arena->m_reserved_pages < (arena_reserve_pages * num_arenas)))
            {
                if (arena != nullptr)
                {
                    v_alloc_release((byte *)arena, (int_t)region_reserve_pages << page_size_shift);
                }
                return nullptr;
            }

            region_t *region      = (region_t *)(arena + 1);
            region->m_arena       = arena;
            region->m_arena_array = (arena_t *)(region + 1);
            region->m_num_arenas  = num_arenas;

            // initialize the arena's, handout 'arena_reserve_size' to each arena
            arena_t *arena_array  = region->m_arena_array;
            byte    *base_address = (byte *)region->m_arena->m_base;
            for (s16 i = 0; i < num_arenas; i++)
            {
                arena_t *a           = &arena_array[i];
                a->m_base            = base_address;
                a->m_pos             = 0;
                a->m_header_pages    = 0;
                a->m_reserved_pages  = arena_reserve_pages;
                a->m_committed_pages = 0;
                a->m_page_size_shift = region->m_arena->m_page_size_shift;
                a->m_alignment_shift = region->m_arena->m_alignment_shift;

                base_address += (arena_reserve_pages << a->m_page_size_shift);
            }

            return region;
        }

        void destroy(region_t *region)
        {
            // todo
        }

        arena_t *get_arena(region_t *region, s16 index)
        {
            // todo
            return nullptr;
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

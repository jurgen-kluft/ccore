#include "ccore/c_vmem.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"

#if defined(TARGET_PC)
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>

namespace ncore
{
    s32 v_alloc_get_page_size()
    {
        SYSTEM_INFO sys_info;
        GetSystemInfo(&sys_info);
        return sys_info.dwPageSize;
    }

    void *v_alloc_reserve(int_t size) { return VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_NOACCESS); }

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
            the current commitment state of each page."
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

namespace ncore
{
    s32 v_alloc_get_page_size()
    {
        const s32 page_size = sysconf(_SC_PAGESIZE);
        if (page_size <= 0)
        {
            return 0;
        }
        return page_size;
    }
    void *v_alloc_reserve(int_t size)
    {
        void *ptr = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
        return ptr == MAP_FAILED ? nullptr : ptr;
    }
    bool v_alloc_commit(void *addr, int_t size)
    {
        const s32 result = mprotect(addr, size, PROT_READ | PROT_WRITE);
        return result == 0;
    }
    bool v_alloc_decommit(void *addr, int_t extra_size)
    {
        s32 result = madvise(addr, extra_size, MADV_DONTNEED);
        if (result == 0)
        {
            result = mprotect(addr, extra_size, PROT_NONE);
        }
        return result == 0;
    }
    bool v_alloc_release(void *addr, int_t size) { return munmap(addr, size) == 0; }
}  // namespace ncore

#else

namespace ncore
{
    s32   v_alloc_get_page_size() { return 0; }
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
    static s32 sCommit(byte *base_address, s32 current_committed_pages, int_t want_committed_pages, s32 total_reserved_pages, s16 page_size_shift)
    {
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
        return current_committed_pages;
    }

    arena_t *gCreateArena(int_t reserve_size, int_t commit_size)
    {
        const s32 minimum_pages   = 4;
        const s32 page_size       = v_alloc_get_page_size();
        const s8  page_size_shift = math::g_ilog2(page_size);
        const s32 reserve_pages   = math::g_max((s32)(math::g_alignUp(reserve_size, page_size) >> page_size_shift), minimum_pages);

        byte *base_address = (byte *)v_alloc_reserve((int_t)reserve_pages << page_size_shift);
        if (base_address == nullptr)
            return nullptr;

        const s32 commit_pages    = math::g_max((s32)(math::g_alignUp(commit_size, (int_t)page_size) >> page_size_shift), minimum_pages);
        s32       committed_pages = sCommit(base_address, 0, commit_pages, reserve_pages, page_size_shift);
        if (committed_pages == 0)
        {
            v_alloc_release(base_address, reserve_size);
            return nullptr;
        }
        arena_t *arena            = (arena_t *)base_address;
        arena->m_pos              = sizeof(arena_t);
        arena->m_page_size_shift  = page_size_shift;
        arena->m_alignment_shift  = 4;  // default alignment shift (4 = 16 bytes)
        arena->m_reserved_pages   = reserve_pages;
        arena->m_committed_pages  = committed_pages;
        arena->m_pages_commit_min = minimum_pages;
        return arena;
    }

    bool arena_t::committed(int_t committed_size_in_bytes)
    {
        const s32  want_committed_pages = math::g_max((s32)(math::g_alignUp(committed_size_in_bytes + sizeof(arena_t), (int_t)1 << m_page_size_shift) >> m_page_size_shift), m_pages_commit_min);
        const s32  committed_pages      = sCommit((byte *)this, m_committed_pages, want_committed_pages, m_reserved_pages, m_page_size_shift);
        const bool success              = (committed_pages == want_committed_pages);
        m_committed_pages               = committed_pages;
        return success;
    }

    // commits (allocate) size number of bytes and possibly grows the committed region.
    // returns a pointer to the allocated memory or nullptr if allocation failed.
    void *arena_t::alloc(int_t size)
    {
        if (size == 0)
            return nullptr;  // we will consider this an error

        // align the size to the default alignment
        const int_t size_in_bytes = math::g_alignUp(size, alignment());

        // When allocating, will our pointer stay within our committed region, if not we
        // need to commit more pages.
        if ((m_pos + size_in_bytes) > (m_committed_pages << m_page_size_shift))
        {
            const int_t size_request_in_pages = math::g_alignUp(size_in_bytes, (int_t)1 << m_page_size_shift) >> m_page_size_shift;
            const int_t used_committed_pages  = math::g_alignUp(m_pos, (int_t)1 << m_page_size_shift) >> m_page_size_shift;
            if ((used_committed_pages + size_request_in_pages) > m_committed_pages)
            {
                if ((m_committed_pages + size_request_in_pages) > m_reserved_pages)
                    return nullptr;

                int_t extra_needed_pages = math::g_max((used_committed_pages + size_request_in_pages) - m_committed_pages, (int_t)m_pages_commit_min);
                extra_needed_pages       = math::g_min(extra_needed_pages, (int_t)(m_reserved_pages - m_committed_pages));
                if ((m_committed_pages + extra_needed_pages) > m_reserved_pages)
                    return nullptr;

                const bool result = v_alloc_commit((byte *)this + ((int_t)m_committed_pages << m_page_size_shift), extra_needed_pages << m_page_size_shift);
                if (!result)
                    return nullptr;
                m_committed_pages += (s32)extra_needed_pages;
            }
        }
        void *ptr = (byte *)this + m_pos;
        m_pos += size_in_bytes;
        return ptr;
    }

    void *arena_t::alloc(int_t size, s32 align)
    {
        if (size == 0 || align <= 0)
            return nullptr;

        align = math::g_max((int_t)align, alignment());

        // ensure alignment is a power of two
        ASSERTS(math::g_ispo2(align), "Error: alignment value should be a power of 2");

        // align the position to the requested alignment
        const int_t aligning = (math::g_alignUp(m_pos, (int_t)align) - m_pos);

        // we let our core commit function handle the rest
        void *ptr = alloc(size + aligning);

        // out of memory ?
        if (ptr == nullptr)
            return nullptr;

        // align the pointer to the given alignment
        ptr = (void *)((byte *)ptr + aligning);
        return ptr;
    }

    void *arena_t::alloc_and_zero(int_t size)
    {
        void *ptr = alloc(size);
        nmem::memset(ptr, 0, size);
        return ptr;
    }

    void *arena_t::alloc_and_zero(int_t size, s32 alignment)
    {
        void *ptr = alloc(size, alignment);
        nmem::memset(ptr, 0, size);
        return ptr;
    }

    int_t arena_t::save_point() const { return m_pos; }

    void arena_t::restore_point(int_t pos)
    {
        ASSERT(pos >= 0 && pos <= m_pos);
#    ifdef TARGET_DEBUG
        // clear the memory that is being 'freed' for debug purposes
        nmem::memset((byte *)this + pos, 0xFEFEFEFE, m_pos - pos);
#    endif
        m_pos = pos;
    }

    void arena_t::shrink()
    {
        // ensure current used memory is aligned up to page size
        const int_t used_in_bytes = math::g_alignUp(m_pos, ((int_t)1 << m_page_size_shift));
        const int_t used_in_pages = used_in_bytes >> m_page_size_shift;

        byte       *decommit_start         = (byte *)this + used_in_bytes;
        const int_t decommit_size_in_bytes = (m_committed_pages - used_in_pages) << m_page_size_shift;
        if (decommit_size_in_bytes > 0)
        {
            if (v_alloc_decommit(decommit_start, decommit_size_in_bytes))
            {
                m_committed_pages = (s32)used_in_pages;
            }
        }
    }

    void arena_t::reset() { m_pos = 0; }

    bool arena_t::release()
    {
        const bool partof_vmem = ((byte *)this >= (byte *)this && (byte *)this < ((byte *)this + (m_reserved_pages << m_page_size_shift)));
        if (partof_vmem)
        {
            return v_alloc_release((void *)this, m_reserved_pages << m_page_size_shift);
        }

        if (v_alloc_release((void *)this, m_reserved_pages << m_page_size_shift))
        {
            m_pos              = 0;
            m_alignment_shift  = 4;  // default alignment (1 << 4) = 16 bytes
            m_reserved_pages   = 0;
            m_committed_pages  = 0;
            m_page_size_shift  = 0;
            m_pages_commit_min = 4;
            return true;
        }
        return false;
    }
}  // namespace ncore

#else

namespace ncore
{
    bool arena_t::reserved(int_t reserve_size)
    {
        CC_UNUSED(reserve_size);
        return false;  // reserve failed
    }

    bool arena_t::committed(int_t committed_size_in_bytes)
    {
        CC_UNUSED(committed_size_in_bytes);
        return false;
    }

    // commits (allocate) size number of bytes and possibly grows the committed region.
    // returns a pointer to the allocated memory or nullptr if allocation failed.
    void *arena_t::commit(int_t size)
    {
        CC_UNUSED(size);
        return nullptr;  // we will consider this an error
    }

    void *arena_t::commit(int_t size, s32 alignment)
    {
        CC_UNUSED(size);
        CC_UNUSED(alignment);
        return nullptr;
    }

    void *arena_t::commit_and_zero(int_t size)
    {
        CC_UNUSED(size);
        return nullptr;
    }

    void *arena_t::commit_and_zero(int_t size, s32 alignment)
    {
        CC_UNUSED(size);
        CC_UNUSED(alignment);
        return nullptr;
    }

    int_t arena_t::save_point() const { return m_pos; }
    void  arena_t::restore_point_point(int_t size) { m_pos = size; }
    void  arena_t::shrink() {}
    void  arena_t::reset() { m_pos = 0; }
    bool  arena_t::release() { return true; }
}  // namespace ncore

#endif

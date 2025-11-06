#include "ccore/c_vmem.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"

#if defined(TARGET_PC)
#    define WIN32_LEAN_AND_MEAN
#    include <windows.h>

namespace ncore
{
    static s32 v_alloc_get_page_size()
    {
        SYSTEM_INFO sys_info;
        GetSystemInfo(&sys_info);
        return sys_info.dwPageSize;
    }

    static void *v_alloc_reserve(int_t size) { return VirtualAlloc(nullptr, size, MEM_RESERVE, PAGE_NOACCESS); }

    static bool v_alloc_commit(void *addr, int_t size)
    {
        void *result = VirtualAlloc(addr, size, MEM_COMMIT, PAGE_READWRITE);
        return result != nullptr;
    }

    static bool v_alloc_decommit(void *addr, int_t size)
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
    static bool v_alloc_release(void *addr, int_t size)
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
    static s32 v_alloc_get_page_size()
    {
        const s32 page_size = sysconf(_SC_PAGESIZE);
        if (page_size <= 0)
        {
            return 0;
        }
        return page_size;
    }
    static void *v_alloc_reserve(int_t size)
    {
        void *ptr = mmap(nullptr, size, PROT_NONE, MAP_PRIVATE | MAP_ANON, -1, 0);
        return ptr == MAP_FAILED ? nullptr : ptr;
    }
    static bool v_alloc_commit(void *addr, int_t size)
    {
        const s32 result = mprotect(addr, size, PROT_READ | PROT_WRITE);
        return result == 0;
    }
    static bool v_alloc_decommit(void *addr, int_t extra_size)
    {
        s32 result = madvise(addr, extra_size, MADV_DONTNEED);
        if (result == 0)
        {
            result = mprotect(addr, extra_size, PROT_NONE);
        }
        return result == 0;
    }
    static bool v_alloc_release(void *addr, int_t size) { return munmap(addr, size) == 0; }
}  // namespace ncore

#else

namespace ncore
{
    static s32   v_alloc_get_page_size() { return 0; }
    static void *v_alloc_reserve(int_t size)
    {
        CC_UNUSED(size);
        return nullptr;
    }
    static bool v_alloc_commit(void *addr, int_t size)
    {
        CC_UNUSED(addr);
        CC_UNUSED(size);
        return false;
    }
    static bool v_alloc_decommit(void *addr, int_t extra_size)
    {
        CC_UNUSED(addr);
        CC_UNUSED(extra_size);
        return false;
    }
    static bool v_alloc_release(void *addr, int_t size)
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
    bool vmem_arena_t::reserved(int_t reserve_size)
    {
        const s32 page_size = v_alloc_get_page_size();
        reserve_size        = math::g_alignUp(reserve_size, page_size);
        m_page_size_shift   = math::g_ilog2(page_size);

        m_base = (byte *)v_alloc_reserve(reserve_size);
        if (m_base != nullptr)
        {
            m_pos              = 0;
            m_alignment        = 16;  // default alignment
            m_reserved_pages   = (s32)(reserve_size >> m_page_size_shift);
            m_committed_pages  = 0;
            m_pages_commit_min = 4;
            return true;
        }
        return false;  // reserve failed
    }

    bool vmem_arena_t::committed(int_t committed_size_in_bytes)
    {
        if (m_base == nullptr)
        {
            return false;
        }

        const int_t size_in_bytes = math::g_alignUp(committed_size_in_bytes, ((int_t)1 << m_page_size_shift));
        const int_t size_in_pages = size_in_bytes >> m_page_size_shift;

        if (size_in_pages > m_committed_pages)
        {
            int_t extra_needed_pages = math::g_max(size_in_pages - m_committed_pages, (int_t)m_pages_commit_min);
            extra_needed_pages       = math::g_min(extra_needed_pages, (int_t)(m_reserved_pages - m_committed_pages));
            if ((m_committed_pages + extra_needed_pages) > m_reserved_pages)
            {
                return false;
            }

            const bool result = v_alloc_commit(m_base + ((int_t)m_committed_pages << m_page_size_shift), extra_needed_pages << m_page_size_shift);
            if (!result)
            {
                return false;
            }
            m_committed_pages += (s32)extra_needed_pages;
        }
        else if (size_in_pages < m_committed_pages)
        {
            // decommit the extra pages
            const int_t decommit_size_in_bytes = ((int_t)m_committed_pages - size_in_pages) << m_page_size_shift;
            byte       *decommit_start         = m_base + (size_in_pages << m_page_size_shift);
            if (!v_alloc_decommit(decommit_start, decommit_size_in_bytes))
            {
                return false;
            }
            m_committed_pages = (s32)size_in_pages;
        }
        return true;
    }

    int_t vmem_arena_t::save() const { return m_pos; }

    // commits (allocate) size number of bytes and possibly grows the committed region.
    // returns a pointer to the allocated memory or nullptr if allocation failed.
    void *vmem_arena_t::commit(int_t size)
    {
        if (size == 0)
        {
            return nullptr;  // we will consider this an error
        }

        const int_t size_in_bytes = math::g_alignUp(size, (int_t)m_alignment);

        // When allocating, will our pointer stay within our committed region, if not we
        // need to commit more pages.
        if ((m_pos + size_in_bytes) > (m_committed_pages << m_page_size_shift))
        {
            if (m_base == nullptr)
            {
                if (!reserved(cDEFAULT_ARENA_CAPACITY))
                {
                    return nullptr;
                }
            }

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

                const bool result = v_alloc_commit(m_base + ((int_t)m_committed_pages << m_page_size_shift), extra_needed_pages << m_page_size_shift);
                if (!result)
                    return nullptr;
                m_committed_pages += (s32)extra_needed_pages;
            }
        }
        void *ptr = m_base + m_pos;
        m_pos += size_in_bytes;
        return ptr;
    }

    void *vmem_arena_t::commit(int_t size, s32 alignment)
    {
        if (size == 0 || alignment <= 0)
        {
            return nullptr;
        }

        alignment = math::g_max(alignment, m_alignment);

        // ensure alignment is a power of two
        ASSERTS(math::g_ispo2(alignment), "Error: alignment value should be a power of 2");

        // align the size to the default alignment
        size = math::g_alignUp(size, (int_t)m_alignment);

        // align the position to the given alignment
        const int_t pos      = math::g_alignUp(m_pos, (int_t)alignment);
        const int_t aligning = (pos - m_pos);

        void *ptr = commit(size + aligning);

        // align the pointer to the given alignment
        if (ptr == nullptr)
            return nullptr;

        ptr = (void *)((byte *)ptr + aligning);
        return ptr;
    }

    void *vmem_arena_t::commit_and_zero(int_t size)
    {
        void *ptr = commit(size);
        nmem::memset(ptr, 0, size);
        return ptr;
    }

    void *vmem_arena_t::commit_and_zero(int_t size, s32 alignment)
    {
        void *ptr = commit(size, alignment);
        nmem::memset(ptr, 0, size);
        return ptr;
    }

    void vmem_arena_t::restore(int_t size) { m_pos = size; }

    void vmem_arena_t::shrink()
    {
        // ensure current used memory is aligned up to page size
        const int_t used_in_bytes = math::g_alignUp(m_pos, ((int_t)1 << m_page_size_shift));
        const int_t used_in_pages = used_in_bytes >> m_page_size_shift;

        byte       *decommit_start         = m_base + used_in_bytes;
        const int_t decommit_size_in_bytes = (m_committed_pages - used_in_pages) << m_page_size_shift;
        if (decommit_size_in_bytes > 0)
        {
            if (v_alloc_decommit(decommit_start, decommit_size_in_bytes))
            {
                m_committed_pages = (s32)used_in_pages;
            }
        }
    }

    void vmem_arena_t::reset() { m_pos = 0; }

    bool vmem_arena_t::release()
    {
        if (m_base == nullptr)
            return true;

        const bool partof_vmem = ((byte *)this >= m_base && (byte *)this < (m_base + (m_reserved_pages << m_page_size_shift)));
        if (partof_vmem)
        {
            return v_alloc_release(m_base, m_reserved_pages << m_page_size_shift);
        }

        if (v_alloc_release(m_base, m_reserved_pages << m_page_size_shift))
        {
            m_base             = nullptr;
            m_pos              = 0;
            m_alignment        = 16;  // default alignment
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
    bool vmem_arena_t::reserved(int_t reserve_size)
    {
        CC_UNUSED(reserve_size);
        return false;  // reserve failed
    }

    bool vmem_arena_t::committed(int_t committed_size_in_bytes)
    {
        CC_UNUSED(committed_size_in_bytes);
        return false;
    }

    int_t vmem_arena_t::save() const { return m_pos; }

    // commits (allocate) size number of bytes and possibly grows the committed region.
    // returns a pointer to the allocated memory or nullptr if allocation failed.
    void *vmem_arena_t::commit(int_t size)
    {
        CC_UNUSED(size);
        return nullptr;  // we will consider this an error
    }

    void *vmem_arena_t::commit(int_t size, s32 alignment)
    {
        CC_UNUSED(size);
        CC_UNUSED(alignment);
        return nullptr;
    }

    void *vmem_arena_t::commit_and_zero(int_t size)
    {
        CC_UNUSED(size);
        return nullptr;
    }

    void *vmem_arena_t::commit_and_zero(int_t size, s32 alignment)
    {
        CC_UNUSED(size);
        CC_UNUSED(alignment);
        return nullptr;
    }

    void vmem_arena_t::restore(int_t size) { m_pos = size; }
    void vmem_arena_t::shrink() {}
    void vmem_arena_t::reset() { m_pos = 0; }
    bool vmem_arena_t::release() { return true; }
}  // namespace ncore

#endif

#include "ccore/c_vmem.h"
#include "ccore/c_math.h"

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
#    error "Unsupported platform"
#endif

namespace ncore
{
    bool vmem_allocator_t::reserve(int_t reserve_size)
    {
        const s32 page_size = v_alloc_get_page_size();
        reserve_size        = math::g_alignUp(reserve_size, (int_t)page_size);
        m_page_size_shift   = math::g_ilog2(page_size);

        m_base = (byte *)v_alloc_reserve(reserve_size);
        if (m_base != nullptr)
        {
            m_pos              = 0;
            m_alignment        = 16;  // default alignment
            m_reserved_pages   = reserve_size;
            m_committed_pages  = 0;
            m_pages_commit_min = 4;
            return true;
        }
        return false;  // reserve failed
    }

    int_t vmem_allocator_t::save() const { return m_pos; }

    // commits (allocate) size number of bytes and possibly grows the committed region.
    // returns a pointer to the allocated memory or nullptr if allocation failed.
    void *vmem_allocator_t::commit(int_t size)
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
                if (!reserve(cDEFAULT_ARENA_CAPACITY))
                {
                    return nullptr;
                }
            }

            const int_t size_request_in_pages = math::g_alignUp(size_in_bytes, (int_t)1 << m_page_size_shift) >> m_page_size_shift;
            const int_t used_committed_pages  = math::g_alignUp(m_pos, (int_t)1 << m_page_size_shift) >> m_page_size_shift;
            if ((used_committed_pages + size_request_in_pages) > m_committed_pages)
            {
                const int_t extra_needed_pages = math::g_max((used_committed_pages + size_request_in_pages) - m_committed_pages, (int_t)m_pages_commit_min);
                const bool  result             = v_alloc_commit(m_base + (m_committed_pages << m_page_size_shift), extra_needed_pages << m_page_size_shift);
                if (!result)
                {
                    return nullptr;  // failed commit
                }
                m_committed_pages += extra_needed_pages;
            }
        }
        void *ptr = m_base + m_pos;
        m_pos += size_in_bytes;
        return ptr;
    }

    void* vmem_allocator_t::commit(int_t size, s32 alignment)
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
        const int_t pos = math::g_alignUp(m_pos, (int_t)alignment);

        void* ptr = commit(size + (pos - m_pos));

        // align the pointer to the given alignment
        if (ptr != nullptr)
        {
            ptr = (void*)((byte*)ptr + (pos - m_pos));
        }

        return ptr;
    }

    void vmem_allocator_t::restore(int_t size) { m_pos = size; }

    void vmem_allocator_t::shrink()
    {
        // ensure current used memory is aligned up to page size
        const int_t used_in_bytes = math::g_alignUp(m_pos, (int_t)(1 << m_page_size_shift));
        const int_t used_in_pages = used_in_bytes >> m_page_size_shift;

        byte       *decommit_start         = m_base + used_in_bytes;
        const int_t decommit_size_in_bytes = (m_committed_pages - used_in_pages) << m_page_size_shift;
        if (decommit_size_in_bytes > 0)
        {
            if (v_alloc_decommit(decommit_start, decommit_size_in_bytes))
            {
                m_committed_pages = used_in_pages;
            }
        }
    }

    void vmem_allocator_t::reset() { m_pos = 0; }

    bool vmem_allocator_t::release()
    {
        if (m_base == nullptr)
            return true;

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

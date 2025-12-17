#ifndef __CCORE_VMEM_ALLOC_H__
#define __CCORE_VMEM_ALLOC_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_allocator.h"

namespace ncore
{
    struct arena_t;

    arena_t* gCreateArena(int_t reserve_size, int_t commit_size);

    // TODO Investigate the use of madvise(MADV_FREE) to decommit memory on Mac, madvise(MADV_DONTNEED) on Linux, and VirtualAlloc(MEM_RESET)
    struct arena_t
    {
        byte* m_base;              // base address of the reserved memory region
        int_t m_pos;               // current position in the committed memory region to allocate from
        s32   m_reserved_pages;    // (unit = pages) total reserved memory size in pages
        s32   m_committed_pages;   // (unit = pages) total committed memory size in pages
        s32   m_pages_commit_min;  // when increasing committed size, this is the minimum amount of pages to commit
        s16   m_alignment_shift;   // default minimum alignment for allocations (default is 4 = (1<<4) = 16 bytes)
        s16   m_page_size_shift;   // page size in shift, used for alignment and memory operations

        inline int_t alignment() const { return (int_t)1 << m_alignment_shift; }
        inline int_t reserved_size() const { return (int_t)m_reserved_pages << m_page_size_shift; }
        inline int_t committed_size() const { return (int_t)m_committed_pages << m_page_size_shift; }

        bool  reserved(int_t reserved_size);               // set reserved size of virtual memory region
        bool  committed(int_t committed_size_in_bytes);    // set committed size of the allocator, this will not change 'pos'
        void* commit(int_t size);                          // allocate 'size' from the reserved region
        void* commit(int_t size, s32 alignment);           // allocate 'size' from the reserved region with the given alignment
        void* commit_and_zero(int_t size);                 // allocate 'size' from the reserved region
        void* commit_and_zero(int_t size, s32 alignment);  // allocate 'size' from the reserved region with the given alignment
        int_t save_point() const;                          // current used size of the allocator
        void  restore_point(int_t pos);                    // restore the allocator to the given position
        void  shrink();                                    // decommit any 'extra' pages
        void  reset();                                     // make all memory available for reuse without releasing it
        bool  release();                                   // release the virtual memory region
    };

    // Platform specific virtual memory functions
    s32   v_alloc_get_page_size();
    void* v_alloc_reserve(int_t size);
    bool  v_alloc_commit(void* addr, int_t size);
    bool  v_alloc_decommit(void* addr, int_t extra_size);
    bool  v_alloc_release(void* addr, int_t size);

    // clang-format off
    class arena_alloc_t : public alloc_t
    {
    public:
        inline arena_alloc_t() : m_vmem(nullptr) {}
        inline arena_alloc_t(arena_t* vmem) : m_vmem(vmem) {}
        arena_t* m_vmem = nullptr;  // virtual memory arena used for allocations
        DCORE_CLASS_PLACEMENT_NEW_DELETE
        virtual void* v_allocate(u32 size, u32 alignment) { return m_vmem->commit((int_t)size, alignment); }
        virtual void  v_deallocate(void*) {}
    };
    // clang-format on

}  // namespace ncore

#endif  // __CCORE_VMEM_ALLOC_H__

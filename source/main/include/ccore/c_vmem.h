#ifndef __CCORE_VMEM_ALLOC_H__
#define __CCORE_VMEM_ALLOC_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_allocator.h"

namespace ncore
{
    const int_t cDEFAULT_ARENA_CAPACITY = 1 * cGB;

    // TODO Investigate the use of madvise(MADV_FREE) to decommit memory on Mac, madvise(MADV_DONTNEED) on Linux, and VirtualAlloc(MEM_RESET)
    struct vmem_arena_t
    {
        byte* m_base;                                      // base address of the reserved memory region
        int_t m_pos;                                       // current position in the committed memory region to allocate from
        s32   m_alignment;                                 // default minimum alignment for allocations (default is 16 bytes)
        s32   m_reserved_pages;                            // (unit = pages) total reserved memory size in pages
        s32   m_committed_pages;                           // (unit = pages) total committed memory size in pages
        s16   m_page_size_shift;                           // page size in shift, used for alignment and memory operations
        s16   m_pages_commit_min;                          // when increasing committed size, this is the minimum amount of pages to commit

        vmem_arena_t()
            : m_base(nullptr)
            , m_pos(0)
            , m_alignment(16)
            , m_reserved_pages(0)
            , m_committed_pages(0)
            , m_page_size_shift(0)
            , m_pages_commit_min(4)
        {
        }

        bool  reserved(int_t reserved_size);               // set reserved size of virtual memory region
        bool  committed(int_t committed_size_in_bytes);    // set committed size of the allocator, this will not change 'pos'
        int_t save() const;                                // current used size of the allocator
        void* commit(int_t size);                          // allocate 'size' from the reserved region
        void* commit(int_t size, s32 alignment);           // allocate 'size' from the reserved region with the given alignment
        void* commit_and_zero(int_t size);                 // allocate 'size' from the reserved region
        void* commit_and_zero(int_t size, s32 alignment);  // allocate 'size' from the reserved region with the given alignment
        void  restore(int_t size);                         // restore the allocator to the given size
        void  shrink();                                    // decommit any 'extra' pages
        void  reset();                                     // make all memory available for reuse without releasing it
        bool  release();                                   // release the virtual memory region

        DCORE_CLASS_PLACEMENT_NEW_DELETE
    };

    class vmem_alloc_t : public alloc_t
    {
    public:
        inline vmem_alloc_t() : m_vmem(nullptr) {}
        inline vmem_alloc_t(vmem_arena_t* vmem) : m_vmem(vmem) {}
        vmem_arena_t* m_vmem = nullptr;  // virtual memory arena used for allocations

        DCORE_CLASS_PLACEMENT_NEW_DELETE

        virtual void* v_allocate(u32 size, u32 alignment) { return m_vmem->commit((int_t)size, alignment); }
        virtual void  v_deallocate(void*) {}
    };

}  // namespace ncore

#endif  // __CCORE_VMEM_ALLOC_H__

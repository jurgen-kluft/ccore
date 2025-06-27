#ifndef __CCORE_VMEM_ALLOC_H__
#define __CCORE_VMEM_ALLOC_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    const int_t cDEFAULT_ARENA_CAPACITY = 1 * cGB;

    struct vmem_allocator_t
    {
        byte* m_base;                             // base address of the reserved memory region
        int_t m_pos;                              // current position in the committed memory region to allocate from
        s32   m_alignment;                        // default minimum alignment for allocations (default is 16 bytes)
        s32   m_reserved_pages;                   // (unit = pages) total reserved memory size in pages
        s32   m_committed_pages;                  // (unit = pages) total committed memory size in pages
        s16   m_page_size_shift;                  // page size in shift, used for alignment and memory operations
        s16   m_pages_commit_min;                 // when increasing committed size, this is the minimum amount of pages to commit
        bool  reserve(int_t reserve_size);        // reserve a virtual memory region
        int_t save() const;                       // current used size of the allocator
        void* commit(int_t size);                 // allocate 'size' from the reserved region
        void* commit(int_t size, s32 alignment);  // allocate 'size' from the reserved region with the given alignment
        void  restore(int_t size);                // restore the allocator to the given size
        void  shrink();                           // decommit any 'extra' pages
        void  reset();                            // make all memory available for reuse without releasing it
        bool  release();                          // release the virtual memory region
    };

    // Example: see source/test/test_vmem.cpp

}  // namespace ncore

#endif  // __CCORE_VMEM_ALLOC_H__

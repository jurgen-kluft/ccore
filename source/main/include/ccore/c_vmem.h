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
        byte* m_base;                       // base address of the reserved memory region
        int_t m_pos;                        // current position in the committed memory region to allocate from
        s32   m_alignment;                  // default minimum alignment for allocations (default is 16 bytes)
        s32   m_reserved_pages;             // (unit = pages) total reserved memory size in pages
        s32   m_committed_pages;            // (unit = pages) total committed memory size in pages
        s16   m_page_size_shift;            // page size in shift, used for alignment and memory operations
        s16   m_pages_commit_min;           // when increasing committed size, this is the minimum amount of pages to commit
        bool  reserve(int_t reserve_size);  // reserve a virtual memory region
        int_t save() const;                 // current used size of the allocator
        void* commit(int_t size);           // allocate 'size' from the reserved region
        void  restore(int_t size);          // restore the allocator to the given size
        void  shrink();                     // decommit any 'extra' pages
        void  reset();                      // make all memory available for reuse without releasing it
        bool  release();                    // release the virtual memory region
    };

    // Example:
    //    vmem_allocator_t arena = {0};
    //    // reserve 8 GB, if you don't call this, then when calling commit, it will reserve
    //    // the default arena capacity of 1 GB.
    //    arena.reserve(8 * cGB);
    //    int_t save = arena.save();      // save a restore point
    //    void* ptr = arena.commit(1024); // allocate 1 KB
    //    ...                             // commit some more
    //    ...                             // do some more work and commit more memory
    //    arena.restore(save);            // restore the size

}  // namespace ncore

#endif  // __CCORE_VMEM_ALLOC_H__

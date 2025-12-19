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

    // TODO Investigate the use of madvise(MADV_FREE) to decommit memory on Mac, madvise(MADV_DONTNEED) on Linux, and VirtualAlloc(MEM_RESET)
    struct arena_t
    {
        int_t m_pos;               // current position in the committed memory region to allocate from
        s32   m_reserved_pages;    // (unit = pages) total reserved memory size in pages
        s32   m_committed_pages;   // (unit = pages) total committed memory size in pages
        s32   m_pages_commit_min;  // this is the minimum amount of pages to commit
        s16   m_alignment_shift;   // default minimum alignment for allocations (default is 4 = (1<<4) = 16 bytes)
        s16   m_page_size_shift;   // page size in shift, used for alignment and memory operations
        int_t m_reserved0;         // unused
    };

    namespace narena
    {
        const int_t c_arena_header_size = 64;

        arena_t* create(int_t reserve_size, int_t commit_size, i32 minimum_pages = 1);

        inline u32   alignment(arena_t* ar) { return (u32)1 << ar->m_alignment_shift; }
        inline int_t reserved_size(arena_t* ar) { return (int_t)ar->m_reserved_pages << ar->m_page_size_shift; }
        inline int_t committed_size(arena_t* ar) { return (int_t)ar->m_committed_pages << ar->m_page_size_shift; }
        inline void* base(arena_t* ar) { return (void*)((byte*)ar); }

        bool  commit(arena_t* ar, int_t size_in_bytes);                              // set committed size of the allocator, this will not change 'pos'
        bool  commit_from_address(arena_t* ar, void* address, int_t size_in_bytes);  // set committed size of the allocator, this will not change 'pos'
        void* alloc(arena_t* ar, int_t size);                                        // allocate 'size' from the reserved region
        void* alloc(arena_t* ar, int_t size, u32 alignment);                         // allocate 'size' from the reserved region with the given alignment
        void* alloc_and_zero(arena_t* ar, int_t size);                               // allocate 'size' from the reserved region
        void* alloc_and_zero(arena_t* ar, int_t size, u32 alignment);                // allocate 'size' from the reserved region with the given alignment
        void* current_address(arena_t* ar);                                          // return the address of the current allocation point
        void  restore_address(arena_t* ar, void* ptr);                               // restore the arena to the given address
        void  shrink(arena_t* ar);                                                   // decommit any 'extra' pages
        void  reset(arena_t* ar);                                                    // make all memory available for reuse without releasing it
        bool  release(arena_t* ar);                                                  // release the virtual memory region

        // Some C++ style helper functions
        template <typename T>
        inline T* allocate(arena_t* a, u32 alignment = sizeof(void*))
        {
            void* ptr = alloc(a, sizeof(T), alignment);
            return (T*)ptr;
        }

        template <typename T>
        inline T* allocate_and_clear(arena_t* a, u32 alignment = sizeof(void*))
        {
            void* ptr = alloc_and_zero(a, sizeof(T), alignment);
            return (T*)ptr;
        }

        template <typename T>
        inline T* allocate_array(arena_t* a, u32 maxsize, u32 alignment = sizeof(void*))
        {
            void* ptr = alloc(a, maxsize * sizeof(T), alignment);
            return (T*)ptr;
        }

        template <typename T>
        inline T* allocate_array_and_clear(arena_t* a, u32 maxsize, u32 alignment = sizeof(void*))
        {
            void* ptr = alloc_and_zero(a, maxsize * sizeof(T), alignment);
            return (T*)ptr;
        }

        // clang-format off
        class aalloc_t : public alloc_t
        {
        public:
            inline aalloc_t() : m_vmem(nullptr) {}
            inline aalloc_t(arena_t* vmem) : m_vmem(vmem) {}
            virtual ~aalloc_t() {
                if (m_vmem != nullptr) {
                    release(m_vmem);
                    m_vmem = nullptr;
                }
            }
            arena_t* m_vmem;  // virtual memory arena used for allocations
            virtual void* v_allocate(u32 size, u32 alignment) { return narena::alloc(m_vmem, (int_t)size, alignment); }
            virtual void  v_deallocate(void*) {}

            DCORE_CLASS_PLACEMENT_NEW_DELETE
        };
        // clang-format on
    }  // namespace narena

    struct arena_scratch_t
    {
        arena_t* m_arena;
        void*    m_save_address;
    };
    inline arena_scratch_t begin_scratch(arena_t* arena)
    {
        arena_scratch_t s;
        s.m_arena        = arena;
        s.m_save_address = narena::current_address(arena);
        return s;
    }
    inline void end_scratch(arena_scratch_t& s)
    {
        narena::restore_address(s.m_arena, s.m_save_address);
        s.m_arena = nullptr;
    }

    // Platform specific virtual memory functions
    s32   v_alloc_get_page_size();
    void* v_alloc_reserve(int_t size);
    bool  v_alloc_commit(void* addr, int_t size);
    bool  v_alloc_decommit(void* addr, int_t extra_size);
    bool  v_alloc_release(void* addr, int_t size);

}  // namespace ncore

#endif  // __CCORE_VMEM_ALLOC_H__

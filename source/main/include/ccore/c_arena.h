#ifndef __CCORE_VMEM_ALLOC_H__
#define __CCORE_VMEM_ALLOC_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_allocator.h"

namespace ncore
{
    struct arena_t
    {
        byte* m_base;             // base address of the arena (after header)
        int_t m_pos;              // current position in the arena to allocate from (relative to m_base)
        u32   m_reserved_pages;   // (unit = pages) reserved number of pages for this arena (relative to arena)
        u32   m_committed_pages;  // (unit = pages) number of committed pages (relative to arena)
        u16   m_header_pages;     // (unit = pages) number of header pages (before m_base)
        s8    m_page_size_shift;  // page size in shift (from system)
        s8    m_alignment_shift;  // default minimum alignment for allocations (default is 4 = (1<<4) = 16 bytes)
        s32   m_padding;          // padding to make the structure aligned to 16 bytes
    };

    namespace narena
    {
        // usage: basic arena
        arena_t* new_arena(int_t arena_reserve_size, int_t arena_commit_size, s8 arena_alignment_shift = 3);
        bool     destroy(arena_t*& ar);

        // usage: stack like arena
        struct stack_t;
        stack_t* new_stack(int_t reserve_size, int_t commit_size, s32 max_depth = 64);  // a virtual memory arena stack
        void     destroy(stack_t*& s);                                                  // destroy the virtual memory arena stack
        arena_t* push_stack(stack_t* s);                                                // create a new arena as a stack on top of the given arena
        bool     pop_stack(stack_t* s);                                                 // destroy the top arena

        // usage: advanced, multiple arenas within the same virtual address space
        // example: A 1GB region with 32 arenas of 32MB each
        struct region_t;
        region_t* new_region(int_t region_reserve_size, int_t arena_reserve_size, s8 arena_alignment_shift, u16 num_arenas);  // a virtual memory arena in a larger region
        void      destroy(region_t* region);                                                                                  // destroy the virtual memory arena region
        arena_t*  get_arena(region_t* region, s16 index);                                                                     // get the arena at the given index within the region

        inline u32    alignment(arena_t* ar) { return (u32)1 << ar->m_alignment_shift; }
        inline uint_t reserved_size(arena_t* ar) { return (uint_t)ar->m_reserved_pages << ar->m_page_size_shift; }
        inline uint_t committed_size(arena_t* ar) { return (uint_t)ar->m_committed_pages << ar->m_page_size_shift; }
        inline byte*  base(arena_t* ar) { return ar->m_base; }
        inline bool   within_committed(arena_t* ar, void* ptr) { return ((ptr_t)ptr >= (ptr_t)base(ar)) && ((ptr_t)ptr < (ptr_t)(base(ar) + committed_size(ar))); }

        bool  commit(arena_t* ar, int_t size_in_bytes);                          // set committed size of the allocator, this will not change 'pos'
        void* alloc(arena_t* ar, int_t size);                                    // allocate 'size' from the reserved region
        void* alloc(arena_t* ar, int_t size, u32 alignment);                     // allocate 'size' from the reserved region with the given alignment
        void* alloc_and_zero(arena_t* ar, int_t size);                           // allocate 'size' from the reserved region
        void* alloc_and_zero(arena_t* ar, int_t size, u32 alignment);            // allocate 'size' from the reserved region with the given alignment
        void* alloc_and_fill(arena_t* ar, int_t size, u32 fill);                 // allocate 'size' from the reserved region and fill with 'fill' word
        void* alloc_and_fill(arena_t* ar, int_t size, u32 alignment, u32 fill);  // allocate 'size' from the reserved region with the given alignment and fill with 'fill' wor
        void* current_address(arena_t* ar);                                      // return the address of the current allocation point
        void  restore_address(arena_t* ar, void* ptr);                           // restore the arena to the given address
        void  shrink(arena_t* ar);                                               // decommit any 'extra' pages
        void  reset(arena_t* ar);                                                // make all memory available for reuse without releasing it

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

        template <typename T>
        inline T* allocate_array_and_fill(arena_t* a, u32 maxsize, u32 fill, u32 alignment = sizeof(void*))
        {
            void* ptr = alloc_and_fill(a, maxsize * sizeof(T), alignment, fill);
            return (T*)ptr;
        }

        // clang-format off
        class aalloc_t : public alloc_t
        {
        public:
            inline aalloc_t() : m_arena(nullptr) {}
            inline aalloc_t(arena_t* vmem) : m_arena(vmem) {}
            virtual ~aalloc_t() {
                if (m_arena != nullptr) {
                    narena::destroy(m_arena);
                    m_arena = nullptr;
                }
            }
            arena_t* m_arena;  // virtual memory arena used for allocations
            virtual void* v_allocate(u32 size, u32 alignment) { return narena::alloc(m_arena, (int_t)size, alignment); }
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
    s8    v_alloc_get_page_size_shift();
    void* v_alloc_reserve(int_t size);
    bool  v_alloc_commit(void* addr, int_t size);
    bool  v_alloc_decommit(void* addr, int_t extra_size);
    bool  v_alloc_release(void* addr, int_t size);

}  // namespace ncore

#endif  // __CCORE_VMEM_ALLOC_H__

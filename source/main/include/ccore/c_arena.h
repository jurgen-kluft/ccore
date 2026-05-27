#ifndef __CCORE_VMEM_ALLOC_H__
#define __CCORE_VMEM_ALLOC_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

#include "ccore/c_allocator.h"
#include "ccore/c_limits.h"

namespace ncore
{
    struct arena_t                // 32 bytes
    {                             //
        byte* m_base;             // base address of the arena (after header)
        int_t m_pos;              // current position in the arena to allocate from (relative to m_base)
        u32   m_reserved_pages;   // (unit = pages) reserved number of pages for this arena (relative to arena)
        u32   m_committed_pages;  // (unit = pages) number of committed pages (relative to arena)
        u8    m_page_size_shift;  // page size in shift (from system)
        u8    m_ownership;        // ownership
        u8    m_active;           // whether this arena is in use
        u8    m_padding1;         // padding
        u32   m_padding2;         // padding
    };

    namespace narena
    {
        // usage: arena, owns the virtual that it reserves, and will do an initial commit for 'commit_size'
        arena_t* new_arena(int_t reserve_size, int_t commit_size);
        // usage: create arena with virtual memory already reserved, and NOTHING yet committed, arena will
        //        not be responsible for releasing the reserved virtual memory.
        arena_t* init_arena(void* base, int_t reserved_size, int_t commit_size);
        // usage: destroy arena, if the arena does not own the virtual memory, then we just nullify the
        //        pointer and return true, otherwise we release the virtual memory.
        bool destroy(arena_t*& arena);

        inline uint_t reserved_size(arena_t* arena) { return (uint_t)arena->m_reserved_pages << arena->m_page_size_shift; }
        inline uint_t committed_size(arena_t* arena) { return (uint_t)arena->m_committed_pages << arena->m_page_size_shift; }
        inline byte*  base_ptr(arena_t* arena) { return arena->m_base; }
        inline bool   within_committed(arena_t* arena, void* ptr) { return ((ptr_t)ptr >= (ptr_t)base_ptr(arena)) && ((ptr_t)ptr < (ptr_t)(base_ptr(arena) + committed_size(arena))); }

        bool  commit(arena_t* arena, int_t size_in_bytes);                          // set committed size of the allocator, this will not change 'pos'
        void* alloc(arena_t* arena, int_t size);                                    // allocate 'size' from the reserved region
        void* alloc(arena_t* arena, int_t size, u32 alignment);                     // allocate 'size' from the reserved region with the given alignment
        void* alloc_and_zero(arena_t* arena, int_t size);                           // allocate 'size' from the reserved region
        void* alloc_and_zero(arena_t* arena, int_t size, u32 alignment);            // allocate 'size' from the reserved region with the given alignment
        void* alloc_and_fill(arena_t* arena, int_t size, u32 fill);                 // allocate 'size' from the reserved region and fill with 'fill' word
        void* alloc_and_fill(arena_t* arena, int_t size, u32 alignment, u32 fill);  // allocate 'size' from the reserved region with the given alignment and fill with 'fill' wor
        void* current_address(arena_t* arena);                                      // return the address of the current allocation point
        void  restore_address(arena_t* arena, void* ptr);                           // restore the arena to the given address
        void  shrink(arena_t* arena);                                               // decommit any 'extra' pages
        void  reset(arena_t* arena);                                                // make all memory available for reuse without releasing it
    }  // namespace narena

    // clang-format off
    class arena_alloc_t : public alloc_t
    {
    public:
        inline arena_alloc_t() : m_arena(nullptr) {}
        inline arena_alloc_t(arena_t* arena) : m_arena(arena) {}
        virtual ~arena_alloc_t() {}

        arena_t* m_arena;

        virtual void* v_allocate(u32 size, u32 alignment) { return narena::alloc(m_arena, (int_t)size, alignment); }
        virtual void  v_deallocate(void*) {}

        DCORE_CLASS_PLACEMENT_NEW_DELETE
    };
    // clang-format on

    // Some C++ style helper functions
    template <typename T>
    inline T* g_allocate(arena_t* arena, u32 alignment = sizeof(void*))
    {
        void* ptr = narena::alloc(arena, sizeof(T), alignment);
        return (T*)ptr;
    }

    template <typename T>
    inline T* g_allocate_memory(arena_t* arena, u32 size, u32 alignment = sizeof(void*))
    {
        ASSERT(size >= sizeof(T));
        void* ptr = narena::alloc(arena, size, alignment);
        return (T*)ptr;
    }

    template <typename T>
    inline void g_deallocate(arena_t* arena, T* ptr)
    {
        // no-op, arena allocations are not deallocated individually
        (void)arena;
        (void)ptr;
    }

    template <typename T>
    inline T* g_allocate_and_clear(arena_t* arena, u32 alignment = sizeof(void*))
    {
        void* ptr = narena::alloc_and_zero(arena, sizeof(T), alignment);
        return (T*)ptr;
    }

    template <typename T>
    inline T* g_allocate_array(arena_t* arena, u32 maxsize, u32 alignment = sizeof(void*))
    {
        void* ptr = narena::alloc(arena, maxsize * sizeof(T), alignment);
        return (T*)ptr;
    }

    template <typename T>
    inline T* g_allocate_array_and_clear(arena_t* arena, u32 maxsize, u32 alignment = sizeof(void*))
    {
        void* ptr = narena::alloc_and_zero(arena, maxsize * sizeof(T), alignment);
        return (T*)ptr;
    }

    template <typename T>
    inline T* g_allocate_array_and_fill(arena_t* arena, u32 maxsize, u32 fill, u32 alignment = sizeof(void*))
    {
        void* ptr = narena::alloc_and_fill(arena, maxsize * sizeof(T), alignment, fill);
        return (T*)ptr;
    }

    struct arena_point_t
    {
        arena_t* m_arena;
        void*    m_address;
    };

    static inline arena_point_t save_point(arena_t* arena)
    {
        arena_point_t s;
        s.m_arena   = arena;
        s.m_address = narena::current_address(arena);
        return s;
    }

    template <typename T>
    static inline T diff_point(arena_point_t const& start, arena_point_t const& end)
    {
        ASSERT(start.m_arena == end.m_arena);
        uint_t diff = g_ptr_diff_in_bytes<uint_t>(end.m_address, start.m_address);
        ASSERT(diff <= type_t<T>::max);
        return (T)diff;
    }

    static inline void restore_point(arena_point_t& s)
    {
        narena::restore_address(s.m_arena, s.m_address);
        s.m_arena = nullptr;
    }

    // Platform specific virtual memory functions
    s32   v_alloc_get_page_size();
    u8    v_alloc_get_page_size_shift();
    void* v_alloc_reserve(int_t size);
    bool  v_alloc_commit(void* addr, int_t size);
    bool  v_alloc_decommit(void* addr, int_t extra_size);
    bool  v_alloc_release(void* addr, int_t size);

}  // namespace ncore

#endif  // __CCORE_VMEM_ALLOC_H__

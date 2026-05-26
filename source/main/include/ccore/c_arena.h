#ifndef __CCORE_VMEM_ALLOC_H__
#define __CCORE_VMEM_ALLOC_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_allocator.h"
#include "ccore/c_limits.h"

namespace ncore
{
    struct arena_t
    {
        byte* m_base;             // base address of the arena (after header)
        int_t m_pos;              // current position in the arena to allocate from (relative to m_base)
        u32   m_reserved_pages;   // (unit = pages) reserved number of pages for this arena (relative to arena)
        u32   m_committed_pages;  // (unit = pages) number of committed pages (relative to arena)
        u16   m_header_pages;     // (unit = pages) number of header pages (before m_base)
        u8    m_page_size_shift;  // page size in shift (from system)
        u8    m_ownership;        // ownership
        u32   m_padding1;         // padding to make the structure aligned to 8 bytes
    };

    namespace narena
    {
        // usage: arena, owns the virtual that it reserves, and will do an initial commit for 'commit_size'
        arena_t* new_arena(int_t reserve_size, int_t commit_size);
        // usage: create arena with virtual memory already reserved, and NOTHING yet committed, arena will
        //        not own the virtual memory, but it will do an initial commit for 'commit_size'
        arena_t* init_arena(void* base, int_t reserved_size, int_t commit_size);
        // usage: arena not part of base, and with virtual memory already reserved, and NOTHING committed, arena
        //        will not own the virtual memory, but it will do an initial commit for 'commit_size'
        void     init_arena(arena_t* ar, void* base, int_t reserved_size, int_t commit_size);
        // usage: destroy arena, if the arena does not own the virtual memory, then we just nullify the
        //        pointer and return true, otherwise we release the virtual memory.
        bool     destroy(arena_t*& ar);

        inline uint_t reserved_size(arena_t* ar) { return (uint_t)ar->m_reserved_pages << ar->m_page_size_shift; }
        inline uint_t committed_size(arena_t* ar) { return (uint_t)ar->m_committed_pages << ar->m_page_size_shift; }
        inline byte*  base_ptr(arena_t* ar) { return ar->m_base; }
        inline bool   within_committed(arena_t* ar, void* ptr) { return ((ptr_t)ptr >= (ptr_t)base_ptr(ar)) && ((ptr_t)ptr < (ptr_t)(base_ptr(ar) + committed_size(ar))); }

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
    }  // namespace narena

    // clang-format off
    class arena_alloc_t : public alloc_t
    {
    public:
        inline arena_alloc_t() : m_arena(nullptr) {}
        inline arena_alloc_t(arena_t* vmem) : m_arena(vmem) {}
        virtual ~arena_alloc_t() {}

        arena_t* m_arena;

        virtual void* v_allocate(u32 size, u32 alignment) { return narena::alloc(m_arena, (int_t)size, alignment); }
        virtual void  v_deallocate(void*) {}

        DCORE_CLASS_PLACEMENT_NEW_DELETE
    };
    // clang-format on

    // Some C++ style helper functions
    template <typename T>
    inline T* g_allocate(arena_t* a, u32 alignment = sizeof(void*))
    {
        void* ptr = narena::alloc(a, sizeof(T), alignment);
        return (T*)ptr;
    }

    template <typename T>
    inline T* g_allocate_memory(arena_t* a, u32 size, u32 alignment = sizeof(void*))
    {
        ASSERT(size >= sizeof(T));
        void* ptr = narena::alloc(a, size, alignment);
        return (T*)ptr;
    }

    template <typename T>
    inline void g_deallocate(arena_t* a, T* ptr)
    {
        // no-op, arena allocations are not deallocated individually
        (void)a;
        (void)ptr;
    }

    template <typename T>
    inline T* g_allocate_and_clear(arena_t* a, u32 alignment = sizeof(void*))
    {
        void* ptr = narena::alloc_and_zero(a, sizeof(T), alignment);
        return (T*)ptr;
    }

    template <typename T>
    inline T* g_allocate_array(arena_t* a, u32 maxsize, u32 alignment = sizeof(void*))
    {
        void* ptr = narena::alloc(a, maxsize * sizeof(T), alignment);
        return (T*)ptr;
    }

    template <typename T>
    inline T* g_allocate_array_and_clear(arena_t* a, u32 maxsize, u32 alignment = sizeof(void*))
    {
        void* ptr = narena::alloc_and_zero(a, maxsize * sizeof(T), alignment);
        return (T*)ptr;
    }

    template <typename T>
    inline T* g_allocate_array_and_fill(arena_t* a, u32 maxsize, u32 fill, u32 alignment = sizeof(void*))
    {
        void* ptr = narena::alloc_and_fill(a, maxsize * sizeof(T), alignment, fill);
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

    template<typename T>
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

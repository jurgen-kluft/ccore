#ifndef __CCORE_ALLOCATOR_H__
#define __CCORE_ALLOCATOR_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_debug.h"

namespace ncore
{
    struct vmem_arena_t;

    // class new and delete
#define DCORE_CLASS_PLACEMENT_NEW_DELETE                                     \
    void* operator new(ncore::uint_t num_bytes, void* mem) { return mem; }   \
    void  operator delete(void* mem, void*) {}                               \
    void* operator new(ncore::uint_t num_bytes) noexcept { return nullptr; } \
    void  operator delete(void* mem) {}

#define DCORE_CLASS_NEW_DELETE(get_allocator_func, align)                  \
    void* operator new(ncore::uint_t num_bytes, void* mem) { return mem; } \
    void  operator delete(void* mem, void*) {}                             \
    void* operator new(ncore::uint_t num_bytes)                            \
    {                                                                      \
        ASSERT(num_bytes < (ncore::uint_t)2 * 1024 * 1024 * 1024);         \
        return get_allocator_func()->allocate((u32)num_bytes, align);      \
    }                                                                      \
    void operator delete(void* mem) { get_allocator_func()->deallocate(mem); }

#define DCORE_CLASS_ARRAY_NEW_DELETE(get_allocator_func, align)       \
    void* operator new[](ncore::uint_t num_bytes)                     \
    {                                                                 \
        ASSERT(num_bytes < (ncore::uint_t)2 * 1024 * 1024 * 1024);    \
        return get_allocator_func()->allocate((u32)num_bytes, align); \
    }                                                                 \
    void operator delete[](void* mem) { get_allocator_func()->deallocate(mem); }

    // =========================================================
    // ================== Allocator Interface ==================
    class alloc_t
    {
    public:
        inline void* allocate(u32 size, u32 alignment) { return v_allocate(size, alignment); }
        inline void* allocate(u32 size) { return v_allocate(size, sizeof(void*)); }
        inline void  deallocate(void* ptr) { v_deallocate(ptr); }

    protected:
        virtual void* v_allocate(u32 size, u32 align) = 0;  // Allocate memory with alignment
        virtual void  v_deallocate(void* p)           = 0;  // Deallocate/Free memory

        virtual ~alloc_t() {}
    };

    // =========================================================
    // ================== Allocator Tracker ====================
    class alloc_tracker_t
    {
    public:
        virtual void push(const char* file, int_t line) = 0;
        virtual void pop()                              = 0;
    };

    // Note: thread_local alloc_tracker_t* sAllocTracker = &sEmptyAllocTracker;
    alloc_tracker_t* g_alloc_tracker();

    class alloc_scope_t
    {
        alloc_tracker_t* m_tracker;
    public:
        alloc_scope_t(const char* file, int_t line)
        {
            m_tracker = g_alloc_tracker();
            m_tracker->push(file, line);
        }
        ~alloc_scope_t() { m_tracker->pop(); }
    };

#define DALLOCATION_SCOPE alloc_scope_t __alloc_scope##__LINE__(__FILE__, __LINE__)

    // =========================================================

    template <typename T>
    inline T* g_construct(alloc_t* a)
    {
        return new (a) T();
    }

    template <typename T>
    inline void g_destruct(alloc_t* a, T* p)
    {
        p->~T();  // Call destructor if T is a class type
        a->deallocate(p);
    }

    template <typename T, uint_t N>
    constexpr u32 g_array_size(T (&)[N])
    {
        return (u32)N;
    }

    inline void* g_allocate_and_memset(alloc_t* alloc, u32 size, u32 value)
    {
        ASSERTS(size > 0, "error: allocation request for an array of size 0");
        u32 const memsize = size;
        void*     ptr     = alloc->allocate(memsize);
        ASSERT(ptr != nullptr && ((ptr_t)ptr & 3) == 0);  // Ensure 4 byte alignment
        u32*       clr32 = (u32*)ptr;
        u32 const* end32 = clr32 + (memsize >> 2);
        while (clr32 < end32)
            *clr32++ = value;
        u8*       clr8 = (u8*)clr32;
        u8 const* end8 = clr8 + (memsize & 3);
        while (clr8 < end8)
            *clr8++ = (u8)value;
        return (void*)ptr;
    }

    template <typename T>
    inline T* g_allocate(alloc_t* a)
    {
        void* ptr = a->allocate(sizeof(T));
        return (T*)ptr;
    }

    template <typename T>
    inline T* g_allocate_and_clear(alloc_t* a)
    {
        void* ptr = g_allocate_and_memset(a, sizeof(T), 0);
        return (T*)ptr;
    }

    template <typename T>
    inline void g_deallocate(alloc_t* a, T*& ptr)
    {
        if (ptr != nullptr)
        {
            a->deallocate(ptr);
            ptr = nullptr;
        }
    }

    template <typename T>
    inline T* g_allocate_array(alloc_t* a, u32 maxsize)
    {
        void* ptr = a->allocate(maxsize * sizeof(T));
        return (T*)ptr;
    }

    template <typename T>
    inline T* g_allocate_array_and_memset(alloc_t* a, u32 maxsize, u32 value)
    {
        ASSERTS(maxsize > 0, "error: allocation request for an array of size 0");
        u32 const memsize = maxsize * sizeof(T);
        void*     ptr     = g_allocate_and_memset(a, memsize, value);
        return (T*)ptr;
    }

    template <typename T>
    inline T* g_allocate_array_and_clear(alloc_t* a, u32 maxsize)
    {
        ASSERTS(maxsize > 0, "error: allocation request for an array of size 0");
        u32 const memsize = maxsize * sizeof(T);
        void*     ptr     = g_allocate_and_memset(a, memsize, 0);
        return (T*)ptr;
    }

    void* g_reallocate(alloc_t* alloc, void* ptr, u32 size, u32 new_size);

    template <typename T>
    inline void g_deallocate_array(alloc_t* a, T*& array)
    {
        if (array != nullptr)
        {
            a->deallocate(array);
            array = nullptr;
        }
    }

    // helper functions
    template <typename T>
    inline T* g_ptr_advance(T* ptr, uint_t size)
    {
        return (T*)((ptr_t)ptr + size);
    }

    template <typename T>
    inline T* g_ptr_align(T* ptr, u32 alignment)
    {
        return (T*)(((ptr_t)ptr + (alignment - 1)) & ~((ptr_t)alignment - 1));
    }

    inline uint_t g_ptr_diff_in_bytes(void* ptr, void* next_ptr) { return (uint_t)((ptr_t)next_ptr - (ptr_t)ptr); }
    inline bool   g_ptr_inside_range(void* buffer, uint_t size_in_bytes, void* ptr) { return (ptr >= buffer) && g_ptr_diff_in_bytes(buffer, ptr) <= size_in_bytes; }

}  // namespace ncore

template <typename T>
inline void* operator new(ncore::uint_t num_bytes, ncore::alloc_t* alloc)
{
    ASSERT(num_bytes < (ncore::uint_t)2 * 1024 * 1024 * 1024);
    void* ptr = alloc->allocate((ncore::u32)num_bytes, alignof(T));
    return ptr;
}

template <typename T>
void operator delete(void* ptr, ncore::alloc_t* allocator)
{
    if (ptr != nullptr)
    {
        ((T*)ptr)->~T();  // Call destructor if T is a class type
        allocator->deallocate(ptr);
    }
}

#endif  // __CCORE_ALLOCATOR_H__

#ifndef __CCORE_ALLOCATOR_H__
#define __CCORE_ALLOCATOR_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_debug.h"

namespace ncore
{
    class alloc_t
    {
    public:
        void* allocate(u32 size, u32 alignment) { return v_allocate(size, alignment); }
        void* allocate(u32 size) { return v_allocate(size, sizeof(void*)); }
        void  deallocate(void* ptr) { v_deallocate(ptr); }

        template <typename T, typename... Args>
        T* construct(Args... args)
        {
            void* mem    = v_allocate(sizeof(T), sizeof(void*));
            T*    object = new (mem) T(args...);
            return object;
        }

        template <typename T, typename... Args>
        T* placement(u32 EXTRA = 0, Args... args)
        {
            void* mem    = v_allocate(sizeof(T) + EXTRA, sizeof(void*));
            T*    object = new (mem) T(args...);
            return object;
        }

        template <typename T>
        void destruct(T* p)
        {
            p->~T();
            v_deallocate(p);
        }

    protected:
        virtual void* v_allocate(u32 size, u32 align) = 0;  // Allocate memory with alignment
        virtual void  v_deallocate(void* p)           = 0;  // Deallocate/Free memory

        virtual ~alloc_t() {}
    };

    template <typename T>
    inline T* g_allocate_array(alloc_t* a, u32 maxsize)
    {
        void* ptr = a->allocate(maxsize * sizeof(T));
        return (T*)ptr;
    }

    template <typename T>
    inline void g_deallocate_array(alloc_t* a, T* array)
    {
        a->deallocate(array);
    }

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

    // helper functions
    template <typename T>
    inline T* g_advance_ptr(T* ptr, uint_t size)
    {
        return (T*)((ptr_t)ptr + size);
    }
    template <typename T>
    inline T* g_align_ptr(T* ptr, u32 alignment)
    {
        return (T*)(((ptr_t)ptr + (alignment - 1)) & ~((ptr_t)alignment - 1));
    }

    inline uint_t g_ptr_diff_bytes(void* ptr, void* next_ptr) { return (uint_t)((ptr_t)next_ptr - (ptr_t)ptr); }
    inline bool   g_ptr_in_range(void* buffer, uint_t size, void* ptr)
    {
        ptr_t begin  = (ptr_t)buffer;
        ptr_t end    = begin + size;
        ptr_t cursor = (ptr_t)ptr;
        return cursor >= begin && cursor < end;
    }

    class alloc_buffer_t : public alloc_t
    {
        u8* m_base;
        u8* m_ptr;
        u8* m_end;
        s64 m_cnt;

    public:
        alloc_buffer_t(u8* buffer, s64 length);

        inline u8*       data() { return m_base; }
        inline u8 const* data() const { return m_base; }
        inline s64       cap() const { return m_end - m_base; }
        inline s64       size() const { return m_ptr - m_base; }

        DCORE_CLASS_PLACEMENT_NEW_DELETE

    protected:
        virtual void* v_allocate(u32 size, u32 align)
        {
            if ((g_align_ptr(m_ptr, align) + size) <= m_end)
            {
                u8* ptr = g_align_ptr(m_ptr, align);
                m_ptr   = ptr + size;
                m_cnt += 1;
                return ptr;
            }
            return nullptr;
        }

        virtual void v_deallocate(void* p)
        {
            if (p != nullptr)
            {
                ASSERT(g_ptr_in_range(m_base, cap(), p));
                ASSERT(m_cnt > 0);
                m_cnt -= 1;
                if (m_cnt == 0)
                    m_ptr = m_base;
            }
        }
    };

};  // namespace ncore

#endif  ///< __CCORE_ALLOCATOR_H__

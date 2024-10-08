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

    template<typename T, typename... Args>
    inline T* g_construct(alloc_t* a, Args... args)
    {
        void* mem = a->allocate(sizeof(T), sizeof(void*));
        return new (mem) T(args...);
    }

    template<typename T>
    inline void g_destruct(alloc_t* a, T* p)
    {
        p->~T();
        a->deallocate(p);
    }

    template <typename T, uint_t N>
    constexpr s32 g_array_size(T (&)[N])
    {
        ASSERT(N < 0x7FFFFFFF);
        return (s32)N;
    }

    template <typename T>
    inline T* g_allocate_array(alloc_t* a, u32 maxsize)
    {
        void* ptr = a->allocate(maxsize * sizeof(T));
        return (T*)ptr;
    }

    inline void* g_allocate_and_memset(alloc_t* alloc, u32 size, u32 value = 0xCDCDCDCD)
    {
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
    inline T* g_allocate_array_and_memset(alloc_t* a, u32 maxsize, u32 value = 0xCDCDCDCD)
    {
        u32 const memsize = maxsize * sizeof(T);
        void*     ptr     = g_allocate_and_memset(a, memsize, value);
        return (T*)ptr;
    }

    void* g_reallocate(alloc_t* alloc, void* ptr, u32 size, u32 new_size);

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
        ptr_t end    = (ptr_t)((uint_t)begin + size);
        ptr_t cursor = (ptr_t)ptr;
        return cursor >= begin && cursor < end;
    }

    class alloc_buffer_t : public alloc_t
    {
        byte* m_base;
        byte* m_ptr;
        s32   m_size;
        s32   m_cnt;

    public:
        alloc_buffer_t();

        void init(byte* buffer, s32 length);

        inline byte*       data() { return m_base; }
        inline byte const* data() const { return m_base; }
        inline s32         cap() const { return m_size; }
        inline s32         size() const { return (s32)(m_ptr - m_base); }
        inline void        reset()
        {
            m_ptr = m_base;
            m_cnt = 0;
        }

        DCORE_CLASS_PLACEMENT_NEW_DELETE

    protected:
        virtual void* v_allocate(u32 size, u32 align)
        {
            if ((g_align_ptr(m_ptr, align) + size) <= (m_base + m_size))
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
                ASSERT(g_ptr_in_range(m_base, (uint_t)cap(), p));
                ASSERT(m_cnt > 0);
                m_cnt -= 1;
                if (m_cnt == 0)
                    m_ptr = m_base;
            }
        }
    };
}  // namespace ncore

#endif  // __CCORE_ALLOCATOR_H__

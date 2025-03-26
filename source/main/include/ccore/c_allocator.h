#ifndef __CCORE_ALLOCATOR_H__
#define __CCORE_ALLOCATOR_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_debug.h"

namespace ncore
{
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

    // Allocator interface
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

    // Fixed size allocator interface
    class fsa_t
    {
    public:
        template <typename T>
        inline T* allocate()
        {
            ASSERT(sizeof(T) <= v_allocsize());
            return static_cast<T*>(v_allocate());
        }

        template <typename T>
        inline void deallocate(T* p)
        {
            ASSERT(sizeof(T) <= v_allocsize());
            v_deallocate(p);
        }

        template <typename T, typename... Args>
        inline T* construct(Args... args)
        {
            ASSERT(sizeof(T) <= v_allocsize());
            void* mem = v_allocate();
            return new (mem) T(args...);
        }

        template <typename T>
        inline void destruct(T* p)
        {
            ASSERT(sizeof(T) <= v_allocsize());
            p->~T();
            v_deallocate(p);
        }

    protected:
        virtual u32   v_allocsize() const = 0;
        virtual void* v_allocate()        = 0;
        virtual void  v_deallocate(void*) = 0;
    };

    // The dexer interface, 'pointer to index' and 'index to pointer'
    class dexer_t
    {
    public:
        inline void* idx2ptr(u32 index) { return v_idx2ptr(index); }
        inline u32   ptr2idx(void const* ptr) const { return v_ptr2idx(ptr); }

        template <typename T>
        inline T* idx2obj(u32 index)
        {
            return static_cast<T*>(v_idx2ptr(index));
        }
        template <typename T>
        inline u32 obj2idx(T const* ptr) const
        {
            return v_ptr2idx(ptr);
        }

    protected:
        virtual void* v_idx2ptr(u32 index)             = 0;
        virtual u32   v_ptr2idx(void const* ptr) const = 0;
    };

    // The pool allocator interface

    template <typename T>
    class pool_t : public fsa_t, public dexer_t
    {
    public:
        ~pool_t() = default;
    };

    // Construct and destruct

    template <typename T, typename... Args>
    inline T* g_construct(alloc_t* a, Args... args)
    {
        return a->construct<T>(args...);
    }

    template <typename T>
    inline void g_destruct(alloc_t* a, T* p)
    {
        a->destruct(p);
    }

    // Array Size (should we make DArrayCount obsolete?)
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

    class fixed_buffer_alloc_t : public alloc_t
    {
        byte* m_base;
        byte* m_ptr;
        s32   m_size;
        s32   m_cnt;

    public:
        fixed_buffer_alloc_t();

        void setup(byte* buffer, s32 length);

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
            if ((g_ptr_align(m_ptr, align) + size) <= (m_base + m_size))
            {
                u8* ptr = g_ptr_align(m_ptr, align);
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
                ASSERT(g_ptr_inside_range(m_base, (uint_t)cap(), p));
                ASSERT(m_cnt > 0);
                m_cnt -= 1;
                if (m_cnt == 0)
                    m_ptr = m_base;
            }
        }
    };

    template <typename T>
    class fixed_pool_alloc_t : public pool_t<T>
    {
        T*  m_base;
        u32 m_free_head;
        u32 m_free_index;
        s32 m_size;
        s32 m_capacity;

    public:
        fixed_pool_alloc_t()
            : m_base(nullptr)
            , m_free_head(0xFFFFFFFF)
            , m_free_index(0)
            , m_size(0)
            , m_capacity(0)
        {
        }

        void setup(T* pool, s32 number_of_elements)
        {
            m_base     = pool;
            m_capacity = number_of_elements;
            reset();
        }

        inline T*       data() { return m_base; }
        inline T const* data() const { return m_base; }
        inline s32      cap() const { return m_capacity; }
        inline s32      size() const { return m_size; }
        inline void     reset()
        {
            m_free_head  = 0xFFFFFFFF;
            m_free_index = 0;
            m_size       = 0;
        }

        DCORE_CLASS_PLACEMENT_NEW_DELETE

    protected:
        virtual u32   v_allocsize() const { return sizeof(T); }
        virtual void* v_idx2ptr(u32 index) { return m_base + index; }
        virtual u32   v_ptr2idx(void const* ptr) const { return (u32)((T*)ptr - m_base); }

        virtual void* v_allocate()
        {
            if (m_free_head != 0xFFFFFFFF)
            {
                u32 const index = m_free_head;
                m_free_head     = *((u32 const*)m_base[index]);
                m_size += 1;
                return v_idx2ptr(index);
            }
            else if (m_free_index < m_capacity)
            {
                u32 const index = m_free_index;
                m_free_index += 1;
                m_size += 1;
                return v_idx2ptr(index);
            }
            return nullptr;
        }

        virtual void v_deallocate(void* p)
        {
            if (p != nullptr)
            {
                ASSERT(g_ptr_inside_range(m_base, (uint_t)m_free_index * sizeof(T), p));
                ASSERT(m_size > 0);

                u32  item_index = v_ptr2idx(p);
                u32* item_ptr   = (u32*)p;
                *item_ptr       = m_free_head;
                m_free_head     = item_index;
                m_size -= 1;
            }
        }
    };

}  // namespace ncore

#endif  // __CCORE_ALLOCATOR_H__

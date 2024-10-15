#include "ccore/c_target.h"
#include "ccore/c_allocator.h"

namespace ncore
{
    fixed_buffer_alloc_t::fixed_buffer_alloc_t()
        : m_base(0)
        , m_ptr(0)
        , m_size(0)
        , m_cnt(0)
    {
    }

    void fixed_buffer_alloc_t::setup(byte* buffer, s32 length)
    {
        m_base = buffer;
        m_ptr  = buffer;
        m_size = length;
        m_cnt  = 0;
    }

    void* g_reallocate(alloc_t* alloc, void* ptr, u32 size, u32 new_size)
    {
        void* newptr = alloc->allocate(new_size, sizeof(void*));

        // copy the old data to the new memory, manually
        ASSERT(newptr != nullptr && ((ptr_t)newptr & 3) == 0); // Ensure 4 byte alignment
        ASSERT(ptr != nullptr && ((ptr_t)ptr & 3) == 0);     // Ensure 4 byte alignment

        u32 const minsize = size < new_size ? size : new_size;

        // TODO, use nmem::memcpy

        u32*       dstptr = (u32*)newptr;
        u32 const* dstend = dstptr + (minsize >> 2);
        u32 const* srcptr = (u32*)ptr;
        while (dstptr < dstend)
            *dstptr++ = *srcptr++;

        u8*       dstptr8 = (u8*)newptr;
        u8 const* dstend8 = dstptr8 + (minsize & 3);
        u8*       srcptr8 = (u8*)srcptr;
        while (dstptr8 < dstend8)
            *dstptr8++ = *srcptr8++;

        alloc->deallocate(ptr);
        return newptr;
    }

}; // namespace ncore

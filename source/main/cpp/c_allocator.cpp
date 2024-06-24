#include "ccore/c_target.h"
#include "ccore/c_allocator.h"

namespace ncore
{
    alloc_buffer_t::alloc_buffer_t()
        : m_base(0)
        , m_ptr(0)
        , m_size(0)
        , m_cnt(0)
    {
    }

    void alloc_buffer_t::init(byte* buffer, s32 length)
    {
        m_base = buffer;
        m_ptr  = buffer;
        m_size = length;
        m_cnt  = 0;
    }

};  // namespace ncore

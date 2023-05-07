#include "ccore/c_target.h"
#include "ccore/c_allocator.h"

namespace ncore
{
    alloc_buffer_t::alloc_buffer_t(u8* buffer, s64 length)
        : m_base(buffer)
        , m_ptr(buffer)
        , m_end(buffer + length)
        , m_cnt(0)
    {
    }

};  // namespace ncore
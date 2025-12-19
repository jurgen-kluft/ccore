#ifndef __CCORE_DEFER_H__
#define __CCORE_DEFER_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    template <typename T>
    class defer_action_t
    {
    public:
        defer_action_t(T object) {}
        void open() {}
        void close() {}
    };

    template <typename T>
    class defer_t
    {
    protected:
        defer_action_t<T> m_defer;

    public:
        defer_t(T o)
            : m_defer(o)
        {
            m_defer.open();
        }
        ~defer_t() { m_defer.close(); }
    };

};  // namespace ncore

#endif  /// __CCORE_DEFER_H__

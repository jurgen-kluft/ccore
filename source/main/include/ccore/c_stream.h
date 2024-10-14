#ifndef __CBASE_STREAM_H__
#define __CBASE_STREAM_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    class writer_t
    {
    public:
        virtual ~writer_t() {}

        inline s64 write(u8 const* data, s64 len) { return v_write(data, len); }

    protected:
        virtual s64 v_write(u8 const*, s64) = 0;
    };

    class reader_t
    {
    public:
        virtual ~reader_t() {}

        inline s64 read(u8* data, s64 len) { return v_read(data, len); }

    protected:
        virtual s64 v_read(u8*, s64) = 0;
    };

    class istream_t : public reader_t, public writer_t
    {
    public:
        inline bool canSeek() const { return v_canSeek(); }
        inline bool canRead() const { return v_canRead(); }
        inline bool canWrite() const { return v_canWrite(); }

        inline u64  getLength() const { return v_getLength(); }
        inline void setLength(u64 length) { v_setLength(length); }
        inline s64  getPos() const { return v_getPos(); }
        inline s64  setPos(s64 pos) { return v_setPos(pos); }

        inline void close() { v_close(); }
        inline void flush() { v_flush(); }

        inline bool canView() const { return v_canView(); }  // Does this stream support zero-copy (can you use 'view()')
        inline s64  view(u8 const*& data, s64 len) { return v_view(data, len); }

    protected:
        virtual bool v_canSeek() const                    = 0;
        virtual bool v_canRead() const                    = 0;
        virtual bool v_canWrite() const                   = 0;
        virtual bool v_canView() const                    = 0;
        virtual void v_flush()                            = 0;
        virtual void v_close()                            = 0;
        virtual u64  v_getLength() const                  = 0;
        virtual void v_setLength(u64 length)              = 0;
        virtual s64  v_setPos(s64 pos)                    = 0;
        virtual s64  v_getPos() const                     = 0;
        virtual s64  v_view(u8 const*& buffer, s64 count) = 0;
    };

}  // namespace ncore

#endif  // __CBASE_STREAM_H__
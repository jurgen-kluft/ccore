#ifndef __CCORE_CALLBACK_DELEGATE_H__
#define __CCORE_CALLBACK_DELEGATE_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    /// Stores a callback for a function taking 1 parameters.
    ///\tparam R Callback function return type.
    template <typename R, typename... Args>
    class callback_t
    {
    public:
        /// Constructs the callback to a specific object and member function.
        ///\param object Pointer to the object to call upon. Care should be taken that this object remains valid as long as the callback may be invoked.
        ///\param function Member function address to call.
        template <typename C>
        callback_t(C* object, R (C::*function)(Args... args))
            : mCallback(new(&mMem) ChildMethod<C>(object, function))
        {
        }

        /// Constructs the callback to a free function or static member function.
        ///\param function Free function address to call.
        callback_t(R (*function)(Args... args))
            : mCallback(new(&mMem) ChildFree(function))
        {
        }

        /// Constructs a callback that can later be set.
        callback_t()
            : mCallback(0)
        {
        }

        callback_t(const callback_t& c)
            : mCallback(c.mCallback)
        {
            if (mCallback)
            {
                s32 const size = (s32)((sizeof(mMem) + 7) / 8);
                for (s32 i = 0; i < size; ++i)
                    mMem[i] = c.mMem[i];
                mCallback = reinterpret_cast<Base*>(&mMem);
            }
        }

        callback_t& operator=(const callback_t& rhs)
        {
            mCallback = rhs.mCallback;
            if (mCallback)
            {
                s32 const size = (s32)((sizeof(mMem) + 7) / 8);
                for (s32 i = 0; i < size; ++i)
                    mMem[i] = rhs.mMem[i];
                mCallback = reinterpret_cast<Base*>(&mMem);
            }

            return *this;
        }

        ~callback_t() {}

        /// Sets the callback to a specific object and member function.
        ///\param object Pointer to the object to call upon. Care should be taken that this object remains valid as long as the callback may be invoked.
        ///\param function Member function address to call.
        template <typename C>
        void Reset(C* object, R (C::*function)(Args... args))
        {
            mCallback = new (&mMem) ChildMethod<C>(object, function);
        }

        /// Sets the callback to a free function or static member function.
        ///\param function Free function address to call.
        void Reset(R (*function)(Args... args)) { mCallback = new (&mMem) ChildFree(function); }

        /// Resests to callback to nothing.
        inline void Reset() { mCallback = nullptr; }

        /// Note that comparison operators may not work with virtual function callbacks.
        inline bool operator==(const callback_t& rhs) const
        {
            if (mCallback && rhs.mCallback)
                return (*mCallback) == (*(rhs.mCallback));
            return mCallback == rhs.mCallback;
        }

        /// Note that comparison operators may not work with virtual function callbacks.
        inline bool operator!=(const callback_t& rhs) const { return !(*this == rhs); }

        /// Note that comparison operators may not work with virtual function callbacks.
        inline bool operator<(const callback_t rhs) const
        {
            if (mCallback && rhs.mCallback)
                return (*mCallback) < (*(rhs.mCallback));
            return mCallback < rhs.mCallback;
        }

        /// Returns true if the callback has been set, or false if the callback is not set and is invalid.
        inline bool IsSet() const { return mCallback != nullptr; }

        /// Invokes the callback.
        inline R operator()(Args... args) const { return (*mCallback)(args...); }

        /// Invokes the callback. This function can sometimes be more convenient than the operator(), which does the same thing.
        inline R Call(Args... args) const { return (*mCallback)(args...); }

    private:
        class Base
        {
        public:
            Base() {}
            virtual R           operator()(Args... args) const    = 0;
            virtual bool        operator==(const Base& rhs) const = 0;
            virtual bool        operator<(const Base& rhs) const  = 0;
            virtual void const* FreeFunction() const              = 0;
            virtual void const* MethodFunction() const            = 0;
            virtual void*       Comp() const                      = 0;  // Returns a pointer used in comparisons.
        };

        class ChildFree : public Base
        {
        public:
            DCORE_CLASS_PLACEMENT_NEW_DELETE

            ChildFree(R (*function)(Args... args))
                : mFunc(function)
            {
            }

            virtual R operator()(Args... args) const final { return mFunc(args...); }

            virtual bool operator==(const Base& rhs) const final
            {
                const ChildFree* const r = (const ChildFree*)rhs.FreeFunction();
                if (r)
                    return (mFunc == r->mFunc);
                return false;
            }

            virtual bool operator<(const Base& rhs) const final
            {
                const ChildFree* const r = (const ChildFree*)rhs.FreeFunction();
                if (r)
                    return (void const*)mFunc < (void const*)r->mFunc;
                return true;  // Free functions will always be less than methods (because comp returns 0).
            }

            virtual void const* FreeFunction() const final { return this; }
            virtual void const* MethodFunction() const final { return nullptr; }

            virtual void* Comp() const final { return 0; }

        private:
            R (*const mFunc)(Args... args);
        };

        template <typename C>
        class ChildMethod : public Base
        {
        public:
            DCORE_CLASS_PLACEMENT_NEW_DELETE

            ChildMethod(C* object, R (C::*function)(Args... args))
                : mObj(object)
                , mFunc(function)
            {
            }

            virtual R operator()(Args... args) const final { return (mObj->*mFunc)(args...); }

            virtual bool operator==(const Base& rhs) const final
            {
                const ChildMethod<C>* const r = (const ChildMethod<C>*)rhs.MethodFunction();
                if (r)
                    return (mObj == r->mObj) && (mFunc == r->mFunc);
                return false;
            }

            virtual bool operator<(const Base& rhs) const final
            {
                const ChildMethod<C>* const r = (const ChildMethod<C>*)rhs.MethodFunction();
                if (r)
                {
                    if (mObj != r->mObj)
                        return mObj < r->mObj;
                    s32 const        size = (s32)((sizeof(mFunc) + 7) / 8);
                    u64 const* const mem  = (u64 const*)&mFunc;
                    u64 const* const rmem = (u64 const*)&(r->mFunc);
                    for (s32 i = 0; i < size; ++i)
                    {
                        if (mem[i] != rmem[i])
                            return mem[i] < rmem[i];
                    }
                    return false;
                }
                return mObj < rhs.Comp();
            }
            virtual void const* FreeFunction() const final { return nullptr; }
            virtual void const* MethodFunction() const final { return this; }

            virtual void* Comp() const final { return mObj; }

        private:
            C* const mObj;
            R (C::* const mFunc)(Args... args);
        };

        /// This class is only to find the worst case method pointer size.
        class unknown;

        u64   mMem[(sizeof(ChildMethod<unknown>) + 7) / 8];  // Reserve memory for creating useful objects later.
        Base* mCallback;
    };
}  // namespace ncore

#endif  // __CCORE_ALLOCATOR_H__

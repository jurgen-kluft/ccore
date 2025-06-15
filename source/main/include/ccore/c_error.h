#ifndef __CCORE_ERROR_H__
#define __CCORE_ERROR_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    // ------------------------------------------------------------------------
    // error_t
    //
    // This is a type that is used to represent error codes

    typedef s64 error_t;

    namespace nerror
    {
        // Basic error codes
        const error_t cSuccess         = 0;    // No error.
        const error_t cError           = -1;   // Generic error.
        const error_t cNotFound        = -2;   // Not found error.
        const error_t cInvalidArgument = -3;   // Invalid argument error.
        const error_t cCoreMaxError    = -10;  // Maximum error code for core errors.

        // error to string
        const char* to_string(error_t error);

        // global error management
        void error(error_t error);
        error_t last_error();
        const char* last_error_string();

        // to string function object (chained as a list of handlers)
        typedef const char* (*to_string_handler_func_t)(error_t error);

        bool insert_handler(to_string_handler_func_t handler);
        void remove_handler(to_string_handler_func_t handler);

    }  // namespace nerror

};  // namespace ncore

#endif  /// __CCORE_ERROR_H__

#include "ccore/c_target.h"
#include "ccore/c_error.h"

namespace ncore
{
    namespace nerror
    {
        static error_t g_last_errors[8]  = {cSuccess};
        static s32     g_num_last_errors = 0;

        void error(error_t error)
        {
            if (g_num_last_errors < DArrayCount(g_last_errors))
            {
                g_last_errors[g_num_last_errors++] = error;
            }
            else
            {
                // If we reach the limit, we discard the oldest error
                for (s32 i = 1; i < DArrayCount(g_last_errors); ++i)
                    g_last_errors[i - 1] = g_last_errors[i];
                g_last_errors[DArrayCount(g_last_errors) - 1] = error;
            }
        }

        static s32                      gNumErrorToStringHandlers  = 0;
        static to_string_handler_func_t gErrorToStringHandlers[32] = {nullptr};

        static const char* s_error_to_string(error_t error)
        {
            const char* error_string = "Unknown Error";
            if (error >= cCoreMaxError && error <= cSuccess)
            {
                switch (error)
                {
                    case cSuccess: error_string = "Success"; break;
                    case cError: error_string = "Error"; break;
                    case cNotFound: error_string = "Not Found"; break;
                    case cInvalidArgument: error_string = "Invalid Argument"; break;
                }
            }
            else
            {
                for (s32 i = 0; i < gNumErrorToStringHandlers; ++i)
                {
                    const char* str = gErrorToStringHandlers[i](error);
                    if (str != nullptr)
                    {
                        error_string = str;
                        break;
                    }
                }
            }
            return error_string;
        }

        bool insert_handler(to_string_handler_func_t handler)
        {
            if (handler == nullptr || gNumErrorToStringHandlers >= DArrayCount(gErrorToStringHandlers))
                return false;

            gErrorToStringHandlers[gNumErrorToStringHandlers++] = handler;
            return true;
        }

        void remove_handler(to_string_handler_func_t handler)
        {
            if (handler == nullptr)
                return;
            for (s32 i = 0; i < gNumErrorToStringHandlers; ++i)
            {
                if (gErrorToStringHandlers[i] == handler)
                {
                    gErrorToStringHandlers[i] = gErrorToStringHandlers[gNumErrorToStringHandlers - 1];
                    gNumErrorToStringHandlers--;
                    break;
                }
            }
        }

        error_t last_error()
        {
            if (g_num_last_errors > 0)
                return g_last_errors[g_num_last_errors - 1];
            return cSuccess;  // No errors recorded
        }

        const char* last_error_string()
        {
            if (g_num_last_errors > 0)
                return s_error_to_string(g_last_errors[g_num_last_errors - 1]);
            return nullptr;
        }

    }  // namespace nerror
}  // namespace ncore

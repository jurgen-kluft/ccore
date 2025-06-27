#include "ccore/c_target.h"
#include "ccore/c_error.h"

namespace ncore
{
    namespace nerror
    {
        struct error_handler_t
        {
            error_t                  begin;
            error_t                  end;
            to_string_handler_func_t handler;
        };
        static error_handler_t gErrorToStringHandlers[32] = {};
        static s32             gNumErrorToStringHandlers  = 0;
        static s64             gErrorBase                 = -cCoreNumErrors;

        static const char* s_error_to_string(error_t error)
        {
            const char* error_string = "Unknown Error";
            if (error >= -cCoreNumErrors && error <= cSuccess)
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
                    if (error < gErrorToStringHandlers[i].begin || error > gErrorToStringHandlers[i].end)
                        continue;

                    const char* str = gErrorToStringHandlers[i].handler(error);
                    if (str != nullptr)
                    {
                        error_string = str;
                        break;
                    }
                }
            }
            return error_string;
        }

        const char* to_string(error_t error) { return s_error_to_string(error); }

        s64 insert_handler(to_string_handler_func_t handler, s64 error_count)
        {
            if (handler == nullptr || gNumErrorToStringHandlers >= DArrayCount(gErrorToStringHandlers))
                return cMaximumReached;

            gErrorBase -= error_count;
            gErrorToStringHandlers[gNumErrorToStringHandlers].handler = handler;
            gErrorToStringHandlers[gNumErrorToStringHandlers].begin   = gErrorBase;
            gErrorToStringHandlers[gNumErrorToStringHandlers].end     = gErrorBase + error_count;
            gNumErrorToStringHandlers += 1;
            return gErrorBase;
        }

        void remove_handler(to_string_handler_func_t handler)
        {
            if (handler == nullptr)
                return;
            for (s32 i = 0; i < gNumErrorToStringHandlers; ++i)
            {
                if (gErrorToStringHandlers[i].handler == handler)
                {
                    gErrorToStringHandlers[i] = gErrorToStringHandlers[gNumErrorToStringHandlers - 1];
                    gNumErrorToStringHandlers--;
                    break;
                }
            }
        }

        // Global error management

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
            return "Success";  // No errors recorded
        }

    }  // namespace nerror
}  // namespace ncore

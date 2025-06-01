#include "ccore/c_target.h"
#include "ccore/c_error.h"

namespace ncore
{
    namespace nerror
    {
        static s32 gNumErrorToStringHandlers = 0;
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
    }  // namespace nerror
}  // namespace ncore


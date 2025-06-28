#include "ccore/c_target.h"
#include "ccore/c_debug.h"

#ifdef D_ASSERT

#    ifndef D_ASSERT_DISABLE_STDIO
#        include <stdio.h>
#    endif

namespace ncore
{
    //==============================================================================
    // Default input func
    //==============================================================================
    class asserthandler_default_t : public asserthandler_t
    {
    public:
        bool handle_assert(const char* fileName, s32 lineNumber, const char* exprString, const char* messageString);
    };

    static asserthandler_default_t sAssertHandlerDefault;
    static asserthandler_t*        sAssertHandler = &sAssertHandlerDefault;

    void gSetAssertHandler(asserthandler_t* handler)
    {
        if (handler != nullptr)
        {
            sAssertHandler = handler;
            return;
        }
        sAssertHandler = &sAssertHandlerDefault;
    }

    bool gAssertHandler(const char* fileName, s32 lineNumber, const char* exprString, const char* messageString)
    {
        return sAssertHandler->handle_assert(fileName, lineNumber, exprString, messageString);
    }

    bool asserthandler_default_t::handle_assert(const char* fileName, s32 lineNumber, const char* exprString, const char* messageString)
    {
        //
        // Survive nullptr entries
        //
        if (fileName == nullptr)
            fileName = "Unknown";
        if (exprString == nullptr)
            exprString = "Unknown";
        if (messageString == nullptr)
            messageString = "Unknown";

#    ifndef D_ASSERT_DISABLE_STDIO
        printf("Assert: %s(%d): %s; %s ", fileName, lineNumber, exprString, messageString);
#    endif

        //
        // Default: Skip this assert
        //
        return false;
    }

};  // namespace ncore

#else

namespace ncore
{
    void gSetAssertHandler(asserthandler_t* handler) {}

    bool gAssertHandler(const char* fileName, s32 lineNumber, const char* exprString, const char* messageString) { return false; }
}  // namespace ncore

#endif

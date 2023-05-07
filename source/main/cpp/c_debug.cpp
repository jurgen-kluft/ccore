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
        bool handle_assert(u32& flags, const char* fileName, s32 lineNumber, const char* exprString, const char* messageString);
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

    //------------------------------------------------------------------------------
    // Summary:
    //     PRIVATE FUNCTION
    // Arguments:
    //     flags                - Reference to a local copy of flags for the assert.
    //     fileName             - File name in which the assert happen
    //     lineNumber           - Line number where the assert happen
    //     exprString           - Expression of the assert
    //     messageString        - Additional string containing information about the
    //                            assert.
    // Returns:
    //     bool - True when the program should be halted, False other wise
    // Description:
    //     This function is call when an assert happens.
    // See Also:
    //     gAssertHandler
    //------------------------------------------------------------------------------

    bool gAssertHandler(u32& flags, const char* fileName, s32 lineNumber, const char* exprString, const char* messageString) { return sAssertHandler->handle_assert(flags, fileName, lineNumber, exprString, messageString); }

    bool asserthandler_default_t::handle_assert(u32& flags, const char* fileName, s32 lineNumber, const char* exprString, const char* messageString)
    {
        //
        // handle flags
        //
        if ((flags & asserthandler_t::DB_FLAG_IGNORE) != 0)
        {
            return False;
        }

        //
        // next time ignore it
        //
        flags |= asserthandler_t::DB_FLAG_IGNORE;

        //
        // Survive nullptr entries
        //
        if (fileName == nullptr)
            fileName = "Unknown";
        if (exprString == nullptr)
            exprString = "Unknown";
        if (messageString == nullptr)
            messageString = "Unknown";

            //
            // Create the report to print
            //
            // utf32::runez<1024> report;
            // utf32::runez<64> fmtstr;
            // ascii::crunes_t fmtascii("*  EXPR: %s\n*  MSG : %s\n*  FILE: %s\n*  LINE: %d\n");
            // utf::copy(fmtascii, fmtstr);
            // utf32::sprintf(report, fmtstr, va_t(exprString), va_t(messageString), va_t(fileName), va_t(lineNumber));

            //
            // Dump the scope info
            //
#    ifndef D_ASSERT_DISABLE_STDIO

        // D_LogError("Assert", "%s(%d): %s; %s ", va_t(fileName), va_t(lineNumber), va_t(exprString), va_t(messageString));
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

    bool gAssertHandler(u32& flags, const char* fileName, s32 lineNumber, const char* exprString, const char* messageString) { return false; }
}  // namespace ncore

#endif

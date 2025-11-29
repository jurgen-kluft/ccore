#ifndef __CCORE_PRINTF_H__
#define __CCORE_PRINTF_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_va_list.h"

namespace ncore
{
    /**
     * Output a character to a custom device like UART, used by the printf() function
     * This function is declared here only. You have to write your custom implementation somewhere
     * \param character Character to output
     */
    //void putchar(char character);
    typedef void (*putchar_func_t)(char character);
    extern putchar_func_t putchar;

    s32 printf_(const char* format, const va_t* argv, s32 argc);

    template<typename... Args>
    s32 printf(const char* format, Args... _args)
    {
        const va_t argv[] = {_args...};
        const s32  argc   = sizeof(argv) / sizeof(argv[0]);
        return printf_(format, argv, argc);
    }

    /**
     * Tiny sprintf implementation
     * Due to security reasons (buffer overflow) YOU SHOULD CONSIDER USING (V)SNPRINTF INSTEAD!
     * \param buffer A pointer to the buffer where to store the formatted string. MUST be big enough to store the output!
     * \param format A string that specifies the format of the output
     * \return The number of characters that are WRITTEN into the buffer, not counting the terminating null character
     */
    s32 sprintf_(char* buffer, const char* format, const va_t* argv, s32 argc);
    template<typename... Args>
    s32 sprintf(char* buffer, const char* format, Args... _args)
    {
        const va_t argv[] = {_args...};
        const s32  argc   = sizeof(argv) / sizeof(argv[0]);
        return sprintf_(buffer, format, argv, argc);
    }

    /**
     * Tiny snprintf/vsnprintf implementation
     * \param buffer A pointer to the buffer where to store the formatted string
     * \param count The maximum number of characters to store in the buffer, including a terminating null character
     * \param format A string that specifies the format of the output
     * \param va A value identifying a variable arguments list
     * \return The number of characters that COULD have been written into the buffer, not counting the terminating
     *         null character. A value equal or larger than count indicates truncation. Only when the returned value
     *         is non-negative and less than count, the string has been completely written.
     */
    s32 snprintf_(char* buffer, size_t count, const char* format, const va_t* argv, s32 argc);
    template<typename... Args>
    s32 snprintf(char* buffer, size_t count, const char* format, Args... _args)
    {
        const va_t argv[] = {_args...};
        const s32  argc   = sizeof(argv) / sizeof(argv[0]);
        return snprintf_(buffer, count, format, argv, argc);
    }

    s32 vsnprintf_(char* buffer, size_t count, const char* format, const va_t* argv, s32 argc);
    template<typename... Args>
    s32 vsnprintf(char* buffer, size_t count, const char* format, Args... _args)
    {
        const va_t argv[] = {_args...};
        const s32  argc   = sizeof(argv) / sizeof(argv[0]);
        return vsnprintf_(buffer, count, format, argv, argc);
    }

    /**
     * Tiny vprintf implementation
     * \param format A string that specifies the format of the output
     * \param va A value identifying a variable arguments list
     * \return The number of characters that are WRITTEN into the buffer, not counting the terminating null character
     */
    s32 vprintf_(const char* format, const va_t* argv, s32 argc);

    template<typename... Args>
    s32 vprintf(const char* format, Args... _args)
    {
        const va_t argv[] = {_args...};
        const s32  argc   = sizeof(argv) / sizeof(argv[0]);
        return vprintf_(format, argv, argc);
    }

    /**
     * printf with output function
     * You may use this as dynamic alternative to printf() with its fixed _putchar() output
     * \param out An output function which takes one character and an argument pointer
     * \param arg An argument pointer for user data passed to output function
     * \param format A string that specifies the format of the output
     * \return The number of characters that are sent to the output function, not counting the terminating null character
     */
    s32 fctprintf_(void (*out)(char character, void* arg), void* arg, const char* format, const va_t* argv, s32 argc);
    template<typename... Args>
    s32 fctprintf(void (*out)(char character, void* arg), void* arg, const char* format, Args... _args)
    {
        const va_t argv[] = {_args...};
        const s32  argc   = sizeof(argv) / sizeof(argv[0]);
        return fctprintf_(out, arg, format, argv, argc);
    }

}  // namespace ncore

#endif  // __CCORE_PRINTF_H__

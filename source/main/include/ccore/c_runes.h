#ifndef __CCORE_RUNES_V2_H__
#define __CCORE_RUNES_V2_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
#undef printf

    const uchar32 cEOS = 0x00;  // \0, end of string
    const uchar32 cEOF = 0x05;  // end of file
    const uchar32 cEOL = 0x0A;  // \n, end of line
    const uchar32 cCR  = 0x0D;  // \r, carriage return
    const uchar32 cTAB = 0x09;  // \t, tab

    namespace ascii
    {
        typedef char        rune;
        typedef rune*       prune;
        typedef const rune* pcrune;
        static const char   TERMINATOR   = '\0';
        static const u8     TYPE         = 1;
        static const u64    EMPTY_STRING = 0;

        void reverse(char* str, char* end);

        // return false when there is not enough space in the output
        // would need at least 10 characters for u32 and 20 for u64
        char* utoa(u32 val, char* cursor, char const* end, s32 base, bool octzero = false, bool lowercase = true);
        char* itoa(s32 val, char* cursor, char const* end, s32 base, bool octzero = false, bool lowercase = true);
        char* utoa(u64 val, char* cursor, char const* end, s32 base, bool octzero = false, bool lowercase = true);
        char* itoa(s64 val, char* cursor, char const* end, s32 base, bool octzero = false, bool lowercase = true);

        enum EBoolTypes
        {
            TrueFalse = 0,
            YesNo     = 1,
            LowerCase = 0,
            UpperCase = 2,
            CamelCase = 4,
        };
        char* btoa(bool val, char* cursor, char const* end, s8 caseType = TrueFalse | LowerCase);

        s32 compare(pcrune left, pcrune right);
        s32 compare(pcrune str1, u32 len1, pcrune str2, u32 len2);
        s32 strlen(pcrune str);
        s32 strlen(pcrune str, pcrune& end, pcrune eos = nullptr);

        // -------------------------------------------------------------------------------
        inline bool is_space(rune c) { return ((c == 0x09) || (c == 0x0A) || (c == 0x0D) || (c == ' ')); }
        inline bool is_whitespace(rune c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'; }
        inline bool is_upper(rune c) { return ((c >= 'A') && (c <= 'Z')); }
        inline bool is_lower(rune c) { return ((c >= 'a') && (c <= 'z')); }
        inline bool is_alpha(rune c) { return (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'))); }
        inline bool is_digit(rune c) { return ((c >= '0') && (c <= '9')); }
        inline bool is_hexa(rune c) { return (((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f')) || ((c >= '0') && (c <= '9'))); }
        inline rune to_upper(rune c) { return ((c >= 'a') && (c <= 'z')) ? c + (rune)('A' - 'a') : c; }
        inline rune to_lower(rune c) { return ((c >= 'A') && (c <= 'Z')) ? c + (rune)('a' - 'A') : c; }
        inline u32  to_digit(rune c) { return ((c >= '0') && (c <= '9')) ? (c - '0') : c; }
        inline u32  hex_to_number(rune c) { return ((c >= '0') && (c <= '9')) ? (c - '0') : ((c >= 'A') && (c <= 'F')) ? (c - 'A' + 10) : ((c >= 'a') && (c <= 'f')) ? (c - 'a' + 10) : 0; }
        inline char to_dec_char(u8 val) { return "0123456789??????"[val & 0xf]; }
        inline char to_hex_char(u8 val, bool lowercase) { return (lowercase) ? "0123456789abcdef"[val & 0xf] : "0123456789ABCDEF"[val & 0xf]; }
        inline bool is_equal(rune a, rune b) { return (a == b); }
        inline bool is_equalfold(rune a, rune b) { return (to_lower(a) == to_lower(b)); }

    }  // namespace ascii

    namespace ucs2
    {
        static const u8      TYPE         = 8;
        static const uchar16 TERMINATOR   = {'\0'};
        static const u64     EMPTY_STRING = 0;
        typedef uchar16      rune;
        typedef rune*        prune;
        typedef const rune*  pcrune;

        s32 compare(pcrune left, pcrune right);
        s32 compare(pcrune str1, u32 len1, pcrune str2, u32 len2);
        s32 strlen(pcrune str);                           // return length of string in runes
        s32 strlen(pcrune str, pcrune& end, pcrune eos);  // return length of string in runes

        inline bool is_space(rune c) { return ((c == 0x09) || (c == 0x0A) || (c == 0x0D) || (c == ' ')); }
        inline bool is_whitespace(rune c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'; }
        inline bool is_upper(rune c) { return ((c >= 'A') && (c <= 'Z')); }
        inline bool is_lower(rune c) { return ((c >= 'a') && (c <= 'z')); }
        inline bool is_alpha(rune c) { return (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'))); }
        inline bool is_digit(rune c) { return ((c >= '0') && (c <= '9')); }
        inline bool is_hexa(rune c) { return (((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f')) || ((c >= '0') && (c <= '9'))); }
        inline rune to_upper(rune c) { return ((c >= 'a') && (c <= 'z')) ? c + (rune)('A' - 'a') : c; }
        inline rune to_lower(rune c) { return ((c >= 'A') && (c <= 'Z')) ? c + (rune)('a' - 'A') : c; }
        inline u32  to_digit(rune c) { return ((c >= '0') && (c <= '9')) ? (c - '0') : c; }
        inline u32  hex_to_number(rune c) { return ((c >= '0') && (c <= '9')) ? (c - '0') : ((c >= 'A') && (c <= 'F')) ? (c - 'A' + 10) : ((c >= 'a') && (c <= 'f')) ? (c - 'a' + 10) : 0; }
        inline char to_dec_char(u8 val) { return "0123456789??????"[val & 0xf]; }
        inline char to_hex_char(u8 val, bool lowercase) { return (lowercase) ? "0123456789abcdef"[val & 0xf] : "0123456789ABCDEF"[val & 0xf]; }
        inline bool is_equal(rune a, rune b) { return (a == b); }
        inline bool is_equalfold(rune a, rune b) { return (to_lower(a) == to_lower(b)); }
    }  // namespace ucs2

    namespace utf8
    {
        static const u8     TYPE         = 16;
        static const u8     TERMINATOR   = '\0';
        static const u64    EMPTY_STRING = 0;
        typedef uchar8      rune;
        typedef rune*       prune;
        typedef const rune* pcrune;

        s32 compare(pcrune left, pcrune right);
        s32 compare(pcrune str1, u32 len1, pcrune str2, u32 len2);
        s32 strlen(pcrune str);                           // return length of string in runes
        s32 strlen(pcrune str, pcrune& end, pcrune eos);  // return length of string in runes

        inline bool is_space(rune c) { return ((c == 0x09) || (c == 0x0A) || (c == 0x0D) || (c == ' ')); }
        inline bool is_whitespace(rune c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'; }
        inline bool is_upper(rune c) { return ((c >= 'A') && (c <= 'Z')); }
        inline bool is_lower(rune c) { return ((c >= 'a') && (c <= 'z')); }
        inline bool is_alpha(rune c) { return (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'))); }
        inline bool is_digit(rune c) { return ((c >= '0') && (c <= '9')); }
        inline bool is_hexa(rune c) { return (((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f')) || ((c >= '0') && (c <= '9'))); }
        inline rune to_upper(rune c) { return ((c >= 'a') && (c <= 'z')) ? c + (rune)('A' - 'a') : c; }
        inline rune to_lower(rune c) { return ((c >= 'A') && (c <= 'Z')) ? c + (rune)('a' - 'A') : c; }
        inline u32  to_digit(rune c) { return ((c >= '0') && (c <= '9')) ? (c - '0') : c; }
        inline u32  hex_to_number(rune c) { return ((c >= '0') && (c <= '9')) ? (c - '0') : ((c >= 'A') && (c <= 'F')) ? (c - 'A' + 10) : ((c >= 'a') && (c <= 'f')) ? (c - 'a' + 10) : 0; }
        inline char to_dec_char(u8 val) { return "0123456789??????"[val & 0xf]; }
        inline char to_hex_char(u8 val, bool lowercase) { return (lowercase) ? "0123456789abcdef"[val & 0xf] : "0123456789ABCDEF"[val & 0xf]; }
        inline bool is_equal(rune a, rune b) { return (a == b); }
        inline bool is_equalfold(rune a, rune b) { return (to_lower(a) == to_lower(b)); }
    }  // namespace utf8

    namespace utf16
    {
        static const u8      TYPE         = 2;
        static const uchar16 TERMINATOR   = '\0';
        static const u64     EMPTY_STRING = 0;
        typedef uchar16      rune;
        typedef rune*        prune;
        typedef const rune*  pcrune;

        s32 compare(pcrune left, pcrune right);
        s32 compare(pcrune str1, u32 len1, pcrune str2, u32 len2);
        s32 strlen(pcrune str);
        s32 strlen(pcrune str, pcrune& end, pcrune eos);

        inline bool is_space(rune c) { return ((c == 0x09) || (c == 0x0A) || (c == 0x0D) || (c == ' ')); }
        inline bool is_whitespace(rune c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'; }
        inline bool is_upper(rune c) { return ((c >= 'A') && (c <= 'Z')); }
        inline bool is_lower(rune c) { return ((c >= 'a') && (c <= 'z')); }
        inline bool is_alpha(rune c) { return (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'))); }
        inline bool is_digit(rune c) { return ((c >= '0') && (c <= '9')); }
        inline bool is_hexa(rune c) { return (((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f')) || ((c >= '0') && (c <= '9'))); }
        inline rune to_upper(rune c) { return ((c >= 'a') && (c <= 'z')) ? c + (rune)('A' - 'a') : c; }
        inline rune to_lower(rune c) { return ((c >= 'A') && (c <= 'Z')) ? c + (rune)('a' - 'A') : c; }
        inline u32  to_digit(rune c) { return ((c >= '0') && (c <= '9')) ? (c - '0') : c; }
        inline u32  hex_to_number(rune c) { return ((c >= '0') && (c <= '9')) ? (c - '0') : ((c >= 'A') && (c <= 'F')) ? (c - 'A' + 10) : ((c >= 'a') && (c <= 'f')) ? (c - 'a' + 10) : 0; }
        inline char to_dec_char(u8 val) { return "0123456789??????"[val & 0xf]; }
        inline char to_hex_char(u8 val, bool lowercase) { return (lowercase) ? "0123456789abcdef"[val & 0xf] : "0123456789ABCDEF"[val & 0xf]; }
        inline bool is_equal(rune a, rune b) { return (a == b); }
        inline bool is_equalfold(rune a, rune b) { return (to_lower(a) == to_lower(b)); }
    }  // namespace utf16

    namespace utf32
    {
        static const u8      TYPE         = 4;
        static const uchar32 TERMINATOR   = '\0';
        static const u64     EMPTY_STRING = 0;
        typedef uchar32      rune;
        typedef rune*        prune;
        typedef const rune*  pcrune;

        s32 compare(pcrune left, pcrune right);
        s32 compare(pcrune str1, u32 len1, pcrune str2, u32 len2);
        s32 strlen(pcrune str);
        s32 strlen(pcrune str, pcrune& end, pcrune eos);

        inline bool is_space(rune c) { return ((c == 0x09) || (c == 0x0A) || (c == 0x0D) || (c == ' ')); }
        inline bool is_whitespace(rune c) { return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v'; }
        inline bool is_upper(rune c) { return ((c >= 'A') && (c <= 'Z')); }
        inline bool is_lower(rune c) { return ((c >= 'a') && (c <= 'z')); }
        inline bool is_alpha(rune c) { return (((c >= 'A') && (c <= 'Z')) || ((c >= 'a') && (c <= 'z'))); }
        inline bool is_digit(rune c) { return ((c >= '0') && (c <= '9')); }
        inline bool is_hexa(rune c) { return (((c >= 'A') && (c <= 'F')) || ((c >= 'a') && (c <= 'f')) || ((c >= '0') && (c <= '9'))); }
        inline rune to_upper(rune c) { return ((c >= 'a') && (c <= 'z')) ? c + (rune)('A' - 'a') : c; }
        inline rune to_lower(rune c) { return ((c >= 'A') && (c <= 'Z')) ? c + (rune)('a' - 'A') : c; }
        inline u32  to_digit(rune c) { return ((c >= '0') && (c <= '9')) ? (c - '0') : c; }
        inline u32  hex_to_number(rune c) { return ((c >= '0') && (c <= '9')) ? (c - '0') : ((c >= 'A') && (c <= 'F')) ? (c - 'A' + 10) : ((c >= 'a') && (c <= 'f')) ? (c - 'a' + 10) : 0; }
        inline char to_dec_char(u8 val) { return "0123456789??????"[val & 0xf]; }
        inline char to_hex_char(u8 val, bool lowercase) { return (lowercase) ? "0123456789abcdef"[val & 0xf] : "0123456789ABCDEF"[val & 0xf]; }
        inline bool is_equal(rune a, rune b) { return (a == b); }
        inline bool is_equalfold(rune a, rune b) { return (to_lower(a) == to_lower(b)); }
    }  // namespace utf32

};  // namespace ncore

#endif  ///< __CBASE_RUNES2_H__

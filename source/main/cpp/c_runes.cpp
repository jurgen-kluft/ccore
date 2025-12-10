#include "ccore/c_target.h"
#include "ccore/c_debug.h"
#include "ccore/c_runes.h"

namespace ncore
{
    namespace ascii
    {
        s32 strlen(pcrune _str)
        {
            u32 cur = 0;
            while (true)
            {
                uchar32 c = _str[cur];
                if (c == TERMINATOR)
                    break;
                cur++;
            }
            return (s32)cur;
        }

        s32 strlen(pcrune _str, pcrune& _end, pcrune _eos)
        {
            u32 cur = 0;
            // u32 end = (_eos == nullptr) ? 0x7fffffff : (u32)(_eos - _str);
            while (true)
            {
                uchar32 c = _str[cur];
                if (c == TERMINATOR)
                    break;
                cur++;
            }
            _end = _str + cur;
            return (s32)cur;
        }

        s32 compare(pcrune str1, u32 len1, pcrune str2, u32 len2)
        {
            if (len1 == 0 && len2 == 0)
                return 0;
            else if (len1 == 0)
                return -1;
            else if (len2 == 0)
                return 1;

            u32 cursor1 = 0;
            u32 cursor2 = 0;
            while (cursor1 < len1 && cursor2 < len2)
            {
                uchar32 c1 = str1[cursor1++];
                uchar32 c2 = str2[cursor2++];
                if (c1 == c2)
                {
                    if (c1 == cEOS)
                        break;
                }
                else
                {
                    if (c1 < c2)
                        return -1;
                    else
                        return 1;
                }
            }
            return 0;
        }

        s32 compare(pcrune str1, pcrune str2)
        {
            if (str1 == nullptr && str2 == nullptr)
                return 0;
            else if (str1 == nullptr)
                return -1;
            else if (str2 == nullptr)
                return 1;

            while (*str1 == *str2)
            {
                if (*str1 == TERMINATOR)
                    return 0;
                str1++;
                str2++;
            }
            return (*str1 < *str2) ? -1 : 1;
        }

        void reverse(char* str, char* end)
        {
            // Reverse work buffer
            char* head = str;
            char* tail = end - 1;
            while (head < tail)
            {
                char t = *head;
                *head  = *tail;
                *tail  = t;
                head += 1;
                tail -= 1;
            }
        }

        static s32 divmod10(u32 value, u8& remainder)
        {
            const u32 q = (value / 10);
            remainder   = (u8)(value - (q * 10));
            return (s32)(q);
        }

        static s32 divmod10(s32 value, s8& remainder)
        {
            const s32 q = (value / 10);
            remainder   = (s8)(value - (q * 10));
            return q;
        }

        char* utoa(u32 val, char* cursor, char const* end, s32 base, bool octzero, bool lowercase)
        {
            ASSERT(cursor != nullptr && end != nullptr);

            if ((end - cursor) < 10)
                return cursor;

            rune c;
            s32  sval;
            u8   mod;
            s8   smod;

            char* w = cursor + 10;

            switch (base)
            {
                case 10:
                    // On many machines, unsigned arithmetic is harder than
                    // signed arithmetic, so we do at most one unsigned mod and
                    // divide; this is sufficient to reduce the range of
                    // the incoming value to where signed arithmetic works.

                    if (val > ((~(u32)0) >> 1))
                    {
                        sval = divmod10(val, mod);
                        c    = (uchar32)to_dec_char(mod);
                        w -= 1;
                        *w = (char)c;
                    }
                    else
                    {
                        sval = (s32)val;
                    }

                    do
                    {
                        sval = divmod10(sval, smod);
                        c    = to_dec_char((u8)smod);
                        w -= 1;
                        *w = (char)c;
                    } while (sval != 0);

                    break;

                case 8:
                    do
                    {
                        c = to_dec_char(val & 7);
                        w -= 1;
                        *w = (char)c;
                        val >>= 3;
                    } while (val);
                    if (octzero && c != '0')
                    {
                        *w-- = '0';
                    }
                    break;

                case 16:
                    do
                    {
                        w -= 1;
                        *w = to_hex_char(val & 15, lowercase);
                        val >>= 4;
                    } while (val);

                    break;

                default:
                    /* oops */
                    break;
            }

            // The conversion might not use all 10 characters, so we need to
            // move the characters down to the bottom of the buffer.
            const char* we = cursor + 10;
            while (w < we)
                *cursor++ = *w++;
            return cursor;
        }

        char* itoa(s32 val, char* cursor, char const* end, s32 base, bool octzero, bool lowercase)
        {
            ASSERT(cursor != nullptr && end != nullptr);

            if ((end - cursor) < 11)
                return cursor;

            if (val < 0)
            {
                *cursor++ = '-';
                val       = -val;
            }
            return utoa((u32)val, cursor, end, base, octzero, lowercase);
        }

        static s64 divmod10(u64 value, u8& remainder)
        {
            const u64 q = (value / 10);
            remainder   = (u8)(value - (q * 10));
            return (s64)(q);
        }

        static s64 divmod10(s64 value, s8& remainder)
        {
            const s64 q = (value / 10);
            remainder   = (s8)(value - (q * 10));
            return q;
        }

        char* utoa(u64 val, char* cursor, char const* end, s32 base, bool octzero, bool lowercase)
        {
            rune c;
            s64  sval;
            s8   smod;
            u8   mod;

            if ((end - cursor) < 20)
                return cursor;

            char* w = cursor + 20;

            switch (base)
            {
                case 10:
                    // On many machines, unsigned arithmetic is harder than
                    // signed arithmetic, so we do at most one unsigned mod and
                    // divide; this is sufficient to reduce the range of
                    // the incoming value to where signed arithmetic works.

                    if (val > ((~(u64)0) >> 1))
                    {
                        sval = divmod10(val, mod);
                        c    = to_dec_char(mod);
                        *--w = (char)c;
                    }
                    else
                    {
                        sval = (s64)val;
                    }

                    do
                    {
                        sval = divmod10(sval, smod);
                        c    = to_dec_char((u8)smod);
                        *--w = (char)c;
                    } while (sval != 0);

                    break;

                case 8:
                    do
                    {
                        c    = to_dec_char(val & 7);
                        *--w = (char)c;
                        val >>= 3;
                    } while (val);

                    if (octzero && c != '0')
                    {
                        c    = '0';
                        *--w = (char)c;
                    }

                    break;

                case 16:
                    do
                    {
                        c    = to_hex_char((u8)val, lowercase);
                        *--w = (char)c;
                        val >>= 4;
                    } while (val);

                    break;

                default:
                    /* oops */
                    break;
            }

            // The conversion might not use all 10 characters, so we need to
            // move the characters down to the bottom of the buffer.
            const char* we = cursor + 20;
            while (w < we)
                *cursor++ = *w++;

            return cursor;
        }

        char* itoa(s64 val, char* cursor, char const* end, s32 base, bool octzero, bool lowercase)
        {
            ASSERT(cursor != nullptr && end != nullptr);

            if ((end - cursor) < 11)
                return cursor;

            if (val < 0)
            {
                *cursor++ = '-';
                val       = -val;
            }
            return utoa((u64)val, cursor, end, base, octzero, lowercase);
        }

        char* btoa(bool val, char* cursor, char const* end, s8 caseType)
        {
            const char* boolstr;
            switch (caseType)
            {
                case TrueFalse | LowerCase: boolstr = (val ? "true" : "false"); break;
                case TrueFalse | CamelCase: boolstr = (val ? "True" : "False"); break;
                case TrueFalse | UpperCase: boolstr = (val ? "TRUE" : "FALSE"); break;
                case YesNo | LowerCase: boolstr = (val ? "yes" : "no"); break;
                case YesNo | CamelCase: boolstr = (val ? "Yes" : "No"); break;
                case YesNo | UpperCase: boolstr = (val ? "YES" : "NO"); break;
                default: ASSERT(false); return cursor;
            }
            while (*boolstr != '\0' && cursor < end)
                *cursor++ = *boolstr++;
            return cursor;
        }

    }  // namespace ascii

    namespace ucs2
    {
        s32 compare(pcrune str1, pcrune str2)
        {
            if (str1 == nullptr && str2 == nullptr)
                return 0;
            else if (str1 == nullptr)
                return -1;
            else if (str2 == nullptr)
                return 1;
            while (*str1 == *str2)
            {
                if (*str1 == TERMINATOR)
                    return 0;
                str1++;
                str2++;
            }
            return (*str1 < *str2) ? -1 : 1;
        }

        s32 compare(pcrune str1, u32 len1, pcrune str2, u32 len2)
        {
            if (len1 == 0 && len2 == 0)
                return 0;
            else if (len1 == 0)
                return -1;
            else if (len2 == 0)
                return 1;

            u32 cursor1 = 0;
            u32 cursor2 = 0;
            while (cursor1 < len1 && cursor2 < len2)
            {
                rune c1 = str1[cursor1++];
                rune c2 = str2[cursor2++];
                if (c1 == c2)
                {
                    if (c1 == cEOS)
                        break;
                }
                else
                {
                    if (c1 < c2)
                        return -1;
                    else
                        return 1;
                }
            }
            return 0;
        }

        s32 strlen(pcrune _str, pcrune& _end, pcrune _eos)
        {
            u32 cur = 0;
            //u32 end = (_eos == nullptr) ? 0x7fffffff : (u32)(_eos - _str);
            while (true)
            {
                u32  cursor = cur;
                rune c      = _str[cursor++];
                if (c == TERMINATOR)
                    break;
                cur = cursor;
            }
            _end = _str + cur;
            return (s32)cur;
        }

        s32 strlen(pcrune _str)
        {
            u32 cur = 0;
            while (true)
            {
                rune c = _str[cur];
                if (c == TERMINATOR)
                    break;
                cur++;
            }
            return (s32)cur;
        }
    }  // namespace ucs2

    namespace utf8
    {
        s32 compare(pcrune str1, pcrune str2)
        {
            if (str1 == nullptr && str2 == nullptr)
                return 0;
            else if (str1 == nullptr)
                return -1;
            else if (str2 == nullptr)
                return 1;
            while (*str1 == *str2)
            {
                if (*str1 == TERMINATOR)
                    return 0;
                str1++;
                str2++;
            }
            return (*str1 < *str2) ? -1 : 1;
        }

        s32 compare(pcrune str1, u32 len1, pcrune str2, u32 len2)
        {
            if (len1 == 0 && len2 == 0)
                return 0;
            else if (len1 == 0)
                return -1;
            else if (len2 == 0)
                return 1;

            u32 cursor1 = 0;
            u32 cursor2 = 0;
            while (cursor1 < len1 && cursor2 < len2)
            {
                rune c1 = str1[cursor1++];
                rune c2 = str2[cursor2++];
                if (c1 == c2)
                {
                    if (c1 == cEOS)
                        break;
                }
                else
                {
                    if (c1 < c2)
                        return -1;
                    else
                        return 1;
                }
            }
            return 0;
        }

        s32 strlen(pcrune _str, pcrune& _end, pcrune _eos)
        {
            u32 cur = 0;
            //u32 end = (_eos == nullptr) ? 0x7fffffff : (u32)(_eos - _str);
            while (true)
            {
                u32  cursor = cur;
                rune c      = _str[cursor++];
                if (c == TERMINATOR)
                    break;
                cur = cursor;
            }
            _end = _str + cur;
            return (s32)cur;
        }

        s32 strlen(pcrune _str)
        {
            u32 cur = 0;
            //u32 end = 0x7fffffff;
            while (true)
            {
                u32  cursor = cur;
                rune c      = _str[cursor++];
                if (c == TERMINATOR)
                    break;
                cur = cursor;
            }
            return (s32)cur;
        }
    }  // namespace utf8

    namespace utf16
    {
        s32 compare(pcrune str1, pcrune str2)
        {
            if (str1 == nullptr && str2 == nullptr)
                return 0;
            else if (str1 == nullptr)
                return -1;
            else if (str2 == nullptr)
                return 1;

            while (*str1 == *str2)
            {
                if (*str1 == TERMINATOR)
                    return 0;
                str1++;
                str2++;
            }
            return (*str1 < *str2) ? -1 : 1;
        }

        s32 compare(pcrune str1, u32 len1, pcrune str2, u32 len2)
        {
            if (len1 == 0 && len2 == 0)
                return 0;
            else if (len1 == 0)
                return -1;
            else if (len2 == 0)
                return 1;

            u32 cursor1 = 0;
            u32 cursor2 = 0;
            while (cursor1 < len1 && cursor2 < len2)
            {
                rune c1 = str1[cursor1++];
                rune c2 = str2[cursor2++];
                if (c1 == c2)
                {
                    if (c1 == cEOS)
                        break;
                }
                else
                {
                    if (c1 < c2)
                        return -1;
                    else
                        return 1;
                }
            }
            return 0;
        }

        s32 strlen(pcrune _str, pcrune& _end, pcrune _eos)
        {
            u32 cur = 0;
            //u32 end = (_eos == nullptr) ? 0x7fffffff : (u32)(_eos - _str);
            while (true)
            {
                u32  cursor = cur;
                rune c      = _str[cursor++];
                if (c == TERMINATOR)
                    break;
                cur = cursor;
            }
            _end = _str + cur;
            return (s32)cur;
        }

        s32 strlen(pcrune _str)
        {
            u32 cur = 0;
            //u32 end = 0x7fffffff;
            while (true)
            {
                u32  cursor = cur;
                rune c      = _str[cursor++];
                if (c == TERMINATOR)
                    break;
                cur = cursor;
            }
            return (s32)cur;
        }
    }  // namespace utf16

    namespace utf32
    {
        s32 compare(pcrune str1, pcrune str2)
        {
            if (str1 == nullptr && str2 == nullptr)
                return 0;
            else if (str1 == nullptr)
                return -1;
            else if (str2 == nullptr)
                return 1;

            while (*str1 == *str2)
            {
                if (*str1 == TERMINATOR)
                    return 0;
                str1++;
                str2++;
            }
            return (*str1 < *str2) ? -1 : 1;
        }

        s32 compare(pcrune str1, u32 len1, pcrune str2, u32 len2)
        {
            if (len1 == 0 && len2 == 0)
                return 0;
            else if (len1 == 0)
                return -1;
            else if (len2 == 0)
                return 1;

            u32 cursor1 = 0;
            u32 cursor2 = 0;
            while (cursor1 < len1 && cursor2 < len2)
            {
                rune c1 = str1[cursor1++];
                rune c2 = str2[cursor2++];
                if (c1 == c2)
                {
                    if (c1 == cEOS)
                        break;
                }
                else
                {
                    if (c1 < c2)
                        return -1;
                    else
                        return 1;
                }
            }
            return 0;
        }

        s32 strlen(pcrune _str, pcrune& _end, pcrune _eos)
        {
            u32 cur = 0;
            //u32 end = (_eos == nullptr) ? 0x7fffffff : (u32)(_eos - _str);
            while (true)
            {
                u32  cursor = cur;
                rune c      = _str[cursor++];
                if (c == TERMINATOR)
                    break;
                cur = cursor;
            }
            _end = _str + cur;
            return (s32)cur;
        }

        s32 strlen(pcrune _str)
        {
            u32 cur = 0;
            //u32 end = 0x7fffffff;
            while (true)
            {
                u32  cursor = cur;
                rune c      = _str[cursor++];
                if (c == TERMINATOR)
                    break;
                cur = cursor;
            }
            return (s32)cur;
        }
    }  // namespace utf32

}  // namespace ncore

#include "ccore/c_memory.h"

namespace ncore
{
    namespace nmem
    {
#define wsize (int_t)sizeof(u32)
#define wmask (wsize - 1)
        void *memcpy(void *dst0, const void *src0, int_t length)
        {
            // Copy a block of memory, handling overlap.
            // This is the routine that actually implements
            // (the portable versions of) bcopy, memcpy, and memmove.

            char       *dst = (char *)dst0;
            const char *src = (const char *)src0;
            ptr_t       t;

            if (length == 0 || dst == src) /* nothing to do */
                goto done;

            /*
             * Macros: loop-t-times; and loop-t-times, t>0
             */
#define TLOOP(s) \
    if (t)       \
    TLOOP1(s)
#define TLOOP1(s) \
    do            \
    {             \
        s;        \
    } while (--t)

            if (dst < src)
            {
                /*
                 * Copy forward.
                 */
                t = (ptr_t)src; /* only need low bits */
                if ((t | (ptr_t)dst) & wmask)
                {
                    /*
                     * Try to align operands.  This cannot be done
                     * unless the low bits match.
                     */
                    if ((t ^ (ptr_t)dst) & wmask || length < wsize)
                        t = length;
                    else
                        t = wsize - (t & wmask);
                    length -= t;
                    TLOOP1(*dst++ = *src++);
                }
                /*
                 * Copy whole words, then mop up any trailing bytes.
                 */
                t = length / wsize;
                TLOOP(*(u32 *)dst = *(u32 *)src; src += wsize; dst += wsize);
                t = length & wmask;
                TLOOP(*dst++ = *src++);
            }
            else
            {
                /*
                 * Copy backwards.  Otherwise essentially the same.
                 * Alignment works as before, except that it takes
                 * (t&wmask) bytes to align, not wsize-(t&wmask).
                 */
                src += length;
                dst += length;
                t = (ptr_t)src;
                if ((t | (ptr_t)dst) & wmask)
                {
                    if ((t ^ (ptr_t)dst) & wmask || length <= wsize)
                        t = length;
                    else
                        t &= wmask;
                    length -= t;
                    TLOOP1(*--dst = *--src);
                }
                t = length / wsize;
                TLOOP(src -= wsize; dst -= wsize; *(u32 *)dst = *(u32 *)src);
                t = length & wmask;
                TLOOP(*--dst = *--src);
            }
        done:
            return (dst0);
        }

        void *memset(void *dest, u32 c, int_t n)
        {
            unsigned char *s = (unsigned char *)dest;
            ptr_t          k;

            // Fill head and tail with minimal branching. Each
            // conditional ensures that all the subsequently used
            // offsets are well-defined and in the dest region.

            if (!n)
                return dest;

            int_t const r = 8 ^ ((n ^ 8) & -(n < 8)); // min(x, y)
            switch (((r - 1) & 7) >> 1)
            {
                case 3: s[3] = c; s[n-4] = c;
                case 2: s[2] = c; s[n-3] = c;
                case 1: s[1] = c; s[n-2] = c;
                case 0: s[0] = c; s[n-1] = c;
            }
            if (((n-1) & ~7) == 0)
                return dest;

            // Advance pointer to align it at a 4-byte boundary,
            // and truncate n to a multiple of 4. The previous code
            // already took care of any head/tail that get cut off
            // by the alignment.

            k = (4 - ((ptr_t)s & 3)) & 3;
            s += k;
            n -= k;
            n /= 4;

            u32 *ws = (u32 *)s;
            u32  wc = c & 0xFF;
            wc |= ((wc << 8) | (wc << 16) | (wc << 24));

            // Pure C fallback with no aliasing violations.
            for (; n; n--, ws++)
                *ws = wc;

            return dest;
        }

        void *memmove(void *inDest, const void *inSrc, int_t inLength) { return memcpy(inDest, inSrc, inLength); }

        s32 memcmp(const void *p1, const void *p2, int_t inLength)
        {
            u32 i;

            if (!p1)
                return 1;
            if (!p2)
                return -1;

            // p1 and p2 are the same memory? easy peasy! bail out
            if (p1 == p2)
            {
                return 0;
            }

            // This for loop does the comparing and pointer moving...
            for (i = 0; (i < inLength) && (*(u8 *)p1 == *(u8 *)p2); i++, p1 = 1 + (u8 *)p1, p2 = 1 + (u8 *)p2)
            {
                // empty body
            }

            // if i == length, then we have passed the test
            return (i == inLength) ? 0 : (*(u8 *)p1 - *(u8 *)p2);
        }

        void memswap(void *inLHS, void *inRHS, int_t inLength)
        {
            char *p1 = (char *)inLHS;
            char *p2 = (char *)inRHS;
            ASSERT(((p1 < p2) && ((p1 + inLength) <= p2)) || ((p2 < p1) && ((p2 + inLength) <= p1)));
            u8 *lhs = (u8 *)inLHS;
            u8 *rhs = (u8 *)inRHS;
            for (u32 i = 0; i < inLength; i++)
            {
                const u8 t = lhs[i];
                lhs[i]     = rhs[i];
                rhs[i]     = t;
            }
        }
    }  // namespace nmem
}  // namespace ncore

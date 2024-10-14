#include "ccore/c_memory.h"

#if 0

#    include <arm_neon.h>

namespace ncore
{
    namespace nmem
    {
        //---------------------------------------------------------------------
        // fast copy for different sizes
        //---------------------------------------------------------------------
        static D_INLINE void memcpy_neon_16(void *dst, const void *src)
        {
            uint8x16_t m0 = vld1q_u8((const uint8_t *)src);
            vst1q_u8((uint8_t *)dst, m0);
        }

        static D_INLINE void memcpy_neon_32(void *dst, const void *src)
        {
            uint8x16_t m0 = vld1q_u8((const uint8_t *)src);
            uint8x16_t m1 = vld1q_u8((const uint8_t *)src + 16);
            vst1q_u8((uint8_t *)dst, m0);
            vst1q_u8((uint8_t *)dst + 16, m1);
        }

        static D_INLINE void memcpy_neon_64(void *dst, const void *src)
        {
            uint8x16_t m0 = vld1q_u8((const uint8_t *)src);
            uint8x16_t m1 = vld1q_u8((const uint8_t *)src + 16);
            uint8x16_t m2 = vld1q_u8((const uint8_t *)src + 32);
            uint8x16_t m3 = vld1q_u8((const uint8_t *)src + 48);
            vst1q_u8((uint8_t *)dst, m0);
            vst1q_u8((uint8_t *)dst + 16, m1);
            vst1q_u8((uint8_t *)dst + 32, m2);
            vst1q_u8((uint8_t *)dst + 48, m3);
        }

        static D_INLINE void memcpy_neon_128(void *dst, const void *src)
        {
            uint8x16_t m0 = vld1q_u8((const uint8_t *)src);
            uint8x16_t m1 = vld1q_u8((const uint8_t *)src + 16);
            uint8x16_t m2 = vld1q_u8((const uint8_t *)src + 32);
            uint8x16_t m3 = vld1q_u8((const uint8_t *)src + 48);
            uint8x16_t m4 = vld1q_u8((const uint8_t *)src + 64);
            uint8x16_t m5 = vld1q_u8((const uint8_t *)src + 80);
            uint8x16_t m6 = vld1q_u8((const uint8_t *)src + 96);
            uint8x16_t m7 = vld1q_u8((const uint8_t *)src + 112);
            vst1q_u8((uint8_t *)dst, m0);
            vst1q_u8((uint8_t *)dst + 16, m1);
            vst1q_u8((uint8_t *)dst + 32, m2);
            vst1q_u8((uint8_t *)dst + 48, m3);
            vst1q_u8((uint8_t *)dst + 64, m4);
            vst1q_u8((uint8_t *)dst + 80, m5);
            vst1q_u8((uint8_t *)dst + 96, m6);
            vst1q_u8((uint8_t *)dst + 112, m7);
        }

        static D_INLINE void *memcpy_le_128_bytes(void *dst, const void *src, uint_t size)
        {
            unsigned char       *dd = ((unsigned char *)dst);
            const unsigned char *ss = ((const unsigned char *)src);

            switch (size & (128 | 64 | 32 | 16))
            {
                case 128: memcpy_neon_128(dd, ss); return dd + 128;
                case 64 | 32 | 16:
                    memcpy_neon_64(dd, ss);
                    memcpy_neon_32(dd + 64, ss + 64);
                    memcpy_neon_16(dd + 64 + 32, ss + 64 + 32);
                    break;
                case 64 | 32:
                    memcpy_neon_64(dd, ss);
                    memcpy_neon_32(dd + 64, ss + 64);
                    break;
                case 64 | 16:
                    memcpy_neon_64(dd, ss);
                    memcpy_neon_16(dd + 64, ss + 64);
                    break;
                case 64: memcpy_neon_64(dd, ss); break;
                case 32 | 16:
                    memcpy_neon_32(dd, ss);
                    memcpy_neon_16(dd + 32, ss + 32);
                    break;
                case 32: memcpy_neon_32(dd, ss); break;
                case 16: memcpy_neon_16(dd, ss); break;
            }
            dd += size & (128 | 64 | 32 | 16);
            dd -= 16 - (size & 15);
            ss -= 16 - (size & 15);
            memcpy_neon_16(dd, ss);
            return dst;
        }

        //---------------------------------------------------------------------
        // main routine, using NEON instructions to copy memory
        //---------------------------------------------------------------------
        static uint_t c_L2_CacheSize = 0x200000;  // L2-cache size
        static void  *memcpy_SSE(void *destination, const void *source, uint_t size)
        {
            unsigned char       *dst = (unsigned char *)destination;
            const unsigned char *src = (const unsigned char *)source;
            uint_t               padding;

            // small memory copy
            if (size <= 128)
            {
                return memcpy_le_128_bytes(dst, src, size);
            }

            // align destination to 16 bytes boundary
            padding = (16 - (((uint_t)dst) & 15)) & 15;

            if (padding > 0)
            {
                uint8x16_t head = vld1q_u8((const uint8_t *)src);
                vst1q_u8((uint8_t *)dst, head);
                dst += padding;
                src += padding;
                size -= padding;
            }

            // medium size copy
            if (size <= c_L2_CacheSize)
            {
                uint8x16_t c0, c1, c2, c3, c4, c5, c6, c7;

                for (; size >= 128; size -= 128)
                {
                    c0 = vld1q_u8((const uint8_t *)src);
                    c1 = vld1q_u8((const uint8_t *)src + 16);
                    c2 = vld1q_u8((const uint8_t *)src + 32);
                    c3 = vld1q_u8((const uint8_t *)src + 48);
                    c4 = vld1q_u8((const uint8_t *)src + 64);
                    c5 = vld1q_u8((const uint8_t *)src + 80);
                    c6 = vld1q_u8((const uint8_t *)src + 96);
                    c7 = vld1q_u8((const uint8_t *)src + 112);
                    __builtin_prefetch((const char *)(src + 256), 0, 0);
                    src += 128;

                    vst1q_u8((uint8_t *)dst, c0);
                    vst1q_u8((uint8_t *)dst + 16, c1);
                    vst1q_u8((uint8_t *)dst + 32, c2);
                    vst1q_u8((uint8_t *)dst + 48, c3);
                    vst1q_u8((uint8_t *)dst + 64, c4);
                    vst1q_u8((uint8_t *)dst + 80, c5);
                    vst1q_u8((uint8_t *)dst + 96, c6);
                    vst1q_u8((uint8_t *)dst + 112, c7);
                    dst += 128;
                }
            }
            else
            {
                // big memory copy

                uint8x16_t c0, c1, c2, c3, c4, c5, c6, c7;
                __builtin_prefetch((const char *)(src), 0, 0);

                if ((((uint_t)src) & 15) == 0)
                {  // source aligned
                    for (; size >= 128; size -= 128)
                    {
                        c0 = vld1q_u8((const uint8_t *)src);
                        c1 = vld1q_u8((const uint8_t *)src + 16);
                        c2 = vld1q_u8((const uint8_t *)src + 32);
                        c3 = vld1q_u8((const uint8_t *)src + 48);
                        c4 = vld1q_u8((const uint8_t *)src + 64);
                        c5 = vld1q_u8((const uint8_t *)src + 80);
                        c6 = vld1q_u8((const uint8_t *)src + 96);
                        c7 = vld1q_u8((const uint8_t *)src + 112);
                        __builtin_prefetch((const char *)(src + 256), 0, 0);
                        src += 128;

                        vst1q_u8((uint8_t *)dst, c0);
                        vst1q_u8((uint8_t *)dst + 16, c1);
                        vst1q_u8((uint8_t *)dst + 32, c2);
                        vst1q_u8((uint8_t *)dst + 48, c3);
                        vst1q_u8((uint8_t *)dst + 64, c4);
                        vst1q_u8((uint8_t *)dst + 80, c5);
                        vst1q_u8((uint8_t *)dst + 96, c6);
                        vst1q_u8((uint8_t *)dst + 112, c7);
                        dst += 128;
                    }
                }
                else
                {  // source unaligned
                    for (; size >= 128; size -= 128)
                    {
                        c0 = vld1q_u8((const uint8_t *)src);
                        c1 = vld1q_u8((const uint8_t *)src + 16);
                        c2 = vld1q_u8((const uint8_t *)src + 32);
                        c3 = vld1q_u8((const uint8_t *)src + 48);
                        c4 = vld1q_u8((const uint8_t *)src + 64);
                        c5 = vld1q_u8((const uint8_t *)src + 80);
                        c6 = vld1q_u8((const uint8_t *)src + 96);
                        c7 = vld1q_u8((const uint8_t *)src + 112);
                        __builtin_prefetch((const char *)(src + 256), 0, 0);
                        src += 128;

                        vst1q_u8((uint8_t *)dst, c0);
                        vst1q_u8((uint8_t *)dst + 16, c1);
                        vst1q_u8((uint8_t *)dst + 32, c2);
                        vst1q_u8((uint8_t *)dst + 48, c3);
                        vst1q_u8((uint8_t *)dst + 64, c4);
                        vst1q_u8((uint8_t *)dst + 80, c5);
                        vst1q_u8((uint8_t *)dst + 96, c6);
                        vst1q_u8((uint8_t *)dst + 112, c7);
                        dst += 128;
                    }
                }
                __builtin___clear_cache((char *)destination, (char *)(destination + size));
            }

            memcpy_le_128_bytes(dst, src, size);

            return destination;
        }

    }  // namespace nmem
}  // namespace ncore

#endif  // TARGET_CPU_ARM

#include "ccore/c_memory.h"

#define CC_SSE 2

#if CC_SSE >= 2

#include <emmintrin.h>

namespace ncore
{
    #define D_INLINE inline
    namespace nmem
    {
        // https://github.com/skywind3000/FastMemcpy

        //---------------------------------------------------------------------
        // fast copy for different sizes
        //---------------------------------------------------------------------
        static D_INLINE void memcpy_sse2_16(void *dst, const void *src)
        {
            __m128i m0 = _mm_loadu_si128(((const __m128i *)src) + 0);
            _mm_storeu_si128(((__m128i *)dst) + 0, m0);
        }

        static D_INLINE void memcpy_sse2_32(void *dst, const void *src)
        {
            __m128i m0 = _mm_loadu_si128(((const __m128i *)src) + 0);
            __m128i m1 = _mm_loadu_si128(((const __m128i *)src) + 1);
            _mm_storeu_si128(((__m128i *)dst) + 0, m0);
            _mm_storeu_si128(((__m128i *)dst) + 1, m1);
        }

        static D_INLINE void memcpy_sse2_64(void *dst, const void *src)
        {
            __m128i m0 = _mm_loadu_si128(((const __m128i *)src) + 0);
            __m128i m1 = _mm_loadu_si128(((const __m128i *)src) + 1);
            __m128i m2 = _mm_loadu_si128(((const __m128i *)src) + 2);
            __m128i m3 = _mm_loadu_si128(((const __m128i *)src) + 3);
            _mm_storeu_si128(((__m128i *)dst) + 0, m0);
            _mm_storeu_si128(((__m128i *)dst) + 1, m1);
            _mm_storeu_si128(((__m128i *)dst) + 2, m2);
            _mm_storeu_si128(((__m128i *)dst) + 3, m3);
        }

        static D_INLINE void memcpy_sse2_128(void *dst, const void *src)
        {
            __m128i m0 = _mm_loadu_si128(((const __m128i *)src) + 0);
            __m128i m1 = _mm_loadu_si128(((const __m128i *)src) + 1);
            __m128i m2 = _mm_loadu_si128(((const __m128i *)src) + 2);
            __m128i m3 = _mm_loadu_si128(((const __m128i *)src) + 3);
            __m128i m4 = _mm_loadu_si128(((const __m128i *)src) + 4);
            __m128i m5 = _mm_loadu_si128(((const __m128i *)src) + 5);
            __m128i m6 = _mm_loadu_si128(((const __m128i *)src) + 6);
            __m128i m7 = _mm_loadu_si128(((const __m128i *)src) + 7);
            _mm_storeu_si128(((__m128i *)dst) + 0, m0);
            _mm_storeu_si128(((__m128i *)dst) + 1, m1);
            _mm_storeu_si128(((__m128i *)dst) + 2, m2);
            _mm_storeu_si128(((__m128i *)dst) + 3, m3);
            _mm_storeu_si128(((__m128i *)dst) + 4, m4);
            _mm_storeu_si128(((__m128i *)dst) + 5, m5);
            _mm_storeu_si128(((__m128i *)dst) + 6, m6);
            _mm_storeu_si128(((__m128i *)dst) + 7, m7);
        }

        static D_INLINE void *memcpy_le_128_bytes(void *dst, const void *src, size_t size)
        {
            unsigned char       *dd = ((unsigned char *)dst);
            const unsigned char *ss = ((const unsigned char *)src);

            switch (size & (128 | 64 | 32 | 16))
            {
                case 128: memcpy_sse2_128(dd, ss); return dd + 128;
                case 64 | 32 | 16:
                    memcpy_sse2_64(dd, ss);
                    memcpy_sse2_32(dd + 64, ss + 64);
                    memcpy_sse2_16(dd + 64 + 32, ss + 64 + 32);
                    break;
                case 64 | 32:
                    memcpy_sse2_64(dd, ss);
                    memcpy_sse2_32(dd + 64, ss + 64);
                    break;
                case 64 | 16:
                    memcpy_sse2_64(dd, ss);
                    memcpy_sse2_16(dd + 64, ss + 64);
                    break;
                case 64: memcpy_sse2_64(dd, ss); break;
                case 32 | 16:
                    memcpy_sse2_32(dd, ss);
                    memcpy_sse2_16(dd + 32, ss + 32);
                    break;
                case 32: memcpy_sse2_32(dd, ss); break;
                case 16: memcpy_sse2_16(dd, ss); break;
            }
            dd += size & (128 | 64 | 32 | 16);
            dd -= 16 - (size & 15);
            ss -= 16 - (size & 15);
            memcpy_sse2_16(dd, ss);
            dd += 16;

            return dd;
        }

        //---------------------------------------------------------------------
        // main routine, using SSE instructions to copy memory
        //---------------------------------------------------------------------
        static size_t        c_L2_CacheSize = 0x200000;  // L2-cache size
        static void *memcpy_SSE(void *destination, const void *source, size_t size)
        {
            unsigned char       *dst       = (unsigned char *)destination;
            const unsigned char *src       = (const unsigned char *)source;
            size_t               padding;

            // small memory copy
            if (size <= 128)
            {
                return memcpy_le_128_bytes(dst, src, size);
            }

            // align destination to 16 bytes boundary
            padding = (16 - (((size_t)dst) & 15)) & 15;

            if (padding > 0)
            {
                __m128i head = _mm_loadu_si128((const __m128i *)src);
                _mm_storeu_si128((__m128i *)dst, head);
                dst += padding;
                src += padding;
                size -= padding;
            }

            // medium size copy
            if (size <= c_L2_CacheSize)
            {
                __m128i c0, c1, c2, c3, c4, c5, c6, c7;

                for (; size >= 128; size -= 128)
                {
                    c0 = _mm_loadu_si128(((const __m128i *)src) + 0);
                    c1 = _mm_loadu_si128(((const __m128i *)src) + 1);
                    c2 = _mm_loadu_si128(((const __m128i *)src) + 2);
                    c3 = _mm_loadu_si128(((const __m128i *)src) + 3);
                    c4 = _mm_loadu_si128(((const __m128i *)src) + 4);
                    c5 = _mm_loadu_si128(((const __m128i *)src) + 5);
                    c6 = _mm_loadu_si128(((const __m128i *)src) + 6);
                    c7 = _mm_loadu_si128(((const __m128i *)src) + 7);
                    _mm_prefetch((const char *)(src + 256), _MM_HINT_NTA);
                    src += 128;
                    _mm_store_si128((((__m128i *)dst) + 0), c0);
                    _mm_store_si128((((__m128i *)dst) + 1), c1);
                    _mm_store_si128((((__m128i *)dst) + 2), c2);
                    _mm_store_si128((((__m128i *)dst) + 3), c3);
                    _mm_store_si128((((__m128i *)dst) + 4), c4);
                    _mm_store_si128((((__m128i *)dst) + 5), c5);
                    _mm_store_si128((((__m128i *)dst) + 6), c6);
                    _mm_store_si128((((__m128i *)dst) + 7), c7);
                    dst += 128;
                }
            }
            else
            {  // big memory copy
                __m128i c0, c1, c2, c3, c4, c5, c6, c7;

                _mm_prefetch((const char *)(src), _MM_HINT_NTA);

                if ((((size_t)src) & 15) == 0)
                {  // source aligned
                    for (; size >= 128; size -= 128)
                    {
                        c0 = _mm_load_si128(((const __m128i *)src) + 0);
                        c1 = _mm_load_si128(((const __m128i *)src) + 1);
                        c2 = _mm_load_si128(((const __m128i *)src) + 2);
                        c3 = _mm_load_si128(((const __m128i *)src) + 3);
                        c4 = _mm_load_si128(((const __m128i *)src) + 4);
                        c5 = _mm_load_si128(((const __m128i *)src) + 5);
                        c6 = _mm_load_si128(((const __m128i *)src) + 6);
                        c7 = _mm_load_si128(((const __m128i *)src) + 7);
                        _mm_prefetch((const char *)(src + 256), _MM_HINT_NTA);
                        src += 128;
                        _mm_stream_si128((((__m128i *)dst) + 0), c0);
                        _mm_stream_si128((((__m128i *)dst) + 1), c1);
                        _mm_stream_si128((((__m128i *)dst) + 2), c2);
                        _mm_stream_si128((((__m128i *)dst) + 3), c3);
                        _mm_stream_si128((((__m128i *)dst) + 4), c4);
                        _mm_stream_si128((((__m128i *)dst) + 5), c5);
                        _mm_stream_si128((((__m128i *)dst) + 6), c6);
                        _mm_stream_si128((((__m128i *)dst) + 7), c7);
                        dst += 128;
                    }
                }
                else
                {  // source unaligned
                    for (; size >= 128; size -= 128)
                    {
                        c0 = _mm_loadu_si128(((const __m128i *)src) + 0);
                        c1 = _mm_loadu_si128(((const __m128i *)src) + 1);
                        c2 = _mm_loadu_si128(((const __m128i *)src) + 2);
                        c3 = _mm_loadu_si128(((const __m128i *)src) + 3);
                        c4 = _mm_loadu_si128(((const __m128i *)src) + 4);
                        c5 = _mm_loadu_si128(((const __m128i *)src) + 5);
                        c6 = _mm_loadu_si128(((const __m128i *)src) + 6);
                        c7 = _mm_loadu_si128(((const __m128i *)src) + 7);
                        _mm_prefetch((const char *)(src + 256), _MM_HINT_NTA);
                        src += 128;
                        _mm_stream_si128((((__m128i *)dst) + 0), c0);
                        _mm_stream_si128((((__m128i *)dst) + 1), c1);
                        _mm_stream_si128((((__m128i *)dst) + 2), c2);
                        _mm_stream_si128((((__m128i *)dst) + 3), c3);
                        _mm_stream_si128((((__m128i *)dst) + 4), c4);
                        _mm_stream_si128((((__m128i *)dst) + 5), c5);
                        _mm_stream_si128((((__m128i *)dst) + 6), c6);
                        _mm_stream_si128((((__m128i *)dst) + 7), c7);
                        dst += 128;
                    }
                }
                _mm_sfence();
            }

            memcpy_le_128_bytes(dst, src, size);

            return destination;
        }

    }  // namespace nmem
}  // namespace ncore

#endif

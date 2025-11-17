#include "ccore/c_memory.h"
#include "ccore/c_target.h"
#include "ccore/c_random.h"

namespace ncore
{
    namespace nrandom
    {
// protections that produce different results:
// 1: normal valid behavior
// 2: extra protection against entropy loss (probability=2^-63), aka. "blind multiplication"
#define WYHASH_PROTECTION 1

        // 128bit multiply function
        static inline u64 _wyrot(u64 x) { return (x >> 32) | (x << 32); }

        static inline void _wymum(u64* A, u64* B)
        {
            u64 ha = *A >> 32, hb = *B >> 32, la = (u32)*A, lb = (u32)*B, hi, lo;
            u64 rh = ha * hb, rm0 = ha * lb, rm1 = hb * la, rl = la * lb, t = rl + (rm0 << 32), c = t < rl;
            lo = t + (rm1 << 32);
            c += lo < t;
            hi = rh + (rm0 >> 32) + (rm1 >> 32) + c;
#if (WYHASH_PROTECTION > 1)
            *A ^= lo;
            *B ^= hi;
#else
            *A = lo;
            *B = hi;
#endif
        }

        // multiply and xor mix function, aka MUM
        static inline u64 _wymix(u64 A, u64 B)
        {
            _wymum(&A, &B);
            return A ^ B;
        }

// read functions
#ifdef D_LITTLE_ENDIAN
        static inline u64 _wyr8(const u8* p)
        {
            u64 v;
            g_memcpy(&v, p, 8);
            return v;
        }
        static inline u64 _wyr4(const u8* p)
        {
            u32 v;
            g_memcpy(&v, p, 4);
            return v;
        }
#else
        static inline u64 _wyr8(const u8* p)
        {
            u64 v;
            g_memcpy(&v, p, 8);
            return (((v >> 56) & 0xff) | ((v >> 40) & 0xff00) | ((v >> 24) & 0xff0000) | ((v >> 8) & 0xff000000) | ((v << 8) & 0xff00000000) | ((v << 24) & 0xff0000000000) | ((v << 40) & 0xff000000000000) | ((v << 56) & 0xff00000000000000));
        }
        static inline u64 _wyr4(const u8* p)
        {
            u32 v;
            g_memcpy(&v, p, 4);
            return (((v >> 24) & 0xff) | ((v >> 8) & 0xff00) | ((v << 8) & 0xff0000) | ((v << 24) & 0xff000000));
        }
#endif
        static inline u64 _wyr3(const u8* p, uint_t k) { return (((u64)p[0]) << 16) | (((u64)p[k >> 1]) << 8) | p[k - 1]; }

        // the default secret parameters
        // static const u64 _wyp[4] = {0xa0761d6478bd642full, 0xe7037ed1a0b428dbull, 0x8ebc6af09c88c6e3ull, 0x589965cc75374cc3ull};

        // a useful 64bit-64bit mix function to produce deterministic pseudo random numbers that can pass BigCrush and PractRand
        //         static inline u64 wyhash64(u64 A, u64 B)
        //         {
        //             A ^= 0xa0761d6478bd642full;
        //             B ^= 0xe7037ed1a0b428dbull;
        //             _wymum(&A, &B);
        //             return _wymix(A ^ 0xa0761d6478bd642full, B ^ 0xe7037ed1a0b428dbull);
        //         }

        // The wyrand PRNG that pass BigCrush and PractRand
        // Referenced externally in c_base.cpp
        u64 wyrand(u64* seed)
        {
            *seed += 0xa0761d6478bd642full;
            return _wymix(*seed, *seed ^ 0xe7037ed1a0b428dbull);
        }

        void wyrand(u64* seed, u8* buffer, u32 size)
        {
            u64 s = *seed;
            u32 i = 0;
            while (i < size)
            {
                s += 0xa0761d6478bd642full;
                u64 const l  = _wymix(s, s ^ 0xe7037ed1a0b428dbull);
                u8 const* lp = (u8 const*)&l;
                for (u32 j = 0; j < 8 && i < size; j++, i++)
                    buffer[i] = lp[j];
            }
            *seed = s;
        }
    }  // namespace nrandom

    void rand_t::reset(s64 inSeed) { m_seed = (u64)inSeed; }
    u32  rand_t::rand32() { return (u32)nrandom::wyrand(&m_seed); }
    u64  rand_t::rand64() { return nrandom::wyrand(&m_seed); }
    void rand_t::generate(u8* outData, u32 numBytes) { nrandom::wyrand(&m_seed, outData, numBytes); }

}  // namespace ncore

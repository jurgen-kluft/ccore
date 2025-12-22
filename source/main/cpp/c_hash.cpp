#include "ccore/c_target.h"
#include "ccore/c_debug.h"
#include "ccore/c_hash.h"
#include "ccore/c_memory.h"
#include "ccore/c_runes.h"

namespace ncore
{
    namespace nhash
    {
        namespace xxhash32
        {
            u32 XXH32(void const *const input, size_t const length, u32 const seed);

            static u32 const PRIME32_1 = 0x9E3779B1U; /* 0b10011110001101110111100110110001 */
            static u32 const PRIME32_2 = 0x85EBCA77U; /* 0b10000101111010111100101001110111 */
            static u32 const PRIME32_3 = 0xC2B2AE3DU; /* 0b11000010101100101010111000111101 */
            static u32 const PRIME32_4 = 0x27D4EB2FU; /* 0b00100111110101001110101100101111 */
            static u32 const PRIME32_5 = 0x165667B1U; /* 0b00010110010101100110011110110001 */

            /* Rotates value left by amt. */
            static u32 XXH_rotl32(u32 const value, u32 const amt) { return (value << (amt % 32)) | (value >> (32 - (amt % 32))); }

            /* Portably reads a 32-bit little endian integer from data at the given offset. */
            static u32 XXH_read32(u8 const *const data, size_t const offset)
            {
                // 4 bytes into a u32 in little-endian order
                return (u32)data[offset + 0] | ((u32)data[offset + 1] << 8) | ((u32)data[offset + 2] << 16) | ((u32)data[offset + 3] << 24);
                //return (u32)data[offset + 3] | ((u32)data[offset + 2] << 8) | ((u32)data[offset + 2] << 16) | ((u32)data[offset + 3] << 24);
            }

            /* Mixes input into acc. */
            static u32 XXH32_round(u32 acc, u32 const input)
            {
                acc += input * PRIME32_2;
                acc = XXH_rotl32(acc, 13);
                acc *= PRIME32_1;
                return acc;
            }

            /* Mixes all bits to finalize the hash. */
            static u32 XXH32_avalanche(u32 hash)
            {
                hash ^= hash >> 15;
                hash *= PRIME32_2;
                hash ^= hash >> 13;
                hash *= PRIME32_3;
                hash ^= hash >> 16;
                return hash;
            }

            /* The XXH32 hash function.
             * input:   The data to hash.
             * length:  The length of input. It is undefined behavior to have length larger than the
             *          capacity of input.
             * seed:    A 32-bit value to seed the hash with.
             * returns: The 32-bit calculated hash value. */
            u32 XXH32(void const *const input, size_t const length, u32 const seed)
            {
                u8 const *const data = (u8 const *)input;
                u32             hash;
                size_t          remaining = length;
                size_t          offset    = 0;

                /* Don't dereference a null pointer. The reference implementation notably doesn't
                 * check for this by default. */
                if (input == NULL)
                {
                    return XXH32_avalanche(seed + PRIME32_5);
                }

                if (remaining >= 16)
                {
                    /* Initialize our accumulators */
                    u32 acc1 = seed + PRIME32_1 + PRIME32_2;
                    u32 acc2 = seed + PRIME32_2;
                    u32 acc3 = seed + 0;
                    u32 acc4 = seed - PRIME32_1;

                    while (remaining >= 16)
                    {
                        acc1 = XXH32_round(acc1, XXH_read32(data, offset));
                        offset += 4;
                        acc2 = XXH32_round(acc2, XXH_read32(data, offset));
                        offset += 4;
                        acc3 = XXH32_round(acc3, XXH_read32(data, offset));
                        offset += 4;
                        acc4 = XXH32_round(acc4, XXH_read32(data, offset));
                        offset += 4;
                        remaining -= 16;
                    }

                    hash = XXH_rotl32(acc1, 1) + XXH_rotl32(acc2, 7) + XXH_rotl32(acc3, 12) + XXH_rotl32(acc4, 18);
                }
                else
                {
                    /* Not enough data for the main loop, put something in there instead. */
                    hash = seed + PRIME32_5;
                }

                hash += (u32)length;

                /* Process the remaining data. */
                while (remaining >= 4)
                {
                    hash += XXH_read32(data, offset) * PRIME32_3;
                    hash = XXH_rotl32(hash, 17);
                    hash *= PRIME32_4;
                    offset += 4;
                    remaining -= 4;
                }

                while (remaining != 0)
                {
                    hash += (u32)data[offset] * PRIME32_5;
                    hash = XXH_rotl32(hash, 11);
                    hash *= PRIME32_1;
                    --remaining;
                    ++offset;
                }
                return XXH32_avalanche(hash);
            }
        }  // namespace xxhash32

        namespace xxhash32_text
        {
            u32 XXH32(char const *const input, size_t const length, u32 const seed);

            static u32 const PRIME32_1 = 0x9E3779B1U; /* 0b10011110001101110111100110110001 */
            static u32 const PRIME32_2 = 0x85EBCA77U; /* 0b10000101111010111100101001110111 */
            static u32 const PRIME32_3 = 0xC2B2AE3DU; /* 0b11000010101100101010111000111101 */
            static u32 const PRIME32_4 = 0x27D4EB2FU; /* 0b00100111110101001110101100101111 */
            static u32 const PRIME32_5 = 0x165667B1U; /* 0b00010110010101100110011110110001 */

            /* Rotates value left by amt. */
            static u32 XXH_rotl32(u32 const value, u32 const amt) { return (value << (amt % 32)) | (value >> (32 - (amt % 32))); }

            /* Portably reads a 32-bit little endian integer from data at the given offset. */
            static u32 XXH_read32(char const *const data, size_t const offset)
            {
                char c4 = ascii::to_lower(data[offset + 0]);
                char c3 = ascii::to_lower(data[offset + 1]);
                char c2 = ascii::to_lower(data[offset + 2]);
                char c1 = ascii::to_lower(data[offset + 3]);
                return (u32)c1 | ((u32)c2 << 8) | ((u32)c3 << 16) | ((u32)c4 << 24);
            }

            /* Mixes input into acc. */
            static u32 XXH32_round(u32 acc, u32 const input)
            {
                acc += input * PRIME32_2;
                acc = XXH_rotl32(acc, 13);
                acc *= PRIME32_1;
                return acc;
            }

            /* Mixes all bits to finalize the hash. */
            static u32 XXH32_avalanche(u32 hash)
            {
                hash ^= hash >> 15;
                hash *= PRIME32_2;
                hash ^= hash >> 13;
                hash *= PRIME32_3;
                hash ^= hash >> 16;
                return hash;
            }

            /* The XXH32 hash function.
             * input:   The data to hash.
             * length:  The length of input. It is undefined behavior to have length larger than the
             *          capacity of input.
             * seed:    A 32-bit value to seed the hash with.
             * returns: The 32-bit calculated hash value. */
            u32 XXH32(char const *const input, size_t const length, u32 const seed)
            {
                char const *const data = input;
                u32               hash;
                size_t            remaining = length;
                size_t            offset    = 0;

                /* Don't dereference a null pointer. The reference implementation notably doesn't
                 * check for this by default. */
                if (input == NULL)
                {
                    return XXH32_avalanche(seed + PRIME32_5);
                }

                if (remaining >= 16)
                {
                    /* Initialize our accumulators */
                    u32 acc1 = seed + PRIME32_1 + PRIME32_2;
                    u32 acc2 = seed + PRIME32_2;
                    u32 acc3 = seed + 0;
                    u32 acc4 = seed - PRIME32_1;

                    while (remaining >= 16)
                    {
                        acc1 = XXH32_round(acc1, XXH_read32(data, offset));
                        offset += 4;
                        acc2 = XXH32_round(acc2, XXH_read32(data, offset));
                        offset += 4;
                        acc3 = XXH32_round(acc3, XXH_read32(data, offset));
                        offset += 4;
                        acc4 = XXH32_round(acc4, XXH_read32(data, offset));
                        offset += 4;
                        remaining -= 16;
                    }

                    hash = XXH_rotl32(acc1, 1) + XXH_rotl32(acc2, 7) + XXH_rotl32(acc3, 12) + XXH_rotl32(acc4, 18);
                }
                else
                {
                    /* Not enough data for the main loop, put something in there instead. */
                    hash = seed + PRIME32_5;
                }

                hash += (u32)length;

                /* Process the remaining data. */
                while (remaining >= 4)
                {
                    hash += XXH_read32(data, offset) * PRIME32_3;
                    hash = XXH_rotl32(hash, 17);
                    hash *= PRIME32_4;
                    offset += 4;
                    remaining -= 4;
                }

                while (remaining != 0)
                {
                    hash += (u32)data[offset] * PRIME32_5;
                    hash = XXH_rotl32(hash, 11);
                    hash *= PRIME32_1;
                    --remaining;
                    ++offset;
                }
                return XXH32_avalanche(hash);
            }
        }  // namespace xxhash32_text

        namespace xxhash64
        {
            u64 XXH64(void const *const input, size_t const length, u64 const seed);

            static u64 const PRIME64_1 = 0x9E3779B185EBCA87ULL; /* 0b1001111000110111011110011011000110000101111010111100101010000111 */
            static u64 const PRIME64_2 = 0xC2B2AE3D27D4EB4FULL; /* 0b1100001010110010101011100011110100100111110101001110101101001111 */
            static u64 const PRIME64_3 = 0x165667B19E3779F9ULL; /* 0b0001011001010110011001111011000110011110001101110111100111111001 */
            static u64 const PRIME64_4 = 0x85EBCA77C2B2AE63ULL; /* 0b1000010111101011110010100111011111000010101100101010111001100011 */
            static u64 const PRIME64_5 = 0x27D4EB2F165667C5ULL; /* 0b0010011111010100111010110010111100010110010101100110011111000101 */

            /* Rotates value left by amt bits. */
            static u64 XXH_rotl64(u64 const value, u32 const amt) { return (value << (amt % 64)) | (value >> (64 - amt % 64)); }

            /* Portably reads a 32-bit little endian integer from data at the given offset. */
            static u32 XXH_read32(u8 const *const data, size_t const offset) { return (u32)data[offset + 0] | ((u32)data[offset + 1] << 8) | ((u32)data[offset + 2] << 16) | ((u32)data[offset + 3] << 24); }

            /* Portably reads a 64-bit little endian integer from data at the given offset. */
            static u64 XXH_read64(u8 const *const data, size_t const offset)
            {
                return (u64)data[offset + 0] | ((u64)data[offset + 1] << 8) | ((u64)data[offset + 2] << 16) | ((u64)data[offset + 3] << 24) | ((u64)data[offset + 4] << 32) | ((u64)data[offset + 5] << 40) | ((u64)data[offset + 6] << 48) |
                       ((u64)data[offset + 7] << 56);
            }

            /* Mixes input into acc, this is mostly used in the first loop. */
            static u64 XXH64_round(u64 acc, u64 const input)
            {
                acc += input * PRIME64_2;
                acc = XXH_rotl64(acc, 31);
                acc *= PRIME64_1;
                return acc;
            }

            /* Merges acc into hash to finalize */
            static u64 XXH64_mergeRound(u64 hash, u64 const acc)
            {
                hash ^= XXH64_round(0, acc);
                hash *= PRIME64_1;
                hash += PRIME64_4;
                return hash;
            }

            /* Mixes all bits to finalize the hash. */
            static u64 XXH64_avalanche(u64 hash)
            {
                hash ^= hash >> 33;
                hash *= PRIME64_2;
                hash ^= hash >> 29;
                hash *= PRIME64_3;
                hash ^= hash >> 32;
                return hash;
            }

            /* The XXH64 hash function.
             * input:   The data to hash.
             * length:  The length of input. It is undefined behavior to have length larger than the
             *          capacity of input.
             * seed:    A 64-bit value to seed the hash with.
             * returns: The 64-bit calculated hash value. */
            u64 XXH64(void const *const input, size_t const length, u64 const seed)
            {
                u8 const *const data      = (u8 const *)input;
                u64             hash      = 0;
                size_t          remaining = length;
                size_t          offset    = 0;

                /* Don't dereference a null pointer. The reference implementation notably doesn't
                 * check for this by default. */
                if (input == NULL)
                {
                    return XXH64_avalanche(seed + PRIME64_5);
                }

                if (remaining >= 32)
                {
                    /* Initialize our accumulators */
                    u64 acc1 = seed + PRIME64_1 + PRIME64_2;
                    u64 acc2 = seed + PRIME64_2;
                    u64 acc3 = seed + 0;
                    u64 acc4 = seed - PRIME64_1;

                    while (remaining >= 32)
                    {
                        acc1 = XXH64_round(acc1, XXH_read64(data, offset));
                        offset += 8;
                        acc2 = XXH64_round(acc2, XXH_read64(data, offset));
                        offset += 8;
                        acc3 = XXH64_round(acc3, XXH_read64(data, offset));
                        offset += 8;
                        acc4 = XXH64_round(acc4, XXH_read64(data, offset));
                        offset += 8;
                        remaining -= 32;
                    }

                    hash = XXH_rotl64(acc1, 1) + XXH_rotl64(acc2, 7) + XXH_rotl64(acc3, 12) + XXH_rotl64(acc4, 18);

                    hash = XXH64_mergeRound(hash, acc1);
                    hash = XXH64_mergeRound(hash, acc2);
                    hash = XXH64_mergeRound(hash, acc3);
                    hash = XXH64_mergeRound(hash, acc4);
                }
                else
                {
                    /* Not enough data for the main loop, put something in there instead. */
                    hash = seed + PRIME64_5;
                }

                hash += (u64)length;

                /* Process the remaining data. */
                while (remaining >= 8)
                {
                    hash ^= XXH64_round(0, XXH_read64(data, offset));
                    hash = XXH_rotl64(hash, 27);
                    hash *= PRIME64_1;
                    hash += PRIME64_4;
                    offset += 8;
                    remaining -= 8;
                }

                if (remaining >= 4)
                {
                    hash ^= (u64)XXH_read32(data, offset) * PRIME64_1;
                    hash = XXH_rotl64(hash, 23);
                    hash *= PRIME64_2;
                    hash += PRIME64_3;
                    offset += 4;
                    remaining -= 4;
                }

                while (remaining != 0)
                {
                    hash ^= (u64)data[offset] * PRIME64_5;
                    hash = XXH_rotl64(hash, 11);
                    hash *= PRIME64_1;
                    ++offset;
                    --remaining;
                }

                return XXH64_avalanche(hash);
            }
        }  // namespace xxhash64

        namespace xxhash64_text
        {
            u64 XXH64(char const *const input, size_t const length, u64 const seed);

            static u64 const PRIME64_1 = 0x9E3779B185EBCA87ULL; /* 0b1001111000110111011110011011000110000101111010111100101010000111 */
            static u64 const PRIME64_2 = 0xC2B2AE3D27D4EB4FULL; /* 0b1100001010110010101011100011110100100111110101001110101101001111 */
            static u64 const PRIME64_3 = 0x165667B19E3779F9ULL; /* 0b0001011001010110011001111011000110011110001101110111100111111001 */
            static u64 const PRIME64_4 = 0x85EBCA77C2B2AE63ULL; /* 0b1000010111101011110010100111011111000010101100101010111001100011 */
            static u64 const PRIME64_5 = 0x27D4EB2F165667C5ULL; /* 0b0010011111010100111010110010111100010110010101100110011111000101 */

            /* Rotates value left by amt bits. */
            static u64 XXH_rotl64(u64 const value, u32 const amt) { return (value << (amt % 64)) | (value >> (64 - amt % 64)); }

            /* Portably reads a 32-bit little endian integer from data at the given offset. */
            static u32 XXH_read32(char const *const data, size_t const offset)
            {
                char c1 = ascii::to_lower(data[offset + 0]);
                char c2 = ascii::to_lower(data[offset + 1]);
                char c3 = ascii::to_lower(data[offset + 2]);
                char c4 = ascii::to_lower(data[offset + 3]);
                return (u32)c1 | ((u32)c2 << 8) | ((u32)c3 << 16) | ((u32)c4 << 24);
            }

            /* Portably reads a 64-bit little endian integer from data at the given offset. */
            static u64 XXH_read64(char const *const data, size_t const offset)
            {
                char c1 = ascii::to_lower(data[offset + 0]);
                char c2 = ascii::to_lower(data[offset + 1]);
                char c3 = ascii::to_lower(data[offset + 2]);
                char c4 = ascii::to_lower(data[offset + 3]);
                char c5 = ascii::to_lower(data[offset + 4]);
                char c6 = ascii::to_lower(data[offset + 5]);
                char c7 = ascii::to_lower(data[offset + 6]);
                char c8 = ascii::to_lower(data[offset + 7]);
                return (u64)c1 | ((u64)c2 << 8) | ((u64)c3 << 16) | ((u64)c4 << 24) | ((u64)c5 << 32) | ((u64)c6 << 40) | ((u64)c7 << 48) | ((u64)c8 << 56);
            }

            /* Mixes input into acc, this is mostly used in the first loop. */
            static u64 XXH64_round(u64 acc, u64 const input)
            {
                acc += input * PRIME64_2;
                acc = XXH_rotl64(acc, 31);
                acc *= PRIME64_1;
                return acc;
            }

            /* Merges acc into hash to finalize */
            static u64 XXH64_mergeRound(u64 hash, u64 const acc)
            {
                hash ^= XXH64_round(0, acc);
                hash *= PRIME64_1;
                hash += PRIME64_4;
                return hash;
            }

            /* Mixes all bits to finalize the hash. */
            static u64 XXH64_avalanche(u64 hash)
            {
                hash ^= hash >> 33;
                hash *= PRIME64_2;
                hash ^= hash >> 29;
                hash *= PRIME64_3;
                hash ^= hash >> 32;
                return hash;
            }

            /* The XXH64 hash function.
             * input:   The data to hash.
             * length:  The length of input. It is undefined behavior to have length larger than the
             *          capacity of input.
             * seed:    A 64-bit value to seed the hash with.
             * returns: The 64-bit calculated hash value. */
            u64 XXH64(char const *const input, size_t const length, u64 const seed)
            {
                char const *const data      = (char const *)input;
                u64               hash      = 0;
                size_t            remaining = length;
                size_t            offset    = 0;

                /* Don't dereference a null pointer. The reference implementation notably doesn't
                 * check for this by default. */
                if (input == NULL)
                {
                    return XXH64_avalanche(seed + PRIME64_5);
                }

                if (remaining >= 32)
                {
                    /* Initialize our accumulators */
                    u64 acc1 = seed + PRIME64_1 + PRIME64_2;
                    u64 acc2 = seed + PRIME64_2;
                    u64 acc3 = seed + 0;
                    u64 acc4 = seed - PRIME64_1;

                    while (remaining >= 32)
                    {
                        acc1 = XXH64_round(acc1, XXH_read64(data, offset));
                        offset += 8;
                        acc2 = XXH64_round(acc2, XXH_read64(data, offset));
                        offset += 8;
                        acc3 = XXH64_round(acc3, XXH_read64(data, offset));
                        offset += 8;
                        acc4 = XXH64_round(acc4, XXH_read64(data, offset));
                        offset += 8;
                        remaining -= 32;
                    }

                    hash = XXH_rotl64(acc1, 1) + XXH_rotl64(acc2, 7) + XXH_rotl64(acc3, 12) + XXH_rotl64(acc4, 18);

                    hash = XXH64_mergeRound(hash, acc1);
                    hash = XXH64_mergeRound(hash, acc2);
                    hash = XXH64_mergeRound(hash, acc3);
                    hash = XXH64_mergeRound(hash, acc4);
                }
                else
                {
                    /* Not enough data for the main loop, put something in there instead. */
                    hash = seed + PRIME64_5;
                }

                hash += (u64)length;

                /* Process the remaining data. */
                while (remaining >= 8)
                {
                    hash ^= XXH64_round(0, XXH_read64(data, offset));
                    hash = XXH_rotl64(hash, 27);
                    hash *= PRIME64_1;
                    hash += PRIME64_4;
                    offset += 8;
                    remaining -= 8;
                }

                if (remaining >= 4)
                {
                    hash ^= (u64)XXH_read32(data, offset) * PRIME64_1;
                    hash = XXH_rotl64(hash, 23);
                    hash *= PRIME64_2;
                    hash += PRIME64_3;
                    offset += 4;
                    remaining -= 4;
                }

                while (remaining != 0)
                {
                    hash ^= (u64)data[offset] * PRIME64_5;
                    hash = XXH_rotl64(hash, 11);
                    hash *= PRIME64_1;
                    ++offset;
                    --remaining;
                }

                return XXH64_avalanche(hash);
            }
        }  // namespace xxhash64_text

        namespace xxhash64_streaming
        {
            typedef enum
            {
                FALSE,
                TRUE
            } XXH_bool;

            struct XXH64_state_t
            {
                u64  acc1;
                u64  acc2;
                u64  acc3;
                u64  acc4;
                u8   temp_buffer[32];  /* Leftover data from a previous update */
                u32  temp_buffer_size; /* how much data is in the temp buffer */
                u32  has_large_len;    /* Whether we had enough to do full rounds. */
                u64  total_len_64;     /* The length of the data truncated to 32 bits. */
            };

            typedef enum
            {
                XXH_OK    = 0,
                XXH_ERROR = 1
            } XXH_errorcode;

            /*======   Streaming   ======*/
            XXH64_state_t *XXH64_createState(void);
            XXH_errorcode  XXH64_freeState(XXH64_state_t *const state);
            void           XXH64_copyState(XXH64_state_t *const dest, XXH64_state_t const *const src);

            XXH_errorcode XXH64_reset(XXH64_state_t *const statePtr, u64 const seed);
            XXH_errorcode XXH64_update(XXH64_state_t *const statePtr, void const *const input, size_t const length);
            u64           XXH64_digest(XXH64_state_t const *const statePtr);

            static u64 const PRIME64_1 = 0x9E3779B185EBCA87ULL; /* 0b1001111000110111011110011011000110000101111010111100101010000111 */
            static u64 const PRIME64_2 = 0xC2B2AE3D27D4EB4FULL; /* 0b1100001010110010101011100011110100100111110101001110101101001111 */
            static u64 const PRIME64_3 = 0x165667B19E3779F9ULL; /* 0b0001011001010110011001111011000110011110001101110111100111111001 */
            static u64 const PRIME64_4 = 0x85EBCA77C2B2AE63ULL; /* 0b1000010111101011110010100111011111000010101100101010111001100011 */
            static u64 const PRIME64_5 = 0x27D4EB2F165667C5ULL; /* 0b0010011111010100111010110010111100010110010101100110011111000101 */

            /* Rotates value left by amt. */
            static u64 XXH_rotl64(u64 const value, u32 const amt) { return (value << (amt % 64)) | (value >> (64 - (amt % 64))); }

            /* Portably reads a 32-bit little endian integer from data at the given offset. */
            static u32 XXH_read32(u8 const *const data, size_t const offset) { return (u32)data[offset + 0] | ((u32)data[offset + 1] << 8) | ((u32)data[offset + 2] << 16) | ((u32)data[offset + 3] << 24); }

            /* Portably reads a 64-bit little endian integer from data at the given offset. */
            static u64 XXH_read64(u8 const *const data, size_t const offset)
            {
                return (u64)data[offset + 0] | ((u64)data[offset + 1] << 8) | ((u64)data[offset + 2] << 16) | ((u64)data[offset + 3] << 24) | ((u64)data[offset + 4] << 32) | ((u64)data[offset + 5] << 40) | ((u64)data[offset + 6] << 48) |
                       ((u64)data[offset + 7] << 56);
            }

            /* Mixes input into acc. */
            static u64 XXH64_round(u64 acc, u64 const input)
            {
                acc += input * PRIME64_2;
                acc = XXH_rotl64(acc, 31);
                acc *= PRIME64_1;
                return acc;
            }

            /* Merges acc into hash to finalize */
            static u64 XXH64_mergeRound(u64 hash, u64 const acc)
            {
                hash ^= XXH64_round(0, acc);
                hash *= PRIME64_1;
                hash += PRIME64_4;
                return hash;
            }

            /* Mixes all bits to finalize the hash. */
            static u64 XXH64_avalanche(u64 hash)
            {
                hash ^= hash >> 33;
                hash *= PRIME64_2;
                hash ^= hash >> 29;
                hash *= PRIME64_3;
                hash ^= hash >> 32;
                return hash;
            }

            /* Resets an XXH64_state_t.
             * state:   The state to reset.
             * seed:    The seed to use.
             * returns: XXH_OK on success, XXH_ERROR on error. */
            XXH_errorcode XXH64_reset(XXH64_state_t *const state, u64 const seed)
            {
                /* Don't write into a null pointer. The official implementation doesn't check
                 * for this. */
                if (state == NULL)
                {
                    return XXH_ERROR;
                }

                nmem::memset(state, 0, sizeof(XXH64_state_t));

                state->acc1 = seed + PRIME64_1 + PRIME64_2;
                state->acc2 = seed + PRIME64_2;
                state->acc3 = seed + 0;
                state->acc4 = seed - PRIME64_1;
                return XXH_OK;
            }

            /* The XXH64 hash function update loop.
             * state:   The current state. It is undefined behavior to overlap with input.
             * input:   The data to hash. It is undefined behavior to overlap with state.
             * length:  The length of input. It is undefined behavior to have length larger than the
             *          capacity of input.
             * returns: XXH_OK on success, XXH_ERROR on failure. */
            XXH_errorcode XXH64_update(XXH64_state_t *const state, void const *const input, size_t const length)
            {
                u8 const *const data = (u8 const *)input;
                size_t          remaining;
                size_t          offset = 0;

                /* Don't dereference a null pointer. The reference implementation notably doesn't
                 * check for this by default. */
                if (state == NULL || input == NULL)
                {
                    return XXH_ERROR;
                }

                state->total_len_64 += (u64)length;

                if (state->has_large_len == FALSE && (length >= 32 || state->total_len_64 >= 32))
                {
                    state->has_large_len = TRUE;
                }

                if (state->temp_buffer_size + length < 32)
                {
                    /* We don't have a full buffer, so we just copy the data over and return. */
                    nmem::memcpy(&state->temp_buffer[state->temp_buffer_size], input, length);
                    state->temp_buffer_size += (u32)length;
                    return XXH_OK;
                }

                remaining = state->temp_buffer_size + length;

                while (remaining >= 32)
                {
                    /* fill up our temp buffer */
                    nmem::memcpy(&state->temp_buffer[state->temp_buffer_size], &data[offset], 32 - state->temp_buffer_size);

                    /* do our rounds */
                    state->acc1 = XXH64_round(state->acc1, XXH_read64(state->temp_buffer, 0));
                    state->acc2 = XXH64_round(state->acc2, XXH_read64(state->temp_buffer, 8));
                    state->acc3 = XXH64_round(state->acc3, XXH_read64(state->temp_buffer, 16));
                    state->acc4 = XXH64_round(state->acc4, XXH_read64(state->temp_buffer, 24));

                    /* done with the rounds */
                    remaining -= 32;
                    offset += 32 - state->temp_buffer_size;
                    state->temp_buffer_size = 0;
                }

                if (remaining != 0)
                {
                    nmem::memcpy(state->temp_buffer, &data[offset], remaining);
                    state->temp_buffer_size = (u32)remaining;
                }

                return XXH_OK;
            }

            /* Finalizes an XXH64_state_t and returns the seed.
             * state:   The state to finalize. This is not modified.
             * returns: The calculated 64-bit hash. */
            u64 XXH64_digest(XXH64_state_t const *const state)
            {
                u64 hash;
                u64 remaining = state->temp_buffer_size;
                u64 offset    = 0;

                if (state->has_large_len == TRUE)
                {
                    hash = XXH_rotl64(state->acc1, 1) + XXH_rotl64(state->acc2, 7) + XXH_rotl64(state->acc3, 12) + XXH_rotl64(state->acc4, 18);

                    hash = XXH64_mergeRound(hash, state->acc1);
                    hash = XXH64_mergeRound(hash, state->acc2);
                    hash = XXH64_mergeRound(hash, state->acc3);
                    hash = XXH64_mergeRound(hash, state->acc4);
                }
                else
                {
                    /* Not enough data for the main loop, put something in there instead. */
                    hash = state->acc3 /* will be seed because of the + 0 */ + PRIME64_5;
                }

                hash += state->total_len_64;

                /* Process the remaining data. */
                while (remaining >= 8)
                {
                    hash ^= XXH64_round(0, XXH_read64(state->temp_buffer, offset));
                    hash = XXH_rotl64(hash, 27);
                    hash *= PRIME64_1;
                    hash += PRIME64_4;
                    offset += 8;
                    remaining -= 8;
                }

                if (remaining >= 4)
                {
                    hash ^= (u64)XXH_read32(state->temp_buffer, offset) * PRIME64_1;
                    hash = XXH_rotl64(hash, 23);
                    hash *= PRIME64_2;
                    hash += PRIME64_3;
                    offset += 4;
                    remaining -= 4;
                }

                while (remaining != 0)
                {
                    hash ^= (u64)state->temp_buffer[offset] * PRIME64_5;
                    hash = XXH_rotl64(hash, 11);
                    hash *= PRIME64_1;
                    ++offset;
                    --remaining;
                }

                return XXH64_avalanche(hash);
            }
        }  // namespace xxhash64_streaming

        u32 datahash32(u8 const *data, u32 size, u32 seed) { return xxhash32::XXH32((void const *)data, (size_t)size, seed); }
        u64 datahash64(u8 const *data, u32 size, u64 seed) { return xxhash64::XXH64((void const *)data, (size_t)size, seed); }

        u32 strhash32(const char *str, u32 seed) { return xxhash32_text::XXH32(str, ascii::strlen(str), seed); }
        u32 strhash32(const char *str, const char *end, u32 seed) { return xxhash32_text::XXH32(str, (size_t)(end - str), seed); }
        u32 strhash32_lowercase(const char *str, u32 seed) { return xxhash32_text::XXH32(str, ascii::strlen(str), seed); }
        u32 strhash32_lowercase(const char *str, const char *end, u32 seed) { return xxhash32_text::XXH32(str, (size_t)(end - str), seed); }

        u64 strhash64(const char *str, u64 seed) { return xxhash64_text::XXH64(str, ascii::strlen(str), seed); }
        u64 strhash64(const char *str, const char *end, u64 seed) { return xxhash64_text::XXH64(str, (size_t)(end - str), seed); }
        u64 strhash64_lowercase(const char *str, u64 seed) { return xxhash64_text::XXH64(str, ascii::strlen(str), seed); }
        u64 strhash64_lowercase(const char *str, const char *end, u64 seed) { return xxhash64_text::XXH64(str, (size_t)(end - str), seed); }
    }  // namespace nhash
}  // namespace ncore

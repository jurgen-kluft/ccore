#include "ccore/c_hash.h"
#include "ccore/c_runes.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(hash)
{
    UNITTEST_FIXTURE(xxhash32)
    {
#define TEST_DATA_SIZE 101

        /* Checks a hash value. */
        static bool test_sequence(u8 const* const test_data, u32 const length, u32 const seed, u32 const expected)
        {
            u32 const result = nhash::datahash32(test_data, length, seed);
            return (result == expected);
        }

        static u32 const PRIME32_1 = 0x9E3779B1U; /* 0b10011110001101110111100110110001 */

        UNITTEST_TEST(fundamental)
        {
            u32 const prime                     = PRIME32_1;
            u8        test_data[TEST_DATA_SIZE] = {0};
            u32       byte_gen                  = prime;
            int       i                         = 0;

            /* Fill the test_data buffer with "random" data */
            for (; i < TEST_DATA_SIZE; i++)
            {
                test_data[i] = (u8)(byte_gen >> 24);
                byte_gen *= byte_gen;
            }

            CHECK_TRUE(test_sequence(NULL, 0, 0, 0x02CC5D05U));
            CHECK_TRUE(test_sequence(NULL, 0, prime, 0x36B78AE7U));
            CHECK_TRUE(test_sequence(test_data, 1, 0, 0xB85CBEE5U));
            CHECK_TRUE(test_sequence(test_data, 1, prime, 0xD5845D64U));
            CHECK_TRUE(test_sequence(test_data, 14, 0, 0xE5AA0AB4U));
            CHECK_TRUE(test_sequence(test_data, 14, prime, 0x4481951DU));
            CHECK_TRUE(test_sequence(test_data, TEST_DATA_SIZE, 0, 0x1F1AA412U));
            CHECK_TRUE(test_sequence(test_data, TEST_DATA_SIZE, prime, 0x498EC8E2U));
        }

    }
}
UNITTEST_SUITE_END

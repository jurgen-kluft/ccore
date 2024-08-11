#include "ccore/c_binary_search.h"
#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(test_binary_search)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        static s8 sComparePredicate(const void* inItem, const void* inData, s64 inIndex)
        {
            s32 a = *(s32*)inItem;

            s32* data = (s32*)inData;
            s32  b    = data[inIndex];

            if (a < b)
                return -1;
            else if (a > b)
                return 1;
            return 0;
        }

        static s8 sCompareBigPredicate(const void* inItem, const void* inData, s64 inIndex)
        {
            u64  a    = *(u64*)inItem;
            u64* data = (u64*)inData;
            u64  b    = data[inIndex];

            if (a < b)
                return -1;
            else if (a > b)
                return 1;
            else
                return 0;
        }

        UNITTEST_TEST(_search)
        {
            const u32 sorted_random_number_list[] = {127,  157,  297,  305,  322,  483,  490,  687,  767,  839,  850,  901,  947,  966,  1033, 1113, 1174, 1346, 1380, 1459, 1781, 1820, 1857, 1872, 1890,
                                                     1981, 2303, 2327, 2355, 2517, 2549, 2659, 2749, 2791, 2847, 3085, 3293, 3512, 3532, 3606, 3630, 3681, 3727, 3744, 3845, 3860, 3899, 3942, 4025, 4053};

            u32 key        = 1890;
            s32 idx_of_key = g_BinarySearch(sorted_random_number_list, sizeof(sorted_random_number_list) / sizeof(sorted_random_number_list[0]), key);

            CHECK_EQUAL(sorted_random_number_list[idx_of_key], key);
        }

        UNITTEST_TEST(_search2)
        {
            const s32 list[] = {100};

            s32 key        = 1890;
            s32 idx_of_key = g_BinarySearch(list, sizeof(list) / sizeof(s32), key);
            CHECK_EQUAL(-1, idx_of_key);
            key        = 18;
            idx_of_key = g_BinarySearch(list, sizeof(list) / sizeof(s32), key);
            CHECK_EQUAL(-1, idx_of_key);
        }

        UNITTEST_TEST(_search3)
        {
            const u32 sorted_random_number_list[] = {127,  157,  297,  305,  322,  483,  490,  687,  767,  839,  850,  901,  947,  966,  1033, 1113, 1174, 1346, 1380, 1459, 1781, 1820, 1857, 1872, 1890,
                                                     1981, 2303, 2327, 2355, 2517, 2549, 2659, 2749, 2791, 2847, 3085, 3293, 3512, 3532, 3606, 3630, 3681, 3727, 3744, 3845, 3860, 3899, 3942, 4025, 4053};

            for (s32 i = 0; i <= 7; ++i)
            {
                s32 const key        = sorted_random_number_list[i * 7];
                s32 const idx_of_key = g_BinarySearch(sorted_random_number_list, sizeof(sorted_random_number_list) / sizeof(sorted_random_number_list[0]), key);
                CHECK_EQUAL(i * 7, idx_of_key);
            }
        }

        UNITTEST_TEST(bigsearch)
        {
            const u64 sorted_random_number_list[] = {0x800A9640, 0x800A98C0, 0x800A9B40, 0x800A9DA0, 0x800AA020, 0x800AA520, 0x800AA780, 0x800AA820};

            for (s32 i = 0; i <= 7; ++i)
            {
                u64 const key        = sorted_random_number_list[i];
                s32 const idx_of_key = g_BinarySearch(sorted_random_number_list, sizeof(sorted_random_number_list) / sizeof(sorted_random_number_list[0]), key);
                CHECK_EQUAL(i, idx_of_key);
            }
        }
    }
}
UNITTEST_SUITE_END

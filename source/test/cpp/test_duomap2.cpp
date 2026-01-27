#include "ccore/c_duomap1.h"
#include "ccore/c_memory.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(duomap1)
{
    UNITTEST_FIXTURE(duomap12)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(lazy_free)
        {
            u64 bin00;
            u64 bin10;
            u64 bin1[64];

            bin00 = D_U64_MAX;
            bin10 = D_U64_MAX;
            g_memclr(bin1, sizeof(bin1));

            const u32 maxbits = 512;

            u64 expected_bin00 = D_U64_MAX;
            u64 expected_bin10 = D_U64_MAX;
            u64 expected_bin1  = D_U64_MAX;

            nduomap12::setup_lazy(&bin00, &bin10, bin1, maxbits);

            CHECK_EQUAL(expected_bin00, bin00);
            CHECK_EQUAL(expected_bin10, bin10);

            for (u32 i = 0; i < maxbits; i++)
            {
                nduomap12::tick_lazy(&bin00, &bin10, bin1, maxbits, i);
                if ((i & 63) == 0)
                {
                    expected_bin00 = (expected_bin00 << 1);
                    expected_bin1  = 0;
                }
                else
                {
                    expected_bin1 = 0;
                }
                CHECK_EQUAL(expected_bin00, bin00);
                CHECK_EQUAL(expected_bin10, bin10);
                CHECK_EQUAL(expected_bin1, bin1[i >> 6]);
            }
        }

        UNITTEST_TEST(set_and_is_set)
        {
            u64 bin00;
            u64 bin01;
            u64 bin1[64];

            bin00 = 0;
            bin01 = D_U64_MAX;

            const u32 maxbits = 512;

            u64 expected_bin00 = 0;
            u64 expected_bin10 = 0xffffffffffffffffull;
            for (u32 i = 0; i < maxbits; i++)
            {
                nduomap12::set(&bin00, &bin01, bin1, maxbits, i);

                if ((i & 63) == 0)
                {
                    expected_bin10 = (u64)(expected_bin10 << 1);
                }

                if ((i & 63) == 63)
                {
                    expected_bin00 = (expected_bin00 << 1) | 1;
                }

                CHECK_EQUAL(expected_bin00, bin00);
                CHECK_EQUAL(expected_bin10, bin01);
                CHECK_TRUE(nduomap12::get(&bin00, &bin01, bin1, maxbits, i));
            }
        }

        UNITTEST_TEST(clr_and_is_clr)
        {
            u64 bin00 = D_U64_MAX;
            u64 bin10 = D_U64_MAX;

            u64 bin1[64];
            g_memset(bin1, 0xFFFFFFFF, sizeof(bin1));

            const u32 maxbits = 512;

            u64 expected_bin00 = D_U64_MAX;
            u64 expected_bin10 = D_U64_MAX;
            for (u32 i = 0; i < maxbits; i++)
            {
                if ((i & 63) == 0)
                {
                    expected_bin00 = (expected_bin00 << 1);
                }

                nduomap12::clr(&bin00, &bin10, bin1, maxbits, i);

                CHECK_EQUAL(expected_bin00, bin00);
                CHECK_EQUAL(expected_bin10, bin10);
                CHECK_FALSE(nduomap12::get(&bin00, &bin10, bin1, maxbits, i));
            }
        }

        UNITTEST_TEST(find0)
        {
            u64 bin00;
            u64 bin01;
            u64 bin1[64];

            g_memset(&bin00, 0xFFFFFFFF, sizeof(bin00));
            g_memset(&bin01, 0xFFFFFFFF, sizeof(bin01));
            g_memset(bin1, 0xFFFFFFFF, sizeof(bin1));

            const s32 maxbits = 512;

            CHECK_EQUAL((s32)-1, nduomap12::find0(&bin00, &bin01, bin1, maxbits));
            for (s32 i = maxbits - 1; i >= 0; i -= 1)
            {
                nduomap12::clr(&bin00, &bin01, bin1, maxbits, i);
                CHECK_EQUAL((s32)i, nduomap12::find0(&bin00, &bin01, bin1, maxbits));
            }
        }

        UNITTEST_TEST(find0_and_set)
        {
            u64 bin1[64];
            g_memclr(bin1, sizeof(bin1));

            u64 bin00 = 0;
            u64 bin10 = D_U64_MAX;

            const s32 maxbits = 512;

            CHECK_EQUAL((s32)0, nduomap12::find0(&bin00, &bin10, bin1, maxbits));
            for (s32 i = 0; i < maxbits; i++)
            {
                CHECK_EQUAL((s32)i, nduomap12::find0_and_set(&bin00, &bin10, bin1, maxbits));
            }
        }
    }
}
UNITTEST_SUITE_END

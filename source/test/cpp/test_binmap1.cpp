#include "ccore/c_binmap1.h"
#include "ccore/c_memory.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(binmap1)
{
    UNITTEST_FIXTURE(binmap12)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(lazy_free)
        {
            u64       bin0;
            u64       bin1[64];

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const u32 maxbits = 512;

            u64 expected_bin0 = D_U64_MAX;
            u64 expected_bin1 = D_U64_MAX;

            nbinmap12::setup_free_lazy(&bin0, bin1, maxbits);

            CHECK_EQUAL(expected_bin0, bin0);
            for (u32 i = 0; i < maxbits; i++)
            {
                nbinmap12::tick_free_lazy(&bin0, (u64*)bin1, maxbits, i);
                if ((i & 63) == 0)
                {
                    expected_bin0 = (expected_bin0 << 1);
                    expected_bin1 = D_U64_MAX << 1;
                }
                else
                {
                    expected_bin1 = (u64)(expected_bin1 << 1);
                }
                CHECK_EQUAL(expected_bin0, bin0);
                CHECK_EQUAL(expected_bin1, bin1[i >> 6]);
            }
        }

        UNITTEST_TEST(lazy_used)
        {
            // todo
        }

        UNITTEST_TEST(set_and_is_set)
        {
            u64       bin0;
            u64       bin1[64];

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const u32 maxbits = 512;

            u64 expected_bin0 = 0;
            u64 expected_bin1 = 0;
            for (u32 i = 0; i < maxbits; i++)
            {
                if ((i & 63) == 0)
                {
                    expected_bin1 = 0;
                }

                nbinmap12::set(&bin0, bin1, maxbits, i);

                expected_bin1 = (u64)((expected_bin1 << 1) | 1);
                if ((i & 63) == 63)
                {
                    expected_bin0 = (expected_bin0 << 1) | 1;
                }

                CHECK_EQUAL(expected_bin0, bin0);
                CHECK_EQUAL(expected_bin1, bin1[i >> 6]);
                CHECK_TRUE(nbinmap12::get(&bin0, bin1, maxbits, i));
            }
        }

        UNITTEST_TEST(clr_and_is_clr)
        {
            u64       bin0;
            u64       bin1[64];

            g_memset(&bin0, 0xFFFFFFFF, sizeof(bin0));
            g_memset(bin1, 0xFFFFFFFF, sizeof(bin1));

            const u32 maxbits = 512;

            u64 expected_bin0 = D_U64_MAX;
            u64 expected_bin1 = D_U64_MAX;
            for (u32 i = 0; i < maxbits; i++)
            {
                if ((i & 63) == 0)
                {
                    expected_bin1 = D_U64_MAX;
                    expected_bin0 = (expected_bin0 << 1);
                }
                expected_bin1 = (u64)(expected_bin1 << 1);

                nbinmap12::clr(&bin0, bin1, maxbits, i);

                CHECK_EQUAL(expected_bin0, bin0);
                CHECK_EQUAL(expected_bin1, bin1[i >> 6]);
                CHECK_FALSE(nbinmap12::get(&bin0, bin1, maxbits, i));
            }
        }

        UNITTEST_TEST(find)
        {
            u64       bin0;
            u64       bin1[64];

            g_memset(&bin0, 0xFFFFFFFF, sizeof(bin0));
            g_memset(bin1, 0xFFFFFFFF, sizeof(bin1));

            const s32 maxbits = 512;

            CHECK_EQUAL((s32)-1, nbinmap12::find(&bin0, bin1, maxbits));

            for (s32 i = maxbits - 1; i >= 0; i -= 1)
            {
                nbinmap12::clr(&bin0, bin1, maxbits, i);

                CHECK_EQUAL((s32)i, nbinmap12::find(&bin0, bin1, maxbits));
            }
        }

        UNITTEST_TEST(find_and_set)
        {
            u64       bin0;
            u64       bin1[64];

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const s32 maxbits = 512;

            CHECK_EQUAL((s32)0, nbinmap12::find(&bin0, bin1, maxbits));

            for (s32 i = 0; i < maxbits; i++)
            {
                CHECK_EQUAL((s32)i, nbinmap12::find_and_set(&bin0, bin1, maxbits));
            }
        }
    }
}
UNITTEST_SUITE_END

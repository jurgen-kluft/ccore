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

        UNITTEST_TEST(lazy_tracks_used_bits)
        {
            u64       bin0;
            u64       bin1[64];
            u64       expected_bin1[64];

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));
            g_memclr(expected_bin1, sizeof(expected_bin1));

            const u32 maxbits = 130;

            u64 expected_bin0 = 0;

            nbinmap12::setup_lazy(&bin0, bin1, maxbits);

            CHECK_EQUAL(expected_bin0, bin0);
            for (u32 i = 0; i < maxbits; i++)
            {
                nbinmap12::tick_lazy(&bin0, bin1, maxbits, i);

                expected_bin1[i >> 6] |= ((u64)1 << (i & 63));
                expected_bin0 |= ((u64)1 << (i >> 6));

                CHECK_EQUAL(expected_bin0, bin0);
                CHECK_EQUAL(expected_bin1[i >> 6], bin1[i >> 6]);
                CHECK_TRUE(nbinmap12::get(&bin0, bin1, maxbits, i));
            }

            CHECK_EQUAL((s32)0, nbinmap12::find(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)(maxbits - 1), nbinmap12::find_last(&bin0, bin1, maxbits));
        }

        UNITTEST_TEST(set_and_is_set)
        {
            u64       bin0;
            u64       bin1[64];
            u32 const bits[]          = {0, 63, 64, 129};
            u64 const expected_bin0[] = {1, 1, 3, 7};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const u32 maxbits = 130;

            for (u32 i = 0; i < 4; ++i)
            {
                nbinmap12::set(&bin0, bin1, maxbits, bits[i]);

                CHECK_EQUAL(expected_bin0[i], bin0);
                CHECK_TRUE(nbinmap12::get(&bin0, bin1, maxbits, bits[i]));
            }

            CHECK_EQUAL((((u64)1 << 63) | 1ull), bin1[0]);
            CHECK_EQUAL((u64)1, bin1[1]);
            CHECK_EQUAL((u64)2, bin1[2]);
        }

        UNITTEST_TEST(clr_and_is_clr)
        {
            u64       bin0;
            u64       bin1[64];
            u32 const bits[]          = {0, 63, 64, 129};
            u64 const expected_bin0[] = {7, 6, 4, 0};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const u32 maxbits = 130;

            for (u32 i = 0; i < 4; ++i)
                nbinmap12::set(&bin0, bin1, maxbits, bits[i]);

            for (u32 i = 0; i < 4; ++i)
            {
                nbinmap12::clr(&bin0, bin1, maxbits, bits[i]);

                CHECK_EQUAL(expected_bin0[i], bin0);
                CHECK_FALSE(nbinmap12::get(&bin0, bin1, maxbits, bits[i]));
            }
        }

        UNITTEST_TEST(find_and_remove)
        {
            u64       bin0;
            u64       bin1[64];
            u32 const bits[] = {129, 64, 5};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const s32 maxbits = 130;

            CHECK_EQUAL((s32)-1, nbinmap12::find(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)-1, nbinmap12::find_and_remove(&bin0, bin1, maxbits));

            for (u32 i = 0; i < 3; ++i)
                nbinmap12::set(&bin0, bin1, maxbits, bits[i]);

            CHECK_EQUAL((s32)5, nbinmap12::find(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)129, nbinmap12::find_last(&bin0, bin1, maxbits));

            CHECK_EQUAL((s32)5, nbinmap12::find_and_remove(&bin0, bin1, maxbits));
            CHECK_FALSE(nbinmap12::get(&bin0, bin1, maxbits, 5));
            CHECK_EQUAL((s32)64, nbinmap12::find_and_remove(&bin0, bin1, maxbits));
            CHECK_FALSE(nbinmap12::get(&bin0, bin1, maxbits, 64));
            CHECK_EQUAL((s32)129, nbinmap12::find_and_remove(&bin0, bin1, maxbits));
            CHECK_FALSE(nbinmap12::get(&bin0, bin1, maxbits, 129));
            CHECK_EQUAL((s32)-1, nbinmap12::find(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)-1, nbinmap12::find_last(&bin0, bin1, maxbits));
        }

        UNITTEST_TEST(find_last_and_remove)
        {
            u64       bin0;
            u64       bin1[64];
            u32 const bits[] = {3, 64, 129};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const s32 maxbits = 130;

            for (u32 i = 0; i < 3; ++i)
                nbinmap12::set(&bin0, bin1, maxbits, bits[i]);

            CHECK_EQUAL((s32)129, nbinmap12::find_last_and_remove(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)64, nbinmap12::find_last_and_remove(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)3, nbinmap12::find_last_and_remove(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)-1, nbinmap12::find_last_and_remove(&bin0, bin1, maxbits));
        }

        UNITTEST_TEST(find_after_and_before)
        {
            u64       bin0;
            u64       bin1[64];
            u32 const bits[] = {2, 65, 66, 129};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const u32 maxbits = 130;

            for (u32 i = 0; i < 4; ++i)
                nbinmap12::set(&bin0, bin1, maxbits, bits[i]);

            CHECK_EQUAL((s32)65, nbinmap12::find_after(&bin0, bin1, maxbits, 2));
            CHECK_EQUAL((s32)66, nbinmap12::find_after(&bin0, bin1, maxbits, 65));
            CHECK_EQUAL((s32)129, nbinmap12::find_after(&bin0, bin1, maxbits, 66));
            CHECK_EQUAL((s32)-1, nbinmap12::find_after(&bin0, bin1, maxbits, 129));

            CHECK_EQUAL((s32)66, nbinmap12::find_before(&bin0, bin1, maxbits, 129));
            CHECK_EQUAL((s32)65, nbinmap12::find_before(&bin0, bin1, maxbits, 66));
            CHECK_EQUAL((s32)2, nbinmap12::find_before(&bin0, bin1, maxbits, 65));
            CHECK_EQUAL((s32)-1, nbinmap12::find_before(&bin0, bin1, maxbits, 2));
        }

        UNITTEST_TEST(partial_range_boundary)
        {
            u64 bin0;
            u64 bin1[64];

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const u32 maxbits = 65;

            nbinmap12::set(&bin0, bin1, maxbits, 64);

            CHECK_EQUAL((s32)64, nbinmap12::find(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)64, nbinmap12::find_last(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)64, nbinmap12::find_after(&bin0, bin1, maxbits, 63));
            CHECK_EQUAL((s32)-1, nbinmap12::find_after(&bin0, bin1, maxbits, 64));
            CHECK_EQUAL((s32)64, nbinmap12::find_before(&bin0, bin1, maxbits, 65));
            CHECK_EQUAL((s32)-1, nbinmap12::find_before(&bin0, bin1, maxbits, 64));
        }
    }

    UNITTEST_FIXTURE(binmap18)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(lazy_tracks_used_bits)
        {
            u64       bin0;
            u64       bin1[64];
            u64       bin2[128];
            u32 const bits[] = {0, 63, 64, 4095, 4096};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));
            g_memclr(bin2, sizeof(bin2));

            const u32 maxbits = 4097;

            nbinmap18::setup_lazy(&bin0, bin1, bin2, maxbits);

            for (u32 i = 0; i < 5; ++i)
            {
                nbinmap18::tick_lazy(&bin0, bin1, bin2, maxbits, bits[i]);
                CHECK_TRUE(nbinmap18::get(&bin0, bin1, bin2, maxbits, bits[i]));
            }

            CHECK_EQUAL((s32)0, nbinmap18::find(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4096, nbinmap18::find_last(&bin0, bin1, bin2, maxbits));
        }

        UNITTEST_TEST(find_and_remove)
        {
            u64       bin0;
            u64       bin1[64];
            u64       bin2[128];
            u32 const bits[] = {4999, 4096, 65, 3};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));
            g_memclr(bin2, sizeof(bin2));

            const s32 maxbits = 5000;

            CHECK_EQUAL((s32)-1, nbinmap18::find(&bin0, bin1, bin2, maxbits));

            for (u32 i = 0; i < 4; ++i)
                nbinmap18::set(&bin0, bin1, bin2, maxbits, bits[i]);

            CHECK_EQUAL((s32)3, nbinmap18::find(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4999, nbinmap18::find_last(&bin0, bin1, bin2, maxbits));

            CHECK_EQUAL((s32)3, nbinmap18::find_and_remove(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)65, nbinmap18::find_and_remove(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4096, nbinmap18::find_and_remove(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4999, nbinmap18::find_and_remove(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)-1, nbinmap18::find(&bin0, bin1, bin2, maxbits));
        }

        UNITTEST_TEST(find_last_and_directional)
        {
            u64       bin0;
            u64       bin1[64];
            u64       bin2[128];
            u32 const bits[] = {2, 65, 4096, 4098, 4999};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));
            g_memclr(bin2, sizeof(bin2));

            const u32 maxbits = 5000;

            for (u32 i = 0; i < 5; ++i)
                nbinmap18::set(&bin0, bin1, bin2, maxbits, bits[i]);

            CHECK_EQUAL((s32)4999, nbinmap18::find_last_and_remove(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4098, nbinmap18::find_last(&bin0, bin1, bin2, maxbits));

            CHECK_EQUAL((s32)65, nbinmap18::find_after(&bin0, bin1, bin2, maxbits, 2));
            CHECK_EQUAL((s32)4096, nbinmap18::find_after(&bin0, bin1, bin2, maxbits, 65));
            CHECK_EQUAL((s32)4098, nbinmap18::find_after(&bin0, bin1, bin2, maxbits, 4096));
            CHECK_EQUAL((s32)-1, nbinmap18::find_after(&bin0, bin1, bin2, maxbits, 4098));

            CHECK_EQUAL((s32)4096, nbinmap18::find_before(&bin0, bin1, bin2, maxbits, 4098));
            CHECK_EQUAL((s32)65, nbinmap18::find_before(&bin0, bin1, bin2, maxbits, 4096));
            CHECK_EQUAL((s32)2, nbinmap18::find_before(&bin0, bin1, bin2, maxbits, 65));
            CHECK_EQUAL((s32)-1, nbinmap18::find_before(&bin0, bin1, bin2, maxbits, 2));
        }

        UNITTEST_TEST(partial_range_boundary)
        {
            u64 bin0;
            u64 bin1[64];
            u64 bin2[128];

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));
            g_memclr(bin2, sizeof(bin2));

            const u32 maxbits = 4097;

            nbinmap18::set(&bin0, bin1, bin2, maxbits, 4096);

            CHECK_EQUAL((s32)4096, nbinmap18::find(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4096, nbinmap18::find_last(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4096, nbinmap18::find_after(&bin0, bin1, bin2, maxbits, 4095));
            CHECK_EQUAL((s32)-1, nbinmap18::find_after(&bin0, bin1, bin2, maxbits, 4096));
            CHECK_EQUAL((s32)4096, nbinmap18::find_before(&bin0, bin1, bin2, maxbits, 4097));
            CHECK_EQUAL((s32)-1, nbinmap18::find_before(&bin0, bin1, bin2, maxbits, 4096));
        }
    }

    UNITTEST_FIXTURE(binmap24)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(lazy_tracks_used_bits)
        {
            u64       bin0;
            u64       bin1[64];
            u64       bin2[128];
            u64       bin3[4097];
            u32 const bits[] = {0, 63, 64, 262143, 262144};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));
            g_memclr(bin2, sizeof(bin2));
            g_memclr(bin3, sizeof(bin3));

            const u32 maxbits = 262145;

            nbinmap24::setup_lazy(&bin0, bin1, bin2, bin3, maxbits);

            for (u32 i = 0; i < 5; ++i)
            {
                nbinmap24::tick_lazy(&bin0, bin1, bin2, bin3, maxbits, bits[i]);
                CHECK_TRUE(nbinmap24::get(&bin0, bin1, bin2, bin3, maxbits, bits[i]));
            }

            CHECK_EQUAL((s32)0, nbinmap24::find(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)262144, nbinmap24::find_last(&bin0, bin1, bin2, bin3, maxbits));
        }

        UNITTEST_TEST(find_and_remove)
        {
            u64       bin0;
            u64       bin1[64];
            u64       bin2[128];
            u64       bin3[5000];
            u32 const bits[] = {299999, 262144, 65, 3};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));
            g_memclr(bin2, sizeof(bin2));
            g_memclr(bin3, sizeof(bin3));

            const s32 maxbits = 300000;

            CHECK_EQUAL((s32)-1, nbinmap24::find(&bin0, bin1, bin2, bin3, maxbits));

            for (u32 i = 0; i < 4; ++i)
                nbinmap24::set(&bin0, bin1, bin2, bin3, maxbits, bits[i]);

            CHECK_EQUAL((s32)3, nbinmap24::find(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)299999, nbinmap24::find_last(&bin0, bin1, bin2, bin3, maxbits));

            CHECK_EQUAL((s32)3, nbinmap24::find_and_remove(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)65, nbinmap24::find_and_remove(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)262144, nbinmap24::find_and_remove(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)299999, nbinmap24::find_and_remove(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)-1, nbinmap24::find(&bin0, bin1, bin2, bin3, maxbits));
        }

        UNITTEST_TEST(find_last_and_directional)
        {
            u64       bin0;
            u64       bin1[64];
            u64       bin2[128];
            u64       bin3[5000];
            u32 const bits[] = {2, 65, 262144, 262146, 299999};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));
            g_memclr(bin2, sizeof(bin2));
            g_memclr(bin3, sizeof(bin3));

            const u32 maxbits = 300000;

            for (u32 i = 0; i < 5; ++i)
                nbinmap24::set(&bin0, bin1, bin2, bin3, maxbits, bits[i]);

            CHECK_EQUAL((s32)299999, nbinmap24::find_last_and_remove(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)262146, nbinmap24::find_last(&bin0, bin1, bin2, bin3, maxbits));

            CHECK_EQUAL((s32)65, nbinmap24::find_after(&bin0, bin1, bin2, bin3, maxbits, 2));
            CHECK_EQUAL((s32)262144, nbinmap24::find_after(&bin0, bin1, bin2, bin3, maxbits, 65));
            CHECK_EQUAL((s32)262146, nbinmap24::find_after(&bin0, bin1, bin2, bin3, maxbits, 262144));
            CHECK_EQUAL((s32)-1, nbinmap24::find_after(&bin0, bin1, bin2, bin3, maxbits, 262146));

            CHECK_EQUAL((s32)262144, nbinmap24::find_before(&bin0, bin1, bin2, bin3, maxbits, 262146));
            CHECK_EQUAL((s32)65, nbinmap24::find_before(&bin0, bin1, bin2, bin3, maxbits, 262144));
            CHECK_EQUAL((s32)2, nbinmap24::find_before(&bin0, bin1, bin2, bin3, maxbits, 65));
            CHECK_EQUAL((s32)-1, nbinmap24::find_before(&bin0, bin1, bin2, bin3, maxbits, 2));
        }

        UNITTEST_TEST(partial_range_boundary)
        {
            u64 bin0;
            u64 bin1[64];
            u64 bin2[128];
            u64 bin3[4097];

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));
            g_memclr(bin2, sizeof(bin2));
            g_memclr(bin3, sizeof(bin3));

            const u32 maxbits = 262145;

            nbinmap24::set(&bin0, bin1, bin2, bin3, maxbits, 262144);

            CHECK_EQUAL((s32)262144, nbinmap24::find(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)262144, nbinmap24::find_last(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)262144, nbinmap24::find_after(&bin0, bin1, bin2, bin3, maxbits, 262143));
            CHECK_EQUAL((s32)-1, nbinmap24::find_after(&bin0, bin1, bin2, bin3, maxbits, 262144));
            CHECK_EQUAL((s32)262144, nbinmap24::find_before(&bin0, bin1, bin2, bin3, maxbits, 262145));
            CHECK_EQUAL((s32)-1, nbinmap24::find_before(&bin0, bin1, bin2, bin3, maxbits, 262144));
        }
    }
}
UNITTEST_SUITE_END

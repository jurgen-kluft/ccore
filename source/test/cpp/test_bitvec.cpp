#include "ccore/c_bitvec.h"
#include "ccore/c_memory.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(bitvec)
{
    UNITTEST_FIXTURE(bitvec12)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(lazy_used_bits)
        {
            u64       bin0;
            u64       bin1[64];

            g_memset(&bin0, -1, sizeof(bin0));
            g_memset(bin1, -1, sizeof(bin1));

            const u32 maxbits = 130;

            nbitvec12::setup_used_lazy(&bin0, bin1, maxbits);

            CHECK_EQUAL(0, bin0);
            for (u32 i = 0; i < maxbits; i++)
            {
                nbitvec12::tick_used_lazy(&bin0, bin1, maxbits, i);

                CHECK_EQUAL(0, bin0);
                CHECK_EQUAL(0, bin1[i >> 6]);
                CHECK_FALSE(nbitvec12::get(&bin0, bin1, maxbits, i));
            }

            // There should be no free bits left since now they are all set to used!
            CHECK_EQUAL((s32)-1, nbitvec12::find_free(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)-1, nbitvec12::find_free_last(&bin0, bin1, maxbits));
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

            for (u32 i = 0; i < DARRAYSIZE(bits); ++i)
            {
                nbitvec12::set_free(&bin0, bin1, maxbits, bits[i]);

                CHECK_EQUAL(expected_bin0[i], bin0);
                CHECK_TRUE(nbitvec12::get(&bin0, bin1, maxbits, bits[i]));
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

            for (u32 i = 0; i < DARRAYSIZE(bits); ++i)
                nbitvec12::set_free(&bin0, bin1, maxbits, bits[i]);

            for (u32 i = 0; i < DARRAYSIZE(bits); ++i)
            {
                nbitvec12::set_used(&bin0, bin1, maxbits, bits[i]);

                CHECK_EQUAL(expected_bin0[i], bin0);
                CHECK_FALSE(nbitvec12::get(&bin0, bin1, maxbits, bits[i]));
            }
        }

        UNITTEST_TEST(find_free_and_remove)
        {
            u64       bin0;
            u64       bin1[64];
            u32 const bits[] = {129, 64, 5};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const s32 maxbits = 130;

            CHECK_EQUAL((s32)-1, nbitvec12::find_free(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)-1, nbitvec12::find_free_and_remove(&bin0, bin1, maxbits));

            for (u32 i = 0; i < DARRAYSIZE(bits); ++i)
                nbitvec12::set_free(&bin0, bin1, maxbits, bits[i]);

            CHECK_EQUAL((s32)5, nbitvec12::find_free(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)129, nbitvec12::find_free_last(&bin0, bin1, maxbits));

            CHECK_EQUAL((s32)5, nbitvec12::find_free_and_remove(&bin0, bin1, maxbits));
            CHECK_FALSE(nbitvec12::get(&bin0, bin1, maxbits, 5));
            CHECK_EQUAL((s32)64, nbitvec12::find_free_and_remove(&bin0, bin1, maxbits));
            CHECK_FALSE(nbitvec12::get(&bin0, bin1, maxbits, 64));
            CHECK_EQUAL((s32)129, nbitvec12::find_free_and_remove(&bin0, bin1, maxbits));
            CHECK_FALSE(nbitvec12::get(&bin0, bin1, maxbits, 129));
            CHECK_EQUAL((s32)-1, nbitvec12::find_free(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)-1, nbitvec12::find_free_last(&bin0, bin1, maxbits));
        }

        UNITTEST_TEST(find_free_last_and_remove)
        {
            u64       bin0;
            u64       bin1[64];
            u32 const bits[] = {3, 64, 129};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const s32 maxbits = 130;

            for (u32 i = 0; i < DARRAYSIZE(bits); ++i)
                nbitvec12::set_free(&bin0, bin1, maxbits, bits[i]);

            CHECK_EQUAL((s32)129, nbitvec12::find_free_last_and_remove(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)64, nbitvec12::find_free_last_and_remove(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)3, nbitvec12::find_free_last_and_remove(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)-1, nbitvec12::find_free_last_and_remove(&bin0, bin1, maxbits));
        }

        UNITTEST_TEST(find_after_and_before)
        {
            u64       bin0;
            u64       bin1[64];
            u32 const bits[] = {2, 65, 66, 129};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const u32 maxbits = 130;

            for (u32 i = 0; i < DARRAYSIZE(bits); ++i)
                nbitvec12::set_free(&bin0, bin1, maxbits, bits[i]);

            CHECK_EQUAL((s32)65, nbitvec12::find_free_after(&bin0, bin1, maxbits, 2));
            CHECK_EQUAL((s32)66, nbitvec12::find_free_after(&bin0, bin1, maxbits, 65));
            CHECK_EQUAL((s32)129, nbitvec12::find_free_after(&bin0, bin1, maxbits, 66));
            CHECK_EQUAL((s32)-1, nbitvec12::find_free_after(&bin0, bin1, maxbits, 129));

            CHECK_EQUAL((s32)66, nbitvec12::find_free_before(&bin0, bin1, maxbits, 129));
            CHECK_EQUAL((s32)65, nbitvec12::find_free_before(&bin0, bin1, maxbits, 66));
            CHECK_EQUAL((s32)2, nbitvec12::find_free_before(&bin0, bin1, maxbits, 65));
            CHECK_EQUAL((s32)-1, nbitvec12::find_free_before(&bin0, bin1, maxbits, 2));
        }

        UNITTEST_TEST(partial_range_boundary)
        {
            u64 bin0;
            u64 bin1[64];

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));

            const u32 maxbits = 65;

            nbitvec12::set_free(&bin0, bin1, maxbits, 64);

            CHECK_EQUAL((s32)64, nbitvec12::find_free(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)64, nbitvec12::find_free_last(&bin0, bin1, maxbits));
            CHECK_EQUAL((s32)64, nbitvec12::find_free_after(&bin0, bin1, maxbits, 63));
            CHECK_EQUAL((s32)-1, nbitvec12::find_free_after(&bin0, bin1, maxbits, 64));
            CHECK_EQUAL((s32)64, nbitvec12::find_free_before(&bin0, bin1, maxbits, 65));
            CHECK_EQUAL((s32)-1, nbitvec12::find_free_before(&bin0, bin1, maxbits, 64));
        }
    }

    UNITTEST_FIXTURE(bitvec18)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(lazy_used_bits)
        {
            u64       bin0;
            u64       bin1[64];
            u64       bin2[128];

            g_memset(&bin0, -1, sizeof(bin0));
            g_memset(bin1, -1, sizeof(bin1));
            g_memset(bin2, -1, sizeof(bin2));

            const u32 maxbits = 512;

            nbitvec18::setup_used_lazy(&bin0, bin1, bin2, maxbits);

            for (u32 i = 0; i < 260; ++i)
            {
                nbitvec18::tick_used_lazy(&bin0, bin1, bin2, maxbits, i);
                CHECK_FALSE(nbitvec18::get(&bin0, bin1, bin2, maxbits, i));
            }

            for (u32 i = 260; i < maxbits; ++i)
            {
                nbitvec18::tick_used_lazy(&bin0, bin1, bin2, maxbits, i);
                CHECK_FALSE(nbitvec18::get(&bin0, bin1, bin2, maxbits, i));
            }

            CHECK_EQUAL((s32)-1, nbitvec18::find_free(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)-1, nbitvec18::find_free_last(&bin0, bin1, bin2, maxbits));
        }

        UNITTEST_TEST(find_free_and_remove)
        {
            u64       bin0;
            u64       bin1[64];
            u64       bin2[128];
            u32 const bits[] = {4999, 4096, 65, 3};

            g_memclr(&bin0, sizeof(bin0));
            g_memclr(bin1, sizeof(bin1));
            g_memclr(bin2, sizeof(bin2));

            const s32 maxbits = 5000;

            CHECK_EQUAL((s32)-1, nbitvec18::find_free(&bin0, bin1, bin2, maxbits));

            for (u32 i = 0; i < DARRAYSIZE(bits); ++i)
                nbitvec18::set_free(&bin0, bin1, bin2, maxbits, bits[i]);

            CHECK_EQUAL((s32)3, nbitvec18::find_free(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4999, nbitvec18::find_free_last(&bin0, bin1, bin2, maxbits));

            CHECK_EQUAL((s32)3, nbitvec18::find_free_and_remove(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)65, nbitvec18::find_free_and_remove(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4096, nbitvec18::find_free_and_remove(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4999, nbitvec18::find_free_and_remove(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)-1, nbitvec18::find_free(&bin0, bin1, bin2, maxbits));
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

            for (u32 i = 0; i < DARRAYSIZE(bits); ++i)
                nbitvec18::set_free(&bin0, bin1, bin2, maxbits, bits[i]);

            CHECK_EQUAL((s32)4999, nbitvec18::find_free_last_and_remove(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4098, nbitvec18::find_free_last(&bin0, bin1, bin2, maxbits));

            CHECK_EQUAL((s32)65, nbitvec18::find_free_after(&bin0, bin1, bin2, maxbits, 2));
            CHECK_EQUAL((s32)4096, nbitvec18::find_free_after(&bin0, bin1, bin2, maxbits, 65));
            CHECK_EQUAL((s32)4098, nbitvec18::find_free_after(&bin0, bin1, bin2, maxbits, 4096));
            CHECK_EQUAL((s32)-1, nbitvec18::find_free_after(&bin0, bin1, bin2, maxbits, 4098));

            CHECK_EQUAL((s32)4096, nbitvec18::find_free_before(&bin0, bin1, bin2, maxbits, 4098));
            CHECK_EQUAL((s32)65, nbitvec18::find_free_before(&bin0, bin1, bin2, maxbits, 4096));
            CHECK_EQUAL((s32)2, nbitvec18::find_free_before(&bin0, bin1, bin2, maxbits, 65));
            CHECK_EQUAL((s32)-1, nbitvec18::find_free_before(&bin0, bin1, bin2, maxbits, 2));
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

            nbitvec18::set_free(&bin0, bin1, bin2, maxbits, 4096);

            CHECK_EQUAL((s32)4096, nbitvec18::find_free(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4096, nbitvec18::find_free_last(&bin0, bin1, bin2, maxbits));
            CHECK_EQUAL((s32)4096, nbitvec18::find_free_after(&bin0, bin1, bin2, maxbits, 4095));
            CHECK_EQUAL((s32)-1, nbitvec18::find_free_after(&bin0, bin1, bin2, maxbits, 4096));
            CHECK_EQUAL((s32)4096, nbitvec18::find_free_before(&bin0, bin1, bin2, maxbits, 4097));
            CHECK_EQUAL((s32)-1, nbitvec18::find_free_before(&bin0, bin1, bin2, maxbits, 4096));
        }
    }

    UNITTEST_FIXTURE(bitvec24)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(lazy_used_bits)
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

            nbitvec24::setup_used_lazy(&bin0, bin1, bin2, bin3, maxbits);

            for (u32 i = 0; i < DARRAYSIZE(bits); ++i)
            {
                nbitvec24::tick_used_lazy(&bin0, bin1, bin2, bin3, maxbits, bits[i]);
                CHECK_FALSE(nbitvec24::get(&bin0, bin1, bin2, bin3, maxbits, bits[i]));
            }

            CHECK_EQUAL((s32)-1, nbitvec24::find_free(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)-1, nbitvec24::find_free_last(&bin0, bin1, bin2, bin3, maxbits));
        }

        UNITTEST_TEST(find_free_and_remove)
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

            CHECK_EQUAL((s32)-1, nbitvec24::find_free(&bin0, bin1, bin2, bin3, maxbits));

            for (u32 i = 0; i < DARRAYSIZE(bits); ++i)
                nbitvec24::set_free(&bin0, bin1, bin2, bin3, maxbits, bits[i]);

            CHECK_EQUAL((s32)3, nbitvec24::find_free(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)299999, nbitvec24::find_free_last(&bin0, bin1, bin2, bin3, maxbits));

            CHECK_EQUAL((s32)3, nbitvec24::find_free_and_remove(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)65, nbitvec24::find_free_and_remove(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)262144, nbitvec24::find_free_and_remove(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)299999, nbitvec24::find_free_and_remove(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)-1, nbitvec24::find_free(&bin0, bin1, bin2, bin3, maxbits));
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

            for (u32 i = 0; i < DARRAYSIZE(bits); ++i)
                nbitvec24::set_free(&bin0, bin1, bin2, bin3, maxbits, bits[i]);

            CHECK_EQUAL((s32)299999, nbitvec24::find_free_last_and_remove(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)262146, nbitvec24::find_free_last(&bin0, bin1, bin2, bin3, maxbits));

            CHECK_EQUAL((s32)65, nbitvec24::find_free_after(&bin0, bin1, bin2, bin3, maxbits, 2));
            CHECK_EQUAL((s32)262144, nbitvec24::find_free_after(&bin0, bin1, bin2, bin3, maxbits, 65));
            CHECK_EQUAL((s32)262146, nbitvec24::find_free_after(&bin0, bin1, bin2, bin3, maxbits, 262144));
            CHECK_EQUAL((s32)-1, nbitvec24::find_free_after(&bin0, bin1, bin2, bin3, maxbits, 262146));

            CHECK_EQUAL((s32)262144, nbitvec24::find_free_before(&bin0, bin1, bin2, bin3, maxbits, 262146));
            CHECK_EQUAL((s32)65, nbitvec24::find_free_before(&bin0, bin1, bin2, bin3, maxbits, 262144));
            CHECK_EQUAL((s32)2, nbitvec24::find_free_before(&bin0, bin1, bin2, bin3, maxbits, 65));
            CHECK_EQUAL((s32)-1, nbitvec24::find_free_before(&bin0, bin1, bin2, bin3, maxbits, 2));
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

            nbitvec24::set_free(&bin0, bin1, bin2, bin3, maxbits, 262144);

            CHECK_EQUAL((s32)262144, nbitvec24::find_free(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)262144, nbitvec24::find_free_last(&bin0, bin1, bin2, bin3, maxbits));
            CHECK_EQUAL((s32)262144, nbitvec24::find_free_after(&bin0, bin1, bin2, bin3, maxbits, 262143));
            CHECK_EQUAL((s32)-1, nbitvec24::find_free_after(&bin0, bin1, bin2, bin3, maxbits, 262144));
            CHECK_EQUAL((s32)262144, nbitvec24::find_free_before(&bin0, bin1, bin2, bin3, maxbits, 262145));
            CHECK_EQUAL((s32)-1, nbitvec24::find_free_before(&bin0, bin1, bin2, bin3, maxbits, 262144));
        }
    }
}
UNITTEST_SUITE_END

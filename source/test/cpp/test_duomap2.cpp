#include "ccore/c_duomap1.h"
#include "ccore/c_memory.h"

#include "cunittest/cunittest.h"

using namespace ncore;

namespace
{
    static void init_duomap10_empty(u32& free0, u32& used0, u32 (&bin1)[1])
    {
        free0   = 0;
        used0   = (u32)~(u32)0;
        bin1[0] = 0;
        nduomap10::clear_all_free(&free0, &used0, bin1, 32);
    }

    static void init_duomap10_full(u32& free0, u32& used0, u32 (&bin1)[1])
    {
        init_duomap10_empty(free0, used0, bin1);
        for (u32 bit = 0; bit < 32; ++bit)
            nduomap10::set_used(&free0, &used0, bin1, 32, bit);
    }

    static void init_duomap12_empty(u64& free0, u64& used0, u64 (&bin1)[1])
    {
        free0   = 0;
        used0   = (u64)~(u64)0;
        bin1[0] = 0;
        nduomap12::clear_all_free(&free0, &used0, bin1, 64);
    }

    static void init_duomap12_full(u64& free0, u64& used0, u64 (&bin1)[1])
    {
        init_duomap12_empty(free0, used0, bin1);
        for (u32 bit = 0; bit < 64; ++bit)
            nduomap12::set_used(&free0, &used0, bin1, 64, bit);
    }

    static void init_duomap15_empty(u32& free0, u32 (&free1)[1], u32& used0, u32 (&used1)[1], u32 (&bin2)[1])
    {
        free0    = 0;
        free1[0] = 0;
        used0    = (u32)~(u32)0;
        used1[0] = (u32)~(u32)0;
        bin2[0]  = 0;
        nduomap15::clear_all_free(&free0, free1, &used0, used1, bin2, 32);
    }

    static void init_duomap15_empty_64(u32& free0, u32 (&free1)[1], u32& used0, u32 (&used1)[1], u32 (&bin2)[2])
    {
        free0    = 0;
        free1[0] = 0;
        used0    = (u32)~(u32)0;
        used1[0] = (u32)~(u32)0;
        bin2[0]  = 0;
        bin2[1]  = 0;
        nduomap15::clear_all_free(&free0, free1, &used0, used1, bin2, 64);
    }

    static void init_duomap18_empty(u64& free0, u64 (&free1)[1], u64& used0, u64 (&used1)[1], u64 (&bin2)[2])
    {
        free0    = 0;
        free1[0] = 0;
        used0    = (u64)~(u64)0;
        used1[0] = (u64)~(u64)0;
        bin2[0]  = 0;
        bin2[1]  = 0;
        nduomap18::clear_all_free(&free0, free1, &used0, used1, bin2, 128);
    }

    static void init_duomap20_empty_64(u32& free0, u32 (&free1)[1], u32 (&free2)[1], u32& used0, u32 (&used1)[1], u32 (&used2)[1], u32 (&bin3)[2])
    {
        free0    = 0;
        free1[0] = 0;
        free2[0] = 0;
        used0    = (u32)~(u32)0;
        used1[0] = (u32)~(u32)0;
        used2[0] = (u32)~(u32)0;
        bin3[0]  = 0;
        bin3[1]  = 0;
        nduomap20::clear_all_free(&free0, free1, free2, &used0, used1, used2, bin3, 64);
    }

    static void init_duomap20_empty_1056(u32& free0, u32 (&free1)[1], u32 (&free2)[2], u32& used0, u32 (&used1)[1], u32 (&used2)[2], u32 (&bin3)[33])
    {
        free0    = 0;
        free1[0] = 0;
        free2[0] = 0;
        free2[1] = 0;
        used0    = (u32)~(u32)0;
        used1[0] = (u32)~(u32)0;
        used2[0] = (u32)~(u32)0;
        used2[1] = (u32)~(u32)0;
        for (u32 i = 0; i < 33; ++i)
            bin3[i] = 0;
        nduomap20::clear_all_free(&free0, free1, free2, &used0, used1, used2, bin3, 1056);
    }
}

UNITTEST_SUITE_BEGIN(duomap1)
{
    UNITTEST_FIXTURE(duomap10)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(set_single_bit)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            nduomap10::set_used(&free0, &used0, bin1, 32, 0);

            CHECK_TRUE(nduomap10::get(&free0, &used0, bin1, 32, 0));
        }

        UNITTEST_TEST(set_free_single_bit)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            nduomap10::set_used(&free0, &used0, bin1, 32, 0);

            nduomap10::set_free(&free0, &used0, bin1, 32, 0);

            CHECK_FALSE(nduomap10::get(&free0, &used0, bin1, 32, 0));
        }

        UNITTEST_TEST(set_multiple_bits)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            nduomap10::set_used(&free0, &used0, bin1, 32, 5);
            nduomap10::set_used(&free0, &used0, bin1, 32, 10);
            nduomap10::set_used(&free0, &used0, bin1, 32, 15);

            CHECK_TRUE(nduomap10::get(&free0, &used0, bin1, 32, 5));
            CHECK_TRUE(nduomap10::get(&free0, &used0, bin1, 32, 10));
            CHECK_TRUE(nduomap10::get(&free0, &used0, bin1, 32, 15));
        }

        UNITTEST_TEST(get_after_clear)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            nduomap10::set_used(&free0, &used0, bin1, 32, 3);
            CHECK_TRUE(nduomap10::get(&free0, &used0, bin1, 32, 3));

            nduomap10::set_free(&free0, &used0, bin1, 32, 3);
            CHECK_FALSE(nduomap10::get(&free0, &used0, bin1, 32, 3));
        }

        UNITTEST_TEST(find_free_in_cleared_map)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            s32 bit = nduomap10::find_free(&free0, &used0, bin1, 32);
            CHECK_TRUE(bit >= 0 && bit < 32);
        }

        UNITTEST_TEST(find_free_fails_when_all_set)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_full(free0, used0, bin1);

            s32 bit = nduomap10::find_free(&free0, &used0, bin1, 32);
            CHECK_EQUAL(-1, bit);
        }

        UNITTEST_TEST(find_free_partial_set)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            // Set some bits, leaving others free
            nduomap10::set_used(&free0, &used0, bin1, 32, 0);
            nduomap10::set_used(&free0, &used0, bin1, 32, 1);
            nduomap10::set_used(&free0, &used0, bin1, 32, 2);

            s32 bit = nduomap10::find_free(&free0, &used0, bin1, 32);
            CHECK_TRUE(bit >= 3 && bit < 32);
        }

        UNITTEST_TEST(find_used_fails_on_cleared_map)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            s32 bit = nduomap10::find_used(&free0, &used0, bin1, 32);
            CHECK_EQUAL(-1, bit);
        }

        UNITTEST_TEST(find_used_succeeds_when_bits_set)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            nduomap10::set_used(&free0, &used0, bin1, 32, 5);
            nduomap10::set_used(&free0, &used0, bin1, 32, 10);

            s32 bit = nduomap10::find_used(&free0, &used0, bin1, 32);
            CHECK_EQUAL(5, bit);
        }

        UNITTEST_TEST(find_used_finds_first_set_bit)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            nduomap10::set_used(&free0, &used0, bin1, 32, 7);

            s32 bit = nduomap10::find_used(&free0, &used0, bin1, 32);
            CHECK_EQUAL(7, bit);
        }

        UNITTEST_TEST(alloc_finds_and_marks)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            s32 bit = nduomap10::alloc(&free0, &used0, bin1, 32);
            CHECK_TRUE(bit >= 0 && bit < 32);
            CHECK_TRUE(nduomap10::get(&free0, &used0, bin1, 32, bit));
        }

        UNITTEST_TEST(alloc_sequential)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            s32 bit1 = nduomap10::alloc(&free0, &used0, bin1, 32);
            s32 bit2 = nduomap10::alloc(&free0, &used0, bin1, 32);

            CHECK_TRUE(bit1 >= 0);
            CHECK_TRUE(bit2 >= 0);
            CHECK_NOT_EQUAL(bit1, bit2);
        }

        UNITTEST_TEST(free_clears_bit)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            s32 bit = nduomap10::alloc(&free0, &used0, bin1, 32);
            CHECK_TRUE(nduomap10::get(&free0, &used0, bin1, 32, bit));

            CHECK_EQUAL(bit, nduomap10::free(&free0, &used0, bin1, 32));
            CHECK_FALSE(nduomap10::get(&free0, &used0, bin1, 32, bit));
        }

        UNITTEST_TEST(free_sequential)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            s32 bit1 = nduomap10::alloc(&free0, &used0, bin1, 32);
            s32 bit2 = nduomap10::alloc(&free0, &used0, bin1, 32);

            s32 freed1 = nduomap10::free(&free0, &used0, bin1, 32);
            s32 freed2 = nduomap10::free(&free0, &used0, bin1, 32);

            CHECK_EQUAL(bit1, freed1);
            CHECK_EQUAL(bit2, freed2);
        }

        UNITTEST_TEST(alloc_last_in_cleared_map)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            s32 bit = nduomap10::alloc_last(&free0, &used0, bin1, 32);
            CHECK_EQUAL(31, bit);
        }

        UNITTEST_TEST(free_last_with_set_bits)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            nduomap10::set_used(&free0, &used0, bin1, 32, 5);
            nduomap10::set_used(&free0, &used0, bin1, 32, 10);

            s32 bit = nduomap10::free_last(&free0, &used0, bin1, 32);
            CHECK_EQUAL(10, bit);
        }

        UNITTEST_TEST(allocate_and_free_all)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            // Allocate all bits
            for (int i = 0; i < 32; i++)
            {
                s32 bit = nduomap10::alloc(&free0, &used0, bin1, 32);
                CHECK_TRUE(bit >= 0);
            }

            // Should be no more free bits
            s32 bit = nduomap10::alloc(&free0, &used0, bin1, 32);
            CHECK_EQUAL(-1, bit);
        }

        UNITTEST_TEST(find_before_after_and_last)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_empty(free0, used0, bin1);

            nduomap10::set_used(&free0, &used0, bin1, 32, 2);
            nduomap10::set_used(&free0, &used0, bin1, 32, 7);
            nduomap10::set_used(&free0, &used0, bin1, 32, 15);

            CHECK_EQUAL(7, nduomap10::find_used_after(&free0, &used0, bin1, 32, 2));
            CHECK_EQUAL(-1, nduomap10::find_used_before(&free0, &used0, bin1, 32, 2));
            CHECK_EQUAL(7, nduomap10::find_used_before(&free0, &used0, bin1, 32, 15));
            CHECK_EQUAL(15, nduomap10::find_used_last(&free0, &used0, bin1, 32));

            CHECK_EQUAL(3, nduomap10::find_free_after(&free0, &used0, bin1, 32, 2));
            CHECK_EQUAL(6, nduomap10::find_free_before(&free0, &used0, bin1, 32, 7));
            CHECK_EQUAL(31, nduomap10::find_free_last(&free0, &used0, bin1, 32));
        }

        UNITTEST_TEST(clear_all_used_marks_all_bits_used)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_duomap10_full(free0, used0, bin1);

            nduomap10::clear_all_used(&free0, &used0, bin1, 32);

            CHECK_EQUAL(0, nduomap10::find_used(&free0, &used0, bin1, 32));
            CHECK_EQUAL(-1, nduomap10::find_free(&free0, &used0, bin1, 32));
        }

        UNITTEST_TEST(lazy_setup_and_tick_open_first_word)
        {
            u32 free0 = 0xFFFFFFFFu;
            u32 used0 = 0xFFFFFFFFu;
            u32 bin1[1] = {0xFFFFFFFFu};

            nduomap10::setup_used_lazy(&free0, &used0, bin1, 32);

            CHECK_EQUAL(-1, nduomap10::find_free(&free0, &used0, bin1, 32));
            CHECK_EQUAL(-1, nduomap10::find_used(&free0, &used0, bin1, 32));

            nduomap10::tick_used_lazy(&free0, &used0, bin1, 32, 0);

            CHECK_EQUAL(0, nduomap10::find_free(&free0, &used0, bin1, 32));
            CHECK_EQUAL(-1, nduomap10::find_used(&free0, &used0, bin1, 32));
        }
    }

    UNITTEST_FIXTURE(duomap12)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(set_get_and_find_variants)
        {
            u64 free0;
            u64 used0;
            u64 bin1[1];
            init_duomap12_empty(free0, used0, bin1);

            nduomap12::set_used(&free0, &used0, bin1, 64, 1);
            nduomap12::set_used(&free0, &used0, bin1, 64, 33);
            nduomap12::set_used(&free0, &used0, bin1, 64, 63);

            CHECK_TRUE(nduomap12::get(&free0, &used0, bin1, 64, 1));
            CHECK_TRUE(nduomap12::get(&free0, &used0, bin1, 64, 33));
            CHECK_TRUE(nduomap12::get(&free0, &used0, bin1, 64, 63));

            CHECK_EQUAL(1, nduomap12::find_used(&free0, &used0, bin1, 64));
            CHECK_EQUAL(33, nduomap12::find_used_after(&free0, &used0, bin1, 64, 1));
            CHECK_EQUAL(-1, nduomap12::find_used_before(&free0, &used0, bin1, 64, 1));
            CHECK_EQUAL(33, nduomap12::find_used_before(&free0, &used0, bin1, 64, 63));
            CHECK_EQUAL(63, nduomap12::find_used_last(&free0, &used0, bin1, 64));

            CHECK_EQUAL(0, nduomap12::find_free(&free0, &used0, bin1, 64));
            CHECK_EQUAL(2, nduomap12::find_free_after(&free0, &used0, bin1, 64, 1));
            CHECK_EQUAL(32, nduomap12::find_free_before(&free0, &used0, bin1, 64, 33));
            CHECK_EQUAL(62, nduomap12::find_free_last(&free0, &used0, bin1, 64));
        }

        UNITTEST_TEST(alloc_and_free_variants)
        {
            u64 free0;
            u64 used0;
            u64 bin1[1];
            init_duomap12_empty(free0, used0, bin1);

            CHECK_EQUAL(0, nduomap12::alloc(&free0, &used0, bin1, 64));
            CHECK_EQUAL(63, nduomap12::alloc_last(&free0, &used0, bin1, 64));
            CHECK_EQUAL(0, nduomap12::free(&free0, &used0, bin1, 64));
            CHECK_EQUAL(63, nduomap12::free_last(&free0, &used0, bin1, 64));
        }

        UNITTEST_TEST(clear_all_used_marks_all_bits_used)
        {
            u64 free0;
            u64 used0;
            u64 bin1[1];
            init_duomap12_full(free0, used0, bin1);

            nduomap12::clear_all_used(&free0, &used0, bin1, 64);

            CHECK_EQUAL(0, nduomap12::find_used(&free0, &used0, bin1, 64));
            CHECK_EQUAL(-1, nduomap12::find_free(&free0, &used0, bin1, 64));
        }

        UNITTEST_TEST(lazy_setup_and_tick_open_first_word)
        {
            u64 free0 = (u64)~(u64)0;
            u64 used0 = (u64)~(u64)0;
            u64 bin1[1] = {(u64)~(u64)0};

            nduomap12::setup_used_lazy(&free0, &used0, bin1, 64);

            CHECK_EQUAL(-1, nduomap12::find_free(&free0, &used0, bin1, 64));
            CHECK_EQUAL(-1, nduomap12::find_used(&free0, &used0, bin1, 64));

            nduomap12::tick_used_lazy(&free0, &used0, bin1, 64, 0);

            CHECK_EQUAL(0, nduomap12::find_free(&free0, &used0, bin1, 64));
            CHECK_EQUAL(-1, nduomap12::find_used(&free0, &used0, bin1, 64));
        }
    }

    UNITTEST_FIXTURE(duomap15)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(set_free_reopens_full_leaf)
        {
            u32 free0;
            u32 free1[1];
            u32 used0;
            u32 used1[1];
            u32 bin2[1];
            init_duomap15_empty(free0, free1, used0, used1, bin2);

            for (u32 bit = 0; bit < 32; ++bit)
                nduomap15::set_used(&free0, free1, &used0, used1, bin2, 32, bit);

            CHECK_EQUAL(-1, nduomap15::find_free(&free0, free1, &used0, used1, bin2, 32));

            nduomap15::set_free(&free0, free1, &used0, used1, bin2, 32, 7);

            CHECK_EQUAL(7, nduomap15::find_free(&free0, free1, &used0, used1, bin2, 32));
            CHECK_FALSE(nduomap15::get(&free0, free1, &used0, used1, bin2, 32, 7));
        }

        UNITTEST_TEST(set_free_clears_last_used_leaf)
        {
            u32 free0;
            u32 free1[1];
            u32 used0;
            u32 used1[1];
            u32 bin2[1];
            init_duomap15_empty(free0, free1, used0, used1, bin2);

            nduomap15::set_used(&free0, free1, &used0, used1, bin2, 32, 7);
            CHECK_EQUAL(7, nduomap15::find_used(&free0, free1, &used0, used1, bin2, 32));

            nduomap15::set_free(&free0, free1, &used0, used1, bin2, 32, 7);

            CHECK_EQUAL(-1, nduomap15::find_used(&free0, free1, &used0, used1, bin2, 32));
            CHECK_FALSE(nduomap15::get(&free0, free1, &used0, used1, bin2, 32, 7));
        }

        UNITTEST_TEST(alloc_last_in_cleared_map)
        {
            u32 free0;
            u32 free1[1];
            u32 used0;
            u32 used1[1];
            u32 bin2[1];
            init_duomap15_empty(free0, free1, used0, used1, bin2);

            s32 bit = nduomap15::alloc_last(&free0, free1, &used0, used1, bin2, 32);
            CHECK_EQUAL(31, bit);
        }

        UNITTEST_TEST(free_last_with_set_bits)
        {
            u32 free0;
            u32 free1[1];
            u32 used0;
            u32 used1[1];
            u32 bin2[1];
            init_duomap15_empty(free0, free1, used0, used1, bin2);

            nduomap15::set_used(&free0, free1, &used0, used1, bin2, 32, 5);
            nduomap15::set_used(&free0, free1, &used0, used1, bin2, 32, 10);

            s32 bit = nduomap15::free_last(&free0, free1, &used0, used1, bin2, 32);
            CHECK_EQUAL(10, bit);
        }

        UNITTEST_TEST(find_before_after_and_last_across_words)
        {
            u32 free0;
            u32 free1[1];
            u32 used0;
            u32 used1[1];
            u32 bin2[2];
            init_duomap15_empty_64(free0, free1, used0, used1, bin2);

            nduomap15::set_used(&free0, free1, &used0, used1, bin2, 64, 2);
            nduomap15::set_used(&free0, free1, &used0, used1, bin2, 64, 37);
            nduomap15::set_used(&free0, free1, &used0, used1, bin2, 64, 63);

            CHECK_TRUE(nduomap15::get(&free0, free1, &used0, used1, bin2, 64, 37));
            CHECK_EQUAL(37, nduomap15::find_used_after(&free0, free1, &used0, used1, bin2, 64, 2));
            CHECK_EQUAL(37, nduomap15::find_used_before(&free0, free1, &used0, used1, bin2, 64, 63));
            CHECK_EQUAL(63, nduomap15::find_used_last(&free0, free1, &used0, used1, bin2, 64));

            CHECK_EQUAL(0, nduomap15::find_free(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(3, nduomap15::find_free_after(&free0, free1, &used0, used1, bin2, 64, 2));
            CHECK_EQUAL(36, nduomap15::find_free_before(&free0, free1, &used0, used1, bin2, 64, 37));
            CHECK_EQUAL(62, nduomap15::find_free_last(&free0, free1, &used0, used1, bin2, 64));
        }

        UNITTEST_TEST(alloc_and_free_variants)
        {
            u32 free0;
            u32 free1[1];
            u32 used0;
            u32 used1[1];
            u32 bin2[2];
            init_duomap15_empty_64(free0, free1, used0, used1, bin2);

            CHECK_EQUAL(0, nduomap15::alloc(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(63, nduomap15::alloc_last(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(0, nduomap15::free(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(63, nduomap15::free_last(&free0, free1, &used0, used1, bin2, 64));
        }

        UNITTEST_TEST(lazy_setup_and_tick_marks_words_used)
        {
            u32 free0 = 0xFFFFFFFFu;
            u32 free1[1] = {0xFFFFFFFFu};
            u32 used0 = 0xFFFFFFFFu;
            u32 used1[1] = {0xFFFFFFFFu};
            u32 bin2[2] = {0u, 0u};

            nduomap15::setup_used_lazy(&free0, free1, &used0, used1, bin2, 64);

            CHECK_EQUAL(-1, nduomap15::find_free(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(-1, nduomap15::find_used(&free0, free1, &used0, used1, bin2, 64));

            nduomap15::tick_used_lazy(&free0, free1, &used0, used1, bin2, 64, 0);

            CHECK_EQUAL(0, nduomap15::find_used(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(31, nduomap15::find_used_last(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(-1, nduomap15::find_free(&free0, free1, &used0, used1, bin2, 64));

            nduomap15::tick_used_lazy(&free0, free1, &used0, used1, bin2, 64, 32);

            CHECK_TRUE(nduomap15::get(&free0, free1, &used0, used1, bin2, 64, 32));
            CHECK_EQUAL(63, nduomap15::find_used_last(&free0, free1, &used0, used1, bin2, 64));
        }
    }

    UNITTEST_FIXTURE(duomap18)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(set_get_and_find_variants)
        {
            u64 free0;
            u64 free1[1];
            u64 used0;
            u64 used1[1];
            u64 bin2[2];
            init_duomap18_empty(free0, free1, used0, used1, bin2);

            nduomap18::set_used(&free0, free1, &used0, used1, bin2, 128, 1);
            nduomap18::set_used(&free0, free1, &used0, used1, bin2, 128, 65);
            nduomap18::set_used(&free0, free1, &used0, used1, bin2, 128, 127);

            CHECK_TRUE(nduomap18::get(&free0, free1, &used0, used1, bin2, 128, 1));
            CHECK_TRUE(nduomap18::get(&free0, free1, &used0, used1, bin2, 128, 65));
            CHECK_TRUE(nduomap18::get(&free0, free1, &used0, used1, bin2, 128, 127));

            CHECK_EQUAL(1, nduomap18::find_used(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(65, nduomap18::find_used_after(&free0, free1, &used0, used1, bin2, 128, 1));
            CHECK_EQUAL(65, nduomap18::find_used_before(&free0, free1, &used0, used1, bin2, 128, 127));
            CHECK_EQUAL(127, nduomap18::find_used_last(&free0, free1, &used0, used1, bin2, 128));

            CHECK_EQUAL(0, nduomap18::find_free(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(2, nduomap18::find_free_after(&free0, free1, &used0, used1, bin2, 128, 1));
            CHECK_EQUAL(64, nduomap18::find_free_before(&free0, free1, &used0, used1, bin2, 128, 65));
            CHECK_EQUAL(126, nduomap18::find_free_last(&free0, free1, &used0, used1, bin2, 128));
        }

        UNITTEST_TEST(alloc_and_free_variants)
        {
            u64 free0;
            u64 free1[1];
            u64 used0;
            u64 used1[1];
            u64 bin2[2];
            init_duomap18_empty(free0, free1, used0, used1, bin2);

            CHECK_EQUAL(0, nduomap18::alloc(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(127, nduomap18::alloc_last(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(0, nduomap18::free(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(127, nduomap18::free_last(&free0, free1, &used0, used1, bin2, 128));
        }

        UNITTEST_TEST(lazy_setup_and_tick_marks_words_used)
        {
            u64 free0 = (u64)~(u64)0;
            u64 free1[1] = {(u64)~(u64)0};
            u64 used0 = (u64)~(u64)0;
            u64 used1[1] = {(u64)~(u64)0};
            u64 bin2[2] = {0u, 0u};

            nduomap18::setup_used_lazy(&free0, free1, &used0, used1, bin2, 128);

            CHECK_EQUAL(-1, nduomap18::find_free(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(-1, nduomap18::find_used(&free0, free1, &used0, used1, bin2, 128));

            nduomap18::tick_used_lazy(&free0, free1, &used0, used1, bin2, 128, 0);

            CHECK_EQUAL(0, nduomap18::find_used(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(63, nduomap18::find_used_last(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(-1, nduomap18::find_free(&free0, free1, &used0, used1, bin2, 128));

            nduomap18::tick_used_lazy(&free0, free1, &used0, used1, bin2, 128, 64);

            CHECK_TRUE(nduomap18::get(&free0, free1, &used0, used1, bin2, 128, 64));
            CHECK_EQUAL(127, nduomap18::find_used_last(&free0, free1, &used0, used1, bin2, 128));
        }
    }

    UNITTEST_FIXTURE(duomap20)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(set_get_and_find_variants)
        {
            u32 free0;
            u32 free1[1];
            u32 free2[1];
            u32 used0;
            u32 used1[1];
            u32 used2[1];
            u32 bin3[2];
            init_duomap20_empty_64(free0, free1, free2, used0, used1, used2, bin3);

            nduomap20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 64, 1);
            nduomap20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 64, 33);
            nduomap20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63);

            CHECK_TRUE(nduomap20::get(&free0, free1, free2, &used0, used1, used2, bin3, 64, 1));
            CHECK_TRUE(nduomap20::get(&free0, free1, free2, &used0, used1, used2, bin3, 64, 33));
            CHECK_TRUE(nduomap20::get(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63));

            CHECK_EQUAL(1, nduomap20::find_used(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(33, nduomap20::find_used_after(&free0, free1, free2, &used0, used1, used2, bin3, 64, 1));
            CHECK_EQUAL(-1, nduomap20::find_used_before(&free0, free1, free2, &used0, used1, used2, bin3, 64, 1));
            CHECK_EQUAL(33, nduomap20::find_used_before(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63));
            CHECK_EQUAL(63, nduomap20::find_used_last(&free0, free1, free2, &used0, used1, used2, bin3, 64));

            CHECK_EQUAL(0, nduomap20::find_free(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(2, nduomap20::find_free_after(&free0, free1, free2, &used0, used1, used2, bin3, 64, 1));
            CHECK_EQUAL(32, nduomap20::find_free_before(&free0, free1, free2, &used0, used1, used2, bin3, 64, 33));
            CHECK_EQUAL(62, nduomap20::find_free_last(&free0, free1, free2, &used0, used1, used2, bin3, 64));
        }

        UNITTEST_TEST(alloc_and_free_variants)
        {
            u32 free0;
            u32 free1[1];
            u32 free2[1];
            u32 used0;
            u32 used1[1];
            u32 used2[1];
            u32 bin3[2];
            init_duomap20_empty_64(free0, free1, free2, used0, used1, used2, bin3);

            CHECK_EQUAL(0, nduomap20::alloc(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(63, nduomap20::alloc_last(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(0, nduomap20::free(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(63, nduomap20::free_last(&free0, free1, free2, &used0, used1, used2, bin3, 64));
        }

        UNITTEST_TEST(lazy_setup_and_tick_marks_words_used)
        {
            u32 free0 = 0xFFFFFFFFu;
            u32 free1[1] = {0xFFFFFFFFu};
            u32 free2[1] = {0xFFFFFFFFu};
            u32 used0 = 0xFFFFFFFFu;
            u32 used1[1] = {0xFFFFFFFFu};
            u32 used2[1] = {0xFFFFFFFFu};
            u32 bin3[2] = {0u, 0u};

            nduomap20::setup_used_lazy(&free0, free1, free2, &used0, used1, used2, bin3, 64);

            CHECK_EQUAL(-1, nduomap20::find_free(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(-1, nduomap20::find_used(&free0, free1, free2, &used0, used1, used2, bin3, 64));

            nduomap20::tick_used_lazy(&free0, free1, free2, &used0, used1, used2, bin3, 64, 0);

            CHECK_EQUAL(0, nduomap20::find_used(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(31, nduomap20::find_used_last(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(-1, nduomap20::find_free(&free0, free1, free2, &used0, used1, used2, bin3, 64));

            nduomap20::tick_used_lazy(&free0, free1, free2, &used0, used1, used2, bin3, 64, 32);

            CHECK_TRUE(nduomap20::get(&free0, free1, free2, &used0, used1, used2, bin3, 64, 32));
            CHECK_EQUAL(63, nduomap20::find_used_last(&free0, free1, free2, &used0, used1, used2, bin3, 64));
        }

        UNITTEST_TEST(clear_all_used_marks_all_bits_used)
        {
            u32 free0;
            u32 free1[1];
            u32 free2[1];
            u32 used0;
            u32 used1[1];
            u32 used2[1];
            u32 bin3[2];
            init_duomap20_empty_64(free0, free1, free2, used0, used1, used2, bin3);

            nduomap20::clear_all_used(&free0, free1, free2, &used0, used1, used2, bin3, 64);

            CHECK_EQUAL(0, nduomap20::find_used(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(-1, nduomap20::find_free(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(-1, nduomap20::find_used_before(&free0, free1, free2, &used0, used1, used2, bin3, 64, 0));
            CHECK_EQUAL(-1, nduomap20::find_used_after(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63));
        }

        UNITTEST_TEST(before_after_boundary_pivots)
        {
            u32 free0;
            u32 free1[1];
            u32 free2[1];
            u32 used0;
            u32 used1[1];
            u32 used2[1];
            u32 bin3[2];
            init_duomap20_empty_64(free0, free1, free2, used0, used1, used2, bin3);

            nduomap20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 64, 0);
            nduomap20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63);

            CHECK_EQUAL(-1, nduomap20::find_used_before(&free0, free1, free2, &used0, used1, used2, bin3, 64, 0));
            CHECK_EQUAL(63, nduomap20::find_used_after(&free0, free1, free2, &used0, used1, used2, bin3, 64, 0));
            CHECK_EQUAL(-1, nduomap20::find_used_after(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63));
            CHECK_EQUAL(62, nduomap20::find_free_before(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63));
            CHECK_EQUAL(-1, nduomap20::find_free_after(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63));
        }

        UNITTEST_TEST(crosses_level2_boundary)
        {
            u32 free0;
            u32 free1[1];
            u32 free2[2];
            u32 used0;
            u32 used1[1];
            u32 used2[2];
            u32 bin3[33];
            init_duomap20_empty_1056(free0, free1, free2, used0, used1, used2, bin3);

            nduomap20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1);
            nduomap20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1025);
            nduomap20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1055);

            CHECK_TRUE(nduomap20::get(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1025));
            CHECK_EQUAL(1025, nduomap20::find_used_after(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1));
            CHECK_EQUAL(1025, nduomap20::find_used_before(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1055));
            CHECK_EQUAL(1055, nduomap20::find_used_last(&free0, free1, free2, &used0, used1, used2, bin3, 1056));

            CHECK_EQUAL(2, nduomap20::find_free_after(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1));
            CHECK_EQUAL(1024, nduomap20::find_free_before(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1025));
            CHECK_EQUAL(1054, nduomap20::find_free_last(&free0, free1, free2, &used0, used1, used2, bin3, 1056));
        }
    }
}
UNITTEST_SUITE_END

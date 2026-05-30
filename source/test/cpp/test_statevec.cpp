#include "ccore/c_statevec.h"
#include "ccore/c_memory.h"

#include "cunittest/cunittest.h"

using namespace ncore;

namespace
{
    static void init_statevec10_empty(u32& free0, u32& used0, u32 (&bin1)[1])
    {
        free0   = 0;
        used0   = (u32)~(u32)0;
        bin1[0] = 0;
        nstatevec10::clear_all_free(&free0, &used0, bin1, 32);
    }

    static void init_statevec10_full(u32& free0, u32& used0, u32 (&bin1)[1])
    {
        init_statevec10_empty(free0, used0, bin1);
        for (u32 bit = 0; bit < 32; ++bit)
            nstatevec10::set_used(&free0, &used0, bin1, 32, bit);
    }

    static void init_statevec12_empty(u64& free0, u64& used0, u64 (&bin1)[1])
    {
        free0   = 0;
        used0   = (u64)~(u64)0;
        bin1[0] = 0;
        nstatevec12::clear_all_free(&free0, &used0, bin1, 64);
    }

    static void init_statevec12_full(u64& free0, u64& used0, u64 (&bin1)[1])
    {
        init_statevec12_empty(free0, used0, bin1);
        for (u32 bit = 0; bit < 64; ++bit)
            nstatevec12::set_used(&free0, &used0, bin1, 64, bit);
    }

    static void init_statevec15_empty(u32& free0, u32 (&free1)[1], u32& used0, u32 (&used1)[1], u32 (&bin2)[1])
    {
        free0    = 0;
        free1[0] = 0;
        used0    = (u32)~(u32)0;
        used1[0] = (u32)~(u32)0;
        bin2[0]  = 0;
        nstatevec15::clear_all_free(&free0, free1, &used0, used1, bin2, 32);
    }

    static void init_statevec15_empty_64(u32& free0, u32 (&free1)[1], u32& used0, u32 (&used1)[1], u32 (&bin2)[2])
    {
        free0    = 0;
        free1[0] = 0;
        used0    = (u32)~(u32)0;
        used1[0] = (u32)~(u32)0;
        bin2[0]  = 0;
        bin2[1]  = 0;
        nstatevec15::clear_all_free(&free0, free1, &used0, used1, bin2, 64);
    }

    static void init_statevec18_empty(u64& free0, u64 (&free1)[1], u64& used0, u64 (&used1)[1], u64 (&bin2)[2])
    {
        free0    = 0;
        free1[0] = 0;
        used0    = (u64)~(u64)0;
        used1[0] = (u64)~(u64)0;
        bin2[0]  = 0;
        bin2[1]  = 0;
        nstatevec18::clear_all_free(&free0, free1, &used0, used1, bin2, 128);
    }

    static void init_statevec20_empty_64(u32& free0, u32 (&free1)[1], u32 (&free2)[1], u32& used0, u32 (&used1)[1], u32 (&used2)[1], u32 (&bin3)[2])
    {
        free0    = 0;
        free1[0] = 0;
        free2[0] = 0;
        used0    = (u32)~(u32)0;
        used1[0] = (u32)~(u32)0;
        used2[0] = (u32)~(u32)0;
        bin3[0]  = 0;
        bin3[1]  = 0;
        nstatevec20::clear_all_free(&free0, free1, free2, &used0, used1, used2, bin3, 64);
    }

    static void init_statevec20_empty_1056(u32& free0, u32 (&free1)[1], u32 (&free2)[2], u32& used0, u32 (&used1)[1], u32 (&used2)[2], u32 (&bin3)[33])
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
        nstatevec20::clear_all_free(&free0, free1, free2, &used0, used1, used2, bin3, 1056);
    }
}

UNITTEST_SUITE_BEGIN(statevec1)
{
    UNITTEST_FIXTURE(statevec10)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(set_single_bit)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            nstatevec10::set_used(&free0, &used0, bin1, 32, 0);

            CHECK_TRUE(nstatevec10::get(&free0, &used0, bin1, 32, 0));
        }

        UNITTEST_TEST(set_free_single_bit)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            nstatevec10::set_used(&free0, &used0, bin1, 32, 0);

            nstatevec10::set_free(&free0, &used0, bin1, 32, 0);

            CHECK_FALSE(nstatevec10::get(&free0, &used0, bin1, 32, 0));
        }

        UNITTEST_TEST(set_multiple_bits)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            nstatevec10::set_used(&free0, &used0, bin1, 32, 5);
            nstatevec10::set_used(&free0, &used0, bin1, 32, 10);
            nstatevec10::set_used(&free0, &used0, bin1, 32, 15);

            CHECK_TRUE(nstatevec10::get(&free0, &used0, bin1, 32, 5));
            CHECK_TRUE(nstatevec10::get(&free0, &used0, bin1, 32, 10));
            CHECK_TRUE(nstatevec10::get(&free0, &used0, bin1, 32, 15));
        }

        UNITTEST_TEST(get_after_clear)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            nstatevec10::set_used(&free0, &used0, bin1, 32, 3);
            CHECK_TRUE(nstatevec10::get(&free0, &used0, bin1, 32, 3));

            nstatevec10::set_free(&free0, &used0, bin1, 32, 3);
            CHECK_FALSE(nstatevec10::get(&free0, &used0, bin1, 32, 3));
        }

        UNITTEST_TEST(find_free_in_cleared_map)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            s32 bit = nstatevec10::find_free(&free0, &used0, bin1, 32);
            CHECK_TRUE(bit >= 0 && bit < 32);
        }

        UNITTEST_TEST(find_free_fails_when_all_set)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_full(free0, used0, bin1);

            s32 bit = nstatevec10::find_free(&free0, &used0, bin1, 32);
            CHECK_EQUAL(-1, bit);
        }

        UNITTEST_TEST(find_free_partial_set)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            // Set some bits, leaving others free
            nstatevec10::set_used(&free0, &used0, bin1, 32, 0);
            nstatevec10::set_used(&free0, &used0, bin1, 32, 1);
            nstatevec10::set_used(&free0, &used0, bin1, 32, 2);

            s32 bit = nstatevec10::find_free(&free0, &used0, bin1, 32);
            CHECK_TRUE(bit >= 3 && bit < 32);
        }

        UNITTEST_TEST(find_used_fails_on_cleared_map)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            s32 bit = nstatevec10::find_used(&free0, &used0, bin1, 32);
            CHECK_EQUAL(-1, bit);
        }

        UNITTEST_TEST(find_used_succeeds_when_bits_set)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            nstatevec10::set_used(&free0, &used0, bin1, 32, 5);
            nstatevec10::set_used(&free0, &used0, bin1, 32, 10);

            s32 bit = nstatevec10::find_used(&free0, &used0, bin1, 32);
            CHECK_EQUAL(5, bit);
        }

        UNITTEST_TEST(find_used_finds_first_set_bit)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            nstatevec10::set_used(&free0, &used0, bin1, 32, 7);

            s32 bit = nstatevec10::find_used(&free0, &used0, bin1, 32);
            CHECK_EQUAL(7, bit);
        }

        UNITTEST_TEST(alloc_finds_and_marks)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            s32 bit = nstatevec10::alloc(&free0, &used0, bin1, 32);
            CHECK_TRUE(bit >= 0 && bit < 32);
            CHECK_TRUE(nstatevec10::get(&free0, &used0, bin1, 32, bit));
        }

        UNITTEST_TEST(alloc_sequential)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            s32 bit1 = nstatevec10::alloc(&free0, &used0, bin1, 32);
            s32 bit2 = nstatevec10::alloc(&free0, &used0, bin1, 32);

            CHECK_TRUE(bit1 >= 0);
            CHECK_TRUE(bit2 >= 0);
            CHECK_NOT_EQUAL(bit1, bit2);
        }

        UNITTEST_TEST(free_clears_bit)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            s32 bit = nstatevec10::alloc(&free0, &used0, bin1, 32);
            CHECK_TRUE(nstatevec10::get(&free0, &used0, bin1, 32, bit));

            CHECK_EQUAL(bit, nstatevec10::free(&free0, &used0, bin1, 32));
            CHECK_FALSE(nstatevec10::get(&free0, &used0, bin1, 32, bit));
        }

        UNITTEST_TEST(free_sequential)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            s32 bit1 = nstatevec10::alloc(&free0, &used0, bin1, 32);
            s32 bit2 = nstatevec10::alloc(&free0, &used0, bin1, 32);

            s32 freed1 = nstatevec10::free(&free0, &used0, bin1, 32);
            s32 freed2 = nstatevec10::free(&free0, &used0, bin1, 32);

            CHECK_EQUAL(bit1, freed1);
            CHECK_EQUAL(bit2, freed2);
        }

        UNITTEST_TEST(alloc_last_in_cleared_map)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            s32 bit = nstatevec10::alloc_last(&free0, &used0, bin1, 32);
            CHECK_EQUAL(31, bit);
        }

        UNITTEST_TEST(free_last_with_set_bits)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            nstatevec10::set_used(&free0, &used0, bin1, 32, 5);
            nstatevec10::set_used(&free0, &used0, bin1, 32, 10);

            s32 bit = nstatevec10::free_last(&free0, &used0, bin1, 32);
            CHECK_EQUAL(10, bit);
        }

        UNITTEST_TEST(allocate_and_free_all)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            // Allocate all bits
            for (int i = 0; i < 32; i++)
            {
                s32 bit = nstatevec10::alloc(&free0, &used0, bin1, 32);
                CHECK_TRUE(bit >= 0);
            }

            // Should be no more free bits
            s32 bit = nstatevec10::alloc(&free0, &used0, bin1, 32);
            CHECK_EQUAL(-1, bit);
        }

        UNITTEST_TEST(find_before_after_and_last)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_empty(free0, used0, bin1);

            nstatevec10::set_used(&free0, &used0, bin1, 32, 2);
            nstatevec10::set_used(&free0, &used0, bin1, 32, 7);
            nstatevec10::set_used(&free0, &used0, bin1, 32, 15);

            CHECK_EQUAL(7, nstatevec10::find_used_after(&free0, &used0, bin1, 32, 2));
            CHECK_EQUAL(-1, nstatevec10::find_used_before(&free0, &used0, bin1, 32, 2));
            CHECK_EQUAL(7, nstatevec10::find_used_before(&free0, &used0, bin1, 32, 15));
            CHECK_EQUAL(15, nstatevec10::find_used_last(&free0, &used0, bin1, 32));

            CHECK_EQUAL(3, nstatevec10::find_free_after(&free0, &used0, bin1, 32, 2));
            CHECK_EQUAL(6, nstatevec10::find_free_before(&free0, &used0, bin1, 32, 7));
            CHECK_EQUAL(31, nstatevec10::find_free_last(&free0, &used0, bin1, 32));
        }

        UNITTEST_TEST(clear_all_used_marks_all_bits_used)
        {
            u32 free0;
            u32 used0;
            u32 bin1[1];
            init_statevec10_full(free0, used0, bin1);

            nstatevec10::clear_all_used(&free0, &used0, bin1, 32);

            CHECK_EQUAL(0, nstatevec10::find_used(&free0, &used0, bin1, 32));
            CHECK_EQUAL(-1, nstatevec10::find_free(&free0, &used0, bin1, 32));
        }

        UNITTEST_TEST(lazy_setup_and_tick_open_first_word)
        {
            u32 free0 = 0xFFFFFFFFu;
            u32 used0 = 0xFFFFFFFFu;
            u32 bin1[1] = {0xFFFFFFFFu};

            nstatevec10::setup_used_lazy(&free0, &used0, bin1, 32);

            CHECK_EQUAL(-1, nstatevec10::find_free(&free0, &used0, bin1, 32));
            CHECK_EQUAL(-1, nstatevec10::find_used(&free0, &used0, bin1, 32));

            nstatevec10::tick_used_lazy(&free0, &used0, bin1, 32, 0);

            CHECK_EQUAL(0, nstatevec10::find_free(&free0, &used0, bin1, 32));
            CHECK_EQUAL(-1, nstatevec10::find_used(&free0, &used0, bin1, 32));
        }
    }

    UNITTEST_FIXTURE(statevec12)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(set_get_and_find_variants)
        {
            u64 free0;
            u64 used0;
            u64 bin1[1];
            init_statevec12_empty(free0, used0, bin1);

            nstatevec12::set_used(&free0, &used0, bin1, 64, 1);
            nstatevec12::set_used(&free0, &used0, bin1, 64, 33);
            nstatevec12::set_used(&free0, &used0, bin1, 64, 63);

            CHECK_TRUE(nstatevec12::get(&free0, &used0, bin1, 64, 1));
            CHECK_TRUE(nstatevec12::get(&free0, &used0, bin1, 64, 33));
            CHECK_TRUE(nstatevec12::get(&free0, &used0, bin1, 64, 63));

            CHECK_EQUAL(1, nstatevec12::find_used(&free0, &used0, bin1, 64));
            CHECK_EQUAL(33, nstatevec12::find_used_after(&free0, &used0, bin1, 64, 1));
            CHECK_EQUAL(-1, nstatevec12::find_used_before(&free0, &used0, bin1, 64, 1));
            CHECK_EQUAL(33, nstatevec12::find_used_before(&free0, &used0, bin1, 64, 63));
            CHECK_EQUAL(63, nstatevec12::find_used_last(&free0, &used0, bin1, 64));

            CHECK_EQUAL(0, nstatevec12::find_free(&free0, &used0, bin1, 64));
            CHECK_EQUAL(2, nstatevec12::find_free_after(&free0, &used0, bin1, 64, 1));
            CHECK_EQUAL(32, nstatevec12::find_free_before(&free0, &used0, bin1, 64, 33));
            CHECK_EQUAL(62, nstatevec12::find_free_last(&free0, &used0, bin1, 64));
        }

        UNITTEST_TEST(alloc_and_free_variants)
        {
            u64 free0;
            u64 used0;
            u64 bin1[1];
            init_statevec12_empty(free0, used0, bin1);

            CHECK_EQUAL(0, nstatevec12::alloc(&free0, &used0, bin1, 64));
            CHECK_EQUAL(63, nstatevec12::alloc_last(&free0, &used0, bin1, 64));
            CHECK_EQUAL(0, nstatevec12::free(&free0, &used0, bin1, 64));
            CHECK_EQUAL(63, nstatevec12::free_last(&free0, &used0, bin1, 64));
        }

        UNITTEST_TEST(clear_all_used_marks_all_bits_used)
        {
            u64 free0;
            u64 used0;
            u64 bin1[1];
            init_statevec12_full(free0, used0, bin1);

            nstatevec12::clear_all_used(&free0, &used0, bin1, 64);

            CHECK_EQUAL(0, nstatevec12::find_used(&free0, &used0, bin1, 64));
            CHECK_EQUAL(-1, nstatevec12::find_free(&free0, &used0, bin1, 64));
        }

        UNITTEST_TEST(lazy_setup_and_tick_open_first_word)
        {
            u64 free0 = (u64)~(u64)0;
            u64 used0 = (u64)~(u64)0;
            u64 bin1[1] = {(u64)~(u64)0};

            nstatevec12::setup_used_lazy(&free0, &used0, bin1, 64);

            CHECK_EQUAL(-1, nstatevec12::find_free(&free0, &used0, bin1, 64));
            CHECK_EQUAL(-1, nstatevec12::find_used(&free0, &used0, bin1, 64));

            nstatevec12::tick_used_lazy(&free0, &used0, bin1, 64, 0);

            CHECK_EQUAL(0, nstatevec12::find_free(&free0, &used0, bin1, 64));
            CHECK_EQUAL(-1, nstatevec12::find_used(&free0, &used0, bin1, 64));
        }
    }

    UNITTEST_FIXTURE(statevec15)
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
            init_statevec15_empty(free0, free1, used0, used1, bin2);

            for (u32 bit = 0; bit < 32; ++bit)
                nstatevec15::set_used(&free0, free1, &used0, used1, bin2, 32, bit);

            CHECK_EQUAL(-1, nstatevec15::find_free(&free0, free1, &used0, used1, bin2, 32));

            nstatevec15::set_free(&free0, free1, &used0, used1, bin2, 32, 7);

            CHECK_EQUAL(7, nstatevec15::find_free(&free0, free1, &used0, used1, bin2, 32));
            CHECK_FALSE(nstatevec15::get(&free0, free1, &used0, used1, bin2, 32, 7));
        }

        UNITTEST_TEST(set_free_clears_last_used_leaf)
        {
            u32 free0;
            u32 free1[1];
            u32 used0;
            u32 used1[1];
            u32 bin2[1];
            init_statevec15_empty(free0, free1, used0, used1, bin2);

            nstatevec15::set_used(&free0, free1, &used0, used1, bin2, 32, 7);
            CHECK_EQUAL(7, nstatevec15::find_used(&free0, free1, &used0, used1, bin2, 32));

            nstatevec15::set_free(&free0, free1, &used0, used1, bin2, 32, 7);

            CHECK_EQUAL(-1, nstatevec15::find_used(&free0, free1, &used0, used1, bin2, 32));
            CHECK_FALSE(nstatevec15::get(&free0, free1, &used0, used1, bin2, 32, 7));
        }

        UNITTEST_TEST(alloc_last_in_cleared_map)
        {
            u32 free0;
            u32 free1[1];
            u32 used0;
            u32 used1[1];
            u32 bin2[1];
            init_statevec15_empty(free0, free1, used0, used1, bin2);

            s32 bit = nstatevec15::alloc_last(&free0, free1, &used0, used1, bin2, 32);
            CHECK_EQUAL(31, bit);
        }

        UNITTEST_TEST(free_last_with_set_bits)
        {
            u32 free0;
            u32 free1[1];
            u32 used0;
            u32 used1[1];
            u32 bin2[1];
            init_statevec15_empty(free0, free1, used0, used1, bin2);

            nstatevec15::set_used(&free0, free1, &used0, used1, bin2, 32, 5);
            nstatevec15::set_used(&free0, free1, &used0, used1, bin2, 32, 10);

            s32 bit = nstatevec15::free_last(&free0, free1, &used0, used1, bin2, 32);
            CHECK_EQUAL(10, bit);
        }

        UNITTEST_TEST(find_before_after_and_last_across_words)
        {
            u32 free0;
            u32 free1[1];
            u32 used0;
            u32 used1[1];
            u32 bin2[2];
            init_statevec15_empty_64(free0, free1, used0, used1, bin2);

            nstatevec15::set_used(&free0, free1, &used0, used1, bin2, 64, 2);
            nstatevec15::set_used(&free0, free1, &used0, used1, bin2, 64, 37);
            nstatevec15::set_used(&free0, free1, &used0, used1, bin2, 64, 63);

            CHECK_TRUE(nstatevec15::get(&free0, free1, &used0, used1, bin2, 64, 37));
            CHECK_EQUAL(37, nstatevec15::find_used_after(&free0, free1, &used0, used1, bin2, 64, 2));
            CHECK_EQUAL(37, nstatevec15::find_used_before(&free0, free1, &used0, used1, bin2, 64, 63));
            CHECK_EQUAL(63, nstatevec15::find_used_last(&free0, free1, &used0, used1, bin2, 64));

            CHECK_EQUAL(0, nstatevec15::find_free(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(3, nstatevec15::find_free_after(&free0, free1, &used0, used1, bin2, 64, 2));
            CHECK_EQUAL(36, nstatevec15::find_free_before(&free0, free1, &used0, used1, bin2, 64, 37));
            CHECK_EQUAL(62, nstatevec15::find_free_last(&free0, free1, &used0, used1, bin2, 64));
        }

        UNITTEST_TEST(alloc_and_free_variants)
        {
            u32 free0;
            u32 free1[1];
            u32 used0;
            u32 used1[1];
            u32 bin2[2];
            init_statevec15_empty_64(free0, free1, used0, used1, bin2);

            CHECK_EQUAL(0, nstatevec15::alloc(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(63, nstatevec15::alloc_last(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(0, nstatevec15::free(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(63, nstatevec15::free_last(&free0, free1, &used0, used1, bin2, 64));
        }

        UNITTEST_TEST(lazy_setup_and_tick_marks_words_used)
        {
            u32 free0 = 0xFFFFFFFFu;
            u32 free1[1] = {0xFFFFFFFFu};
            u32 used0 = 0xFFFFFFFFu;
            u32 used1[1] = {0xFFFFFFFFu};
            u32 bin2[2] = {0u, 0u};

            nstatevec15::setup_used_lazy(&free0, free1, &used0, used1, bin2, 64);

            CHECK_EQUAL(-1, nstatevec15::find_free(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(-1, nstatevec15::find_used(&free0, free1, &used0, used1, bin2, 64));

            nstatevec15::tick_used_lazy(&free0, free1, &used0, used1, bin2, 64, 0);

            CHECK_EQUAL(0, nstatevec15::find_used(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(31, nstatevec15::find_used_last(&free0, free1, &used0, used1, bin2, 64));
            CHECK_EQUAL(-1, nstatevec15::find_free(&free0, free1, &used0, used1, bin2, 64));

            nstatevec15::tick_used_lazy(&free0, free1, &used0, used1, bin2, 64, 32);

            CHECK_TRUE(nstatevec15::get(&free0, free1, &used0, used1, bin2, 64, 32));
            CHECK_EQUAL(63, nstatevec15::find_used_last(&free0, free1, &used0, used1, bin2, 64));
        }
    }

    UNITTEST_FIXTURE(statevec18)
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
            init_statevec18_empty(free0, free1, used0, used1, bin2);

            nstatevec18::set_used(&free0, free1, &used0, used1, bin2, 128, 1);
            nstatevec18::set_used(&free0, free1, &used0, used1, bin2, 128, 65);
            nstatevec18::set_used(&free0, free1, &used0, used1, bin2, 128, 127);

            CHECK_TRUE(nstatevec18::get(&free0, free1, &used0, used1, bin2, 128, 1));
            CHECK_TRUE(nstatevec18::get(&free0, free1, &used0, used1, bin2, 128, 65));
            CHECK_TRUE(nstatevec18::get(&free0, free1, &used0, used1, bin2, 128, 127));

            CHECK_EQUAL(1, nstatevec18::find_used(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(65, nstatevec18::find_used_after(&free0, free1, &used0, used1, bin2, 128, 1));
            CHECK_EQUAL(65, nstatevec18::find_used_before(&free0, free1, &used0, used1, bin2, 128, 127));
            CHECK_EQUAL(127, nstatevec18::find_used_last(&free0, free1, &used0, used1, bin2, 128));

            CHECK_EQUAL(0, nstatevec18::find_free(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(2, nstatevec18::find_free_after(&free0, free1, &used0, used1, bin2, 128, 1));
            CHECK_EQUAL(64, nstatevec18::find_free_before(&free0, free1, &used0, used1, bin2, 128, 65));
            CHECK_EQUAL(126, nstatevec18::find_free_last(&free0, free1, &used0, used1, bin2, 128));
        }

        UNITTEST_TEST(alloc_and_free_variants)
        {
            u64 free0;
            u64 free1[1];
            u64 used0;
            u64 used1[1];
            u64 bin2[2];
            init_statevec18_empty(free0, free1, used0, used1, bin2);

            CHECK_EQUAL(0, nstatevec18::alloc(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(127, nstatevec18::alloc_last(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(0, nstatevec18::free(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(127, nstatevec18::free_last(&free0, free1, &used0, used1, bin2, 128));
        }

        UNITTEST_TEST(lazy_setup_and_tick_marks_words_used)
        {
            u64 free0 = (u64)~(u64)0;
            u64 free1[1] = {(u64)~(u64)0};
            u64 used0 = (u64)~(u64)0;
            u64 used1[1] = {(u64)~(u64)0};
            u64 bin2[2] = {0u, 0u};

            nstatevec18::setup_used_lazy(&free0, free1, &used0, used1, bin2, 128);

            CHECK_EQUAL(-1, nstatevec18::find_free(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(-1, nstatevec18::find_used(&free0, free1, &used0, used1, bin2, 128));

            nstatevec18::tick_used_lazy(&free0, free1, &used0, used1, bin2, 128, 0);

            CHECK_EQUAL(0, nstatevec18::find_used(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(63, nstatevec18::find_used_last(&free0, free1, &used0, used1, bin2, 128));
            CHECK_EQUAL(-1, nstatevec18::find_free(&free0, free1, &used0, used1, bin2, 128));

            nstatevec18::tick_used_lazy(&free0, free1, &used0, used1, bin2, 128, 64);

            CHECK_TRUE(nstatevec18::get(&free0, free1, &used0, used1, bin2, 128, 64));
            CHECK_EQUAL(127, nstatevec18::find_used_last(&free0, free1, &used0, used1, bin2, 128));
        }
    }

    UNITTEST_FIXTURE(statevec20)
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
            init_statevec20_empty_64(free0, free1, free2, used0, used1, used2, bin3);

            nstatevec20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 64, 1);
            nstatevec20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 64, 33);
            nstatevec20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63);

            CHECK_TRUE(nstatevec20::get(&free0, free1, free2, &used0, used1, used2, bin3, 64, 1));
            CHECK_TRUE(nstatevec20::get(&free0, free1, free2, &used0, used1, used2, bin3, 64, 33));
            CHECK_TRUE(nstatevec20::get(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63));

            CHECK_EQUAL(1, nstatevec20::find_used(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(33, nstatevec20::find_used_after(&free0, free1, free2, &used0, used1, used2, bin3, 64, 1));
            CHECK_EQUAL(-1, nstatevec20::find_used_before(&free0, free1, free2, &used0, used1, used2, bin3, 64, 1));
            CHECK_EQUAL(33, nstatevec20::find_used_before(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63));
            CHECK_EQUAL(63, nstatevec20::find_used_last(&free0, free1, free2, &used0, used1, used2, bin3, 64));

            CHECK_EQUAL(0, nstatevec20::find_free(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(2, nstatevec20::find_free_after(&free0, free1, free2, &used0, used1, used2, bin3, 64, 1));
            CHECK_EQUAL(32, nstatevec20::find_free_before(&free0, free1, free2, &used0, used1, used2, bin3, 64, 33));
            CHECK_EQUAL(62, nstatevec20::find_free_last(&free0, free1, free2, &used0, used1, used2, bin3, 64));
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
            init_statevec20_empty_64(free0, free1, free2, used0, used1, used2, bin3);

            CHECK_EQUAL(0, nstatevec20::alloc(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(63, nstatevec20::alloc_last(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(0, nstatevec20::free(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(63, nstatevec20::free_last(&free0, free1, free2, &used0, used1, used2, bin3, 64));
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

            nstatevec20::setup_used_lazy(&free0, free1, free2, &used0, used1, used2, bin3, 64);

            CHECK_EQUAL(-1, nstatevec20::find_free(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(-1, nstatevec20::find_used(&free0, free1, free2, &used0, used1, used2, bin3, 64));

            nstatevec20::tick_used_lazy(&free0, free1, free2, &used0, used1, used2, bin3, 64, 0);

            CHECK_EQUAL(0, nstatevec20::find_used(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(31, nstatevec20::find_used_last(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(-1, nstatevec20::find_free(&free0, free1, free2, &used0, used1, used2, bin3, 64));

            nstatevec20::tick_used_lazy(&free0, free1, free2, &used0, used1, used2, bin3, 64, 32);

            CHECK_TRUE(nstatevec20::get(&free0, free1, free2, &used0, used1, used2, bin3, 64, 32));
            CHECK_EQUAL(63, nstatevec20::find_used_last(&free0, free1, free2, &used0, used1, used2, bin3, 64));
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
            init_statevec20_empty_64(free0, free1, free2, used0, used1, used2, bin3);

            nstatevec20::clear_all_used(&free0, free1, free2, &used0, used1, used2, bin3, 64);

            CHECK_EQUAL(0, nstatevec20::find_used(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(-1, nstatevec20::find_free(&free0, free1, free2, &used0, used1, used2, bin3, 64));
            CHECK_EQUAL(-1, nstatevec20::find_used_before(&free0, free1, free2, &used0, used1, used2, bin3, 64, 0));
            CHECK_EQUAL(-1, nstatevec20::find_used_after(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63));
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
            init_statevec20_empty_64(free0, free1, free2, used0, used1, used2, bin3);

            nstatevec20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 64, 0);
            nstatevec20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63);

            CHECK_EQUAL(-1, nstatevec20::find_used_before(&free0, free1, free2, &used0, used1, used2, bin3, 64, 0));
            CHECK_EQUAL(63, nstatevec20::find_used_after(&free0, free1, free2, &used0, used1, used2, bin3, 64, 0));
            CHECK_EQUAL(-1, nstatevec20::find_used_after(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63));
            CHECK_EQUAL(62, nstatevec20::find_free_before(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63));
            CHECK_EQUAL(-1, nstatevec20::find_free_after(&free0, free1, free2, &used0, used1, used2, bin3, 64, 63));
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
            init_statevec20_empty_1056(free0, free1, free2, used0, used1, used2, bin3);

            nstatevec20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1);
            nstatevec20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1025);
            nstatevec20::set_used(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1055);

            CHECK_TRUE(nstatevec20::get(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1025));
            CHECK_EQUAL(1025, nstatevec20::find_used_after(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1));
            CHECK_EQUAL(1025, nstatevec20::find_used_before(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1055));
            CHECK_EQUAL(1055, nstatevec20::find_used_last(&free0, free1, free2, &used0, used1, used2, bin3, 1056));

            CHECK_EQUAL(2, nstatevec20::find_free_after(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1));
            CHECK_EQUAL(1024, nstatevec20::find_free_before(&free0, free1, free2, &used0, used1, used2, bin3, 1056, 1025));
            CHECK_EQUAL(1054, nstatevec20::find_free_last(&free0, free1, free2, &used0, used1, used2, bin3, 1056));
        }
    }
}
UNITTEST_SUITE_END

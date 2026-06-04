#include "ccore/c_target.h"
#include "ccore/c_arena.h"
#include "ccore/c_memory.h"
#include "ccore/c_random.h"

#include "ccore/c_block_bin.h"

#include "cunittest/cunittest.h"

using namespace ncore;

namespace
{
    static const u32    s_item_size     = 16 * cKB;
    static const uint_t s_block_size    = 512 * cKB;
    static const uint_t s_reserved_size = 4 * s_block_size;
}  // namespace

UNITTEST_SUITE_BEGIN(block_bin)
{
    UNITTEST_FIXTURE(create_destroy)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(create_destroy_1)
        {
            bbin_t bin;
            g_memset(&bin, 0xCD, sizeof(bin));

            bin_setup(&bin, s_reserved_size, s_item_size);

            CHECK_NOT_NULL(bin.m_address_base);
            CHECK_EQUAL(s_reserved_size, bin.m_address_size);
            CHECK_TRUE(bin.m_ownership);
            CHECK_EQUAL((u32)0, bin_size(&bin));
            CHECK_EQUAL((u32)0, bin.m_block_count);
            CHECK_EQUAL((u32)4, bin.m_block_max_count);
            CHECK_NOT_NULL(bin.m_blocks);

            bin_destroy(&bin);

            CHECK_NULL(bin.m_address_base);
            CHECK_EQUAL((uint_t)0, bin.m_address_size);
            CHECK_NULL(bin.m_blocks);
            CHECK_EQUAL((u32)0, bin.m_block_count);
            CHECK_EQUAL((u32)0, bin.m_block_max_count);
            CHECK_FALSE(bin.m_ownership);
        }

        UNITTEST_TEST(create_destroy_with_external_base)
        {
            void* external_base = v_alloc_reserve(s_reserved_size);
            CHECK_NOT_NULL(external_base);

            bbin_t bin;
            g_memset(&bin, 0, sizeof(bin));

            bin_setup(&bin, external_base, s_reserved_size, s_item_size);

            CHECK_EQUAL(external_base, bin.m_address_base);
            CHECK_EQUAL(s_reserved_size, bin.m_address_size);
            CHECK_FALSE(bin.m_ownership);
            CHECK_EQUAL((u32)0, bin_size(&bin));
            CHECK_EQUAL((u32)4, bin.m_block_max_count);

            bin_destroy(&bin);

            CHECK_TRUE(v_alloc_release(external_base, s_reserved_size));
        }
    }

    UNITTEST_FIXTURE(alloc_free)
    {
        UNITTEST_TEST(size_tracks_alloc_free_and_reuses_blocks)
        {
            bbin_t bin;
            bin_setup(&bin, s_reserved_size, s_item_size);

            void* a = bin_alloc(&bin, s_item_size);
            void* b = bin_alloc(&bin, s_item_size / 2);
            void* c = bin_alloc(&bin, s_item_size / 4);

            CHECK_NOT_NULL(a);
            CHECK_NOT_NULL(b);
            CHECK_NOT_NULL(c);

            CHECK_EQUAL((u32)3, bin_size(&bin));
            CHECK_EQUAL((u32)3, bin.m_block_count);
            CHECK_EQUAL((void*)((byte*)a + s_block_size), b);
            CHECK_EQUAL((void*)((byte*)b + s_block_size), c);

            bin_free(&bin, b);

            CHECK_EQUAL((u32)2, bin_size(&bin));

            void* d = bin_alloc(&bin, s_item_size / 8);
            CHECK_NOT_NULL(d);
            CHECK_EQUAL(b, d);
            CHECK_EQUAL((u32)3, bin_size(&bin));

            bin_free(&bin, a);
            bin_free(&bin, c);
            bin_free(&bin, d);

            CHECK_EQUAL((u32)0, bin_size(&bin));

            bin_destroy(&bin);
        }

        UNITTEST_TEST(many_allocations_with_random_frees)
        {
            static const u32 max_blocks = 2048;
            static const u32 iterations = 20000;

            bbin_t bin;
            bin_setup(&bin, (uint_t)max_blocks * s_block_size, s_item_size);

            void* slots[max_blocks];
            for (u32 i = 0; i < max_blocks; ++i)
                slots[i] = nullptr;

            xor_random_t rnd(0x1234567890abcdefULL);
            u32          active_count = 0;

            for (u32 i = 0; i < iterations; ++i)
            {
                const bool do_alloc = (active_count == 0) || ((active_count < max_blocks) && ((rnd.rand32() & 1u) != 0));

                if (do_alloc && active_count < max_blocks)
                {
                    void* ptr = bin_alloc(&bin, s_item_size - (rnd.rand32() % (8 * cKB)));
                    CHECK_NOT_NULL(ptr);
                    slots[active_count] = ptr;
                    ++active_count;
                }
                else if (active_count > 0)
                {
                    // do a random swap
                    if (active_count > 1)
                    {
                        u32   slot              = rnd.rand32() % active_count;
                        void* tmp               = slots[slot];
                        slots[slot]             = slots[active_count - 1];
                        slots[active_count - 1] = tmp;
                    }

                    // free from the top
                    --active_count;
                    bin_free(&bin, slots[active_count]);
                    slots[active_count] = nullptr;
                }

                CHECK_EQUAL(active_count, bin_size(&bin));
            }

            for (u32 i = 0; i < active_count; ++i)
            {
                bin_free(&bin, slots[i]);
                slots[i] = nullptr;
            }

            CHECK_EQUAL((u32)0, bin_size(&bin));
            bin_destroy(&bin);
        }

        UNITTEST_TEST(exhausts_reserved_capacity)
        {
            bbin_t bin;
            bin_setup(&bin, 2 * s_block_size, s_item_size);

            void* first  = bin_alloc(&bin, s_item_size);
            void* second = bin_alloc(&bin, s_item_size);
            void* third  = bin_alloc(&bin, s_item_size);

            CHECK_NOT_NULL(first);
            CHECK_NOT_NULL(second);
            CHECK_NULL(third);
            CHECK_EQUAL((u32)2, bin_size(&bin));
            CHECK_EQUAL((u32)2, bin.m_block_count);
            CHECK_EQUAL((u32)2, bin.m_block_max_count);

            bin_free(&bin, first);
            bin_free(&bin, second);

            CHECK_EQUAL((u32)0, bin_size(&bin));

            bin_destroy(&bin);
        }
    }
}
UNITTEST_SUITE_END

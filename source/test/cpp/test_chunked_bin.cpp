#include "ccore/c_target.h"
#include "ccore/c_memory.h"

#include "ccore/c_chunked_bin.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(chunked_bin)
{
    UNITTEST_FIXTURE(create_destroy)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        struct item_t
        {
            u32 m_value;
            f32 m_value2;
        };

        UNITTEST_TEST(create_destroy_1)
        {
            cbin_t bin;
            bin_setup(&bin, 256 * 4 * cKB, sizeof(item_t));  // 256K items, 4K chunk size
            bin_destroy(&bin);
        }
    }

    UNITTEST_FIXTURE(alloc_free)
    {

        struct item_t
        {
            u32 m_value;
            f32 m_value2;
        };

        UNITTEST_TEST(a_few_alloc_free)
        {
            cbin_t bin;
            bin_setup(&bin, 256 * 4 * cKB, sizeof(item_t));  // 256K items, 4K chunk size

            const u32 num_allocs = 1000;
            item_t*   ptrs[num_allocs];

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = (item_t*)bin_alloc(&bin);
                CHECK_NOT_NULL(ptrs[i]);
                ptrs[i]->m_value  = i;
                ptrs[i]->m_value2 = (f32)i * 0.5f;
            }

            for (u32 i = 0; i < num_allocs; ++i)
            {
                CHECK_EQUAL(ptrs[i]->m_value, i);
                CHECK_CLOSE(ptrs[i]->m_value2, (f32)i * 0.5f, 0.001f);
                bin_free(&bin, ptrs[i]);
            }

            bin_destroy(&bin);
        }

        UNITTEST_TEST(size_tracks_alloc_and_free)
        {
            cbin_t bin;
            bin_setup(&bin, 16 * cKB, sizeof(item_t));

            const u32 num_allocs = 300;
            item_t*   ptrs[num_allocs];
            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = nullptr;
            }

            CHECK_EQUAL(bin_size(&bin), (u32)0);

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = (item_t*)bin_alloc(&bin);
                CHECK_NOT_NULL(ptrs[i]);
                CHECK_EQUAL(bin_size(&bin), (u32)(i + 1));
            }

            for (u32 i = 0; i < num_allocs; ++i)
            {
                bin_free(&bin, ptrs[i]);
                CHECK_EQUAL(bin_size(&bin), (u32)(num_allocs - i - 1));
            }

            CHECK_EQUAL(bin_size(&bin), (u32)0);
            bin_destroy(&bin);
        }

        UNITTEST_TEST(free_reuses_slot)
        {
            cbin_t bin;
            bin_setup(&bin, 16 * cKB, sizeof(item_t));

            item_t* a = (item_t*)bin_alloc(&bin);
            item_t* b = (item_t*)bin_alloc(&bin);
            item_t* c = (item_t*)bin_alloc(&bin);

            CHECK_NOT_NULL(a);
            CHECK_NOT_NULL(b);
            CHECK_NOT_NULL(c);

            bin_free(&bin, b);
            item_t* d = (item_t*)bin_alloc(&bin);

            CHECK_NOT_NULL(d);
            CHECK_EQUAL((void*)d, (void*)b);

            bin_free(&bin, a);
            bin_free(&bin, c);
            bin_free(&bin, d);

            bin_destroy(&bin);
        }
    }

    UNITTEST_FIXTURE(stress2)
    {
        struct item_t
        {
            u32 m_value;
            f32 m_value2;
        };

        UNITTEST_TEST(exhausts_reserved_capacity)
        {
            cbin_t bin;
            bin_setup(&bin, 16 * cKB, sizeof(item_t));

            const u32 max_items = (16 * cKB) / sizeof(item_t);
            item_t*   ptr       = nullptr;

            for (u32 i = 0; i < max_items; ++i)
            {
                ptr = (item_t*)bin_alloc(&bin);
                CHECK_NOT_NULL(ptr);
            }

            CHECK_EQUAL(bin_size(&bin), max_items);

            item_t* overflow = (item_t*)bin_alloc(&bin);
            CHECK_NULL(overflow);
            CHECK_EQUAL(bin_size(&bin), max_items);

            bin_destroy(&bin);
        }
    }
}
UNITTEST_SUITE_END

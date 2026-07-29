#include "ccore/c_target.h"
#include "ccore/c_arena.h"
#include "ccore/c_memory.h"

#include "ccore/c_chunk_bin.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(chunk_bin)
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

        UNITTEST_TEST(create_destroy_with_external_storage)
        {
            const uint_t base_size      = 32 * cKB;
            const u32    storage_size   = bin_calculate_size(base_size, sizeof(item_t));
            void*        bin_storage    = v_alloc_reserve(storage_size);
            void*        external_base  = v_alloc_reserve(base_size);

            CHECK_NOT_NULL(bin_storage);
            CHECK_NOT_NULL(external_base);
            CHECK_TRUE(v_alloc_commit(bin_storage, storage_size));

            cbin_t* bin = bin_setup(bin_storage, storage_size, external_base, base_size, sizeof(item_t));
            CHECK_NOT_NULL(bin);
            CHECK_EQUAL(bin_storage, (void*)bin);
            CHECK_EQUAL(external_base, bin->m_address_base);
            CHECK_FALSE(bin->m_ownership);
            CHECK_FALSE(bin->m_chunk_array_ownership);
            CHECK_EQUAL((void*)((byte*)bin_storage + sizeof(cbin_t)), bin->m_chunk_array);

            item_t* item = (item_t*)bin_alloc(bin);
            CHECK_NOT_NULL(item);
            bin_free(bin, item);

            bin_destroy(bin);

            CHECK_TRUE(v_alloc_release(external_base, base_size));
            CHECK_TRUE(v_alloc_release(bin_storage, storage_size));
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

        UNITTEST_TEST(free_from_non_head_active_chunk_preserves_active_head)
        {
            cbin_t bin;
            bin_setup(&bin, 32 * cKB, sizeof(item_t));

            const u32 max_items = bin.m_chunk_max_items;
            item_t**  ptrs      = (item_t**)v_alloc_reserve((max_items + 1) * sizeof(item_t*));
            CHECK_NOT_NULL(ptrs);
            CHECK_TRUE(v_alloc_commit(ptrs, (max_items + 1) * sizeof(item_t*)));

            for (u32 i = 0; i < max_items + 1; ++i)
            {
                ptrs[i] = (item_t*)bin_alloc(&bin);
                CHECK_NOT_NULL(ptrs[i]);
            }

            item_t* reused_from_full_chunk = ptrs[0];
            item_t* only_item_in_second_chunk = ptrs[max_items];

            bin_free(&bin, reused_from_full_chunk);
            bin_free(&bin, only_item_in_second_chunk);

            item_t* next = (item_t*)bin_alloc(&bin);
            CHECK_NOT_NULL(next);
            CHECK_EQUAL((void*)reused_from_full_chunk, (void*)next);

            bin_free(&bin, next);
            for (u32 i = 1; i < max_items; ++i)
            {
                bin_free(&bin, ptrs[i]);
            }

            bin_destroy(&bin);
            CHECK_TRUE(v_alloc_release(ptrs, (max_items + 1) * sizeof(item_t*)));
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

            const u32 max_items = bin.m_chunk_max_items;
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

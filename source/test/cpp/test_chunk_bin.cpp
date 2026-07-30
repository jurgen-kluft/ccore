#include "ccore/c_target.h"
#include "ccore/c_arena.h"
#include "ccore/c_memory.h"

#include "ccore/c_chunk_bin.h"

#include "cunittest/cunittest.h"

using namespace ncore;

namespace
{
    struct bin_storage_t
    {
        cbin_t* m_bin;
        void*   m_storage;
        uint_t m_storage_size;
    };

    static bin_storage_t s_create_bin(uint_t base_size, u16 item_sizeof, void* base_address = nullptr)
    {
        const u32    storage_pages = bin_calculate_size(base_size, item_sizeof);
        const uint_t storage_size  = (uint_t)storage_pages * v_alloc_get_page_size();
        void*        storage       = v_alloc_reserve(storage_size);
        ASSERT(storage != nullptr);
        ASSERT(v_alloc_commit(storage, storage_size));

        bin_storage_t result;
        result.m_bin          = bin_setup(storage, storage_pages, base_address, base_size, item_sizeof);
        result.m_storage      = storage;
        result.m_storage_size = storage_size;
        return result;
    }

    static void s_destroy_bin(bin_storage_t& storage)
    {
        bin_destroy(storage.m_bin);
        ASSERT(v_alloc_release(storage.m_storage, storage.m_storage_size));
        storage.m_bin          = nullptr;
        storage.m_storage      = nullptr;
        storage.m_storage_size = 0;
    }
}  // namespace

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

        UNITTEST_TEST(metadata_size_is_page_count)
        {
            const u32 small_pages = bin_calculate_size(16 * cKB, sizeof(item_t));
            const u32 large_pages = bin_calculate_size(16 * cMB, sizeof(item_t));

            CHECK_TRUE(small_pages > 0);
            CHECK_TRUE(large_pages > small_pages);
        }

        UNITTEST_TEST(create_destroy_owned_base)
        {
            bin_storage_t storage = s_create_bin(256 * 4 * cKB, sizeof(item_t));
            CHECK_NOT_NULL(storage.m_bin);
            CHECK_EQUAL((u32)0, bin_size(storage.m_bin));
            s_destroy_bin(storage);
        }

        UNITTEST_TEST(create_destroy_with_external_storage)
        {
            const uint_t base_size     = 32 * cKB;
            void*        external_base = v_alloc_reserve(base_size);
            CHECK_NOT_NULL(external_base);

            bin_storage_t storage = s_create_bin(base_size, sizeof(item_t), external_base);
            cbin_t*       bin     = storage.m_bin;
            CHECK_NOT_NULL(bin);

            item_t* item = (item_t*)bin_alloc(bin);
            CHECK_NOT_NULL(item);
            CHECK_EQUAL(external_base, (void*)item);
            bin_free(bin, item);

            s_destroy_bin(storage);
            CHECK_TRUE(v_alloc_release(external_base, base_size));
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
            bin_storage_t storage = s_create_bin(256 * 4 * cKB, sizeof(item_t));
            cbin_t*       bin     = storage.m_bin;

            const u32 num_allocs = 1000;
            item_t*   ptrs[num_allocs];

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = (item_t*)bin_alloc(bin);
                CHECK_NOT_NULL(ptrs[i]);
                ptrs[i]->m_value  = i;
                ptrs[i]->m_value2 = (f32)i * 0.5f;
            }

            for (u32 i = 0; i < num_allocs; ++i)
            {
                CHECK_EQUAL(ptrs[i]->m_value, i);
                CHECK_CLOSE(ptrs[i]->m_value2, (f32)i * 0.5f, 0.001f);
                bin_free(bin, ptrs[i]);
            }

            s_destroy_bin(storage);
        }

        UNITTEST_TEST(size_tracks_alloc_and_free)
        {
            bin_storage_t storage = s_create_bin(16 * cKB, sizeof(item_t));
            cbin_t*       bin     = storage.m_bin;

            const u32 num_allocs = 300;
            item_t*   ptrs[num_allocs];
            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = nullptr;
            }

            CHECK_EQUAL(bin_size(bin), (u32)0);

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = (item_t*)bin_alloc(bin);
                CHECK_NOT_NULL(ptrs[i]);
                CHECK_EQUAL(bin_size(bin), (u32)(i + 1));
            }

            for (u32 i = 0; i < num_allocs; ++i)
            {
                bin_free(bin, ptrs[i]);
                CHECK_EQUAL(bin_size(bin), (u32)(num_allocs - i - 1));
            }

            CHECK_EQUAL(bin_size(bin), (u32)0);
            s_destroy_bin(storage);
        }

        UNITTEST_TEST(free_reuses_slot)
        {
            bin_storage_t storage = s_create_bin(16 * cKB, sizeof(item_t));
            cbin_t*       bin     = storage.m_bin;

            item_t* a = (item_t*)bin_alloc(bin);
            item_t* b = (item_t*)bin_alloc(bin);
            item_t* c = (item_t*)bin_alloc(bin);

            CHECK_NOT_NULL(a);
            CHECK_NOT_NULL(b);
            CHECK_NOT_NULL(c);

            bin_free(bin, b);
            item_t* d = (item_t*)bin_alloc(bin);

            CHECK_NOT_NULL(d);
            CHECK_EQUAL((void*)d, (void*)b);

            bin_free(bin, a);
            bin_free(bin, c);
            bin_free(bin, d);

            s_destroy_bin(storage);
        }

        UNITTEST_TEST(layer1_exists_for_32_items_per_chunk)
        {
            struct large_item_t
            {
                u32  m_value;
                byte m_padding[2044];
            };

            static const u32 item_count = 33;
            bin_storage_t    storage   = s_create_bin(128 * cKB, sizeof(large_item_t));
            cbin_t*          bin       = storage.m_bin;
            large_item_t*    items[item_count];

            for (u32 i = 0; i < item_count; ++i)
            {
                items[i] = (large_item_t*)bin_alloc(bin);
                CHECK_NOT_NULL(items[i]);
                items[i]->m_value = i;
            }

            CHECK_EQUAL(item_count, bin_size(bin));
            for (u32 i = 0; i < item_count; ++i)
                CHECK_EQUAL(i, items[i]->m_value);

            large_item_t* first_chunk_item  = items[0];
            large_item_t* second_chunk_item = items[32];
            bin_free(bin, first_chunk_item);
            bin_free(bin, second_chunk_item);

            CHECK_EQUAL((void*)first_chunk_item, bin_alloc(bin));
            CHECK_EQUAL((void*)second_chunk_item, bin_alloc(bin));
            CHECK_EQUAL(item_count, bin_size(bin));

            s_destroy_bin(storage);
        }

        UNITTEST_TEST(free_from_non_head_active_chunk_preserves_active_head)
        {
            bin_storage_t storage = s_create_bin(32 * cKB, sizeof(item_t));
            cbin_t*       bin     = storage.m_bin;

            const u32 max_items = 1024;
            item_t**  ptrs      = (item_t**)v_alloc_reserve((max_items + 1) * sizeof(item_t*));
            CHECK_NOT_NULL(ptrs);
            CHECK_TRUE(v_alloc_commit(ptrs, (max_items + 1) * sizeof(item_t*)));

            for (u32 i = 0; i < max_items + 1; ++i)
            {
                ptrs[i] = (item_t*)bin_alloc(bin);
                CHECK_NOT_NULL(ptrs[i]);
            }

            item_t* reused_from_full_chunk = ptrs[0];
            item_t* only_item_in_second_chunk = ptrs[max_items];

            bin_free(bin, reused_from_full_chunk);
            bin_free(bin, only_item_in_second_chunk);

            item_t* next = (item_t*)bin_alloc(bin);
            CHECK_NOT_NULL(next);
            CHECK_EQUAL((void*)reused_from_full_chunk, (void*)next);

            bin_free(bin, next);
            for (u32 i = 1; i < max_items; ++i)
            {
                bin_free(bin, ptrs[i]);
            }

            s_destroy_bin(storage);
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
            bin_storage_t storage = s_create_bin(16 * cKB, sizeof(item_t));
            cbin_t*       bin     = storage.m_bin;

            const u32 max_items = 1024;
            item_t*   ptr       = nullptr;

            for (u32 i = 0; i < max_items; ++i)
            {
                ptr = (item_t*)bin_alloc(bin);
                CHECK_NOT_NULL(ptr);
            }

            CHECK_EQUAL(bin_size(bin), max_items);

            item_t* overflow = (item_t*)bin_alloc(bin);
            CHECK_NULL(overflow);
            CHECK_EQUAL(bin_size(bin), max_items);

            bin_free(bin, ptr);
            CHECK_EQUAL((void*)ptr, bin_alloc(bin));
            CHECK_EQUAL(bin_size(bin), max_items);

            s_destroy_bin(storage);
        }
    }
}
UNITTEST_SUITE_END

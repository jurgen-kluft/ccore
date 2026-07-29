#include "ccore/c_target.h"
#include "ccore/c_memory.h"
#include "ccore/c_random.h"

#include "ccore/c_block_bin.h"

#include "cunittest/cunittest.h"

using namespace ncore;

namespace
{
    static const u32    s_item_size     = 16 * cKB;
    static const uint_t s_reserved_size = 32 * cMB;

    struct bin_storage_t
    {
        bbin_t* m_bin;
        void*   m_storage;
        uint_t m_storage_size;
    };

    static bin_storage_t s_create_bin(uint_t reserved_size, u32 block_size, void* base_address = nullptr)
    {
        const u32    storage_pages = bin_calculate_size(reserved_size, block_size);
        const uint_t storage_size  = (uint_t)storage_pages * v_alloc_get_page_size();
        void*        storage       = v_alloc_reserve(storage_size);
        ASSERT(storage != nullptr);
        ASSERT(v_alloc_commit(storage, storage_size));

        bin_storage_t result;
        result.m_bin          = bin_setup(storage, storage_pages, base_address, reserved_size, block_size);
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

UNITTEST_SUITE_BEGIN(block_bin)
{
    UNITTEST_FIXTURE(create_destroy)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(metadata_size_is_page_count)
        {
            const u32 small_pages = bin_calculate_size(2 * s_item_size, s_item_size);
            const u32 large_pages = bin_calculate_size((uint_t)8192 * s_item_size, s_item_size);

            CHECK_TRUE(small_pages > 0);
            CHECK_TRUE(large_pages > small_pages);
        }

        UNITTEST_TEST(create_destroy_owned_base)
        {
            bin_storage_t storage = s_create_bin(s_reserved_size, s_item_size);
            CHECK_NOT_NULL(storage.m_bin);
            CHECK_EQUAL((u32)0, bin_size(storage.m_bin));

            void* item = bin_alloc(storage.m_bin, s_item_size);
            CHECK_NOT_NULL(item);
            bin_free(storage.m_bin, item);

            s_destroy_bin(storage);
        }

        UNITTEST_TEST(create_destroy_with_external_base)
        {
            void* external_base = v_alloc_reserve(s_reserved_size);
            CHECK_NOT_NULL(external_base);

            bin_storage_t storage = s_create_bin(s_reserved_size, s_item_size, external_base);
            CHECK_NOT_NULL(storage.m_bin);
            CHECK_EQUAL((u32)0, bin_size(storage.m_bin));

            void* item = bin_alloc(storage.m_bin, s_item_size);
            CHECK_EQUAL(external_base, item);
            bin_free(storage.m_bin, item);

            s_destroy_bin(storage);
            CHECK_TRUE(v_alloc_release(external_base, s_reserved_size));
        }
    }

    UNITTEST_FIXTURE(alloc_free)
    {
        UNITTEST_TEST(size_tracks_alloc_free_and_reuses_blocks)
        {
            bin_storage_t storage = s_create_bin(s_reserved_size, s_item_size);
            bbin_t*       bin     = storage.m_bin;

            void* a = bin_alloc(bin, s_item_size);
            void* b = bin_alloc(bin, s_item_size / 2);
            void* c = bin_alloc(bin, s_item_size / 4);

            CHECK_NOT_NULL(a);
            CHECK_NOT_NULL(b);
            CHECK_NOT_NULL(c);

            CHECK_EQUAL((u32)3, bin_size(bin));
            CHECK_EQUAL((void*)((byte*)a + s_item_size), b);
            CHECK_EQUAL((void*)((byte*)b + s_item_size), c);

            bin_free(bin, b);

            CHECK_EQUAL((u32)2, bin_size(bin));

            void* d = bin_alloc(bin, s_item_size / 8);
            CHECK_NOT_NULL(d);
            CHECK_EQUAL(b, d);
            CHECK_EQUAL((u32)3, bin_size(bin));

            bin_free(bin, a);
            bin_free(bin, c);
            bin_free(bin, d);

            CHECK_EQUAL((u32)0, bin_size(bin));

            s_destroy_bin(storage);
        }

        UNITTEST_TEST(free_list_reuses_blocks_in_lifo_order)
        {
            bin_storage_t storage = s_create_bin(4 * s_item_size, s_item_size);
            bbin_t*       bin     = storage.m_bin;

            void* a = bin_alloc(bin, s_item_size);
            void* b = bin_alloc(bin, s_item_size);
            void* c = bin_alloc(bin, s_item_size);
            void* d = bin_alloc(bin, s_item_size);

            bin_free(bin, b);
            bin_free(bin, d);
            CHECK_EQUAL(d, bin_alloc(bin, s_item_size));
            CHECK_EQUAL(b, bin_alloc(bin, s_item_size));

            bin_free(bin, a);
            bin_free(bin, b);
            bin_free(bin, c);
            bin_free(bin, d);
            CHECK_EQUAL((u32)0, bin_size(bin));

            s_destroy_bin(storage);
        }

        UNITTEST_TEST(commits_requested_partial_block)
        {
            const u32     block_size = 32 * cKB;
            bin_storage_t storage    = s_create_bin(2 * block_size, block_size);
            bbin_t*       bin     = storage.m_bin;
            const u32     size    = v_alloc_get_page_size() + 17;

            byte* item = (byte*)bin_alloc(bin, size);
            CHECK_NOT_NULL(item);
            item[0]        = 0x12;
            item[size - 1] = 0x34;
            CHECK_EQUAL((u32)0x12, (u32)item[0]);
            CHECK_EQUAL((u32)0x34, (u32)item[size - 1]);

            bin_free(bin, item);
            s_destroy_bin(storage);
        }

        UNITTEST_TEST(many_allocations_with_random_frees)
        {
            static const u32 max_blocks = 2048;
            static const u32 iterations = 20000;

            bin_storage_t storage = s_create_bin((uint_t)max_blocks * s_item_size, s_item_size);
            bbin_t*       bin     = storage.m_bin;

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
                    void* ptr = bin_alloc(bin, s_item_size - (rnd.rand32() % (8 * cKB)));
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
                    bin_free(bin, slots[active_count]);
                    slots[active_count] = nullptr;
                }

                CHECK_EQUAL(active_count, bin_size(bin));
            }

            for (u32 i = 0; i < active_count; ++i)
            {
                bin_free(bin, slots[i]);
                slots[i] = nullptr;
            }

            CHECK_EQUAL((u32)0, bin_size(bin));
            s_destroy_bin(storage);
        }

        UNITTEST_TEST(exhausts_reserved_capacity_and_reuses_free_block)
        {
            bin_storage_t storage = s_create_bin(2 * s_item_size, s_item_size);
            bbin_t*       bin     = storage.m_bin;

            void* first  = bin_alloc(bin, s_item_size);
            void* second = bin_alloc(bin, s_item_size);
            void* third  = bin_alloc(bin, s_item_size);

            CHECK_NOT_NULL(first);
            CHECK_NOT_NULL(second);
            CHECK_NULL(third);
            CHECK_EQUAL((u32)2, bin_size(bin));

            bin_free(bin, first);
            CHECK_EQUAL(first, bin_alloc(bin, s_item_size));

            bin_free(bin, first);
            bin_free(bin, second);
            CHECK_EQUAL((u32)0, bin_size(bin));

            s_destroy_bin(storage);
        }
    }
}
UNITTEST_SUITE_END

#include "ccore/c_target.h"
#include "ccore/c_allocator.h"
#include "ccore/c_bin.h"
#include "ccore/c_memory.h"
#include "ccore/c_random.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(bin)
{
    UNITTEST_FIXTURE(bin32)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_ALLOCATOR;

        UNITTEST_TEST(create_destroy_1)
        {
            bin32_t bin;
            bin_setup(&bin, 64, 64 * cKB);
            bin_destroy(&bin);
        }

        UNITTEST_TEST(create_destroy_2)
        {
            bin32_t bin;
            bin_setup(&bin, 64, 256 * cKB);  // 256K items
            bin_destroy(&bin);
        }

        UNITTEST_TEST(a_few_alloc_free)
        {
            bin32_t bin;
            bin_setup(&bin, 64, 64 * cKB);  // 64K items

            const u32 num_allocs = 1000;
            void*     ptrs[num_allocs];

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = bin_alloc(&bin);
                CHECK_NOT_NULL(ptrs[i]);
            }

            for (u32 i = 0; i < num_allocs; ++i)
            {
                bin_free(&bin, ptrs[i]);
            }

            bin_destroy(&bin);
        }

        UNITTEST_TEST(a_lot_more_alloc_free)
        {
            bin32_t bin;
            bin_setup(&bin, 64, 256 * 2 * cKB);  // 512K items

            const u32 num_allocs = 500000;
            void**    ptrs       = g_allocate_array<void*>(Allocator, num_allocs);

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = bin_alloc(&bin);
                CHECK_NOT_NULL(ptrs[i]);
            }

            xor_random_t rnd(0x1234567890abcdef);

            // 'randomly' shuffle pointers
            for (u32 i = 0; i < num_allocs; ++i)
            {
                u32 const a = rnd.rand32() % num_allocs;
                if (a != i)
                    nmem::swap(ptrs[a], ptrs[i]);
            }

            for (u32 i = 0; i < num_allocs; ++i)
            {
                bin_free(&bin, ptrs[i]);
            }

            g_deallocate_array(Allocator, ptrs);

            bin_destroy(&bin);
        }
    }

    UNITTEST_FIXTURE(bin16)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_ALLOCATOR;

        UNITTEST_TEST(create_destroy_1)
        {
            bin16_t bin;
            bin_setup(&bin, 64, 16 * cKB);
            bin_destroy(&bin);
        }

        UNITTEST_TEST(create_destroy_2)
        {
            bin16_t bin;
            bin_setup(&bin, 64, 64 * cKB);  // 64K items
            bin_destroy(&bin);
        }

        UNITTEST_TEST(a_few_alloc_free)
        {
            bin16_t bin;
            bin_setup(&bin, 64, 16 * cKB);  // 16K items

            const u32 num_allocs = 1000;
            void*     ptrs[num_allocs];

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = bin_alloc(&bin);
                CHECK_NOT_NULL(ptrs[i]);
            }

            for (u32 i = 0; i < num_allocs; ++i)
            {
                bin_free(&bin, ptrs[i]);
            }

            bin_destroy(&bin);
        }

        UNITTEST_TEST(a_lot_more_alloc_free)
        {
            bin16_t bin;
            bin_setup(&bin, 64, 64 * cKB);  // 64K items

            const u32 num_allocs = 60000;
            void**    ptrs       = g_allocate_array<void*>(Allocator, num_allocs);

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = bin_alloc(&bin);
                CHECK_NOT_NULL(ptrs[i]);
            }

            xor_random_t rnd(0x1234567890abcdef);

            // 'randomly' shuffle pointers
            for (u32 i = 0; i < num_allocs; ++i)
            {
                u32 const a = rnd.rand32() % num_allocs;
                if (a != i)
                    nmem::swap(ptrs[a], ptrs[i]);
            }

            for (u32 i = 0; i < num_allocs; ++i)
            {
                bin_free(&bin, ptrs[i]);
            }

            g_deallocate_array(Allocator, ptrs);

            bin_destroy(&bin);
        }
    }
}
UNITTEST_SUITE_END

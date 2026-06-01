#include "ccore/c_target.h"
#include "ccore/c_allocator.h"
#include "ccore/c_memory.h"
#include "ccore/c_random.h"

#include "ccore/c_chunked_bin.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(chunked_bin)
{
    UNITTEST_FIXTURE(operations)
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
            bin_setup(&bin, 256 * 4 * cKB, 4 * cKB, sizeof(item_t));  // 256K items, 4K chunk size
            bin_destroy(&bin);
        }

        UNITTEST_TEST(create_destroy_2)
        {
            cbin_t bin;
            bin_setup(&bin, 256 * 16 * cKB, 16 * cKB, sizeof(item_t));  // 256K items, 16K chunk size
            bin_destroy(&bin);
        }

        UNITTEST_TEST(a_few_alloc_free)
        {
            cbin_t bin;
            bin_setup(&bin, 256 * 4 * cKB, 4 * cKB, sizeof(item_t));  // 256K items, 4K chunk size

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
    }
}
UNITTEST_SUITE_END

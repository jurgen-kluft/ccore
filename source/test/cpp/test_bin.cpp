#include "ccore/c_target.h"
#include "ccore/c_allocator.h"
#include "ccore/c_bin.h"
#include "ccore/c_memory.h"
#include "ccore/c_random.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(bin)
{
    UNITTEST_FIXTURE(init)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_ALLOCATOR;

        UNITTEST_TEST(create_destroy_1)
        {
            nbin::bin_t* bin = nbin::make_bin(64, 65536);
            nbin::destroy(bin);
        }

        UNITTEST_TEST(create_destroy_2)
        {
            nbin::bin_t* bin = nbin::make_bin(64, 3 * 1024);  // 256K items
            nbin::destroy(bin);
        }

        UNITTEST_TEST(a_few_alloc_free)
        {
            nbin::bin_t* bin = nbin::make_bin(64, 65536);

            const u32 num_allocs = 1000;
            void*     ptrs[num_allocs];

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = nbin::alloc(bin);
                CHECK_NOT_NULL(ptrs[i]);
            }

            for (u32 i = 0; i < num_allocs; ++i)
            {
                nbin::free(bin, ptrs[i]);
            }

            nbin::destroy(bin);
        }

        UNITTEST_TEST(a_lot_more_alloc_free)
        {
            nbin::bin_t* bin = nbin::make_bin(64, 256 * 2 * 1024);  // 512K items

            const u32 num_allocs = 500000;
            void**    ptrs       = g_allocate_array<void*>(Allocator, num_allocs);

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = nbin::alloc(bin);
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
                nbin::free(bin, ptrs[i]);
            }

            g_deallocate_array(Allocator, ptrs);

            nbin::destroy(bin);
        }
    }
}
UNITTEST_SUITE_END

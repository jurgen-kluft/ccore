#include "ccore/c_target.h"
#include "ccore/c_allocator.h"
#include "ccore/c_bindex.h"
#include "ccore/c_memory.h"
#include "ccore/c_random.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(bindex)
{
    UNITTEST_FIXTURE(bindex16)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_ALLOCATOR;

        UNITTEST_TEST(create_destroy_1)
        {
            nbindex16::bindex_t* bin = nbindex16::make_bin(64, 64 * cKB);
            nbindex16::destroy(bin);
        }

        UNITTEST_TEST(create_destroy_2)
        {
            nbindex16::bindex_t* bin = nbindex16::make_bin(64, 256 * cKB);  // 256K items
            nbindex16::destroy(bin);
        }

        UNITTEST_TEST(a_few_alloc_free)
        {
            nbindex16::bindex_t* bin = nbindex16::make_bin(64, 64 * cKB);  // 64K items

            const u32 num_allocs = 1000;
            i32     ptrs[num_allocs];

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = nbindex16::alloc(bin);
                CHECK_NOT_EQUAL(-1, ptrs[i]);
            }

            for (u32 i = 0; i < num_allocs; ++i)
            {
                nbindex16::free(bin, ptrs[i]);
            }

            nbindex16::destroy(bin);
        }

        UNITTEST_TEST(a_lot_more_alloc_free)
        {
            nbindex16::bindex_t* bin = nbindex16::make_bin(64, 256 * 2 * cKB);  // 512K items

            const u32 num_allocs = 500000;
            i32*    ptrs       = g_allocate_array<i32>(Allocator, num_allocs);

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = nbindex16::alloc(bin);
                CHECK_NOT_EQUAL(-1, ptrs[i]);
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
                nbindex16::free(bin, ptrs[i]);
            }

            g_deallocate_array(Allocator, ptrs);

            nbindex16::destroy(bin);
        }
    }
}
UNITTEST_SUITE_END

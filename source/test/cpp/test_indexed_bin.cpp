#include "ccore/c_target.h"
#include "ccore/c_allocator.h"
#include "ccore/c_indexed_bin.h"
#include "ccore/c_memory.h"
#include "ccore/c_random.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(indexed_bin)
{
    UNITTEST_FIXTURE(bin16)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_ALLOCATOR;

        UNITTEST_TEST(create_destroy_1)
        {
            indexed_bin16_t* bin = make_bin(64);
            destroy(bin);
        }

        UNITTEST_TEST(create_destroy_2)
        {
            indexed_bin16_t* bin = make_bin(64);
            destroy(bin);
        }

        UNITTEST_TEST(a_few_alloc_free)
        {
            indexed_bin16_t* bin = make_bin(64);

            const u32 num_allocs = 1000;
            i32       ptrs[num_allocs];
            i32*      indices = g_allocate_array<i32>(Allocator, num_allocs);

            for (u32 i = 0; i < num_allocs; ++i)
            {
                ptrs[i] = alloc(bin, (u16)i);
                CHECK_NOT_EQUAL(-1, ptrs[i]);
            }

            for (u32 i = 0; i < num_allocs; ++i)
            {
                i32 new_owner = free(bin, ptrs[i]);
                if (new_owner != -1)
                {
                    ptrs[new_owner] = ptrs[i];
                }
                else
                {
                    CHECK_EQUAL(-1, new_owner);  // means there was no swap performed, -2 means error
                }
            }

            g_deallocate_array(Allocator, indices);

            destroy(bin);
        }

        UNITTEST_TEST(a_lot_more_alloc_free)
        {
            indexed_bin16_t* bin = make_bin(64);

            const u32 num_allocs = 50000;
            i32*      ptrs       = g_allocate_array<i32>(Allocator, num_allocs);
            i32*      indices    = g_allocate_array<i32>(Allocator, num_allocs);

            for (u32 i = 0; i < num_allocs; ++i)
            {
                indices[i] = (i32)i;
                ptrs[i]    = alloc(bin, (u16)i);
                CHECK_NOT_EQUAL(-1, ptrs[i]);
            }

            xor_random_t rnd(0x1234567890abcdef);

            // 'randomly' shuffle indices
            for (u32 i = 0; i < num_allocs; ++i)
            {
                u32 const a = rnd.rand32() % num_allocs;
                if (a != i)
                    nmem::swap(indices[a], indices[i]);
            }

            for (u32 i = 0; i < num_allocs; ++i)
            {
                i32 index     = indices[i];
                i32 new_owner = free(bin, ptrs[index]);
                if (new_owner != -1)
                {
                    ptrs[new_owner] = ptrs[index];
                }
                else
                {
                    CHECK_EQUAL(-1, new_owner);  // means there was no swap performed, -2 means error
                }
            }

            g_deallocate_array(Allocator, ptrs);
            g_deallocate_array(Allocator, indices);

            destroy(bin);
        }
    }
}
UNITTEST_SUITE_END

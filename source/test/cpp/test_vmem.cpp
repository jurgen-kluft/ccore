#include "ccore/c_target.h"
#include "ccore/c_arena.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(vmem)
{
    UNITTEST_FIXTURE(init)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(init)
        {
            arena_t* arena = narena::create(8 * cGB, 16 * cMB);  // reserve 8 GB, commit 16 MB

            // reserve 8 GB, if you don't call this, then when calling alloc, it will reserve
            // the default arena capacity of 1 GB.
            CHECK_NOT_NULL(arena);

            void* save = narena::current_address(arena);  // save a restore point
            CHECK_NOT_NULL(save);

            void* ptr1 = narena::alloc(arena, 1020);  // allocate 1 KB
            CHECK_NOT_NULL(ptr1);

            // Write to the allocated memory
            byte* data = (byte*)ptr1;
            for (int_t i = 0; i < 1020; i++)
            {
                data[i] = (byte)i;
            }

            narena::restore_address(arena, save);  // restore the size

            void* ptr2 = narena::alloc(arena, 1020);  // allocate 1 KB
            CHECK_NOT_NULL(ptr2);
            CHECK_EQUAL(ptr1, ptr2);  // should be the same pointer

            narena::destroy(arena);  // release the reserved memory
        }
    }
}
UNITTEST_SUITE_END

#include "ccore/c_target.h"
#include "ccore/c_vmem.h"

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
            arena_t* arena = gCreateArena(8 * cGB, 16 * cMB);  // reserve 8 GB, commit 16 MB

            // reserve 8 GB, if you don't call this, then when calling alloc, it will reserve
            // the default arena capacity of 1 GB.
            CHECK_NOT_NULL(arena);

            int_t save = arena->save_point();  // save a restore point
            CHECK_EQUAL(0, save);

            void* ptr1 = arena->alloc(1020);  // allocate 1 KB
            CHECK_NOT_NULL(ptr1);

            // Write to the allocated memory
            byte* data = (byte*)ptr1;
            for (int_t i = 0; i < 1020; i++)
            {
                data[i] = (byte)i;
            }

            arena->restore_point(save);  // restore the size

            void* ptr2 = arena->alloc(1020);  // allocate 1 KB
            CHECK_NOT_NULL(ptr2);
            CHECK_EQUAL(ptr1, ptr2);  // should be the same pointer

            arena->release();  // release the reserved memory
        }
    }
}
UNITTEST_SUITE_END

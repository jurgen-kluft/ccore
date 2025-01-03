#include "ccore/c_target.h"
#include "ccore/c_memory.h"
#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(memory)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP()
        {
        }

        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(nmem_memset)
        {
            unsigned char buffer1[] = {6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 88, 88};
            CHECK_EQUAL(6, buffer1[0]);
            CHECK_EQUAL(7, buffer1[1]);
            CHECK_EQUAL(8, buffer1[2]);
            CHECK_EQUAL(9, buffer1[3]);

            nmem::memset(buffer1, 0xCD, 4);
            CHECK_EQUAL((unsigned char)0xCD, (unsigned char)buffer1[0]);
            CHECK_EQUAL((unsigned char)0xCD, (unsigned char)buffer1[1]);
            CHECK_EQUAL((unsigned char)0xCD, (unsigned char)buffer1[2]);
            CHECK_EQUAL((unsigned char)0xCD, (unsigned char)buffer1[3]);
            CHECK_EQUAL(10, buffer1[4]);

            nmem::memset(&buffer1[5], 33, 4);
            CHECK_EQUAL(33, buffer1[5]);
            CHECK_EQUAL(33, buffer1[6]);
            CHECK_EQUAL(33, buffer1[7]);
            CHECK_EQUAL(33, buffer1[8]);
            CHECK_EQUAL(5, buffer1[9]);

            CHECK_EQUAL(88, buffer1[10]);
            CHECK_EQUAL(88, buffer1[11]);
        }

        UNITTEST_TEST(nmem_memclr)
        {
            char buffer1[] = {6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 88, 88};
            CHECK_EQUAL(6, buffer1[0]);
            CHECK_EQUAL(7, buffer1[1]);
            CHECK_EQUAL(8, buffer1[2]);
            CHECK_EQUAL(9, buffer1[3]);

            nmem::memclr(buffer1, 4);
            CHECK_EQUAL(0, buffer1[0]);
            CHECK_EQUAL(0, buffer1[1]);
            CHECK_EQUAL(0, buffer1[2]);
            CHECK_EQUAL(0, buffer1[3]);
            CHECK_EQUAL(10, buffer1[4]);

            nmem::memclr(&buffer1[5], 4);
            CHECK_EQUAL(0, buffer1[5]);
            CHECK_EQUAL(0, buffer1[6]);
            CHECK_EQUAL(0, buffer1[7]);
            CHECK_EQUAL(0, buffer1[8]);
            CHECK_EQUAL(5, buffer1[9]);

            CHECK_EQUAL(88, buffer1[10]);
            CHECK_EQUAL(88, buffer1[11]);
        }

        UNITTEST_TEST(nmem_memcpy)
        {
            char buffer1[] = {6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 88, 88};
            char buffer2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 99, 99};
            CHECK_EQUAL(6, buffer1[0]);
            CHECK_EQUAL(7, buffer1[1]);
            CHECK_EQUAL(8, buffer1[2]);
            CHECK_EQUAL(9, buffer1[3]);

            nmem::memcpy(buffer1, buffer2, 4);
            CHECK_EQUAL(1, buffer1[0]);
            CHECK_EQUAL(2, buffer1[1]);
            CHECK_EQUAL(3, buffer1[2]);
            CHECK_EQUAL(4, buffer1[3]);
            CHECK_EQUAL(10, buffer1[4]);

            nmem::memcpy(buffer2, buffer1, 10);
            for (ncore::s32 i = 1; i <= 10; ++i)
                CHECK_EQUAL(buffer1[i - 1], buffer2[i - 1]);

            CHECK_EQUAL(88, buffer1[10]);
            CHECK_EQUAL(88, buffer1[11]);
            CHECK_EQUAL(99, buffer2[10]);
            CHECK_EQUAL(99, buffer2[11]);
        }

        UNITTEST_TEST(nmem_memcmp)
        {
            char buffer1[] = {6, 7, 8, 9, 10, 1, 2, 3, 4, 5, 88, 88};
            char buffer2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 99, 99};
            CHECK_EQUAL(6, buffer1[0]);
            CHECK_EQUAL(7, buffer1[1]);
            CHECK_EQUAL(8, buffer1[2]);
            CHECK_EQUAL(9, buffer1[3]);

            CHECK_FALSE(nmem::memcmp(buffer1, buffer2, 10) == 0);
            CHECK_TRUE(nmem::memcmp(&buffer1[5], buffer2, 5) == 0);
        }

        UNITTEST_TEST(nmem_memswap)
        {
            char buffer1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 88, 88};
            char buffer2[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 99, 99};

            for (ncore::s32 i = 1; i <= 10; ++i)
                CHECK_EQUAL(i, buffer1[i - 1]);
            for (ncore::s32 i = 1; i <= 10; ++i)
                CHECK_EQUAL(i - 1, buffer2[i - 1]);

            nmem::memswap(buffer1, buffer2, 10);

            for (ncore::s32 i = 1; i <= 10; ++i)
                CHECK_EQUAL(i, buffer2[i - 1]);
            for (ncore::s32 i = 1; i <= 10; ++i)
                CHECK_EQUAL(i - 1, buffer1[i - 1]);

            CHECK_EQUAL(88, buffer1[10]);
            CHECK_EQUAL(88, buffer1[11]);
            CHECK_EQUAL(99, buffer2[10]);
            CHECK_EQUAL(99, buffer2[11]);
        }

        UNITTEST_TEST(nmem_memmove)
        {
            char buffer1[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 22, 22, 22, 22, 22, 88, 88};
            for (ncore::s32 i = 1; i <= 10; ++i)
            {
                CHECK_EQUAL(i, buffer1[i - 1]);
            }
            for (ncore::s32 i = 11; i <= 15; ++i)
            {
                CHECK_EQUAL(22, buffer1[i - 1]);
            }

            // Overlapping memory copy
            nmem::memmove(&buffer1[5], buffer1, 10);

            // Testing for 1,2,3,4,5,1,2,3,4,5
            for (ncore::s32 i = 1; i <= 5; ++i)
            {
                CHECK_EQUAL(i, buffer1[i - 1]);
            }
            for (ncore::s32 i = 6; i <= 10; ++i)
            {
                CHECK_EQUAL(i - 5, buffer1[i - 1]);
            }
            // Testing for 6,7,8,9,10 (was 22,22,22,22,22)
            for (ncore::s32 i = 11; i <= 15; ++i)
            {
                CHECK_EQUAL(i - 5, buffer1[i - 1]);
            }

            CHECK_EQUAL(88, buffer1[15]);
            CHECK_EQUAL(88, buffer1[16]);
        }

        UNITTEST_TEST(toKb)
        {
            CHECK_EQUAL(0, nmem::toKb(500));
            CHECK_EQUAL(1, nmem::toKb(1000));
            CHECK_EQUAL(1, nmem::toKb(1024));
            CHECK_EQUAL(1000, nmem::toKb(1000 * 1024));
        }

        UNITTEST_TEST(toMb)
        {
            CHECK_EQUAL(0, nmem::toMb(500 * 1024));
            CHECK_EQUAL(1, nmem::toMb(1000 * 1024));
            CHECK_EQUAL(1, nmem::toMb(1024 * 1024));
            CHECK_EQUAL(1000, nmem::toMb(1000 * 1024 * 1024));
        }

        UNITTEST_TEST(memcpy)
        {
            char buffer1[] = {6, 7, 8, 9, 10, 1, 2, 3, 4, 5};
            char buffer2[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};

            nmem::memcpy(buffer2, buffer1, 5);

            char buffer3[] = {6, 7, 8, 9, 10, 6, 7, 8, 9, 10};

            CHECK_TRUE(nmem::memcmp(buffer2, buffer3, 10) == 0);
        }
    }
}
UNITTEST_SUITE_END

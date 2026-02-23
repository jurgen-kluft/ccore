#include "ccore/c_allocator.h"
#include "ccore/c_array.h"
#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(array)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_ALLOCATOR;

        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        template <typename T>
        T* create_array(u32 capacity)
        {
            T* arr = g_allocate_array<T>(Allocator, capacity);
            for (u32 i = 0; i < capacity; ++i)
                arr[i] = (T)i;
            return arr;
        }

        template <typename T>
        void destroy_array(T * arr)
        {
            g_deallocate_array<T>(Allocator, arr);
        }

        UNITTEST_TEST(push_back)
        {
            i32  capacity = 5;
            i32* a        = create_array<i32>(capacity);

            i32 size  = 0;
            i32 value = 1;
            g_push_back(a, capacity, size, value);

            CHECK_EQUAL(1, size);
            CHECK_EQUAL(5, capacity);
            destroy_array<i32>(a);
        }

        UNITTEST_TEST(pop_back)
        {
            i32  capacity = 5;
            i32* a        = create_array<i32>(capacity);

            i32 size = 0;
            i32 value;
            CHECK_FALSE(g_pop_back(a, capacity, size, value));

            CHECK_EQUAL(0, size);
            CHECK_EQUAL(5, capacity);
            destroy_array<i32>(a);
        }

        UNITTEST_TEST(push_and_pop_back)
        {
            i32  capacity = 5;
            i32* a        = create_array<i32>(capacity);

            i32 size  = 0;
            i32 value = 1;
            g_push_back(a, capacity, size, value);
            CHECK_EQUAL(1, size);
            CHECK_EQUAL(5, capacity);

            CHECK_TRUE(g_pop_back(a, capacity, size, value));
            CHECK_EQUAL(1, value);

            CHECK_EQUAL(0, size);
            CHECK_EQUAL(5, capacity);
            destroy_array<i32>(a);
        }

        UNITTEST_TEST(operator_index)
        {
            i32  capacity = 10;
            i32* a        = create_array<i32>(capacity);

            i32 size = 0;

            for (i32 i = 0; i < capacity; ++i)
                g_push_back(a, capacity, size, i);

            i32& value = g_at(a, capacity, 0);
            value += 10;
            CHECK_EQUAL(10, a[0]);
            CHECK_EQUAL(6, a[6]);
            destroy_array<i32>(a);
        }

        UNITTEST_TEST(swap)
        {
            i32  capacity = 10;
            i32* a        = create_array<i32>(capacity);

            i32 size = 0;

            for (i32 i = 0; i < capacity; ++i)
                g_push_back(a, capacity, size, i);

            g_swap(a, capacity, 4, 7);
            CHECK_EQUAL(4, a[7]);
            CHECK_EQUAL(7, a[4]);

            destroy_array<i32>(a);
        }

        UNITTEST_TEST(remove)
        {
            i32  capacity = 10;
            i32* a        = create_array<i32>(capacity);

            i32 size = 0;

            for (i32 i = 0; i < capacity; ++i)
                g_push_back(a, capacity, size, i);

            CHECK_EQUAL(10, size);
            CHECK_EQUAL(10, capacity);

            g_remove(a, capacity, size, 4);

            CHECK_EQUAL(9, size);
            CHECK_EQUAL(10, capacity);

            CHECK_EQUAL(5, a[4]);
            CHECK_EQUAL(6, a[5]);
            CHECK_EQUAL(9, a[8]);

            g_remove(a, capacity, size, 8);
            CHECK_EQUAL(8, size);
            CHECK_EQUAL(10, capacity);
            CHECK_EQUAL(8, a[7]);

            destroy_array<i32>(a);
        }

        UNITTEST_TEST(swap_remove)
        {
            i32  capacity = 10;
            i32* a        = create_array<i32>(capacity);

            i32 size = 0;

            for (i32 i = 0; i < capacity; ++i)
                g_push_back(a, capacity, size, i);

            CHECK_EQUAL(10, size);
            CHECK_EQUAL(10, capacity);

            g_swap_remove(a, capacity, size, 4);

            CHECK_EQUAL(9, size);
            CHECK_EQUAL(10, capacity);

            CHECK_EQUAL(9, a[4]);
            CHECK_EQUAL(5, a[5]);
            CHECK_EQUAL(8, a[8]);

            g_swap_remove(a, capacity, size, 8);
            CHECK_EQUAL(8, size);
            CHECK_EQUAL(10, capacity);
            CHECK_EQUAL(7, a[7]);

            destroy_array<i32>(a);
        }

        UNITTEST_TEST(view)
        {
            i32  capacity = 10;
            i32* a        = create_array<i32>(capacity);

            i32 size = 0;

            for (i32 i = 0; i < capacity; ++i)
                g_push_back(a, capacity, size, i);

            CHECK_EQUAL(capacity, size);

            const i32* b = g_view(a, capacity, 4, 8);
            CHECK_EQUAL(4, b[0]);
            CHECK_EQUAL(5, b[1]);
            CHECK_EQUAL(6, b[2]);
            CHECK_EQUAL(7, b[3]);

            CHECK_EQUAL(4, g_at(a, capacity, 4));
            CHECK_EQUAL(5, g_at(a, capacity, 5));
            CHECK_EQUAL(6, g_at(a, capacity, 6));
            CHECK_EQUAL(7, g_at(a, capacity, 7));

            destroy_array<i32>(a);
        }
    }
}
UNITTEST_SUITE_END

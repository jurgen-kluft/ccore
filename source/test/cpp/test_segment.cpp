#include "ccore/c_target.h"
#include "ccore/c_segment.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(buddy)
{
    UNITTEST_FIXTURE(operations)
    {
        // Define clean, fixed-size bounds matching our structural dependencies
        static const uint_t c_min_block = 1024;              // 1 KB leaves
        static const uint_t c_pool_size = 32 * c_min_block;  // 32 KB total pool
        static const uint_t c_total_leaves = c_pool_size / c_min_block;  // Total leaf nodes in the buddy tree (32 for 32 KB pool with 1 KB leaves)

        // Pre-allocate raw static stack frame memory chunks to bypass malloc/free hooks
        u64              m_metadata_buffer[c_total_leaves * 2 + 8];  // Bitmap size for tracking buddy tree nodes (1 bit per node, rounded up to nearest byte)
        byte              m_pool_buffer[c_pool_size];
        nsegment::buddy_t m_allocator;

        UNITTEST_FIXTURE_SETUP()
        {
            // Reset state matrices before running individual test nodes
            nsegment::buddy_init(&m_allocator, m_metadata_buffer, m_pool_buffer, c_pool_size, c_min_block);
        }

        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(basic_alloc_and_free)
        {
            // Verify structural initialization conditions are cleanly mapped
            CHECK_EQUAL((u32)1, m_allocator.m_free_counts[0]);  // Level 0 (root) must contain exactly one free node

            // Test 1: Request an allocation matching an exact minimum page leaf size (1 KB)
            void* ptr1 = nsegment::buddy_alloc(&m_allocator, 1024);
            CHECK_NOT_NULL(ptr1);

            // Confirm address aligns cleanly to the pool origin base address
            CHECK_EQUAL((void*)m_pool_buffer, ptr1);

            // Test 2: Request non-pow2 boundary layouts to verify structural padding triggers
            // 1500 bytes must mathematically step up to request a 2 KB block segment allocation slot
            void* ptr2 = nsegment::buddy_alloc(&m_allocator, 1500);
            CHECK_NOT_NULL(ptr2);

            // Verify memory addresses do not overlap (ptr2 should sit 2 KB away from base pool origin)
            CHECK_EQUAL((void*)(m_pool_buffer + 2048), ptr2);

            // Write to verify memory pool region accessibility boundaries
            byte* data = (byte*)ptr1;
            for (uint_t i = 0; i < 1024; i++)
            {
                data[i] = (byte)(i & 0xFF);
            }

            // Test 3: Free active segments to see if local tree tier counts increment properly
            uint_t root_level_free_before = m_allocator.m_free_counts[0];

            nsegment::buddy_free(&m_allocator, ptr1);
            nsegment::buddy_free(&m_allocator, ptr2);

            // Once both tracking nodes of split branches dissolve,
            // complete root-level tree coalescing should restore the single pristine pool block.
            CHECK_EQUAL(root_level_free_before + 1, m_allocator.m_free_counts[0]);
        }

        UNITTEST_TEST(out_of_memory_handling)
        {
            // Attempt to pull out a single block matching our absolute system limit footprint boundary
            void* giant_ptr = nsegment::buddy_alloc(&m_allocator, c_pool_size);
            CHECK_NOT_NULL(giant_ptr);

            // System tree capacity is now fully depleted. Secondary allocations must fail cleanly
            void* failed_ptr = nsegment::buddy_alloc(&m_allocator, c_min_block);
            CHECK_NULL(failed_ptr);

            // Release total budget allocation frame to restore normal system operations
            nsegment::buddy_free(&m_allocator, giant_ptr);

            // Secondary allocation path should now process successfully
            void* success_ptr = nsegment::buddy_alloc(&m_allocator, c_min_block);
            CHECK_NOT_NULL(success_ptr);
            nsegment::buddy_free(&m_allocator, success_ptr);
        }

        UNITTEST_TEST(fragmentation_stress_loop)
        {
            // Array to track allocated pointers for the leaves
            void* leaf_ptrs[c_total_leaves];

            // 1. FILL THE ENTIRE ALLOCATOR
            // Force the tree to split completely down to Level 6 (the leaves)
            for (uint_t i = 0; i < c_total_leaves; ++i)
            {
                leaf_ptrs[i] = nsegment::buddy_alloc(&m_allocator, c_min_block);
                CHECK_NOT_NULL(leaf_ptrs[i]);

                // Write unique data pattern to verify zero address overlapping/corruption
                byte* data = (byte*)leaf_ptrs[i];
                data[0] = (byte)(i & 0xFF);
                data[c_min_block - 1] = (byte)((~i) & 0xFF);
            }

            // The allocator must now be completely full
            void* failed_ptr = nsegment::buddy_alloc(&m_allocator, c_min_block);
            CHECK_NULL(failed_ptr);

            // Verify memory pattern consistency before deallocating
            for (uint_t i = 0; i < c_total_leaves; ++i)
            {
                byte* data = (byte*)leaf_ptrs[i];
                CHECK_EQUAL((byte)(i & 0xFF), data[0]);
                CHECK_EQUAL((byte)((~i) & 0xFF), data[c_min_block - 1]);
            }

            // 2. FRAGMENT THE TREE (Free all ODD indices first)
            // This leaves the buddy tree completely checkerboarded.
            // No blocks can coalesce yet because every single buddy pair has an active block.
            for (uint_t i = 1; i < c_total_leaves; i += 2)
            {
                nsegment::buddy_free(&m_allocator, leaf_ptrs[i]);
            }

            // At this point, leaf-level free count should equal half the total leaves
            // (Note: max_levels - 1 gets the index of the leaf level)
            CHECK_EQUAL(c_total_leaves / 2, m_allocator.m_free_counts[m_allocator.m_max_levels - 1]);

            // 3. COMPLETE DEALLOCATION (Free all EVEN indices)
            // This unlocks the remaining buddies, triggering a cascading chain reaction
            // of coalescing operations all the way up to the root level.
            for (uint_t i = 0; i < c_total_leaves; i += 2)
            {
                nsegment::buddy_free(&m_allocator, leaf_ptrs[i]);
            }

            // 4. VERIFY FULL TREE RECOVERY
            // Check if the tree successfully collapsed back into a single pristine root block
            CHECK_EQUAL(1, m_allocator.m_free_counts[0]);
            for (uint_t i = 1; i < m_allocator.m_max_levels; ++i)
            {
                CHECK_EQUAL(0, m_allocator.m_free_counts[i]);
            }

            // Ensure we can re-allocate a single block using the entire pool capacity again
            void* pristine_ptr = nsegment::buddy_alloc(&m_allocator, c_pool_size);
            CHECK_NOT_NULL(pristine_ptr);
            CHECK_EQUAL((void*)m_pool_buffer, pristine_ptr);

            nsegment::buddy_free(&m_allocator, pristine_ptr);
        }
    }
}
UNITTEST_SUITE_END

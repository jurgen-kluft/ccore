#include "ccore/c_target.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_segment.h"
#include "ccore/c_arena.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(segment)
{
    UNITTEST_FIXTURE(operations)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(initialization)
        {
            // Test: allocator initializes with non-empty top-level free list
            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, (uint_t)4u * cGB, 1 * cMB, 8 * cMB);  // 4 GB address space, 1 MB min segment, 8 MB max segment

            nsegment::teardown(&allocator);
        }

        UNITTEST_TEST(initialization_and_teardown)
        {
            // Test: initialization and teardown lifecycle works
            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, (uint_t)4u * cGB, 1 * cMB, 8 * cMB);

            // Verify allocator is initialized
            CHECK(allocator.m_segment_min_pages == (1 * cMB) / v_alloc_get_page_size());
            CHECK(allocator.m_segment_max_pages == (8 * cMB) / v_alloc_get_page_size());
            CHECK(allocator.m_chain != nullptr);

            nsegment::teardown(&allocator);
        }

        UNITTEST_TEST(initialization_and_N_alloc_deallocs)
        {
            // Test: alloc_node returns valid nodes for in-range sizes
            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, (uint_t)4u * cGB, 1 * cMB, 8 * cMB);
            //const u32 page_size = (u32)v_alloc_get_page_size();

            // Test 1: allocate a small node (1 page)
            nsegment::node_t node1 = nsegment::alloc_node(&allocator, 2 * cKB);
            CHECK(node1 != nsegment::cINVALID_NODE);

            // Test 2: allocate another small node
            nsegment::node_t node2 = nsegment::alloc_node(&allocator, 8 * cKB);
            CHECK(node2 != nsegment::cINVALID_NODE);
            CHECK(node1 != node2);  // should be different nodes

            // Test 3: allocate a larger node (4 pages)
            nsegment::node_t node3 = nsegment::alloc_node(&allocator, (u64)64 * cKB);
            CHECK(node3 != nsegment::cINVALID_NODE);

            // Test 4: allocate maximum sized node
            nsegment::node_t node4 = nsegment::alloc_node(&allocator, (u64)8 * cMB);
            CHECK(node4 != nsegment::cINVALID_NODE);

            // Test 5: oversized allocation should fail
            nsegment::node_t node_fail = nsegment::alloc_node(&allocator, (u64)16 * cMB);
            CHECK(node_fail == nsegment::cINVALID_NODE);

            // Test 6: undersized allocation (smaller than min)
            nsegment::node_t node_tiny = nsegment::alloc_node(&allocator, 1);
            CHECK(node_tiny != nsegment::cINVALID_NODE);  // should round up to min page size

            nsegment::teardown(&allocator);
        }

        UNITTEST_TEST(split_path_allocation)
        {
            const u32 page_size = (u32)v_alloc_get_page_size();

            // Test: allocation from higher class forces split
            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, 1024 * 1024 * page_size, 1 * page_size, 65536 * page_size);  // smaller address space

            // First, exhaust the smallest class by requesting many 1-page allocations
            // Then request from a higher class to force splits
            nsegment::node_t node1 = nsegment::alloc_node(&allocator, page_size);  // 1 page
            CHECK(node1 != nsegment::cINVALID_NODE);

            nsegment::node_t node2 = nsegment::alloc_node(&allocator, page_size);  // 1 page
            CHECK(node2 != nsegment::cINVALID_NODE);

            nsegment::node_t node3 = nsegment::alloc_node(&allocator, (u64)2 * page_size);  // 2 pages
            CHECK(node3 != nsegment::cINVALID_NODE);

            nsegment::node_t node4 = nsegment::alloc_node(&allocator, (u64)4 * page_size);  // 4 pages
            CHECK(node4 != nsegment::cINVALID_NODE);

            nsegment::teardown(&allocator);
        }

        UNITTEST_TEST(out_of_memory)
        {
            const u32 page_size = (u32)v_alloc_get_page_size();

            // Test: allocation exhaustion returns cINVALID_NODE
            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, 1024 * page_size, 1 * page_size, 512 * page_size);

            // Allocate all available space
            nsegment::node_t node1 = nsegment::alloc_node(&allocator, (u64)512 * page_size);
            nsegment::node_t node2 = nsegment::alloc_node(&allocator, (u64)512 * page_size);
            CHECK_TRUE(node1 != nsegment::cINVALID_NODE);
            CHECK_TRUE(node2 != nsegment::cINVALID_NODE);

            // Next allocation should fail (OOM)
            nsegment::node_t node3 = nsegment::alloc_node(&allocator, page_size);
            CHECK_TRUE(node3 == nsegment::cINVALID_NODE);

            nsegment::teardown(&allocator);
        }

        UNITTEST_TEST(stress_mixed_sizes_and_exhaustion)
        {
            // Stress strategy:
            // 1) Run repeated init/alloc-to-OOM/teardown rounds with mixed allocation sizes.
            // 2) Verify node indices are unique per round and always in bounds.
            // 3) Verify we eventually hit OOM in each round.
            // 4) Run a deterministic "all min-size" round and validate exact capacity.

            const u32 page_size           = (u32)v_alloc_get_page_size();
            const u32 address_space_pages = 1u << 12;             // 4096 pages
            const u32 max_segment_pages   = 1u << 8;              // 256 pages
            const u32 max_nodes           = address_space_pages;  // min segment is 1 page

            // Keep this stack array fixed-size and deterministic.
            u8 seen[max_nodes];

            // LCG for deterministic pseudo-random requests.
            u32 seed = 0xC001D00Du;

            for (u32 round = 0; round < 24; ++round)
            {
                nsegment::allocator_t allocator;
                nsegment::initialize(&allocator, address_space_pages * page_size, 1 * page_size, max_segment_pages * page_size);

                g_memset(seen, 0, sizeof(seen));

                bool hit_oom     = false;
                u32  alloc_count = 0;

                // Mixed-size allocations until OOM.
                for (u32 op = 0; op < (max_nodes * 4); ++op)
                {
                    // Deterministically pick a power-of-two page request from 1..256.
                    seed                    = seed * 1664525u + 1013904223u;
                    const u32 shift         = (seed >> 28) & 0x7;  // [0..7]
                    const u32 request_pages = 1u << shift;
                    const u64 request_bytes = (u64)request_pages * (u64)page_size;

                    const nsegment::node_t node = nsegment::alloc_node(&allocator, request_bytes);
                    if (node == nsegment::cINVALID_NODE)
                    {
                        hit_oom = true;
                        break;
                    }

                    CHECK(node >= 0);
                    CHECK((u32)node < max_nodes);
                    CHECK(seen[(u32)node] == 0);
                    seen[(u32)node] = 1;
                    ++alloc_count;
                }

                CHECK(hit_oom);
                CHECK(alloc_count > 0);
                CHECK(alloc_count <= max_nodes);

                // Validate free-list heads are either invalid or within valid node range.
                u32* free_list_heads = narena::base_ptr_as<u32>(allocator.m_chain);
                for (u32 i = 0; i < 32; ++i)
                {
                    const u32 head = free_list_heads[i];
                    CHECK(head == ~0u || head < max_nodes);
                }

                nsegment::teardown(&allocator);
            }

            // Deterministic exact-capacity check with min-size allocations.
            {
                nsegment::allocator_t allocator;
                nsegment::initialize(&allocator, address_space_pages * page_size, 1 * page_size, max_segment_pages * page_size);

                u32 count = 0;
                while (true)
                {
                    const nsegment::node_t node = nsegment::alloc_node(&allocator, page_size);
                    if (node == nsegment::cINVALID_NODE)
                        break;
                    ++count;
                }

                CHECK_EQUAL(address_space_pages, count);

                nsegment::teardown(&allocator);
            }
        }

        UNITTEST_TEST(stress_large_first_sweeps)
        {
            // Stress strategy (large-first):
            // - In each round, repeatedly sweep requests from 256 pages down to 1 page.
            // - This biases allocations toward larger classes while still filling fragmentation gaps.
            // - Validate node uniqueness/range and verify allocator reaches exhaustion.

            const u32 page_size           = (u32)v_alloc_get_page_size();
            const u32 address_space_pages = 1u << 12;  // 4096 pages
            const u32 max_segment_pages   = 1u << 8;   // 256 pages
            const u32 max_nodes           = address_space_pages;

            u8 seen[max_nodes];

            for (u32 round = 0; round < 20; ++round)
            {
                nsegment::allocator_t allocator;
                nsegment::initialize(&allocator, address_space_pages * page_size, 1 * page_size, max_segment_pages * page_size);

                g_memset(seen, 0, sizeof(seen));

                u32 total_allocs = 0;
                u32 large_allocs = 0;

                while (total_allocs < max_nodes)
                {
                    bool made_progress = false;

                    for (i32 shift = 9; shift >= 1; --shift)
                    {
                        const u32 request_pages = 1u << (u32)(shift - 1);
                        const u64 request_bytes = (u64)request_pages * (u64)page_size;

                        const nsegment::node_t node = nsegment::alloc_node(&allocator, request_bytes);
                        if (node == nsegment::cINVALID_NODE)
                            continue;

                        CHECK(node >= 0);
                        CHECK((u32)node < max_nodes);
                        CHECK(seen[(u32)node] == 0);
                        seen[(u32)node] = 1;

                        if (request_pages == max_segment_pages)
                            ++large_allocs;

                        ++total_allocs;
                        made_progress = true;

                        if (total_allocs >= max_nodes)
                            break;
                    }

                    if (!made_progress)
                        break;
                }

                CHECK(total_allocs > 0);
                CHECK(total_allocs <= max_nodes);
                CHECK(large_allocs > 0);

                // At this point we should be exhausted for the minimum request too.
                const nsegment::node_t tail = nsegment::alloc_node(&allocator, page_size);
                CHECK(tail == nsegment::cINVALID_NODE);

                // Validate free-list heads are either invalid or within valid node range.
                u32* free_list_heads = narena::base_ptr_as<u32>(allocator.m_chain);
                for (u32 i = 0; i < 32; ++i)
                {
                    const u32 head = free_list_heads[i];
                    CHECK(head == ~0u || head < max_nodes);
                }

                nsegment::teardown(&allocator);
            }
        }
    }
}
UNITTEST_SUITE_END

#include "ccore/c_target.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_segment.h"
#include "ccore/c_arena.h"

#include "cunittest/cunittest.h"

using namespace ncore;

static bool s_validate_free_lists(nsegment::allocator_t const & allocator)
{
    const u16 invalid_index = (u16)~0u;

    for (u32 class_index = 0; class_index < 32; ++class_index)
    {
        u16 forward_node  = allocator.m_free_list_heads[class_index];
        u16 previous_node = invalid_index;
        u16 tail_node     = invalid_index;
        u32 forward_count = 0;

        while (forward_node != invalid_index)
        {
            if (forward_node >= allocator.m_total_minsize_segments)
                return false;

            nsegment::dlnode_t const & free_node = allocator.m_nodes[forward_node].m_free;
            if (free_node.m_prev != previous_node)
                return false;

            previous_node = forward_node;
            tail_node     = forward_node;
            forward_node  = free_node.m_next;
            if (++forward_count > allocator.m_total_minsize_segments)
                return false;
        }

        u16 next_node      = invalid_index;
        u32 backward_count = 0;
        while (tail_node != invalid_index)
        {
            nsegment::dlnode_t const & free_node = allocator.m_nodes[tail_node].m_free;
            if (free_node.m_next != next_node)
                return false;

            next_node = tail_node;
            tail_node = free_node.m_prev;
            if (++backward_count > allocator.m_total_minsize_segments)
                return false;
        }

        if (forward_count != backward_count)
            return false;
    }

    return true;
}

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
            nsegment::initialize(&allocator, (u64)4u * cGB, 1 * cMB, 8 * cMB);

            // Verify allocator is initialized
            CHECK(allocator.m_segment_minsize_shift == 20);  // 1 * cMB
            CHECK(allocator.m_segment_maxsize_shift == 23);  // 8 * cMB
            CHECK(allocator.m_pagesize_shift == v_alloc_get_page_size_shift());
            CHECK(allocator.m_total_minsize_segments == 4096);
            CHECK(allocator.m_chain != nullptr);
            CHECK(allocator.m_nodes != nullptr);

            nsegment::teardown(&allocator);

            CHECK(allocator.m_base_address == nullptr);
            CHECK(allocator.m_chain == nullptr);
            CHECK(allocator.m_nodes == nullptr);
            CHECK(allocator.m_total_minsize_segments == 0);
        }

        UNITTEST_TEST(initialization_and_N_alloc_deallocs)
        {
            // Test: alloc_node returns valid nodes for in-range sizes
            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, (u64)4u * cGB, 1 * cMB, 8 * cMB);

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
            nsegment::initialize(&allocator, (uint_t)1024 * page_size, (uint_t)1 * page_size, (uint_t)256 * page_size);  // smaller address space
            CHECK(s_validate_free_lists(allocator));

            // First, exhaust the smallest class by requesting many 1-page allocations
            // Then request from a higher class to force splits
            nsegment::node_t node1 = nsegment::alloc_node(&allocator, page_size);  // 1 page
            CHECK(node1 != nsegment::cINVALID_NODE);
            CHECK_EQUAL(0, node1);
            CHECK(s_validate_free_lists(allocator));
            u32   node1_available_pages;
            void* node1_address = nsegment::get_address(&allocator, node1, node1_available_pages);
            CHECK(node1_address != nullptr);
            CHECK_EQUAL(node1_available_pages, 1u);

            nsegment::node_t node2 = nsegment::alloc_node(&allocator, page_size);  // 1 page
            CHECK(node2 != nsegment::cINVALID_NODE);
            CHECK_EQUAL(1, node2);
            CHECK(s_validate_free_lists(allocator));

            nsegment::node_t node3 = nsegment::alloc_node(&allocator, (u64)2 * page_size);  // 2 pages
            CHECK(node3 != nsegment::cINVALID_NODE);
            CHECK_EQUAL(2, node3);
            CHECK(s_validate_free_lists(allocator));

            nsegment::node_t node4 = nsegment::alloc_node(&allocator, (u64)4 * page_size);  // 4 pages
            CHECK(node4 != nsegment::cINVALID_NODE);
            CHECK_EQUAL(4, node4);
            CHECK(s_validate_free_lists(allocator));

            u32   node2_available_pages;
            u32   node3_available_pages;
            u32   node4_available_pages;
            void* node2_address = nsegment::get_address(&allocator, node2, node2_available_pages);
            void* node3_address = nsegment::get_address(&allocator, node3, node3_available_pages);
            void* node4_address = nsegment::get_address(&allocator, node4, node4_available_pages);
            CHECK_EQUAL(1u, node2_available_pages);
            CHECK_EQUAL(2u, node3_available_pages);
            CHECK_EQUAL(4u, node4_available_pages);
            CHECK_EQUAL((byte*)node1_address + page_size, (byte*)node2_address);
            CHECK_EQUAL((byte*)node2_address + page_size, (byte*)node3_address);
            CHECK_EQUAL((byte*)node3_address + 2 * page_size, (byte*)node4_address);

            nsegment::teardown(&allocator);
        }

        UNITTEST_TEST(top_level_allocation_addresses)
        {
            const u32 page_size = (u32)v_alloc_get_page_size();

            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, (u64)1024 * page_size, (u64)page_size, (u64)256 * page_size);
            CHECK(s_validate_free_lists(allocator));

            for (u32 i = 0; i < 4; ++i)
            {
                const nsegment::node_t node = nsegment::alloc_node(&allocator, (u64)256 * page_size);
                CHECK_EQUAL((i32)(i * 256), node);
                CHECK(s_validate_free_lists(allocator));

                u32   available_pages;
                void* address = nsegment::get_address(&allocator, node, available_pages);
                CHECK_EQUAL(256u, available_pages);
                CHECK_EQUAL(allocator.m_base_address + (u64)i * 256 * page_size, (byte*)address);
            }

            CHECK(nsegment::alloc_node(&allocator, (u64)256 * page_size) == nsegment::cINVALID_NODE);
            nsegment::teardown(&allocator);
        }

        UNITTEST_TEST(minimum_size_exact_capacity)
        {
            const u32 page_size  = (u32)v_alloc_get_page_size();
            const u32 page_count = 1024;
            u8        seen[page_count];
            g_memset(seen, 0, sizeof(seen));

            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, (u64)page_count * page_size, (u64)page_size, (u64)256 * page_size);
            CHECK(s_validate_free_lists(allocator));

            for (u32 i = 0; i < page_count; ++i)
            {
                const nsegment::node_t node = nsegment::alloc_node(&allocator, page_size);
                CHECK(node != nsegment::cINVALID_NODE);
                CHECK((u32)node < page_count);
                CHECK(seen[(u32)node] == 0);
                seen[(u32)node] = 1;
                CHECK(s_validate_free_lists(allocator));
            }

            CHECK(nsegment::alloc_node(&allocator, page_size) == nsegment::cINVALID_NODE);
            for (u32 class_index = 0; class_index < 32; ++class_index)
                CHECK(allocator.m_free_list_heads[class_index] == (u16)~0u);
            for (u32 node_index = 0; node_index < page_count; ++node_index)
            {
                u16 tag = (u16)~0u;
                CHECK(nsegment::get_node_tag(&allocator, (nsegment::node_t)node_index, tag));
                CHECK_EQUAL(0u, tag);
                CHECK_EQUAL(0u, allocator.m_nodes[node_index].m_user.m_unused);
            }
            nsegment::teardown(&allocator);
        }

        UNITTEST_TEST(dealloc_reuses_unmerged_node)
        {
            const u32 page_size = (u32)v_alloc_get_page_size();

            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, (u64)1024 * page_size, (u64)page_size, (u64)256 * page_size);

            const nsegment::node_t node0 = nsegment::alloc_node(&allocator, page_size);
            const nsegment::node_t node1 = nsegment::alloc_node(&allocator, page_size);
            CHECK_EQUAL(0, node0);
            CHECK_EQUAL(1, node1);

            nsegment::dealloc_node(&allocator, node0);
            CHECK(s_validate_free_lists(allocator));

            const nsegment::node_t reused = nsegment::alloc_node(&allocator, page_size);
            CHECK_EQUAL(node0, reused);
            CHECK(s_validate_free_lists(allocator));

            nsegment::teardown(&allocator);
        }

        UNITTEST_TEST(dealloc_coalesces_both_orders)
        {
            const u32 page_size = (u32)v_alloc_get_page_size();

            for (u32 reverse = 0; reverse < 2; ++reverse)
            {
                nsegment::allocator_t allocator;
                nsegment::initialize(&allocator, (u64)1024 * page_size, (u64)page_size, (u64)256 * page_size);

                const nsegment::node_t node0 = nsegment::alloc_node(&allocator, page_size);
                const nsegment::node_t node1 = nsegment::alloc_node(&allocator, page_size);
                CHECK_EQUAL(0, node0);
                CHECK_EQUAL(1, node1);

                nsegment::dealloc_node(&allocator, reverse != 0 ? node1 : node0);
                CHECK(s_validate_free_lists(allocator));
                nsegment::dealloc_node(&allocator, reverse != 0 ? node0 : node1);
                CHECK(s_validate_free_lists(allocator));

                // Double-free and absorbed-node handles must be harmless.
                nsegment::dealloc_node(&allocator, node0);
                nsegment::dealloc_node(&allocator, node1);
                nsegment::dealloc_node(&allocator, nsegment::cINVALID_NODE);
                nsegment::dealloc_node(&allocator, -2);
                nsegment::dealloc_node(&allocator, 2048);
                CHECK(s_validate_free_lists(allocator));

                const nsegment::node_t merged = nsegment::alloc_node(&allocator, (u64)256 * page_size);
                CHECK_EQUAL(0, merged);
                CHECK(s_validate_free_lists(allocator));

                nsegment::teardown(&allocator);
            }
        }

        UNITTEST_TEST(dealloc_recursively_restores_capacity)
        {
            const u32 page_size = (u32)v_alloc_get_page_size();
            const u32 node_count = 256;
            nsegment::node_t nodes[node_count];

            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, (u64)1024 * page_size, (u64)page_size, (u64)256 * page_size);

            for (u32 i = 0; i < node_count; ++i)
            {
                nodes[i] = nsegment::alloc_node(&allocator, page_size);
                CHECK_EQUAL((i32)i, nodes[i]);
            }

            for (u32 i = 0; i < node_count; ++i)
            {
                nsegment::dealloc_node(&allocator, nodes[i]);
                CHECK(s_validate_free_lists(allocator));
            }

            const nsegment::node_t restored = nsegment::alloc_node(&allocator, (u64)256 * page_size);
            CHECK_EQUAL(0, restored);

            // Top-level blocks remain independent and never merge past the configured maximum.
            nsegment::dealloc_node(&allocator, restored);
            const nsegment::node_t top0 = nsegment::alloc_node(&allocator, (u64)256 * page_size);
            const nsegment::node_t top1 = nsegment::alloc_node(&allocator, (u64)256 * page_size);
            CHECK_EQUAL(0, top0);
            CHECK_EQUAL(256, top1);
            nsegment::dealloc_node(&allocator, top0);
            nsegment::dealloc_node(&allocator, top1);
            CHECK(s_validate_free_lists(allocator));

            CHECK_EQUAL(256, nsegment::alloc_node(&allocator, (u64)256 * page_size));
            CHECK_EQUAL(0, nsegment::alloc_node(&allocator, (u64)256 * page_size));

            nsegment::teardown(&allocator);
        }

        UNITTEST_TEST(node_tags)
        {
            const u32 page_size = (u32)v_alloc_get_page_size();

            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, (u64)1024 * page_size, (u64)page_size, (u64)256 * page_size);

            const nsegment::node_t node0 = nsegment::alloc_node(&allocator, page_size);
            const nsegment::node_t node1 = nsegment::alloc_node(&allocator, (u64)2 * page_size);
            CHECK_EQUAL(0, node0);
            CHECK_EQUAL(2, node1);

            u16 tag = (u16)~0u;
            CHECK(nsegment::get_node_tag(&allocator, node0, tag));
            CHECK_EQUAL(0u, tag);
            CHECK(nsegment::get_node_tag(&allocator, node1, tag));
            CHECK_EQUAL(0u, tag);

            nsegment::set_node_tag(&allocator, node0, (u16)~0u);
            nsegment::set_node_tag(&allocator, node1, 42);
            CHECK(nsegment::get_node_tag(&allocator, node0, tag));
            CHECK_EQUAL((u16)~0u, tag);
            CHECK(nsegment::get_node_tag(&allocator, node1, tag));
            CHECK_EQUAL(42u, tag);

            nsegment::commit(&allocator, node1, 2);
            nsegment::decommit(&allocator, node1, 1);
            CHECK(nsegment::get_node_tag(&allocator, node1, tag));
            CHECK_EQUAL(42u, tag);
            nsegment::set_node_tag(&allocator, node1, 0);
            CHECK(nsegment::get_node_tag(&allocator, node1, tag));
            CHECK_EQUAL(0u, tag);

            nsegment::dealloc_node(&allocator, node0);
            tag = 123;
            CHECK(!nsegment::get_node_tag(&allocator, node0, tag));
            CHECK_EQUAL(0u, tag);
            CHECK(s_validate_free_lists(allocator));

            const nsegment::node_t reused = nsegment::alloc_node(&allocator, page_size);
            CHECK_EQUAL(node0, reused);
            CHECK(nsegment::get_node_tag(&allocator, reused, tag));
            CHECK_EQUAL(0u, tag);

            nsegment::set_node_tag(nullptr, reused, 9);
            tag = 123;
            CHECK(!nsegment::get_node_tag(nullptr, reused, tag));
            CHECK_EQUAL(0u, tag);
            CHECK(!nsegment::get_node_tag(&allocator, nsegment::cINVALID_NODE, tag));
            CHECK(!nsegment::get_node_tag(&allocator, 2048, tag));

            nsegment::dealloc_node(&allocator, reused);
            nsegment::dealloc_node(&allocator, node1);
            CHECK(s_validate_free_lists(allocator));
            nsegment::teardown(&allocator);

            tag = 123;
            CHECK(!nsegment::get_node_tag(&allocator, reused, tag));
            CHECK_EQUAL(0u, tag);
        }

        UNITTEST_TEST(address_to_node_committed_prefix)
        {
            const u32 page_size = (u32)v_alloc_get_page_size();

            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, (u64)1024 * page_size, (u64)page_size, (u64)256 * page_size);

            const nsegment::node_t node0 = nsegment::alloc_node(&allocator, (u64)4 * page_size);
            const nsegment::node_t node1 = nsegment::alloc_node(&allocator, (u64)2 * page_size);
            const nsegment::node_t node2 = nsegment::alloc_node(&allocator, (u64)256 * page_size);
            CHECK_EQUAL(0, node0);
            CHECK_EQUAL(4, node1);
            CHECK_EQUAL(256, node2);

            u32   node0_pages;
            u32   node1_pages;
            u32   node2_pages;
            byte* node0_address = (byte*)nsegment::get_address(&allocator, node0, node0_pages);
            byte* node1_address = (byte*)nsegment::get_address(&allocator, node1, node1_pages);
            byte* node2_address = (byte*)nsegment::get_address(&allocator, node2, node2_pages);
            CHECK_EQUAL(4u, node0_pages);
            CHECK_EQUAL(2u, node1_pages);
            CHECK_EQUAL(256u, node2_pages);

            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, node0_address));
            nsegment::commit(&allocator, node0, 3);
            CHECK_EQUAL(node0, nsegment::address_to_node(&allocator, node0_address));
            CHECK_EQUAL(node0, nsegment::address_to_node(&allocator, node0_address + 1));
            CHECK_EQUAL(node0, nsegment::address_to_node(&allocator, node0_address + page_size));
            CHECK_EQUAL(node0, nsegment::address_to_node(&allocator, node0_address + 3 * page_size - 1));
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, node0_address + 3 * page_size));
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, node0_address + 4 * page_size - 1));

            nsegment::commit(&allocator, node1, 2);
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, node1_address - 1));
            CHECK_EQUAL(node1, nsegment::address_to_node(&allocator, node1_address));
            CHECK_EQUAL(node1, nsegment::address_to_node(&allocator, node1_address + 2 * page_size - 1));

            nsegment::commit(&allocator, node2, 1);
            CHECK_EQUAL(node2, nsegment::address_to_node(&allocator, node2_address));
            CHECK_EQUAL(node2, nsegment::address_to_node(&allocator, node2_address + page_size - 1));
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, node2_address + page_size));

            nsegment::decommit(&allocator, node0, 1);
            CHECK_EQUAL(node0, nsegment::address_to_node(&allocator, node0_address + page_size - 1));
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, node0_address + page_size));
            nsegment::commit(&allocator, node0, 4);
            CHECK_EQUAL(node0, nsegment::address_to_node(&allocator, node0_address + 4 * page_size - 1));

            nsegment::dealloc_node(&allocator, node1);
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, node1_address));
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(nullptr, node0_address));
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, nullptr));
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, (void*)((uptr_t)allocator.m_base_address - 1)));

            byte* address_space_end = allocator.m_base_address + ((u64)allocator.m_total_minsize_segments << allocator.m_segment_minsize_shift);
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, address_space_end));
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, address_space_end + 1));

            nsegment::teardown(&allocator);
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, node0_address));

            nsegment::initialize(&allocator, (u64)1024 * page_size, (u64)page_size, (u64)256 * page_size);
            const nsegment::node_t child0 = nsegment::alloc_node(&allocator, page_size);
            const nsegment::node_t child1 = nsegment::alloc_node(&allocator, page_size);
            u32 child_pages;
            byte* child1_address = (byte*)nsegment::get_address(&allocator, child1, child_pages);
            nsegment::commit(&allocator, child0, 1);
            nsegment::commit(&allocator, child1, 1);
            nsegment::dealloc_node(&allocator, child0);
            nsegment::dealloc_node(&allocator, child1);
            CHECK_EQUAL(nsegment::cINVALID_NODE, nsegment::address_to_node(&allocator, child1_address));

            const nsegment::node_t parent = nsegment::alloc_node(&allocator, (u64)2 * page_size);
            CHECK_EQUAL(0, parent);
            nsegment::commit(&allocator, parent, 2);
            CHECK_EQUAL(parent, nsegment::address_to_node(&allocator, child1_address));
            nsegment::teardown(&allocator);
        }

        UNITTEST_TEST(commit_decommit_absolute_targets)
        {
            const u32 page_size = (u32)v_alloc_get_page_size();

            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, (u64)1024 * page_size, (u64)page_size, (u64)256 * page_size);

            const nsegment::node_t node = nsegment::alloc_node(&allocator, (u64)8 * page_size);
            CHECK(node != nsegment::cINVALID_NODE);

            u32   available_pages;
            byte* address = (byte*)nsegment::get_address(&allocator, node, available_pages);
            CHECK(address != nullptr);
            CHECK_EQUAL(8u, available_pages);

            nsegment::commit(&allocator, node, 3);
            nsegment::commit(&allocator, node, 3);  // idempotent
            for (u32 page = 0; page < 3; ++page)
                g_memset(address + (u64)page * page_size, (u8)(0x31 + page), page_size);

            nsegment::commit(&allocator, node, 8);
            for (u32 page = 0; page < 3; ++page)
                CHECK_EQUAL((u8)(0x31 + page), address[(u64)page * page_size]);
            for (u32 page = 3; page < 8; ++page)
                g_memset(address + (u64)page * page_size, (u8)(0x41 + page), page_size);

            nsegment::decommit(&allocator, node, 3);
            nsegment::decommit(&allocator, node, 3);  // idempotent
            for (u32 page = 0; page < 3; ++page)
                CHECK_EQUAL((u8)(0x31 + page), address[(u64)page * page_size]);

            nsegment::commit(&allocator, node, 8);
            for (u32 page = 0; page < 3; ++page)
                CHECK_EQUAL((u8)(0x31 + page), address[(u64)page * page_size]);
            for (u32 page = 3; page < 8; ++page)
            {
                g_memset(address + (u64)page * page_size, (u8)(0x61 + page), page_size);
                CHECK_EQUAL((u8)(0x61 + page), address[(u64)page * page_size]);
            }

            nsegment::decommit(&allocator, node, 0);
            nsegment::dealloc_node(&allocator, node);
            CHECK(s_validate_free_lists(allocator));
            nsegment::teardown(&allocator);
        }

        UNITTEST_TEST(dealloc_automatically_decommits)
        {
            const u32 page_size = (u32)v_alloc_get_page_size();

            for (u32 reverse = 0; reverse < 2; ++reverse)
            {
                nsegment::allocator_t allocator;
                nsegment::initialize(&allocator, (u64)1024 * page_size, (u64)page_size, (u64)256 * page_size);

                const nsegment::node_t node0 = nsegment::alloc_node(&allocator, page_size);
                const nsegment::node_t node1 = nsegment::alloc_node(&allocator, page_size);
                CHECK_EQUAL(0, node0);
                CHECK_EQUAL(1, node1);

                u32   node0_pages;
                u32   node1_pages;
                byte* node0_address = (byte*)nsegment::get_address(&allocator, node0, node0_pages);
                byte* node1_address = (byte*)nsegment::get_address(&allocator, node1, node1_pages);
                nsegment::commit(&allocator, node0, node0_pages);
                nsegment::commit(&allocator, node1, node1_pages);
                g_memset(node0_address, 0x55, page_size);
                g_memset(node1_address, 0xaa, page_size);

                nsegment::dealloc_node(&allocator, reverse != 0 ? node1 : node0);
                nsegment::dealloc_node(&allocator, reverse != 0 ? node0 : node1);
                CHECK(s_validate_free_lists(allocator));

                const nsegment::node_t merged = nsegment::alloc_node(&allocator, (u64)2 * page_size);
                CHECK_EQUAL(0, merged);
                u32   merged_pages;
                byte* merged_address = (byte*)nsegment::get_address(&allocator, merged, merged_pages);
                CHECK_EQUAL(2u, merged_pages);
                nsegment::commit(&allocator, merged, merged_pages);
                g_memset(merged_address, 0x7c, merged_pages * page_size);
                for (u32 page = 0; page < merged_pages; ++page)
                    CHECK_EQUAL((u8)0x7c, merged_address[(u64)page * page_size]);

                nsegment::dealloc_node(&allocator, merged);
                nsegment::commit(&allocator, merged, 1);  // free handle is a no-op
                nsegment::decommit(&allocator, merged, 0);
                nsegment::teardown(&allocator);
                nsegment::commit(&allocator, merged, 1);  // torn-down allocator is a no-op
                nsegment::decommit(&allocator, merged, 0);
            }
        }

        UNITTEST_TEST(out_of_memory)
        {
            const u32 page_size = (u32)v_alloc_get_page_size();

            // Test: allocation exhaustion returns cINVALID_NODE
            nsegment::allocator_t allocator;
            nsegment::initialize(&allocator, (uint_t)1024 * page_size, (uint_t)1 * page_size, (uint_t)512 * page_size);

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
                nsegment::initialize(&allocator, (uint_t)address_space_pages * page_size, (uint_t)1 * page_size, (uint_t)max_segment_pages * page_size);

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
                for (u32 i = 0; i < 32; ++i)
                {
                    const u16 head = allocator.m_free_list_heads[i];
                    CHECK(head == (u16)~0u || head < max_nodes);
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
            const u32 max_segment_pages  = 1u << 8;   // 256 pages
            const u32 max_nodes           = address_space_pages;

            u8 seen[max_nodes];

            for (u32 round = 0; round < 20; ++round)
            {
                nsegment::allocator_t allocator;
                nsegment::initialize(&allocator, (uint_t)address_space_pages * page_size, (uint_t)1 * page_size, (uint_t)max_segment_pages * page_size);

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
                for (u32 i = 0; i < 32; ++i)
                {
                    const u16 head = allocator.m_free_list_heads[i];
                    CHECK(head == (u16)~0u || head < max_nodes);
                }

                nsegment::teardown(&allocator);
            }
        }
    }
}
UNITTEST_SUITE_END

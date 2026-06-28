#ifndef __CCORE_SEGMENT_H__
#define __CCORE_SEGMENT_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    struct arena_t;

    namespace nsegment
    {
        typedef i32  node_t;
        const node_t cINVALID_NODE = -1;

        struct allocator_t
        {
            void*    m_base_address;       // base address of the reserved address space for this allocator
            u32      m_total_pages;        // total number of pages in the address space
            u32      m_segment_min_pages;  // minimum number of pages for a segment
            u32      m_segment_max_pages;  // maximum number of pages for a segment
            u32      m_page_size;
            arena_t* m_chain;
        };

        // Note: @address_space_num_pages MUST be a power of two
        void initialize(allocator_t* allocator, u64 address_space_size = 128 * cGB, u64 segment_min_size = 8 * cMB, u64 segment_max_size = 1 * cGB);
        void teardown(allocator_t* allocator);

        // Note: size should be a power-of-two number of pages
        node_t alloc_node(allocator_t* allocator, u64 size);
        void   dealloc_node(allocator_t* allocator, node_t node);

        // return the virtual address of a node from its index including the number of pages available
        void* get_address(allocator_t* allocator, node_t node, u32& num_pages);

        void commit(allocator_t* allocator, node_t node, u32 num_pages);    // num_pages must be <= the number of pages in the block
        void decommit(allocator_t* allocator, node_t node, u32 num_pages);  // num_pages must be <= the number of pages in the block

        // Global allocator functions, these use a global allocator instance and are just convenience functions that forward
        // to the above functions using a global allocator instance.

        // Note: @address_space_num_pages MUST be a power of two
        void initialize(u32 address_space_num_pages = 4u * 1024 * 1024, u32 segment_min_pages = 1, u32 segment_max_pages = 65536);

        node_t alloc_node(u64 size);
        void   dealloc_node(node_t node);

        // return the virtual address of a node from its index including the number of pages available
        void* get_address(node_t node, u32& num_pages);

        void commit(node_t node, u32 num_pages);    // num_pages must be <= the number of pages in the block
        void decommit(node_t node, u32 num_pages);  // num_pages must be <= the number of pages in the block

    }  // namespace nsegment
}  // namespace ncore

#endif  // __CCORE_SEGMENT_H__

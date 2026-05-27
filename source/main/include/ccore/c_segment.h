#ifndef __CCORE_SEGMENT_H__
#define __CCORE_SEGMENT_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

namespace ncore
{
    struct arena_t;

    namespace nsegment
    {
        struct link_t;

        typedef i32 node_t;

        const node_t INVALID_NODE = -1;

        struct allocator_t
        {
            u32     m_segment_min_pages;    // minimum number of pages for a segment
            u32     m_segment_max_pages;    // maximum number of pages for a segment
            link_t* m_free_list_heads[32];  // free list heads, index 0 = 1x min-page class, index 31 = 2^31x min-page class

            arena_t* m_offsets;  // offsets for each node, the offset of where the node starts in the virtual address space
            arena_t* m_flags;    // flags for each node
            arena_t* m_chain;    // links for each node that forms the contigues address space
            arena_t* m_list;     // next index for each node (for free list)
        };

        // Note: @address_space_num_pages MUST be a power of two
        void initialize(allocator_t* allocator, u32 address_space_num_pages = 4u * 1024 * 1024, u32 segment_min_pages = 1, u32 segment_max_pages = 65536);

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

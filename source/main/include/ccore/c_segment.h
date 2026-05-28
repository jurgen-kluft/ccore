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
        // Purpose: Since we have changed the way we 'create' arenas, we now have the ability to easily use many arenas.
        // For example, bin_t itself could be a stand-alone structure and use an arena for items, an arena for m_bin2 and
        // an arena for m_bin3. All of the support for virtual memory is in arena, growing & shrinking.
        // So why would we need this segment allocator in ccore ?

        struct link_t;

        typedef i32 node_t;

        const node_t INVALID_NODE = -1;

        struct allocator_t
        {
            void* m_base_address;         // base address of the reserved address space for this allocator
            u32   m_total_pages;          // total number of pages in the address space
            u32   m_segment_min_pages;    // minimum number of pages for a segment
            u32   m_segment_max_pages;    // maximum number of pages for a segment
            u32   m_free_list_heads[32];  // free list heads (node indices), index 0 = 1x min-page class, index 31 = 2^31x min-page class

            arena_t* m_offsets;  // offsets for each node, the offset in pages from the base address of the arena
            arena_t* m_flags;    // flags for each node
            arena_t* m_chain;    // links for each node that forms the contigues address space
            arena_t* m_list;     // next index for each node (for free list)
        };

        // Note: @address_space_num_pages MUST be a power of two
        void initialize(allocator_t* allocator, u32 address_space_num_pages = 4u * 1024 * 1024, u32 segment_min_pages = 1, u32 segment_max_pages = 65536);
        void teardown(allocator_t* allocator);

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

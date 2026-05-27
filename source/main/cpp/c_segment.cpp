#include "ccore/c_arena.h"
#include "ccore/c_binmap1.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"

#include "ccore/c_segment.h"
#include "ccore/c_arena.h"

namespace ncore
{
    namespace nsegment
    {
        // Power-of~two segments of virtual address space.
        // Splitting linked-list nodes
        // N free list heads for each segment size class (1 page, 2 pages, 4 pages, ..., 65536 pages)

        // We have a reserved address space (arena) where we allocate/free nodes from, so this acts
        // as a virtual memory array of nodes, so we can use indices to refer to nodes instead of pointers,
        // and we can calculate the address of a node from its index and the base address of the arena.

        // The free-list can be a singly linked list
        // The 'memory' chain linked list needs to be a doubly linked list to allow merging of adjacent free nodes

        // So if we return the index to the user then we do not need a complicated data structure just to map back
        // from pointer to node.
        // The user when deallocating needs to provide the index of the node to deallocate.

        struct link_t
        {
            u32 m_next;  // index of the next node in the chain, or 0 if this is the last node in the chain
            u32 m_prev;  // index of the previous node in the chain, or 0 if this is the first node in the chain
        };

        typedef u32 offset_t;
        typedef u8  flags_t;

        enum flags_e
        {
            FLAG_FREE_USED  = 0x1,  // bit 0 indicates if the node is free(0) or used(1)
            FLAG_LEFT_RIGHT = 0x2,  // bit 1 indicates if the node is the left(0) or right(1) buddy
        };

        static inline bool is_free(flags_t flags) { return (flags & FLAG_FREE_USED) == 0; }
        static inline bool is_used(flags_t flags) { return (flags & FLAG_FREE_USED) != 0; }
        static inline bool is_left(flags_t flags) { return (flags & FLAG_LEFT_RIGHT) == 0; }
        static inline bool is_right(flags_t flags) { return (flags & FLAG_LEFT_RIGHT) != 0; }
        static inline void set_free(flags_t& flags) { flags &= ~FLAG_FREE_USED; }
        static inline void set_used(flags_t& flags) { flags |= FLAG_FREE_USED; }
        static inline void set_left(flags_t& flags) { flags &= ~FLAG_LEFT_RIGHT; }
        static inline void set_right(flags_t& flags) { flags |= FLAG_LEFT_RIGHT; }

        // Merging:
        // Merging a node should only be done when the adjacent node is also free and of the same size and it should be the
        // buddy of the node being merged.
        // Every node is marked (flags) as either being a left or right buddy, so if we are freeing a node which is a right buddy, then
        // we should look at the 'previous' node in the chain to check if it is free and of the same size, and if so, we can merge with it.

        // Example allocation:
        // Request 3 pages:
        // - calculate power-of-two ceiling of 3 pages = 4 pages
        // - check free list for 4-page segments, if not empty, pop a segment
        // - if empty, check free list for 8-page segments, if not empty, pop a segment and split into two 4-page segments,
        //   return one and push the other to the 4-page free list
        // - if empty, check free list for 16-page segments, if not empty, pop a segment and split into two 8-page segments,
        //   push both to the 8-page free list, and repeat the process for 4-page segments
        // - etc..
        // - when splitting a segment at a higher level we need to traverse down until we reach the target segment size,
        //   splitting segments at each level and pushing the remaining parts to the appropriate free list

        void initialize(allocator_t* allocator, u32 address_space_num_pages, u32 segment_min_pages, u32 segment_max_pages)
        {
            ASSERT(allocator != nullptr);
            ASSERT(address_space_num_pages > 0 && (address_space_num_pages & (address_space_num_pages - 1)) == 0);  // must be power of two
            ASSERT(segment_min_pages > 0 && (segment_min_pages & (segment_min_pages - 1)) == 0);
            ASSERT(segment_max_pages >= segment_min_pages && (segment_max_pages & (segment_max_pages - 1)) == 0);

            allocator->m_segment_min_pages = segment_min_pages;
            allocator->m_segment_max_pages = segment_max_pages;

            // Worst-case node count: entire address space divided into minimum-size segments.
            const u32 max_nodes      = address_space_num_pages / segment_min_pages;
            const u32 initial_commit = 256;

            allocator->m_offsets = narena::new_arena((int_t)max_nodes * sizeof(offset_t), (int_t)initial_commit * sizeof(offset_t));
            allocator->m_flags   = narena::new_arena((int_t)max_nodes * sizeof(flags_t), (int_t)initial_commit * sizeof(flags_t));
            allocator->m_chain   = narena::new_arena((int_t)max_nodes * sizeof(link_t), (int_t)initial_commit * sizeof(link_t));
            allocator->m_list    = narena::new_arena((int_t)max_nodes * sizeof(u32), (int_t)initial_commit * sizeof(u32));

            for (u32 i = 0; i < 32; ++i)
                allocator->m_free_list_heads[i] = nullptr;

            // Size-class index for the top-level (largest) segments.
            const u32 top_level     = (u32)math::ilog2(segment_max_pages / segment_min_pages);
            const u32 num_top_nodes = address_space_num_pages / segment_max_pages;

            offset_t* offsets = (offset_t*)narena::base_ptr(allocator->m_offsets);
            flags_t*  flags   = (flags_t*)narena::base_ptr(allocator->m_flags);
            link_t*   chain   = (link_t*)narena::base_ptr(allocator->m_chain);
            u32*      list    = (u32*)narena::base_ptr(allocator->m_list);

            u32 free_list_head = ~0u;

            for (u32 i = 0; i < num_top_nodes; ++i)
            {
                narena::alloc(allocator->m_offsets, sizeof(offset_t));
                narena::alloc(allocator->m_flags, sizeof(flags_t));
                narena::alloc(allocator->m_chain, sizeof(link_t));
                narena::alloc(allocator->m_list, sizeof(u32));

                offsets[i] = i * segment_max_pages;

                flags_t f = 0;
                set_free(f);
                if (i & 1)
                    set_right(f);
                else
                    set_left(f);
                flags[i] = f;

                chain[i].m_prev = (i > 0) ? i - 1 : ~0u;
                chain[i].m_next = (i < num_top_nodes - 1) ? i + 1 : ~0u;

                // Push onto free list (LIFO): this node's list-next = current head, then update head.
                list[i]        = free_list_head;
                free_list_head = i;
            }

            allocator->m_free_list_heads[top_level] = (num_top_nodes > 0) ? &chain[free_list_head] : nullptr;
        }

        node_t alloc_node(allocator_t* allocator, u64 size)
        {
            // allocate a node
            // align to next power of two number of pages
            // see if we have any free nodes for that size, if not we need to go look and split

            return INVALID_NODE;
        }

        void dealloc_node(allocator_t* allocator, node_t node)
        {
            if (node == INVALID_NODE)
                return;

            // deallocate a node
            // mark the node as free
            // check merging (recursively merge upwards)
        }

        void* get_address(allocator_t* allocator, node_t node, u32& num_pages)
        {
            if (node == INVALID_NODE)
            {
                num_pages = 0;
                return nullptr;
            }

            // calculate the address of the node from its index and the base address of the arena
            // return the address and the number of pages in the node
            return nullptr;
        }

        // Virtual memory helper functions for committing and decommitting pages of a node, this should
        // be called by the user after allocating a node and before using the memory of the node, and when
        // the memory of the node is no longer needed, respectively.

        void commit(allocator_t* allocator, node_t node, u32 num_pages)
        {
            // commit the pages of the node, this should be called after allocating a node and before using the memory of the node
            // num_pages must be <= the number of pages in the block
        }

        void decommit(allocator_t* allocator, node_t node, u32 num_pages)
        {
            // decommit the pages of the node, this should be called when the memory of the node is no longer needed
            // num_pages must be <= the number of pages in the block
        }

    }  // namespace nsegment
}  // namespace ncore

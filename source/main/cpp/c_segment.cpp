#include "ccore/c_arena.h"
#include "ccore/c_bitvec.h"
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

        struct chain_t
        {
            u32 m_next;                  // index of the next node in the chain, or 0 if this is the last node in the chain
            u32 m_prev;                  // index of the previous node in the chain, or 0 if this is the first node in the chain
            u32 m_offset;                // offset in pages from the base address of the arena
            u32 m_next;                  // index of the next node in the free list, or cINVALID_INDEX if this is the last node in the free list
            u32 m_flags : 8;             // flags for this node (e.g. free/used, left/right buddy)
            u32 m_committed_pages : 24;  // number of pages currently committed for this node (for used nodes) or for the buddy (for free nodes)
        };

        // constraints:
        // - segment size must be a power of two
        // - smallest segment 

        // 20
        // 20
        // 32
        // 20
        // 4
        // 20
        // total = 

        const u32 cINVALID_INDEX = ~0u;

        typedef u32 offset_t;
        typedef u8  flags_t;

        enum flags_e
        {
            FLAG_FREE_USED        = 0x1,  // bit 0 indicates if the node is free(0) or used(1)
            FLAG_LEFT_RIGHT       = 0x2,  // bit 1 indicates if the node is the left(0) or right(1) buddy
            FLAG_LEFT_RIGHT_SHIFT = 1,    // number of bits to shift for the left/right buddy flag
        };

        static inline bool is_free(flags_t flags) { return (flags & FLAG_FREE_USED) == 0; }
        static inline bool is_used(flags_t flags) { return (flags & FLAG_FREE_USED) != 0; }
        static inline bool is_left(flags_t flags) { return (flags & FLAG_LEFT_RIGHT) == 0; }
        static inline bool is_right(flags_t flags) { return (flags & FLAG_LEFT_RIGHT) != 0; }
        static inline void set_free(flags_t& flags) { flags &= ~FLAG_FREE_USED; }
        static inline void set_used(flags_t& flags) { flags |= FLAG_FREE_USED; }
        static inline void set_left(flags_t& flags) { flags &= ~FLAG_LEFT_RIGHT; }
        static inline void set_right(flags_t& flags) { flags |= FLAG_LEFT_RIGHT; }
        static inline void set_side(flags_t& flags, i32 side) { flags = (flags & ~FLAG_LEFT_RIGHT) | ((side & 1) << FLAG_LEFT_RIGHT_SHIFT); }

        // Helper: Convert bytes to page count, round up to power-of-two
        static inline u32 bytes_to_pages(u64 size_bytes, u32 page_size)
        {
            const u64 pages = (size_bytes + page_size - 1) / page_size;
            return (u32)math::ceilpo2((u32)math::min((u64)~0u, pages));
        }

        // Helper: Map page count to class index relative to segment_min_pages (or return -1 if out of range)
        static inline i32 pages_to_class_index(u32 num_pages, u32 segment_min_pages, u32 segment_max_pages)
        {
            ASSERT(math::ispo2(num_pages) && math::ispo2(segment_min_pages) && math::ispo2(segment_max_pages));

            if (num_pages < segment_min_pages || num_pages > segment_max_pages)
                return -1;  // out of range

            // Class index = log2(num_pages / segment_min_pages)
            return (i32)math::ilog2(num_pages / segment_min_pages);
        }

        // Helper: Pop a node from the free list at a given class
        static inline u32 pop_free_list(allocator_t* allocator, i32 class_index, u32* list)
        {
            ASSERT(class_index >= 0 && class_index < 32);
            u32 node_index = allocator->m_free_list_heads[class_index];
            if (node_index != cINVALID_INDEX)
            {
                allocator->m_free_list_heads[class_index] = list[node_index];
            }
            return node_index;
        }

        // Helper: Push a node onto the free list at a given class
        static inline void push_free_list(allocator_t* allocator, i32 class_index, u32 node_index, u32* list)
        {
            ASSERT(class_index >= 0 && class_index < 32);
            list[node_index]                          = allocator->m_free_list_heads[class_index];
            allocator->m_free_list_heads[class_index] = node_index;
        }

        // Helper: Allocate a new node slot from the arena (for splits)
        static inline u32 alloc_node_slot(allocator_t* allocator)
        {
            ASSERT(allocator != nullptr);

            offset_t* offset = g_allocate<offset_t>(allocator->m_offsets);
            flags_t*  flags  = g_allocate<flags_t>(allocator->m_flags);
            link_t*   chain  = g_allocate<link_t>(allocator->m_chain);
            u32*      list   = g_allocate<u32>(allocator->m_list);

            ASSERTS(offset != nullptr, "Segment allocator metadata arena exhausted (offsets)");
            ASSERTS(flags != nullptr, "Segment allocator metadata arena exhausted (flags)");
            ASSERTS(chain != nullptr, "Segment allocator metadata arena exhausted (chain)");
            ASSERTS(list != nullptr, "Segment allocator metadata arena exhausted (list)");

            *offset       = 0;
            *flags        = 0;
            chain->m_prev = cINVALID_INDEX;
            chain->m_next = cINVALID_INDEX;
            *list         = cINVALID_INDEX;

            const u32 new_index = (u32)(offset - (offset_t*)narena::base_ptr(allocator->m_offsets));
            return new_index;
        }

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
            ASSERT(address_space_num_pages > 0 && math::ispo2(address_space_num_pages) != 0);
            ASSERT(math::ispo2(segment_min_pages) != 0 && math::ispo2(segment_max_pages) != 0);
            ASSERT(segment_max_pages >= segment_min_pages);

            allocator->m_total_pages  = address_space_num_pages;
            allocator->m_base_address = v_alloc_reserve((u64)address_space_num_pages * (u64)v_alloc_get_page_size());

            allocator->m_segment_min_pages = segment_min_pages;
            allocator->m_segment_max_pages = segment_max_pages;

            // Worst-case node count: entire address space divided into minimum-size segments.
            const u32 max_nodes = address_space_num_pages / segment_min_pages;

            // Size-class index for the top-level (largest) segments.
            const u32 top_level     = (u32)math::ilog2(segment_max_pages / segment_min_pages);
            const u32 num_top_nodes = address_space_num_pages / segment_max_pages;

            // We already make sure we have enough space in the arena to hold the top nodes + some additional nodes
            // for splitting, so we can commit some initial pages for the top-level nodes, and then we can commit
            // more pages for the lower-level nodes as needed when splitting.
            const u32 initial_commit = num_top_nodes + 256;

            allocator->m_offsets = narena::new_arena((int_t)max_nodes * sizeof(offset_t), (int_t)initial_commit * sizeof(offset_t));
            allocator->m_flags   = narena::new_arena((int_t)max_nodes * sizeof(flags_t), (int_t)initial_commit * sizeof(flags_t));
            allocator->m_chain   = narena::new_arena((int_t)max_nodes * sizeof(link_t), (int_t)initial_commit * sizeof(link_t));
            allocator->m_list    = narena::new_arena((int_t)max_nodes * sizeof(u32), (int_t)initial_commit * sizeof(u32));

            offset_t* offsets = (offset_t*)narena::base_ptr(allocator->m_offsets);
            flags_t*  flags   = (flags_t*)narena::base_ptr(allocator->m_flags);
            link_t*   chain   = (link_t*)narena::base_ptr(allocator->m_chain);
            u32*      list    = (u32*)narena::base_ptr(allocator->m_list);

            u32 free_list_head = cINVALID_INDEX;
            for (u32 i = 0; i < 32; ++i)
                allocator->m_free_list_heads[i] = cINVALID_INDEX;

            for (u32 i = 0; i < num_top_nodes; ++i)
            {
                narena::alloc(allocator->m_offsets, sizeof(offset_t));
                narena::alloc(allocator->m_flags, sizeof(flags_t));
                narena::alloc(allocator->m_chain, sizeof(link_t));
                narena::alloc(allocator->m_list, sizeof(u32));

                offsets[i] = i * segment_max_pages;

                flags_t f = 0;
                set_free(f);
                set_side(f, i & 1);
                flags[i] = f;

                chain[i].m_prev = (i > 0) ? i - 1 : cINVALID_INDEX;
                chain[i].m_next = (i < num_top_nodes - 1) ? i + 1 : cINVALID_INDEX;

                // Push onto free list (LIFO): this node's list-next = current head, then update head.
                list[i]        = free_list_head;
                free_list_head = i;
            }
            allocator->m_free_list_heads[top_level] = free_list_head;
        }

        node_t alloc_node(allocator_t* allocator, u64 size)
        {
            ASSERT(allocator != nullptr);

            // Convert bytes to required pages (power-of-two aligned)
            const u32 page_size      = v_alloc_get_page_size();
            const u32 required_pages = bytes_to_pages(size, page_size);

            // Map to target class index
            const i32 target_class = pages_to_class_index(required_pages, allocator->m_segment_min_pages, allocator->m_segment_max_pages);
            if (target_class < 0)
                return INVALID_NODE;  // size out of range

            offset_t* offsets = (offset_t*)narena::base_ptr(allocator->m_offsets);
            flags_t*  flags   = (flags_t*)narena::base_ptr(allocator->m_flags);
            link_t*   chain   = (link_t*)narena::base_ptr(allocator->m_chain);
            u32*      list    = (u32*)narena::base_ptr(allocator->m_list);

            // Find first non-empty free list at or above target class
            i32 source_class = target_class;
            u32 source_node  = cINVALID_INDEX;

            for (i32 class_idx = target_class; class_idx < 32; ++class_idx)
            {
                source_node = allocator->m_free_list_heads[class_idx];
                if (source_node != cINVALID_INDEX)
                {
                    source_class = class_idx;
                    break;
                }
            }

            if (source_node == cINVALID_INDEX)
                return INVALID_NODE;  // no free nodes available

            // Pop source node
            source_node = pop_free_list(allocator, source_class, list);
            ASSERT(source_node != cINVALID_INDEX);

            // Split downward from source_class to target_class
            i32 current_class = source_class;
            u32 current_node  = source_node;

            while (current_class > target_class)
            {
                // Create right buddy node
                u32 right_buddy = alloc_node_slot(allocator);
                if (right_buddy == cINVALID_INDEX)
                    return INVALID_NODE;  // allocation failed

                const u32 class_page_size = allocator->m_segment_min_pages << current_class;
                const u32 half_page_size  = class_page_size >> 1;

                // Right buddy gets second half of offset
                offsets[right_buddy] = offsets[current_node] + half_page_size;

                // Set flags: current is left, right is right
                flags_t left_flag  = flags[current_node];
                flags_t right_flag = left_flag;
                set_left(left_flag);
                set_right(right_flag);
                set_free(right_flag);
                flags[current_node] = left_flag;
                flags[right_buddy]  = right_flag;

                // Update chain links
                chain[right_buddy].m_prev = current_node;
                chain[right_buddy].m_next = chain[current_node].m_next;
                if (chain[current_node].m_next != cINVALID_INDEX)
                    chain[chain[current_node].m_next].m_prev = right_buddy;
                chain[current_node].m_next = right_buddy;

                // Push right buddy to lower class free list
                current_class--;
                push_free_list(allocator, current_class, right_buddy, list);

                // Continue with left node (already at correct offset)
            }

            // Mark final node as USED
            set_used(flags[current_node]);

            return (node_t)current_node;
        }

        void teardown(allocator_t* allocator)
        {
            if (allocator == nullptr)
                return;

            // Clean up arenas
            narena::destroy(allocator->m_offsets);
            narena::destroy(allocator->m_flags);
            narena::destroy(allocator->m_chain);
            narena::destroy(allocator->m_list);

            allocator->m_offsets = nullptr;
            allocator->m_flags   = nullptr;
            allocator->m_chain   = nullptr;
            allocator->m_list    = nullptr;
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
            offset_t const* offsets = (offset_t const*)narena::base_ptr(allocator->m_offsets);
            const offset_t  offset  = offsets[node];

            // num_pages can be calculated from the node's offset and the next node's offset in the chain
            // note: offset unit is pages
            link_t const* chain = (link_t const*)narena::base_ptr(allocator->m_chain);
            if (chain[node].m_next != cINVALID_INDEX)
            {
                num_pages = (offsets[chain[node].m_next] - offset);
            }
            else
            {
                num_pages = allocator->m_total_pages - offset;
            }

            const u32 page_size = v_alloc_get_page_size();
            return (void*)((u8*)allocator->m_base_address + (offset * page_size));
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

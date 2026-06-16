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
        typedef u32 offset_t;

        // constraints:
        // - segment size must be a power of two
        // - smallest segment is

        // 512 GiB / 8 MiB = 65536 segments
        //

        // sizeof(chain_t) = 24 bytes
        struct chain_t
        {
            u32      m_next;                  // index of the next node in the chain, or 0 if this is the last node in the chain
            u32      m_prev;                  // index of the previous node in the chain, or 0 if this is the first node in the chain
            u32      m_next_free;             // index of the next node in the free list, or cINVALID_INDEX if this is the last node in the free list
            offset_t m_offset;                // offset in pages from the base address of the arena
            u32      m_committed_pages : 24;  // number of pages currently committed for this node (for used nodes) or for the buddy (for free nodes)
            u32      m_flags : 8;             // flags for this node (e.g. free/used, left/right buddy)
        };

        const u32 cINVALID_INDEX = ~0u;

        // 8888888888 888             d8888  .d8888b.   .d8888b.
        // 888        888            d88888 d88P  Y88b d88P  Y88b
        // 888        888           d88P888 888    888 Y88b.
        // 8888888    888          d88P 888 888         "Y888b.
        // 888        888         d88P  888 888  88888     "Y88b.
        // 888        888        d88P   888 888    888       "888
        // 888        888       d8888888888 Y88b  d88P Y88b  d88P
        // 888        88888888 d88P     888  "Y8888P88  "Y8888P"

        typedef u8 flags_t;

        enum flags_e
        {
            FLAG_FREE_USED        = 0x1,  // bit 0 indicates if the node is free(0) or used(1)
            FLAG_LEFT_RIGHT       = 0x2,  // bit 1 indicates if the node is the left(0) or right(1) buddy
            FLAG_LEFT_RIGHT_SHIFT = 1,    // number of bits to shift for the left/right buddy flag
        };

        static inline bool    is_free(flags_t flags) { return (flags & FLAG_FREE_USED) == 0; }
        static inline bool    is_used(flags_t flags) { return (flags & FLAG_FREE_USED) != 0; }
        static inline bool    is_left(flags_t flags) { return (flags & FLAG_LEFT_RIGHT) == 0; }
        static inline bool    is_right(flags_t flags) { return (flags & FLAG_LEFT_RIGHT) != 0; }
        static inline flags_t set_free(flags_t flags) { return flags & ~FLAG_FREE_USED; }
        static inline flags_t set_used(flags_t flags) { return flags | FLAG_FREE_USED; }
        static inline flags_t set_left(flags_t flags) { return flags & ~FLAG_LEFT_RIGHT; }
        static inline flags_t set_right(flags_t flags) { return flags | FLAG_LEFT_RIGHT; }
        static inline flags_t set_side(flags_t flags, i32 side) { return (flags & ~FLAG_LEFT_RIGHT) | ((side & 1) << FLAG_LEFT_RIGHT_SHIFT); }

        // 888    888 8888888888 888      8888888b.  8888888888 8888888b.   .d8888b.
        // 888    888 888        888      888   Y88b 888        888   Y88b d88P  Y88b
        // 888    888 888        888      888    888 888        888    888 Y88b.
        // 8888888888 8888888    888      888   d88P 8888888    888   d88P  "Y888b.
        // 888    888 888        888      8888888P"  888        8888888P"      "Y88b.
        // 888    888 888        888      888        888        888 T88b         "888
        // 888    888 888        888      888        888        888  T88b  Y88b  d88P
        // 888    888 8888888888 88888888 888        8888888888 888   T88b  "Y8888P"

        // Helper: Convert bytes to page count, round up to power-of-two
        static inline u32 s_bytes_to_pages(u64 size_bytes, u32 page_size)
        {
            const u64 pages = (size_bytes + page_size - 1) / page_size;
            return (u32)math::ceilpo2(pages);
        }

        static inline u32 s_chain_ptr_to_index(allocator_t* allocator, chain_t* node)
        {
            if (node == nullptr)
                return cINVALID_INDEX;
            byte* chain_array = narena::base_ptr(allocator->m_chain);
            chain_array += 32 * sizeof(u32);  // skip the free list heads
            u32 node_offset = (u32)(((byte*)node - chain_array) / sizeof(chain_t));
            return node_offset;
        }

        static inline chain_t* s_chain_index_to_ptr(allocator_t* allocator, u32 index)
        {
            ASSERT(allocator != nullptr);
            byte* chain_array = narena::base_ptr(allocator->m_chain);
            chain_array += 32 * sizeof(u32);  // skip the free list heads
            return (chain_t*)(chain_array + index * sizeof(chain_t));
        }

        // Helper: Map page count to class index relative to segment_min_pages (or return -1 if out of range)
        static inline i32 s_pages_to_class_index(u32 num_pages, u32 segment_min_pages, u32 segment_max_pages)
        {
            ASSERT(math::ispo2(num_pages) && math::ispo2(segment_min_pages) && math::ispo2(segment_max_pages));

            if (num_pages > segment_max_pages)
                return -1;  // out of range

            num_pages = math::max(num_pages, segment_min_pages);

            // Class index = log2(num_pages / segment_min_pages)
            return (i32)math::ilog2(num_pages / segment_min_pages);
        }

        // Helper: Pop a node from the free list at a given class
        static inline u32 s_pop_from_free_list(allocator_t* allocator, i32 class_index)
        {
            u32* free_list_heads = narena::base_ptr_as<u32>(allocator->m_chain);

            ASSERT(class_index >= 0 && class_index < 32);
            u32 node_index = free_list_heads[class_index];
            if (node_index != cINVALID_INDEX)
            {
                chain_t* chain_array         = s_chain_index_to_ptr(allocator, (u32)0);  // get pointer to the first node (top-level)
                free_list_heads[class_index] = chain_array[node_index].m_next_free;
            }
            return node_index;
        }

        // Helper: Push a node onto the free list at a given class
        static inline void s_push_on_free_list(allocator_t* allocator, i32 class_index, u32 node_index)
        {
            ASSERT(class_index >= 0 && class_index < 32);
            u32*     free_list_heads            = narena::base_ptr_as<u32>(allocator->m_chain);
            chain_t* chain_array                = s_chain_index_to_ptr(allocator, 0);  // get pointer to the first node (top-level)
            chain_array[node_index].m_next_free = free_list_heads[class_index];
            free_list_heads[class_index]        = node_index;
        }

        //        d8888 888      888      .d88888b.   .d8888b.       888b    888  .d88888b.  8888888b.  8888888888
        //       d88888 888      888     d88P" "Y88b d88P  Y88b      8888b   888 d88P" "Y88b 888  "Y88b 888
        //      d88P888 888      888     888     888 888    888      88888b  888 888     888 888    888 888
        //     d88P 888 888      888     888     888 888             888Y88b 888 888     888 888    888 8888888
        //    d88P  888 888      888     888     888 888             888 Y88b888 888     888 888    888 888
        //   d88P   888 888      888     888     888 888    888      888  Y88888 888     888 888    888 888
        //  d8888888888 888      888     Y88b. .d88P Y88b  d88P      888   Y8888 Y88b. .d88P 888  .d88P 888
        // d88P     888 88888888 88888888 "Y88888P"   "Y8888P"       888    Y888  "Y88888P"  8888888P"  8888888888

        // Helper: Allocate a new node slot from the arena (for splits)
        static inline chain_t* s_alloc_node_slot(allocator_t* allocator)
        {
            ASSERT(allocator != nullptr);

            chain_t* chain = g_allocate<chain_t>(allocator->m_chain);
            if (chain == nullptr)
                return nullptr;  // allocation failed

            chain->m_offset          = 0;
            chain->m_flags           = 0;
            chain->m_prev            = cINVALID_INDEX;
            chain->m_next            = cINVALID_INDEX;
            chain->m_next_free       = cINVALID_INDEX;
            chain->m_committed_pages = 0;

            return chain;
        }

        //        d8888 888      888      .d88888b.   .d8888b.
        //       d88888 888      888     d88P" "Y88b d88P  Y88b
        //      d88P888 888      888     888     888 888    888
        //     d88P 888 888      888     888     888 888
        //    d88P  888 888      888     888     888 888
        //   d88P   888 888      888     888     888 888    888
        //  d8888888888 888      888     Y88b. .d88P Y88b  d88P
        // d88P     888 88888888 88888888 "Y88888P"   "Y8888P"

        node_t alloc_node(allocator_t* allocator, u64 size)
        {
            ASSERT(allocator != nullptr);

            // Convert bytes to required pages (power-of-two aligned)
            const u32 page_size      = v_alloc_get_page_size();
            const u32 required_pages = s_bytes_to_pages(size, page_size);

            // Map to target class index
            const i32 target_class = s_pages_to_class_index(required_pages, allocator->m_segment_min_pages, allocator->m_segment_max_pages);
            if (target_class < 0)
                return cINVALID_NODE;  // size out of range

            u32* free_list_heads = narena::base_ptr_as<u32>(allocator->m_chain);

            // Find first non-empty free list at or above target class
            i32 source_class = target_class;
            u32 source_node  = cINVALID_INDEX;
            for (i32 class_idx = target_class; class_idx < 32; ++class_idx)
            {
                source_node = free_list_heads[class_idx];
                if (source_node != cINVALID_INDEX)
                {
                    source_class = class_idx;
                    break;
                }
            }
            if (source_node == cINVALID_INDEX)
                return cINVALID_NODE;  // no free nodes available

            // Pop source node
            source_node = s_pop_from_free_list(allocator, source_class);
            ASSERT(source_node != cINVALID_INDEX);

            // Split downward from source_class to target_class
            i32 current_class = source_class;

            u32      current_node_index = source_node;
            chain_t* current_node       = s_chain_index_to_ptr(allocator, current_node_index);

            while (current_class > target_class)
            {
                // Create right buddy node
                chain_t* right_buddy = s_alloc_node_slot(allocator);
                if (right_buddy == nullptr)
                    return cINVALID_NODE;  // allocation failed
                u32 right_buddy_index = s_chain_ptr_to_index(allocator, right_buddy);

                const u32 class_segment_size = allocator->m_segment_min_pages << current_class;
                const u32 half_segment_size  = class_segment_size >> 1;

                // Right buddy gets second half of offset
                right_buddy->m_offset = current_node->m_offset + half_segment_size;

                // Set flags: current is left, right is right
                flags_t left_flag     = current_node->m_flags;
                flags_t right_flag    = left_flag;
                left_flag             = set_left(left_flag);
                right_flag            = set_right(right_flag);
                right_flag            = set_free(right_flag);
                current_node->m_flags = left_flag;
                right_buddy->m_flags  = right_flag;

                // Update chain links
                right_buddy->m_prev = current_node_index;
                right_buddy->m_next = current_node->m_next;
                if (current_node->m_next != cINVALID_INDEX)
                {
                    chain_t* current_node_next = s_chain_index_to_ptr(allocator, current_node->m_next);
                    current_node_next->m_prev  = right_buddy_index;
                }
                current_node->m_next = right_buddy_index;

                // Push right buddy to lower class free list
                current_class--;
                s_push_on_free_list(allocator, current_class, right_buddy_index);

                // Continue with left node (already at correct offset)
            }

            // Mark final node as USED
            current_node->m_flags = set_used(current_node->m_flags);

            return (node_t)s_chain_ptr_to_index(allocator, current_node);
        }

        // 8888888b.  8888888888        d8888 888      888      .d88888b.   .d8888b.
        // 888  "Y88b 888              d88888 888      888     d88P" "Y88b d88P  Y88b
        // 888    888 888             d88P888 888      888     888     888 888    888
        // 888    888 8888888        d88P 888 888      888     888     888 888
        // 888    888 888           d88P  888 888      888     888     888 888
        // 888    888 888          d88P   888 888      888     888     888 888    888
        // 888  .d88P 888         d8888888888 888      888     Y88b. .d88P Y88b  d88P
        // 8888888P"  8888888888 d88P     888 88888888 88888888 "Y88888P"   "Y8888P"

        void dealloc_node(allocator_t* allocator, node_t node)
        {
            if (node == cINVALID_NODE)
                return;

            // deallocate a node
            // mark the node as free
            // check merging (recursively merge upwards)
        }

        //        d8888 8888888b.  8888888b.  8888888b.  8888888888 .d8888b.   .d8888b.
        //       d88888 888  "Y88b 888  "Y88b 888   Y88b 888       d88P  Y88b d88P  Y88b
        //      d88P888 888    888 888    888 888    888 888       Y88b.      Y88b.
        //     d88P 888 888    888 888    888 888   d88P 8888888    "Y888b.    "Y888b.
        //    d88P  888 888    888 888    888 8888888P"  888           "Y88b.     "Y88b.
        //   d88P   888 888    888 888    888 888 T88b   888             "888       "888
        //  d8888888888 888  .d88P 888  .d88P 888  T88b  888       Y88b  d88P Y88b  d88P
        // d88P     888 8888888P"  8888888P"  888   T88b 8888888888 "Y8888P"   "Y8888P"

        void* get_address(allocator_t* allocator, node_t node, u32& num_pages)
        {
            if (node == cINVALID_NODE)
            {
                num_pages = 0;
                return nullptr;
            }

            // calculate the address of the node from its index and the base address of the arena
            // return the address and the number of pages in the node
            chain_t const * chain  = s_chain_index_to_ptr(allocator, node);
            const offset_t  offset = chain->m_offset;

            // num_pages can be calculated from the node's offset and the next node's offset in the chain
            // note: offset unit is pages
            if (chain->m_next != cINVALID_INDEX)
            {
                chain_t const * right_buddy = s_chain_index_to_ptr(allocator, chain->m_next);
                num_pages                   = (right_buddy->m_offset - offset);
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

        //  .d8888b.   .d88888b.  888b     d888 888b     d888 8888888 88888888888
        // d88P  Y88b d88P" "Y88b 8888b   d8888 8888b   d8888   888       888
        // 888    888 888     888 88888b.d88888 88888b.d88888   888       888
        // 888        888     888 888Y88888P888 888Y88888P888   888       888
        // 888        888     888 888 Y888P 888 888 Y888P 888   888       888
        // 888    888 888     888 888  Y8P  888 888  Y8P  888   888       888
        // Y88b  d88P Y88b. .d88P 888   "   888 888   "   888   888       888
        //  "Y8888P"   "Y88888P"  888       888 888       888 8888888     888

        void commit(allocator_t* allocator, node_t node, u32 num_pages)
        {
            // commit the pages of the node, this should be called after allocating a node and before using the memory of the node
            // num_pages must be <= the number of pages in the block
        }

        // 8888888b.  8888888888 .d8888b.   .d88888b.  888b     d888 888b     d888 8888888 88888888888
        // 888  "Y88b 888       d88P  Y88b d88P" "Y88b 8888b   d8888 8888b   d8888   888       888
        // 888    888 888       888    888 888     888 88888b.d88888 88888b.d88888   888       888
        // 888    888 8888888   888        888     888 888Y88888P888 888Y88888P888   888       888
        // 888    888 888       888        888     888 888 Y888P 888 888 Y888P 888   888       888
        // 888    888 888       888    888 888     888 888  Y8P  888 888  Y8P  888   888       888
        // 888  .d88P 888       Y88b  d88P Y88b. .d88P 888   "   888 888   "   888   888       888
        // 8888888P"  8888888888 "Y8888P"   "Y88888P"  888       888 888       888 8888888     888

        void decommit(allocator_t* allocator, node_t node, u32 num_pages)
        {
            // decommit the pages of the node, this should be called when the memory of the node is no longer needed
            // num_pages must be <= the number of pages in the block
        }

        // 8888888 888b    888 8888888 88888888888 8888888        d8888 888      8888888 8888888888P 8888888888
        //   888   8888b   888   888       888       888         d88888 888        888         d88P  888
        //   888   88888b  888   888       888       888        d88P888 888        888        d88P   888
        //   888   888Y88b 888   888       888       888       d88P 888 888        888       d88P    8888888
        //   888   888 Y88b888   888       888       888      d88P  888 888        888      d88P     888
        //   888   888  Y88888   888       888       888     d88P   888 888        888     d88P      888
        //   888   888   Y8888   888       888       888    d8888888888 888        888    d88P       888
        // 8888888 888    Y888 8888888     888     8888888 d88P     888 88888888 8888888 d8888888888 8888888888

        void initialize(allocator_t* allocator, uint_t address_space_size, uint_t segment_min_size, uint_t segment_max_size)
        {
            ASSERT(allocator != nullptr);
            ASSERT(address_space_size > 0);

            u32 const page_size = v_alloc_get_page_size();
            ASSERT(segment_min_size >= page_size && segment_max_size >= segment_min_size);
            u32 const address_space_num_pages = s_bytes_to_pages(address_space_size, page_size);
            u32 const segment_min_pages       = s_bytes_to_pages(segment_min_size, page_size);
            u32 const segment_max_pages       = s_bytes_to_pages(segment_max_size, page_size);

            ASSERT(address_space_num_pages > 0 && math::ispo2(address_space_num_pages) != 0);
            ASSERT(math::ispo2(segment_min_pages) != 0 && math::ispo2(segment_max_pages) != 0);
            ASSERT(segment_max_pages >= segment_min_pages);

            allocator->m_total_pages  = address_space_num_pages;
            allocator->m_base_address = v_alloc_reserve((u64)address_space_num_pages * (u64)v_alloc_get_page_size());

            allocator->m_segment_min_pages = segment_min_pages;
            allocator->m_segment_max_pages = segment_max_pages;

            // Worst-case node count: entire address space divided into minimum-size segments.
            const u32 max_nodes = address_space_num_pages / segment_min_pages;

            // The arena stores the free-list heads before the chain node array, so reserve space for both.
            const uint_t free_list_bytes = 32u * sizeof(u32);
            const uint_t chain_bytes     = (uint_t)max_nodes * sizeof(chain_t);

            // Size-class index for the top-level (largest) segments.
            const u32 top_level     = (u32)math::ilog2(segment_max_pages / segment_min_pages);
            const u32 num_top_nodes = address_space_num_pages / segment_max_pages;
            ASSERT((num_top_nodes & 1) == 0);  // must be even to maintain left/right buddy pairs at the top level

            // We already make sure we have enough space in the arena to hold the top nodes + some additional nodes
            // for splitting, so we can commit some initial pages for the top-level nodes, and then we can commit
            // more pages for the lower-level nodes as needed when splitting.
            const u32 initial_commit = num_top_nodes + 256;

            allocator->m_chain = narena::new_arena(free_list_bytes + chain_bytes, free_list_bytes + (uint_t)initial_commit * sizeof(chain_t));

            u32* free_list_heads = g_allocate_array<u32>(allocator->m_chain, 32);
            for (u32 i = 0; i < 32; ++i)
                free_list_heads[i] = cINVALID_INDEX;

            chain_t* previous = nullptr;
            for (u32 i = 0; i < num_top_nodes; ++i)
            {
                chain_t* current           = g_allocate<chain_t>(allocator->m_chain);
                current->m_prev            = (i > 0) ? i - 1 : cINVALID_INDEX;
                current->m_next            = cINVALID_INDEX;
                current->m_next_free       = cINVALID_INDEX;  // will be set when pushing onto free list
                current->m_offset          = i * segment_max_pages;
                current->m_flags           = set_free(0);
                current->m_flags           = set_side(current->m_flags, i & 1);
                current->m_committed_pages = 0;
                if (previous != nullptr)
                    previous->m_next_free = i;
                previous = current;
            }

            free_list_heads[top_level] = 0;
        }

        // 88888888888 8888888888        d8888 8888888b.  8888888b.   .d88888b.  888       888 888b    888
        //     888     888              d88888 888   Y88b 888  "Y88b d88P" "Y88b 888   o   888 8888b   888
        //     888     888             d88P888 888    888 888    888 888     888 888  d8b  888 88888b  888
        //     888     8888888        d88P 888 888   d88P 888    888 888     888 888 d888b 888 888Y88b 888
        //     888     888           d88P  888 8888888P"  888    888 888     888 888d88888b888 888 Y88b888
        //     888     888          d88P   888 888 T88b   888    888 888     888 88888P Y88888 888  Y88888
        //     888     888         d8888888888 888  T88b  888  .d88P Y88b. .d88P 8888P   Y8888 888   Y8888
        //     888     8888888888 d88P     888 888   T88b 8888888P"   "Y88888P"  888P     Y888 888    Y888

        void teardown(allocator_t* allocator)
        {
            if (allocator == nullptr)
                return;
            narena::destroy(allocator->m_chain);
        }

    }  // namespace nsegment
}  // namespace ncore

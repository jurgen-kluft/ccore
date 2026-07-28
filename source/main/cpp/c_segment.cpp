#include "ccore/c_bitvec.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"

#include "ccore/c_segment.h"
#include "ccore/c_arena.h"

namespace ncore
{
    namespace nsegment
    {
        // constraints:
        // - segment size must be a power of two
        // - smallest segment is

        // 512 GiB / 8 MiB = 65536 segments
        //

        // sizeof(chain_t) = 8 bytes
        struct chain_t
        {
            u16 m_next;                  // index of the next node in the chain, or 0 if this is the last node in the chain
            u16 m_prev;                  // index of the previous node in the chain, or 0 if this is the first node in the chain
            u32 m_committed_pages : 24;  // number of pages currently committed for this node (for used nodes) or for the buddy (for free nodes)
            u32 m_flags : 8;             // flags for this node (e.g. free/used, left/right buddy)
        };

        const u16 cINVALID_INDEX = (u16)~0u;

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
        static inline u8 s_bytes_to_size_shift(u64 size_bytes)
        {
            if (size_bytes == 0)
                return 0;
            return (u8)(64 - math::countLeadingZeros((u64)(size_bytes - 1)));
        }

        static inline u32 s_chain_ptr_to_index(allocator_t* allocator, chain_t* node)
        {
            ASSERT(allocator != nullptr);
            ASSERT(allocator->m_chain != nullptr);
            if (node == nullptr)
                return cINVALID_INDEX;
            const u32 node_index = (u32)(node - allocator->m_chain);
            ASSERT(node_index < allocator->m_total_minsize_segments);
            return node_index;
        }

        static inline chain_t* s_chain_index_to_ptr(allocator_t* allocator, u16 index)
        {
            ASSERT(allocator != nullptr);
            ASSERT(allocator->m_chain != nullptr);
            ASSERT(index != cINVALID_INDEX);
            ASSERT(index < allocator->m_total_minsize_segments);
            return &allocator->m_chain[index];
        }

        // Helper: Map page count to class index relative to segment_min_pages (or return -1 if out of range)
        static inline i32 s_pages_to_class_index(u8 requested_size_shift, u8 segment_minsize_shift, u8 segment_maxsize_shift)
        {
            if (requested_size_shift > segment_maxsize_shift)
                return -1;  // out of range
            requested_size_shift = math::max(requested_size_shift, segment_minsize_shift);
            return (i32)(requested_size_shift - segment_minsize_shift);
        }

        // Helper: Pop a node from the free list at a given class
        static inline u32 s_pop_from_free_list(allocator_t* allocator, i32 class_index)
        {
            ASSERT(allocator != nullptr);
            ASSERT(allocator->m_free != nullptr);
            ASSERT(class_index >= 0 && class_index < 32);
            const u16 node_index = allocator->m_free_list_heads[class_index];
            if (node_index != cINVALID_INDEX)
            {
                ASSERT(node_index < allocator->m_total_minsize_segments);
                const u16 next_node                       = allocator->m_free[node_index].m_next;
                allocator->m_free_list_heads[class_index] = next_node;
                if (next_node != cINVALID_INDEX)
                {
                    ASSERT(next_node < allocator->m_total_minsize_segments);
                    allocator->m_free[next_node].m_prev = cINVALID_INDEX;
                }
                allocator->m_free[node_index].m_next = cINVALID_INDEX;
                allocator->m_free[node_index].m_prev = cINVALID_INDEX;
            }
            return node_index;
        }

        // Helper: Push a node onto the free list at a given class
        static inline void s_push_on_free_list(allocator_t* allocator, i32 class_index, u16 node_index)
        {
            ASSERT(allocator != nullptr);
            ASSERT(allocator->m_free != nullptr);
            ASSERT(class_index >= 0 && class_index < 32);
            ASSERT(node_index != cINVALID_INDEX);
            ASSERT(node_index < allocator->m_total_minsize_segments);
            const u16 previous_head              = allocator->m_free_list_heads[class_index];
            allocator->m_free[node_index].m_next = previous_head;
            allocator->m_free[node_index].m_prev = cINVALID_INDEX;
            if (previous_head != cINVALID_INDEX)
            {
                ASSERT(previous_head < allocator->m_total_minsize_segments);
                allocator->m_free[previous_head].m_prev = node_index;
            }
            allocator->m_free_list_heads[class_index] = node_index;
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
            const u32 required_size_shift = s_bytes_to_size_shift(size);

            // Map to target class index
            const i32 target_class = s_pages_to_class_index(required_size_shift, allocator->m_segment_minsize_shift, allocator->m_segment_maxsize_shift);
            if (target_class < 0)
                return cINVALID_NODE;  // size out of range

            // Find first non-empty free list at or above target class
            i32 source_class = target_class;
            u16 source_node  = cINVALID_INDEX;
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
                ASSERT(current_class > 0);
                const u32 right_buddy_index = current_node_index + (1u << (current_class - 1));
                ASSERT(right_buddy_index < allocator->m_total_minsize_segments);
                chain_t* right_buddy = s_chain_index_to_ptr(allocator, (u16)right_buddy_index);

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
                current_node->m_next                        = right_buddy_index;
                right_buddy->m_committed_pages              = 0;
                allocator->m_free[right_buddy_index].m_next = cINVALID_INDEX;
                allocator->m_free[right_buddy_index].m_prev = cINVALID_INDEX;

                // Push right buddy to lower class free list
                current_class--;
                s_push_on_free_list(allocator, current_class, right_buddy_index);

                // Continue with left node (already at correct offset)
            }

            // Mark final node as USED
            current_node->m_flags = set_used(current_node->m_flags);

            return (node_t)current_node_index;
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
            if (allocator == nullptr || allocator->m_base_address == nullptr || node < 0 || (u32)node >= allocator->m_total_minsize_segments)
            {
                num_pages = 0;
                return nullptr;
            }

            // calculate the address of the node from its index and the base address of the arena
            // return the address and the number of pages in the node
            chain_t const * chain = s_chain_index_to_ptr(allocator, node);
            if (is_free(chain->m_flags))
            {
                num_pages = 0;
                return nullptr;
            }

            // num_pages can be calculated from node->index and the next node->index in the chain
            // note: index is in units of minimum segment size
            const u64 min_segments_offset = (u64)node << allocator->m_segment_minsize_shift;
            u32       min_segments_count  = 0;
            if (chain->m_next != cINVALID_INDEX)
            {
                ASSERT(chain->m_next > (u16)node);
                min_segments_count = chain->m_next - (u16)node;
            }

            else
            {
                min_segments_count = (allocator->m_total_minsize_segments - (u16)node);
            }

            num_pages = min_segments_count << (allocator->m_segment_minsize_shift - allocator->m_pagesize_shift);
            return (void*)(allocator->m_base_address + min_segments_offset);
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

        void initialize(allocator_t* allocator, u64 address_space_size, u64 segment_min_size, u64 segment_max_size)
        {
            ASSERT(allocator != nullptr);
            ASSERT(address_space_size > 0);
            ASSERT(math::ispo2(address_space_size));
            ASSERT(math::ispo2(segment_min_size) != 0 && math::ispo2(segment_max_size) != 0);

            u32 const page_size       = v_alloc_get_page_size();
            u8 const  page_size_shift = v_alloc_get_page_size_shift();
            ASSERT(segment_min_size >= page_size && segment_max_size >= segment_min_size);
            u8 const address_space_size_shift = s_bytes_to_size_shift(address_space_size);
            u8 const segment_minsize_shift    = s_bytes_to_size_shift(segment_min_size);
            u8 const segment_maxsize_shift    = s_bytes_to_size_shift(segment_max_size);

            ASSERT(((u64)1 << address_space_size_shift) == address_space_size);
            ASSERT(((u64)1 << segment_minsize_shift) == segment_min_size);
            ASSERT(((u64)1 << segment_maxsize_shift) == segment_max_size);
            ASSERT(segment_max_size >= segment_min_size && segment_max_size <= address_space_size);

            g_memset(allocator, 0, sizeof(allocator_t));
            for (u32 i = 0; i < 32; ++i)
                allocator->m_free_list_heads[i] = cINVALID_INDEX;

            const u32 size_class_count = (u32)segment_maxsize_shift - segment_minsize_shift;
            ASSERT(size_class_count < 32);

            // Worst-case node count: entire address space divided into minimum-size segments.
            const u32 node_count_shift = address_space_size_shift - segment_minsize_shift;
            ASSERT(node_count_shift < 32);
            const u32 max_nodes = 1u << node_count_shift;
            ASSERT(max_nodes < (u32)cINVALID_INDEX);

            const u64 bookkeeping_bytes     = (u64)max_nodes * (sizeof(chain_t) + sizeof(dlnode_t));
            const u64 bookkeeping_num_pages = (bookkeeping_bytes + page_size - 1) >> page_size_shift;
            ASSERT(bookkeeping_num_pages <= 0xffu);
            allocator->m_bookkeeping_num_pages = (u8)bookkeeping_num_pages;

            allocator->m_total_minsize_segments = (u32)(address_space_size >> segment_minsize_shift);
            const u64 reserve_size_bytes        = ((u64)allocator->m_total_minsize_segments << segment_minsize_shift) + ((u64)allocator->m_bookkeeping_num_pages << page_size_shift);
            allocator->m_base_address           = (byte*)v_alloc_reserve(reserve_size_bytes);
            if (allocator->m_base_address == nullptr)
            {
                g_memset(allocator, 0, sizeof(allocator_t));
                for (u32 i = 0; i < 32; ++i)
                    allocator->m_free_list_heads[i] = cINVALID_INDEX;
                return;
            }

            allocator->m_segment_minsize_shift = segment_minsize_shift;
            allocator->m_segment_maxsize_shift = segment_maxsize_shift;
            allocator->m_pagesize_shift        = page_size_shift;

            allocator->m_chain = (chain_t*)(allocator->m_base_address + ((u64)allocator->m_total_minsize_segments << segment_minsize_shift));
            allocator->m_free  = (dlnode_t*)((byte*)allocator->m_chain + max_nodes * sizeof(chain_t));

            // Commit the bookkeeping pages for the chain and free list nodes
            const uint_t bookkeeping_commit_size = (uint_t)allocator->m_bookkeeping_num_pages << page_size_shift;
            if (!v_alloc_commit(allocator->m_chain, bookkeeping_commit_size))
            {
                v_alloc_release(allocator->m_base_address, reserve_size_bytes);
                g_memset(allocator, 0, sizeof(allocator_t));
                for (u32 i = 0; i < 32; ++i)
                    allocator->m_free_list_heads[i] = cINVALID_INDEX;
                return;
            }

            // Size-class index for the top-level (largest) segments.
            const u32 top_level     = size_class_count;
            const u32 num_top_nodes = (u32)(address_space_size >> segment_maxsize_shift);

            allocator->m_free_list_heads[top_level] = 0;

            const u16 step = (u16)(1u << (segment_maxsize_shift - segment_minsize_shift));
            for (u32 i = 0; i < max_nodes; ++i)
            {
                allocator->m_free[i].m_next = cINVALID_INDEX;
                allocator->m_free[i].m_prev = cINVALID_INDEX;
            }
            for (u32 i = 0; i < num_top_nodes; ++i)
            {
                const u16 icurrent = (u16)(i * step);
                const u16 iprev    = (i > 0) ? icurrent - step : cINVALID_INDEX;
                const u16 inext    = (i < num_top_nodes - 1) ? icurrent + step : cINVALID_INDEX;

                chain_t* current                   = &allocator->m_chain[icurrent];
                current->m_prev                    = iprev;
                current->m_next                    = inext;
                current->m_flags                   = set_free(0);
                current->m_flags                   = set_side(current->m_flags, i & 1);
                current->m_committed_pages         = 0;
                allocator->m_free[icurrent].m_next = inext;
                allocator->m_free[icurrent].m_prev = iprev;
            }
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
            if (allocator->m_base_address == nullptr)
                return;
            u64 total_size = (u64)allocator->m_total_minsize_segments << allocator->m_segment_minsize_shift;
            total_size += (u64)allocator->m_bookkeeping_num_pages << allocator->m_pagesize_shift;
            const bool released = v_alloc_release(allocator->m_base_address, total_size);
            ASSERT(released);
            g_memset(allocator, 0, sizeof(allocator_t));
            for (u32 i = 0; i < 32; ++i)
                allocator->m_free_list_heads[i] = cINVALID_INDEX;
        }

    }  // namespace nsegment
}  // namespace ncore

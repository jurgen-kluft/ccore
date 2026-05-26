#include "ccore/c_arena.h"
#include "ccore/c_binmap1.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"

#include "ccore/c_segment.h"

namespace ncore
{
    namespace nsegment
    {
        enum node_state_t
        {
            STATE_FREE      = 0,  // 00: Entirely free
            STATE_PARTIAL   = 1,  // 01: Sub-blocks are allocated
            STATE_ALLOCATED = 2,  // 10: Fully allocated at this level
            STATE_SPLIT     = 3   // 11: Explicitly split down
        };

        // --- Internal Utilities ---

        static inline node_state_t buddy_get_state(const buddy_t* alloc, u32 node_idx)
        {
            u32 bit_pos = node_idx * 2;
            return (node_state_t)((alloc->m_bitmap[bit_pos / 64] >> (bit_pos % 64)) & 3);
        }

        static inline void buddy_set_state(buddy_t* alloc, u32 node_idx, node_state_t state)
        {
            u32 bit_pos               = node_idx * 2;
            u32 word_idx              = bit_pos / 64;
            u32 shift                 = bit_pos % 64;
            u64 mask                  = 3ULL << shift;
            alloc->m_bitmap[word_idx] = (alloc->m_bitmap[word_idx] & ~mask) | ((u64)state << shift);
        }

        // --- Public API ---

        u64 buddy_get_metadata_size(u64 pool_size, u32 min_block_size)
        {
            u64 total_leaves = pool_size / min_block_size;
            u64 total_nodes  = (total_leaves * 2) - 1;
            return ((total_nodes * 2 + 7) / 8 + 7) & ~7ULL;  // 8-byte aligned
        }

        void buddy_init(buddy_t* alloc, void* raw_metadata_buffer, void* raw_pool_buffer, u64 pool_size, u32 min_block_size)
        {
            alloc->m_pool_start     = (u8*)raw_pool_buffer;
            alloc->m_bitmap         = (u64*)raw_metadata_buffer;
            alloc->m_min_block_size = min_block_size;

            // STRICTLY bound to the real physical limits
            alloc->m_total_leaves = (u32)(pool_size / min_block_size);

            // Floor calculation (log2 rounded DOWN) ensures we never exceed pool memory
            alloc->m_max_levels = 32 - math::countLeadingZeros(alloc->m_total_leaves);

            // Wipe bitmap to 0 (all nodes initially free)
            const u64 metadata_bytes = buddy_get_metadata_size(pool_size, min_block_size);
            for (u64 i = 0; i < metadata_bytes / 8; ++i)
            {
                alloc->m_bitmap[i] = 0;
            }

            // Initialize free counters: Level 0 (root) has 1 free block. All other levels start at 0.
            // Smaller blocks only "appear" as free when a larger block gets split.
            alloc->m_free_counts[0] = 1;
            for (u32 i = 1; i < alloc->m_max_levels; ++i)
            {
                alloc->m_free_counts[i] = 0;
            }
        }

        void* buddy_alloc(buddy_t* alloc, u64 size)
        {
            if (size == 0)
                return nullptr;

            if (size < alloc->m_min_block_size)
                size = alloc->m_min_block_size;

            size = math::ceilpo2(size);

            const u32 target_segments = (u32)(size / alloc->m_min_block_size);
            const u32 target_level    = (alloc->m_max_levels - 1) - (31 - math::countLeadingZeros(target_segments));

            // --- OPTIMIZATION: Early O(1) Check ---
            // Scan upward from target_level to see if ANY available larger blocks can be split
            bool space_available = false;
            for (i32 lvl = (i32)target_level; lvl >= 0; --lvl)
            {
                if (alloc->m_free_counts[lvl] > 0)
                {
                    space_available = true;
                    break;
                }
            }
            if (!space_available)
                return nullptr;  // Fast out-of-memory bail

            u32 node_idx      = 0;
            u32 current_level = 0;

            // Descend tree to find/split blocks
            while (current_level < target_level)
            {
                node_state_t state = buddy_get_state(alloc, node_idx);

                if (state == STATE_FREE)
                {
                    // We are splitting a free block: update counter for the current level
                    alloc->m_free_counts[current_level]--;

                    buddy_set_state(alloc, node_idx, STATE_SPLIT);
                    node_idx = node_idx * 2 + 1;  // Pick left child

                    // The right child is now an available free block at the next level
                    alloc->m_free_counts[current_level + 1] += 2;  // Both left and right children are free after the split
                    current_level++;
                }
                else if (state == STATE_SPLIT || state == STATE_PARTIAL)
                {
                    u32 left_child  = node_idx * 2 + 1;
                    u32 right_child = node_idx * 2 + 2;

                    if (current_level + 1 == target_level)
                    {
                        // At the level just above target: the child we pick must itself be FREE,
                        // since a SPLIT/PARTIAL child at target depth is already in use.
                        if (buddy_get_state(alloc, left_child) == STATE_FREE)
                            node_idx = left_child;
                        else if (buddy_get_state(alloc, right_child) == STATE_FREE)
                            node_idx = right_child;
                        else
                            return nullptr;
                    }
                    else
                    {
                        // Still above target: skip fully-allocated children, prefer left.
                        node_state_t left_state  = buddy_get_state(alloc, left_child);
                        node_state_t right_state = buddy_get_state(alloc, right_child);
                        if (left_state != STATE_ALLOCATED)
                            node_idx = left_child;
                        else if (right_state != STATE_ALLOCATED)
                            node_idx = right_child;
                        else
                            return nullptr;
                    }
                    current_level++;
                }
                else
                {
                    return nullptr;
                }
            }

            // Finalize allocation
            buddy_set_state(alloc, node_idx, STATE_ALLOCATED);
            alloc->m_free_counts[target_level]--;  // Decrement the target level counter

            // Propagate changes upwards
            u32 parent = node_idx;
            while (parent > 0)
            {
                parent = (parent - 1) / 2;
                if (buddy_get_state(alloc, parent) != STATE_SPLIT)
                {
                    buddy_set_state(alloc, parent, STATE_PARTIAL);
                }
            }

            u32 level_start_node = (1 << target_level) - 1;
            return alloc->m_pool_start + ((u64)(node_idx - level_start_node) * size);
        }

        void buddy_free(buddy_t* alloc, void* ptr)
        {
            if (!ptr)
                return;

            u64 offset         = (u8*)ptr - alloc->m_pool_start;
            u32 local_leaf_idx = (u32)(offset / alloc->m_min_block_size);
            u32 node_idx       = ((1 << (alloc->m_max_levels - 1)) - 1) + local_leaf_idx;

            while (node_idx > 0 && buddy_get_state(alloc, node_idx) != STATE_ALLOCATED)
            {
                node_idx = (node_idx - 1) / 2;
            }

            if (buddy_get_state(alloc, node_idx) != STATE_ALLOCATED)
                return;

            // Deduce current node level mathematically from its index
            // level = floor(log2(node_idx + 1))
            // u32 current_level = 63 - BIT_CLZ64(node_idx + 1);
            u32 current_level = ((sizeof(node_idx) * 8) - 1) - math::countLeadingZeros(node_idx + 1);

            buddy_set_state(alloc, node_idx, STATE_FREE);
            alloc->m_free_counts[current_level]++;  // Increment counter for newly freed block

            // Coalesce upwards
            while (node_idx > 0)
            {
                u32 buddy_idx  = ((node_idx - 1) ^ 1) + 1;
                u32 parent_idx = (node_idx - 1) / 2;

                if (buddy_get_state(alloc, buddy_idx) == STATE_FREE)
                {
                    // Both are free! They collapse into a single free block one level up.
                    // Remove both individual counts from this level
                    alloc->m_free_counts[current_level] -= 2;

                    buddy_set_state(alloc, parent_idx, STATE_FREE);
                    current_level--;

                    // Add the newly collapsed single block to the parent level count
                    alloc->m_free_counts[current_level]++;
                    node_idx = parent_idx;
                }
                else
                {
                    buddy_set_state(alloc, parent_idx, STATE_PARTIAL);
                    break;
                }
            }
        }

    }  // namespace nsegment
}  // namespace ncore

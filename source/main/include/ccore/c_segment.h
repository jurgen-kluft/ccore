#ifndef __CCORE_SEGMENT_H__
#define __CCORE_SEGMENT_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    namespace nsegment
    {
        void initialize(uint_t address_space_size = 16ull * 1024 * 1024 * 1024, uint_t segment_min_pages = 1, uint_t segment_max_pages = 65536);

        struct allocator_t
        {
            byte*  m_base_address;       // base address of the virtual address space
            uint_t m_size;               // size of the virtual address space in bytes
            u32    m_segment_min_pages;  // minimum number of pages in a segment
            u32    m_segment_max_pages;  // maximum number of pages in a segment
        };

        void* allocate(u64 size, u32* out_block_num_pages);
        void  deallocate(void* ptr);

        void commit(void* ptr, u32 num_pages);    // num_pages must be <= the number of pages in the block
        void decommit(void* ptr, u32 num_pages);  // num_pages must be <= the number of pages in the block

        // Max tree depth supported by 64-bit offset indexing
        const i32 cBUDDY_MAX_LEVELS = 64;

        struct buddy_t
        {
            u8*  m_pool_start;      // Pointer to managed memory payloadG
            u64* m_bitmap;          // Pointer to the flat 2-bit tracking array
            u32  m_max_levels;      // Total depth of the tree
            u32  m_min_block_size;  // Smallest block size (leaf size)
            u32  m_total_leaves;    // Number of blocks at the bottom level

            // Tracks the exact count of completely free (STATE_FREE) blocks at each level
            u32 m_free_counts[cBUDDY_MAX_LEVELS];
        };

        void  buddy_init(buddy_t* alloc, void* raw_metadata_buffer, void* raw_pool_buffer, u64 pool_size, u32 min_block_size);
        void* buddy_alloc(buddy_t* alloc, u64 size);
        void  buddy_free(buddy_t* alloc, void* ptr);

        u64 buddy_get_metadata_size(u64 pool_size, u32 min_block_size);

    }  // namespace nsegment
}  // namespace ncore

#endif  // __CCORE_SEGMENT_H__

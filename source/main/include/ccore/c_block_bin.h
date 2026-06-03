#ifndef __CCORE_BLOCK_BIN_H__
#define __CCORE_BLOCK_BIN_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

namespace ncore
{
    struct arena_t;

    // A bbin manages a virtual address range that is divided into fixed-size blocks.
    // It tracks which blocks are used and which are free using a list.
    // An item is allocated taking the current active head block, and it is removed
    // from the active list and the next free block becomes the new head block.
    // If there is no free block available in the list, a new block is allocated and
    // added to the list.

    // Note: Block size is configurable
    // Note; Reserved size must be a multiple of block size

    struct bbin_t
    {
        void*    m_address_base;            // base address of the virtual address range managed by this bin
        uint_t   m_address_size;            // total size of the virtual address range managed by this bin
        arena_t* m_blocks;                  // memory for array of bblock_t
        u32      m_block_count;             // number of blocks currently allocated
        u32      m_block_max_count;         // maximum number of blocks that can be allocated
        u32      m_block_free_list_head;    // head of the list of empty blocks
        u32      m_block_used_list_head;    // head of the list of full blocks
        u32      m_block_free_list_size;    // number of blocks in the free block list
        u32      m_block_used_list_size;    // number of blocks in the full block list
        u8       m_page_size_shift;         // page size in shift (e.g. 12 for 4 KiB page size)
        u8       m_block_size_shift;        // block size in shift (e.g. 14 for 16 KiB block size)
        b8       m_ownership;               // do we own the reserved address space?
        u8       m_reserved1;               // reserved for future use
    };

    void  bin_setup(bbin_t* bin, uint_t reserved_size, u32 block_size);                      // 16 KiB <= block size <= 512 MiB
    void  bin_setup(bbin_t* bin, void* base_address, uint_t reserved_size, u32 block_size);  // 16 KiB <= block size <= 512 MiB
    void  bin_destroy(bbin_t* bin);                                                          // destroy the bin
    u32   bin_size(bbin_t const* bin);                                                       // number of items currently allocated in the bin
    void* bin_alloc(bbin_t* bin, u32 size);                                                  // allocate an item
    void  bin_free(bbin_t* bin, void* item);                                                 // free an item back to the bin

}  // namespace ncore

#endif  // __CCORE_BLOCK_BIN_H__

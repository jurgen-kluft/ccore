#ifndef __CCORE_CHUNK_BIN_H__
#define __CCORE_CHUNK_BIN_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
    #pragma once
#endif

namespace ncore
{
    struct arena_t;

    // A cbin manages a virtual address range that is divided into fixed-size chunks.
    // It tracks which chunks are used and which are free using a list.
    // Items are allocated from the current active head chunk, and when the chunk is
    // full it is removed from the active list and the next free chunk becomes the new
    // head chunk.
    // If there is no free chunk available in the list, a new chunk is allocated and
    // added to the list.

    // Note: Chunk size is fixed at 16 KiB
    // Note; Reserved size must be a multiple of chunk size

    struct cbin_t
    {
        void*    m_address_base;            // base address of the virtual address range managed by this bin
        uint_t   m_address_size;            // total size of the virtual address range managed by this bin
        arena_t* m_chunks;                  // memory for array of cchunk_t
        arena_t* m_layer1;                  // memory for layer1 of nbitvec10 for all chunks
        u16      m_chunk_max_count;         // maximum number of chunks that can be allocated
        u16      m_chunk_free_list_head;    // head of the list of empty chunks
        u16      m_chunk_active_list_head;  // head of the list of active chunks
        u16      m_chunk_full_list_head;    // head of the list of full chunks
        u16      m_chunk_free_list_size;    // number of chunks in the free chunk list
        u16      m_chunk_active_list_size;  // number of chunks in the active chunk list
        u16      m_chunk_full_list_size;    // number of chunks in the full chunk list
        u16      m_max_items_per_chunk;     // total number of items that can be allocated in a chunk
        u16      m_item_sizeof;             // sizeof(item)
        u16      m_layer1_num_u64;          // number of u32 needed for layer1 of nbitvec10 for a single chunk
        u8       m_chunk_size_shift;        // chunk size in shift (e.g. 14 for 16 KiB chunk size)
        b8       m_ownership;               // do we own the reserved address space?
        u16      m_reserved0;               // reserved for future use
        u32      m_total_items_count;       // number of items currently allocated
        u32      m_reserved1;               // reserved for future use
    };

    void  bin_setup(cbin_t* bin, uint_t reserved_size, u16 item_sizeof);                      // 8 <= item size <= 32 KiB
    void  bin_setup(cbin_t* bin, void* base_address, uint_t reserved_size, u16 item_sizeof);  // 8 <= item size <= 32 KiB
    void  bin_destroy(cbin_t* bin);                                                           // destroy the bin
    u32   bin_size(cbin_t const* bin);                                                        // number of items currently allocated in the bin
    void* bin_alloc(cbin_t* bin);                                                             // allocate an item from the bin
    void  bin_free(cbin_t* bin, void* item);                                                  // free an item back to the bin

}  // namespace ncore

#endif  // __CCORE_CHUNK_BIN_H__

#ifndef __CCORE_CHUNKS_H__
#define __CCORE_CHUNKS_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_bin.h"

namespace ncore
{
    struct cchunk_t;

    // A cbin manages a virtual address range that is divided into fixed-size chunks.
    // It tracks which chunks are used and which are free using a list.
    // Items are allocated from the current active head chunk, and when the chunk is
    // full it is removed from the active list and the next free chunk becomes the new
    // head chunk.
    // If there is no free chunk available in the list, a new chunk is allocated and
    // added to the list.

    // Note: Items per chunk must be >= 4 and <= 32768
    // Note: Chunk size must be a power of 2
    // Note; Reserved size must be a multiple of chunk size

    struct cbin_t
    {
        void*    m_address_base;            // base address of the virtual address range managed by this bin
        uint_t   m_address_size;            // total size of the virtual address range managed by this bin
        arena_t* m_chunks;                  // memory for array of cchunk_t
        u32      m_chunk_count;             // number of chunks currently allocated
        u32      m_chunk_free_list_head;    // head of the list of empty chunks
        u32      m_chunk_active_list_head;  // head of the list of active chunks
        u32      m_chunk_full_list_head;    // head of the list of full chunks
        u32      m_chunk_free_list_size;    // number of chunks in the free chunk list
        u32      m_chunk_active_list_size;  // number of chunks in the active chunk list
        u32      m_chunk_full_list_size;    // number of chunks in the full chunk list
        u32      m_total_items_count;       // number of items currently allocated
        u32      m_item_sizeof;             // sizeof(item)
        u16      m_max_items_per_chunk;     // total number of items that can be allocated in a chunk
        u8       m_chunk_size_shift;        // chunk size shift, e.g. 16 for 64 KiB chunks
    };

    void  bin_setup(cbin_t* bin, uint_t reserved_size, u32 chunk_size, u32 item_sizeof);
    void  bin_destroy(cbin_t* bin);           // destroy the bin
    u32   bin_size(cbin_t const * bin);       // number of items currently allocated in the bin
    void* bin_alloc(cbin_t* bin);             // allocate an item from the bin
    void  bin_free(cbin_t* bin, void* item);  // free an item back to the bin

}  // namespace ncore

#endif  // __CCORE_CHUNKS_H__

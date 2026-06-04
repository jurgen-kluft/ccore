#ifndef __CCORE_CHUNK_BIN_H__
#define __CCORE_CHUNK_BIN_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
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
    // Note: Maximum number of chunks is 4096
    // Note; Reserved size must be a multiple of chunk size

    struct cchunk_t;

    struct cbin_t
    {
        void*    m_address_base;         // base address of the virtual address range managed by this bin
        uint_t   m_address_size;         // total size of the virtual address range managed by this bin
        arena_t* m_arena;                // arena for chunk 'free', 'active', 'full' bitvectors and chunk_t[]
        u16      m_chunk_count;          //
        u16      m_chunk_max_count;      // maximum number of chunks that can be allocated
        u16      m_chunk_free_index;     //
        u16      m_chunk_max_items;      // total number of items that can be allocated in a chunk
        u16      m_sizeof_item;          // sizeof(item)
        u16      m_chunk_entry_num_u64;  // number of u64 needed for sizeof(chunk_t) + chunk (layer0 + layer1)
        u16      m_bitvec_num_u64;       //
        u16      m_num_free_chunks;      //
        u16      m_num_active_chunks;    //
        u8       m_chunk_size_shift;     // chunk size in shift (e.g. 14 for 16 KiB chunk size)
        b8       m_ownership;            // do we own the reserved address space?
        u32      m_total_items_count;    // number of items currently allocated
    };

    void  bin_setup(cbin_t* bin, uint_t reserved_size, u16 item_sizeof);                      // 8 <= item size <= 32 KiB
    void  bin_setup(cbin_t* bin, void* base_address, uint_t reserved_size, u16 item_sizeof);  // 8 <= item size <= 32 KiB
    void  bin_destroy(cbin_t* bin);                                                           // destroy the bin
    u32   bin_size(cbin_t const * bin);                                                       // number of items currently allocated in the bin
    void* bin_alloc(cbin_t* bin);                                                             // allocate an item from the bin
    void  bin_free(cbin_t* bin, void* item);                                                  // free an item back to the bin

}  // namespace ncore

#endif  // __CCORE_CHUNK_BIN_H__

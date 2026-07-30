#ifndef __CCORE_CHUNK_BIN_H__
#define __CCORE_CHUNK_BIN_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    // A cbin manages a virtual address range that is divided into fixed-size chunks.
    // It tracks which chunks are used and which are free using a list.
    // Items are allocated from the current active head chunk, and when the chunk is
    // full it is removed from the active list and the next free chunk becomes the new
    // head chunk. If there is no free chunk available in the list, a new chunk is
    // allocated and added to the list.

    // Note: Maximum number of chunks is 65536
    // Note; Reserved size must be a multiple of chunk size

    // Note: Bin metadata is supplied and owned by the caller
    struct cbin_t;

    u32     bin_calculate_size(uint_t base_size, u16 item_sizeof);  // number of pages needed for the bin and chunk metadata
    cbin_t* bin_setup(void* bin_address, u32 bin_size_in_pages, void* base_address, uint_t base_size, u16 item_sizeof);
    void    bin_destroy(cbin_t* bin);           // destroy the bin
    u32     bin_size(cbin_t const * bin);       // number of items currently allocated in the bin
    void*   bin_alloc(cbin_t* bin);             // allocate an item from the bin
    void    bin_free(cbin_t* bin, void* item);  // free an item back to the bin

}  // namespace ncore

#endif  // __CCORE_CHUNK_BIN_H__

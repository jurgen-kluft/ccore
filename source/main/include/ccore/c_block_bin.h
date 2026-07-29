#ifndef __CCORE_BLOCK_BIN_H__
#define __CCORE_BLOCK_BIN_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    // A bbin manages a virtual address range that is divided into fixed-size blocks.
    // Freed blocks are tracked using an intrusive list stored in the block metadata.

    // Note: Block size is configurable
    // Note: Maximum number of blocks is 65535
    // Note: Reserved size must be a multiple of block size
    // Note: Bin metadata is supplied and owned by the caller
    struct bbin_t;

    u32     bin_calculate_size(uint_t reserved_size, u32 block_size);                                         // number of pages needed for the bin and block metadata
    bbin_t* bin_setup(void* bin_address, u32 bin_size_in_pages, void* base_address, uint_t reserved_size, u32 block_size);  // 16 KiB <= block size <= 512 MiB
    void    bin_destroy(bbin_t* bin);                                                                         // destroy the bin
    u32     bin_size(bbin_t const* bin);                                                                      // number of items currently allocated in the bin
    void*   bin_alloc(bbin_t* bin, u32 size);                                                                 // allocate an item
    void    bin_free(bbin_t* bin, void* item);                                                                // free an item back to the bin

}  // namespace ncore

#endif  // __CCORE_BLOCK_BIN_H__

#include "ccore/c_arena.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_binmap1.h"

#include "ccore/c_region.h"

namespace ncore
{
    namespace nregion
    {
        // Block size = 4MB
        // Blocks are a header and exist in the address space of the region
        struct block_t
        {
            u64 m_bin0;        // binmap for free/used items
            u32 m_free_index;  // next free item index (this also is used to commit pages)
            // followed by bin1/bin2 arrays
            // followed by items
        };

        // A region holds multiple blocks
        // Reserved address space = block capacity * 4MB
        struct region_t
        {
            arena_t* m_arena;           // we use an arena to back the region
            u32      m_block_capacity;  // maximum number of blocks in this region
            // binmap, array of bin0 u64 for free/used blocks, and active blocks for / 16 / 32 / 64 / 128 / 256 / 512 / 1024
            // binmap, array of bin1 array's for free/used blocks, and active blocks for / 16 / 32 / 64 / 128 / 256 / 512 / 1024
        };

        region_t* create(u32 max_number_of_blocks)
        {
            // create the region
            // initialize the binmap to track free blocks

            return nullptr;
        }

    }  // namespace nregion
}  // namespace ncore

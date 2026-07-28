#include "ccore/c_arena.h"
#include "ccore/c_bitvec.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"

#include "ccore/c_block_bin.h"

namespace ncore
{

    // 888888b.   888      .d88888b.   .d8888b.  888    d8P
    // 888  "88b  888     d88P" "Y88b d88P  Y88b 888   d8P
    // 888  .88P  888     888     888 888    888 888  d8P
    // 8888888K.  888     888     888 888        888d88K
    // 888  "Y88b 888     888     888 888        8888888b
    // 888    888 888     888     888 888    888 888  Y88b
    // 888   d88P 888     Y88b. .d88P Y88b  d88P 888   Y88b
    // 8888888P"  88888888 "Y88888P"   "Y8888P"  888    Y88b

    struct bblock_t
    {
        u32 m_pages_committed;  // number of pages committed by this block
    };

    static inline void s_block_init(bbin_t* bin, bblock_t* block)
    {
        block->m_pages_committed = 0;  // no pages committed yet
    }

    static void* s_commit_block(bbin_t* bin, bblock_t* block, u32 block_index, u32 item_size)
    {
        // commit enough pages for this block to cover the item size, if not already committed
        const u32 block_size    = (u32)1 << bin->m_block_size_shift;
        byte*     block_address = (byte*)bin->m_address_base + ((uint_t)block_index * block_size);

        const u32 required_pages = (item_size + ((u32)1 << bin->m_page_size_shift) - 1) >> bin->m_page_size_shift;
        if (block->m_pages_committed < required_pages)
        {
            const u32 pages_to_commit = required_pages - block->m_pages_committed;

            v_alloc_commit(block_address + (block->m_pages_committed << bin->m_page_size_shift), pages_to_commit << bin->m_page_size_shift);
            block->m_pages_committed += pages_to_commit;
            return block_address;
        }
        else if (block->m_pages_committed > required_pages)
        {
            // decommit pages if the block has more pages committed than needed for the item size
            const u32 pages_to_decommit = block->m_pages_committed - required_pages;

            v_alloc_decommit(block_address + (required_pages << bin->m_page_size_shift), pages_to_decommit << bin->m_page_size_shift);
            block->m_pages_committed -= pages_to_decommit;
            return block_address;
        }

        return block_address;
    }

    // free an item back to the block, this is called when an item is freed
    static void s_release_block(bbin_t* bin, bblock_t* block, u32 block_index, void* item_ptr)
    {
        const u32 block_size = (u32)1 << bin->m_block_size_shift;
        if (block->m_pages_committed > 0)
        {
            byte* block_address = (byte*)bin->m_address_base + ((uint_t)block_index * block_size);
            v_alloc_decommit(block_address, (block->m_pages_committed) << bin->m_page_size_shift);
            block->m_pages_committed = 0;
        }
    }

    //        d8888 888      888      .d88888b.   .d8888b.
    //       d88888 888      888     d88P" "Y88b d88P  Y88b
    //      d88P888 888      888     888     888 888    888
    //     d88P 888 888      888     888     888 888
    //    d88P  888 888      888     888     888 888
    //   d88P   888 888      888     888     888 888    888
    //  d8888888888 888      888     Y88b. .d88P Y88b  d88P
    // d88P     888 88888888 88888888 "Y88888P"   "Y8888P"

    void* bin_alloc(bbin_t* bin, u32 item_size)
    {
        u64* layer0 = narena::base_ptr_as<u64>(bin->m_arena);
        u64* layer1 = layer0 + 1;
        u64* layer2 = layer1 + bin->m_layer1_size;

        bblock_t* active_block_ptr   = nullptr;
        s32       active_block_index = -1;
        if (bin->m_block_count < bin->m_block_free_index)
        {
            active_block_index = nbitvec18::find_free(layer0, layer1, layer2, bin->m_block_free_index);
            ASSERT(active_block_index >= 0 && (u32)active_block_index < bin->m_block_free_index);
            bblock_t* block_array = bin->m_blocks;
            active_block_ptr      = &block_array[active_block_index];
            s_block_init(bin, active_block_ptr);

            // This block is now used, mark it as used in the bit vector
            nbitvec18::set_used(layer0, layer1, layer2, bin->m_block_free_index, active_block_index);
        }
        else
        {
            if (bin->m_block_free_index >= bin->m_block_max_count)
                return nullptr;

            active_block_index = bin->m_block_free_index++;
            active_block_ptr   = g_allocate<bblock_t>(bin->m_arena);

            // This block is now used, mark it as used in the bit vector
            nbitvec18::tick_used_lazy(layer0, layer1, layer2, bin->m_block_free_index, active_block_index);

            s_block_init(bin, active_block_ptr);
        }

        bin->m_block_count += 1;
        void* item = s_commit_block(bin, active_block_ptr, active_block_index, item_size);
        if (item == nullptr)
            return nullptr;

        return item;
    }

    // 8888888888 8888888b.  8888888888 8888888888
    // 888        888   Y88b 888        888
    // 888        888    888 888        888
    // 8888888    888   d88P 8888888    8888888
    // 888        8888888P"  888        888
    // 888        888 T88b   888        888
    // 888        888  T88b  888        888
    // 888        888   T88b 8888888888 8888888888

    void bin_free(bbin_t* bin, void* ptr)
    {
        ASSERT(ptr != nullptr && ptr >= bin->m_address_base && ptr < (byte*)bin->m_address_base + bin->m_address_size);

        // Find the block this item belongs to
        const u8  block_size_shift = bin->m_block_size_shift;
        const u32 block_index      = (u32)((uint_t)((byte*)ptr - (byte*)bin->m_address_base) >> block_size_shift);
        bblock_t* block            = &bin->m_blocks[block_index];
        s_release_block(bin, block, block_index, ptr);
        bin->m_block_count -= 1;

        // This block is now free, mark it as free in the bit vector
        u64* layer0 = narena::base_ptr_as<u64>(bin->m_arena);
        u64* layer1 = layer0 + 1;
        u64* layer2 = layer1 + bin->m_layer1_size;
        nbitvec18::set_free(layer0, layer1, layer2, bin->m_block_free_index, block_index);
    }

    //  .d8888b.  8888888888 88888888888 888     888 8888888b.
    // d88P  Y88b 888            888     888     888 888   Y88b
    // Y88b.      888            888     888     888 888    888
    //  "Y888b.   8888888        888     888     888 888   d88P
    //     "Y88b. 888            888     888     888 8888888P"
    //       "888 888            888     888     888 888
    // Y88b  d88P 888            888     Y88b. .d88P 888
    //  "Y8888P"  8888888888     888      "Y88888P"  888

    void  bin_setup(bbin_t* bin, void* base_address, uint_t reserved_size, u32 block_size)
    {
        // Note: Block size >= 16 KiB
        ASSERT(block_size >= (16 * cKB));

        bin->m_page_size_shift = v_alloc_get_page_size_shift();

        // Find the appropriate block size based on the block size, this is done by
        // calculating the block size shift, which must be at least the page size shift.
        const u8 block_size_shift  = math::max((u8)math::ilog2(math::ceilpo2(block_size)), (u8)bin->m_page_size_shift);
        ASSERT(block_size_shift > 0);
        const u32 block_size_aligned = (u32)1 << block_size_shift;

        // the maximum number of blocks is calculated based on the reserved
        // size and the calculated block size, but must be < 65536.
        const u32 max_block_count = (u32)(reserved_size / block_size_aligned);
        ASSERT(max_block_count > 0 && max_block_count < 65536);

        if (base_address != nullptr)
        {
            bin->m_address_base = base_address;
            bin->m_address_size = reserved_size;
            bin->m_ownership    = false;
        }
        else
        {
            bin->m_address_base = v_alloc_reserve(reserved_size);
            bin->m_address_size = reserved_size;
            bin->m_ownership    = true;
        }

        // Force to always have 3 layers so that we use nbitvec18
        const u32 bin2_size = (max_block_count + 63) / 64;
        const u32 bin1_size = (bin2_size + 63) / 64;
        const u32 bin0_size = 1;

        const u32 bitvec_bytes = ((bin0_size + bin1_size + bin2_size) * sizeof(u64));
        const u32 blocks_bytes = max_block_count * sizeof(bblock_t);
        bin->m_arena           = narena::new_arena(bitvec_bytes + blocks_bytes, bitvec_bytes);

        u64* layer0        = g_allocate_array<u64>(bin->m_arena, bin0_size);
        u64* layer1        = g_allocate_array<u64>(bin->m_arena, bin1_size);
        u64* layer2        = g_allocate_array<u64>(bin->m_arena, bin2_size);
        bin->m_layer1_size = bin1_size;

        nbitvec18::setup_used_lazy(layer0, layer1, layer2, max_block_count);

        bin->m_block_count      = 0;
        bin->m_block_max_count  = (u16)max_block_count;
        bin->m_block_free_index = 0;
        bin->m_blocks           = narena::current_ptr_as<bblock_t>(bin->m_arena);
        bin->m_block_size_shift = block_size_shift;
    }

    void  bin_setup(bbin_t* bin, uint_t reserved_size, u32 block_size) { bin_setup(bin, nullptr, reserved_size, block_size); }

    u32 bin_size(bbin_t const * bin)
    {
        // The global item count
        return bin->m_block_count;
    }

    // 8888888b.  8888888888 .d8888b. 88888888888 8888888b.   .d88888b. Y88b   d88P
    // 888  "Y88b 888       d88P  Y88b    888     888   Y88b d88P" "Y88b Y88b d88P
    // 888    888 888       Y88b.         888     888    888 888     888  Y88o88P
    // 888    888 8888888    "Y888b.      888     888   d88P 888     888   Y888P
    // 888    888 888           "Y88b.    888     8888888P"  888     888    888
    // 888    888 888             "888    888     888 T88b   888     888    888
    // 888  .d88P 888       Y88b  d88P    888     888  T88b  Y88b. .d88P    888
    // 8888888P"  8888888888 "Y8888P"     888     888   T88b  "Y88888P"     888

    void bin_destroy(bbin_t* bin)
    {
        if (bin->m_arena != nullptr)
        {
            narena::destroy(bin->m_arena);
        }
        if (bin->m_address_base != nullptr)
        {
            if (bin->m_ownership)
            {
                v_alloc_release(bin->m_address_base, bin->m_address_size);
            }
        }

        g_memclr(bin, sizeof(bbin_t));
    }

}  // namespace ncore

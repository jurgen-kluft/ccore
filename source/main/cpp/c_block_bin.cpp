#include "ccore/c_arena.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"

#include "ccore/c_block_bin.h"

namespace ncore
{
    const u32 c_index32_null = 0xFFFFFFFF;

    // 888      8888888 .d8888b. 88888888888
    // 888        888  d88P  Y88b    888
    // 888        888  Y88b.         888
    // 888        888   "Y888b.      888
    // 888        888      "Y88b.    888
    // 888        888        "888    888
    // 888        888  Y88b  d88P    888
    // 88888888 8888888 "Y8888P"     888

#define DLIST_NEXT 0
#define DLIST_PREV 1

    static inline void s_dlist_insert(void* array, u32 item_size, u32& head, u32& item_count, u32 index)
    {
        u32* item        = (u32*)((byte*)array + index * item_size);
        item[DLIST_NEXT] = head;
        item[DLIST_PREV] = c_index32_null;
        if (head != c_index32_null)
        {
            u32* head_item        = (u32*)((byte*)array + head * item_size);
            head_item[DLIST_PREV] = index;
        }
        head = index;
        ++item_count;
    }

    static inline void s_dlist_remove(void* array, u32 item_size, u32& head, u32& item_count, u32 index)
    {
        u32*      item = (u32*)((byte*)array + index * item_size);
        const u32 next = item[DLIST_NEXT];
        const u32 prev = item[DLIST_PREV];
        if (next != c_index32_null)
        {
            u32* next_item        = (u32*)((byte*)array + next * item_size);
            next_item[DLIST_PREV] = prev;
        }
        if (prev != c_index32_null)
        {
            u32* prev_item        = (u32*)((byte*)array + prev * item_size);
            prev_item[DLIST_NEXT] = next;
        }
        else
        {
            head = next;
        }
        --item_count;
    }

    static inline u32 s_dlist_pop(void* array, u32 item_size, u32& head, u32& item_count)
    {
        if (head != c_index32_null)
        {
            const u32 item     = head;
            u32*      item_ptr = (u32*)((byte*)array + item * item_size);
            head               = item_ptr[DLIST_NEXT];
            if (head != c_index32_null)
            {
                u32* head_ptr        = (u32*)((byte*)array + head * item_size);
                head_ptr[DLIST_PREV] = c_index32_null;
            }
            --item_count;
            return item;
        }
        return c_index32_null;
    }

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
        u32 m_next;             // next block in the free/active block list, or c_index32_null if this is the last block in the list
        u32 m_prev;             // previous block in the free/active block list, or c_index32_null if this is the first block in the list
        u32 m_pages_committed;  // number of pages committed by this block
    };

    static inline void s_block_init(bbin_t* bin, bblock_t* block)
    {
        block->m_next            = c_index32_null;
        block->m_prev            = c_index32_null;
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
        bblock_t* active_block       = nullptr;
        u32       active_block_index = c_index32_null;

        bblock_t* block_array = narena::base_ptr_as<bblock_t>(bin->m_blocks);
        if (bin->m_block_free_list_head != c_index32_null)
        {
            active_block_index = s_dlist_pop(block_array, sizeof(bblock_t), bin->m_block_free_list_head, bin->m_block_free_list_size);
            active_block       = block_array + active_block_index;
            s_block_init(bin, active_block);
        }
        else
        {
            // No free block, need to allocate a new block
            const u32 block_free_index = (u32)(narena::current_pos(bin->m_blocks) / sizeof(bblock_t));
            if (block_free_index >= bin->m_block_max_count)
                return nullptr;

            active_block_index = block_free_index;
            active_block       = g_allocate<bblock_t>(bin->m_blocks);

            s_block_init(bin, active_block);
        }

        bin->m_block_count += 1;
        void* item = s_commit_block(bin, active_block, active_block_index, item_size);
        if (item == nullptr)
            return nullptr;

        // This block is now used, add it to the used block list
        s_dlist_insert(block_array, sizeof(bblock_t), bin->m_block_used_list_head, bin->m_block_used_list_size, active_block_index);

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

        bblock_t* block_array      = narena::base_ptr_as<bblock_t>(bin->m_blocks);
        const u8  block_size_shift = bin->m_block_size_shift;

        // Find the block this item belongs to
        const u32 block_index = (u32)((uint_t)((byte*)ptr - (byte*)bin->m_address_base) >> block_size_shift);
        bblock_t* block       = block_array + block_index;
        s_release_block(bin, block, block_index, ptr);
        bin->m_block_count -= 1;

        // This block is now free, move it to the free list
        s_dlist_remove(block_array, sizeof(bblock_t), bin->m_block_used_list_head, bin->m_block_used_list_size, block_index);
        s_dlist_insert(block_array, sizeof(bblock_t), bin->m_block_free_list_head, bin->m_block_free_list_size, block_index);
    }

    //  .d8888b.  8888888888 88888888888 888     888 8888888b.
    // d88P  Y88b 888            888     888     888 888   Y88b
    // Y88b.      888            888     888     888 888    888
    //  "Y888b.   8888888        888     888     888 888   d88P
    //     "Y88b. 888            888     888     888 8888888P"
    //       "888 888            888     888     888 888
    // Y88b  d88P 888            888     Y88b. .d88P 888
    //  "Y8888P"  8888888888     888      "Y88888P"  888

    // - sizeof(item) >=    8 B && sizeof(item) <=  128 B -> block-size =   4 KiB
    // - sizeof(item)  >  128 B && sizeof(item) <=  256 B -> block-size =   8 KiB
    // - sizeof(item)  >  256 B && sizeof(item) <=  512 B -> block-size =  16 KiB
    // - sizeof(item)  >  512 B && sizeof(item) <=  1 KiB -> block-size =  32 KiB
    // - sizeof(item)  >  1 KiB && sizeof(item) <=  2 KiB -> block-size =  64 KiB
    // - sizeof(item)  >  2 KiB && sizeof(item) <=  4 KiB -> block-size = 128 KiB
    // - sizeof(item)  >  4 KiB && sizeof(item) <=  8 KiB -> block-size = 256 KiB
    // - sizeof(item)  >  8 KiB && sizeof(item) <= 16 KiB -> block-size = 512 KiB
    // - sizeof(item)  > 16 KiB && sizeof(item) <= 32 KiB -> block-size =   1 MiB

    // clang-format off
    const static u8 s_cchunk_configs[] = {
         12, 12, 12, // 1<<0, 1<<1, 1<<2
         12, 12, 12, // 1<<3, 1<<4, 1<<5,
         12, 12, 13, // 1<<6, 1<<7, 1<<8,
         14, 15, 16, // 1<<9, 1<<10, 1<<11,
         17, 18, 19, // 1<<12, 1<<13, 1<<14,
         20, 21,     // 1<<15, 1<<16
    };
    // clang-format on

    void bin_setup(bbin_t* bin, void* base_address, uint_t reserved_size, u16 item_sizeof)
    {
        // Note: Item size >= 16 KiB
        ASSERT(item_sizeof >= (16 * cKB));

        bin->m_page_size_shift = v_alloc_get_page_size_shift();

        // Find the appropriate block size based on the item size, this is done by
        // looking up the 's_cchunk_configs' array, which has predefined block size shifts
        // for different item size ranges.
        const u8 item_size_shift  = math::max((u8)math::ilog2(math::ceilpo2(item_sizeof)), (u8)3);
        const u8 block_size_shift = math::max(s_cchunk_configs[item_size_shift], bin->m_page_size_shift);

        ASSERT(block_size_shift > 0);
        const u32 block_size = (u32)1 << block_size_shift;

        // the maximum number of chunks is calculated based on the reserved
        // size and the calculated block size, but must be < 65536.
        const u32 max_block_count = (u32)(reserved_size / block_size);
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

        bin->m_block_free_list_head = c_index32_null;
        bin->m_block_used_list_head = c_index32_null;

        bin->m_block_free_list_size = 0;
        bin->m_block_used_list_size = 0;
        bin->m_block_count          = 0;
        bin->m_block_max_count      = (u16)max_block_count;
        bin->m_blocks               = narena::new_arena(max_block_count * sizeof(bblock_t), 0);
        bin->m_block_size_shift     = block_size_shift;
    }

    void bin_setup(bbin_t* bin, uint_t reserved_size, u16 item_sizeof) { bin_setup(bin, nullptr, reserved_size, item_sizeof); }

    u32 bin_size(bbin_t const* bin)
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
        if (bin->m_blocks != nullptr)
        {
            narena::destroy(bin->m_blocks);
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

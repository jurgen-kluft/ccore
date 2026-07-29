#include "ccore/c_math.h"
#include "ccore/c_memory.h"

#include "ccore/c_block_bin.h"

namespace ncore
{
    static const u16 cINVALID_BLOCK_INDEX = (u16)~0u;

    union bblock_t
    {
        u32 m_pages_committed;  // number of pages committed by this block
        u32 m_free_next;        // index of the next free block in the list
    };

    struct bbin_t
    {
        void* m_address_base;           // base address of the virtual address range managed by this bin
        u32   m_address_size_in_pages;  // total size of the virtual address range managed by this bin
        u8    m_bin_size_in_pages;      // total size of the bin structure (+ array of blocks) in pages
        u8    m_block_size_shift;       // block size in shift (e.g. 14 for 16 KiB block size)
        u16   m_free_head;              // index of the head of the free list
        u16   m_block_count;            // number of blocks currently allocated
        u16   m_block_max_count;        // maximum number of blocks that can be allocated
        u16   m_block_free_index;       // highest free index
        u8    m_page_size_shift;        // page size in shift (e.g. 12 for 4 KiB page size)
        b8    m_ownership;              // do we own the reserved address space?
        // bblock_t* m_blocks;          // array of blocks (follows bbin_t in memory)
    };

    static inline bblock_t* s_blocks(bbin_t* bin) { return (bblock_t*)((byte*)bin + sizeof(bbin_t)); }

    static void s_add_to_list(bbin_t* bin, u16& head, u16 block_index)
    {
        bblock_t* blocks                = s_blocks(bin);
        blocks[block_index].m_free_next = head;
        head                            = block_index;
    }

    static u16 s_pop_from_list(bbin_t* bin, u16& head)
    {
        if (head == cINVALID_BLOCK_INDEX)
            return cINVALID_BLOCK_INDEX;

        bblock_t* blocks                = s_blocks(bin);
        u16       block_index           = head;
        head                            = blocks[block_index].m_free_next;
        blocks[block_index].m_free_next = cINVALID_BLOCK_INDEX;
        return block_index;
    }

    struct bbin_layout_t
    {
        u8  m_page_size_shift;
        u8  m_block_size_shift;
        u16 m_block_max_count;
        u8  m_bin_size_in_pages;
    };

    static void s_bin_calculate_size(uint_t reserved_size, u32 block_size, bbin_layout_t& layout)
    {
        ASSERT(block_size >= (16 * cKB));

        const u8  page_size_shift    = v_alloc_get_page_size_shift();
        const u8  block_size_shift   = math::max((u8)math::ilog2(math::ceilpo2(block_size)), page_size_shift);
        const u32 block_size_aligned = (u32)1 << block_size_shift;
        ASSERT(block_size_shift > 0);
        ASSERT((reserved_size % block_size_aligned) == 0);

        const u32 max_block_count = (u32)(reserved_size / block_size_aligned);
        ASSERT(max_block_count > 0 && max_block_count <= cINVALID_BLOCK_INDEX);

        const uint_t page_size     = (uint_t)1 << page_size_shift;
        const uint_t address_pages = reserved_size >> page_size_shift;
        ASSERT(address_pages <= (uint_t)0xFFFFFFFFULL);
        uint_t required_size        = sizeof(bbin_t) + ((uint_t)max_block_count * sizeof(bblock_t));
        required_size               = math::alignUp(required_size, page_size);
        const uint_t required_pages = required_size >> page_size_shift;
        ASSERT(required_pages > 0 && required_pages <= 255);

        layout.m_page_size_shift   = page_size_shift;
        layout.m_block_size_shift  = block_size_shift;
        layout.m_block_max_count   = (u16)max_block_count;
        layout.m_bin_size_in_pages = (u8)required_pages;
    }

    static void* s_commit_block(bbin_t* bin, bblock_t* block, u16 block_index, u32 item_size)
    {
        const u32 block_size    = (u32)1 << bin->m_block_size_shift;
        byte*     block_address = (byte*)bin->m_address_base + ((uint_t)block_index * block_size);

        const u32 required_pages = (item_size + ((u32)1 << bin->m_page_size_shift) - 1) >> bin->m_page_size_shift;
        if (block->m_pages_committed < required_pages)
        {
            const u32 pages_to_commit = required_pages - block->m_pages_committed;

            ASSERT(v_alloc_commit(block_address + (block->m_pages_committed << bin->m_page_size_shift), pages_to_commit << bin->m_page_size_shift));
            block->m_pages_committed += pages_to_commit;
            return block_address;
        }
        else if (block->m_pages_committed > required_pages)
        {
            const u32 pages_to_decommit = block->m_pages_committed - required_pages;

            v_alloc_decommit(block_address + (required_pages << bin->m_page_size_shift), pages_to_decommit << bin->m_page_size_shift);
            block->m_pages_committed -= pages_to_decommit;
            return block_address;
        }

        return block_address;
    }

    static void s_release_block(bbin_t* bin, bblock_t* block, u16 block_index)
    {
        const u32 block_size = (u32)1 << bin->m_block_size_shift;
        if (block->m_pages_committed > 0)
        {
            byte* block_address = (byte*)bin->m_address_base + ((uint_t)block_index * block_size);
            v_alloc_decommit(block_address, block->m_pages_committed << bin->m_page_size_shift);
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
        ASSERT(item_size <= ((u32)1 << bin->m_block_size_shift));

        u16 active_block_index;
        if (bin->m_free_head != cINVALID_BLOCK_INDEX)
        {
            active_block_index = s_pop_from_list(bin, bin->m_free_head);
        }
        else
        {
            if (bin->m_block_free_index >= bin->m_block_max_count)
                return nullptr;

            active_block_index = bin->m_block_free_index++;
        }

        bblock_t* blocks                       = s_blocks(bin);
        bblock_t* active_block                 = &blocks[active_block_index];
        active_block->m_pages_committed = 0;

        bin->m_block_count += 1;
        return s_commit_block(bin, active_block, active_block_index, item_size);
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
        const uint_t address_size = (uint_t)bin->m_address_size_in_pages << bin->m_page_size_shift;
        ASSERT(ptr != nullptr && ptr >= bin->m_address_base && ptr < (byte*)bin->m_address_base + address_size);

        const u8     block_size_shift = bin->m_block_size_shift;
        const uint_t address_offset   = (uint_t)((byte*)ptr - (byte*)bin->m_address_base);
        ASSERT((address_offset & (((uint_t)1 << block_size_shift) - 1)) == 0);
        const u16 block_index = (u16)(address_offset >> block_size_shift);
        ASSERT(block_index < bin->m_block_free_index && bin->m_block_count > 0);

        bblock_t* blocks = s_blocks(bin);
        bblock_t* block  = &blocks[block_index];
        s_release_block(bin, block, block_index);
        bin->m_block_count -= 1;

        s_add_to_list(bin, bin->m_free_head, block_index);
    }

    //  .d8888b.  8888888888 88888888888 888     888 8888888b.
    // d88P  Y88b 888            888     888     888 888   Y88b
    // Y88b.      888            888     888     888 888    888
    //  "Y888b.   8888888        888     888     888 888   d88P
    //     "Y88b. 888            888     888     888 8888888P"
    //       "888 888            888     888     888 888
    // Y88b  d88P 888            888     Y88b. .d88P 888
    //  "Y8888P"  8888888888     888      "Y88888P"  888

    u32 bin_calculate_size(uint_t reserved_size, u32 block_size)
    {
        bbin_layout_t layout;
        s_bin_calculate_size(reserved_size, block_size, layout);
        return layout.m_bin_size_in_pages;
    }

    bbin_t* bin_setup(void* bin_address, u32 bin_size_in_pages, void* base_address, uint_t reserved_size, u32 block_size)
    {
        ASSERT(bin_address != nullptr);

        bbin_layout_t layout;
        s_bin_calculate_size(reserved_size, block_size, layout);
        ASSERT(bin_size_in_pages >= layout.m_bin_size_in_pages);

        bbin_t* bin = (bbin_t*)bin_address;
        g_memclr(bin, sizeof(bbin_t));
        if (base_address != nullptr)
        {
            bin->m_address_base = base_address;
            bin->m_ownership    = false;
        }
        else
        {
            bin->m_address_base = v_alloc_reserve(reserved_size);
            ASSERT(bin->m_address_base != nullptr);
            bin->m_ownership = true;
        }

        bin->m_address_size_in_pages = (u32)(reserved_size >> layout.m_page_size_shift);
        bin->m_bin_size_in_pages     = layout.m_bin_size_in_pages;
        bin->m_block_size_shift      = layout.m_block_size_shift;
        bin->m_free_head             = cINVALID_BLOCK_INDEX;
        bin->m_block_count           = 0;
        bin->m_block_max_count       = layout.m_block_max_count;
        bin->m_block_free_index      = 0;
        bin->m_page_size_shift       = layout.m_page_size_shift;
        return bin;
    }

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
        if (bin->m_address_base != nullptr && bin->m_ownership)
        {
            const uint_t address_size = (uint_t)bin->m_address_size_in_pages << bin->m_page_size_shift;
            v_alloc_release(bin->m_address_base, address_size);
        }

        g_memclr(bin, sizeof(bbin_t));
    }

}  // namespace ncore

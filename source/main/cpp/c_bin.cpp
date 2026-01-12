#include "ccore/c_arena.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_binmap1.h"

#include "ccore/c_bin.h"

namespace ncore
{
    namespace nbin
    {
        /*
        We want to track the number of allocations per page, so we can free pages
        when they become completely unused.
        We will limit the maximum number of allocations per page to 65535, so we can use u16
        to track the number of allocations per page.
        The page tracking also has a small binmap to track which pages are completely free or used
        and to take advantage of spatial locality when requesting a new page for allocations.
        We will also limit the empty hot pages to a certain number, so we don't keep too many
        empty pages around.
        This will of course add some overhead to each bin, but it will also reduce the committed
        memory for the items, especially when the item-size is small compared to the page-size.
        We will give the user the option to specify the page-size and number of hot empty pages
        when creating the bin.
        */
        struct blocks_t
        {
            u16 m_block_count;        // number of blocks
            u16 m_empty_block_high;   // highest free
            u8  m_empty_block_count;  // number of empty blocks to keep
            u8  m_pagesize_shift;     // page-size = 1 << pagesize_shift
            u8  m_bin_level_count;    // binmap, number of levels
            u16 m_bin3_offset;        // (unit = number of u16, relative to &m_bin0)
            u16 m_bin2_offset;        // (unit = number of u16, relative to &m_bin0)
            // u16 m_bin1_offset;        // (unit = number of u16, relative to &m_bin0)
            u16 m_bin0;  // binmap level 0 (top level)
            // u16* m_count_per_block;  // count per block (at a specific offset)
        };

        struct bin_t
        {
            u32 m_items_committed_pages;    // (unit = pages) number of pages committed for items
            u32 m_items_free_index;         // current 'bit' used to allocate next free item
            u32 m_items_count;              // number of items currently in use
            u32 m_items_capacity;           // maximum number of items for this bin
            u32 m_binmap_commit_threshold;  // threshold of when to commit +1 page for binmap
            u16 m_item_size;                // size of one item
            u16 m_bin_committed_pages;      // (unit = pages) number of pages committed for bin, including binmap
            u8  m_pagesize_shift;           // page-size = 1 << pagesize_shift
            u8  m_bin_level_count;          // binmap, number of levels
            // u16 m_bin1_offset;            // (unit = number of u64, relative to &m_bin0), always == 1
            u16 m_bin2_offset;  // (unit = number of u64, relative to &m_bin0)
            u16 m_bin3_offset;  // (unit = number of u64, relative to &m_bin0)
            u64 m_bin0;         // binmap level 0 (top level)
        };

        static const int_t c_bin_hdr_max_size = sizeof(bin_t) + (((u32)64 << 18) >> 3) + (((u32)64 << 12) >> 3) + (((u32)64 << 6) >> 3);

        bin_t* make_bin(u16 item_size, u32 max_items)
        {
            const s32 page_size       = v_alloc_get_page_size();
            const s8  page_size_shift = v_alloc_get_page_size_shift();

            ASSERT(item_size < (page_size >> 1));

            const int_t items_memory_size = math::alignUp((int_t)item_size * max_items, page_size);
            const int_t bin_hdr_max_size  = math::alignUp(c_bin_hdr_max_size, page_size);

            byte* base_address = (byte*)v_alloc_reserve(bin_hdr_max_size + items_memory_size);

            // bin header commit size includes bin_t and most of the binmap data but not all
            nbinmap::layout64_t layout;
            nbinmap::compute(max_items, layout);
            int_t bin_hdr_commit_size = sizeof(bin_t);
            bin_hdr_commit_size += layout.m_bin1 * sizeof(u64);
            bin_hdr_commit_size += (layout.m_bin3 > 0) ? layout.m_bin2 * sizeof(u64) : 0;
            bin_hdr_commit_size += (layout.m_bin2 > 0 || layout.m_bin3 > 0) ? 8 * sizeof(u64) : 0;
            bin_hdr_commit_size = math::alignUp(bin_hdr_commit_size, page_size);

            if (!v_alloc_commit(base_address, bin_hdr_commit_size))
            {
                v_alloc_release(base_address, bin_hdr_max_size + items_memory_size);
                return nullptr;
            }

            const u16 items_pages_committed = 1;
            if (!v_alloc_commit(base_address + bin_hdr_max_size, (int_t)items_pages_committed << page_size_shift))
            {
                v_alloc_release(base_address, bin_hdr_max_size + items_memory_size);
                return nullptr;
            }

            bin_t* bin                   = (bin_t*)base_address;
            bin->m_items_committed_pages = items_pages_committed;
            bin->m_items_free_index      = 0;
            bin->m_items_count           = 0;
            bin->m_items_capacity        = max_items;
            bin->m_item_size             = item_size;
            bin->m_pagesize_shift        = page_size_shift;
            bin->m_bin_committed_pages   = (u8)(bin_hdr_commit_size >> page_size_shift);
            bin->m_bin_level_count       = layout.m_levels;
            bin->m_bin2_offset           = layout.m_bin0 + layout.m_bin1;
            bin->m_bin3_offset           = bin->m_bin2_offset + layout.m_bin2;
            bin->m_bin0                  = D_U64_MAX;

            const u32 bin_binx               = ((u32)sizeof(bin_t) - (u32)sizeof(u64)) + ((u32)bin->m_bin3_offset * sizeof(u64));
            const u32 items_commit_threshold = (u32)(bin_hdr_commit_size - bin_binx) * 8;
            bin->m_binmap_commit_threshold   = math::min(items_commit_threshold, bin->m_items_capacity);

            return bin;
        }

        void destroy(bin_t* bin)
        {
            const u32   page_size              = (1 << bin->m_pagesize_shift);
            const int_t bin_hdr_max_size       = math::alignUp(c_bin_hdr_max_size, page_size);
            const int_t bin_hdr_committed_size = (bin->m_bin_committed_pages << bin->m_pagesize_shift);
            const int_t items_reserved_size    = math::alignUp((int_t)bin->m_item_size * bin->m_items_capacity, page_size);
            const int_t items_committed_size   = (bin->m_items_committed_pages << bin->m_pagesize_shift);
            const int_t total_reserved_size    = bin_hdr_max_size + items_reserved_size;

            byte* base_address = (byte*)bin;

            // decommit pages of header + pages of items
            v_alloc_decommit(base_address, bin_hdr_committed_size);
            v_alloc_decommit(base_address + bin_hdr_max_size, items_committed_size);

            // release the whole reserved region
            v_alloc_release(base_address, total_reserved_size);
        }

        u32 size(bin_t* bin) { return bin->m_items_count; }
        u32 capacity(bin_t* bin) { return bin->m_items_capacity; }
        u32 highwater_mark(bin_t* bin) { return bin->m_items_free_index; }

        void* alloc(bin_t* bin)
        {
            if (bin->m_items_count >= bin->m_items_capacity)
                return nullptr;  // bin is full

            const u32   page_size        = (1 << bin->m_pagesize_shift);
            const int_t bin_hdr_max_size = math::alignUp(c_bin_hdr_max_size, page_size);
            byte*       items            = (byte*)bin + bin_hdr_max_size;
            u64*        bm0              = (u64*)&bin->m_bin0;
            if (bin->m_items_count < bin->m_items_free_index)
            {
                s32 item_index = -1;

                // We should have a free item in the binmap, where is it?
                switch (bin->m_bin_level_count)
                {
                    case 3: item_index = nbinmap24::find_and_set(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bm0 + bin->m_bin3_offset, bin->m_items_capacity); break;
                    case 2: item_index = nbinmap18::find_and_set(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bin->m_items_capacity); break;
                    case 1: item_index = nbinmap12::find_and_set(bm0, bm0 + 1, bin->m_items_capacity); break;
                    case 0: item_index = nbinmap6::find_and_set(bm0, bin->m_items_capacity); break;
                }

                bin->m_items_count += 1;
                byte* item = items + (item_index * bin->m_item_size);
                return item;
            }
            else
            {
                // Before touching the binmap, make sure we have committed enough memory for it
                if (bin->m_items_free_index >= bin->m_binmap_commit_threshold)
                {
                    // commit +1 page for binmap
                    if (!v_alloc_commit((byte*)bin + (bin->m_bin_committed_pages << bin->m_pagesize_shift), page_size))
                        return nullptr;
                    bin->m_bin_committed_pages += 1;
                    // recalculate the new commit threshold
                    bin->m_binmap_commit_threshold += (page_size * 8);
                }

                switch (bin->m_bin_level_count)
                {
                    case 3: nbinmap24::tick_used_lazy(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bm0 + bin->m_bin3_offset, bin->m_items_capacity, bin->m_items_free_index); break;
                    case 2: nbinmap18::tick_used_lazy(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bin->m_items_capacity, bin->m_items_free_index); break;
                    case 1: nbinmap12::tick_used_lazy(bm0, bm0 + 1, bin->m_items_capacity, bin->m_items_free_index); break;
                    case 0: break;
                }
                const s32 item_index = bin->m_items_free_index++;
                byte*     item       = items + (item_index * bin->m_item_size);

                // Do we need to commit more pages for items?
                byte* items_end = (byte*)bin + bin_hdr_max_size + (bin->m_items_committed_pages << bin->m_pagesize_shift);
                if (item + bin->m_item_size > items_end)
                {
                    // commit one more page
                    if (!v_alloc_commit(items_end, page_size))
                    {
                        return nullptr;
                    }
                    bin->m_items_committed_pages += 1;
                }

                bin->m_items_count += 1;
                return item;
            }
        }

        void free(bin_t* bin, void* ptr)
        {
            const u32   page_size        = (1 << bin->m_pagesize_shift);
            const int_t bin_hdr_max_size = math::alignUp(c_bin_hdr_max_size, page_size);
            const byte* items            = (const byte*)bin + bin_hdr_max_size;
            const s32   item_index       = (s32)(((const byte*)ptr - items) / bin->m_item_size);
            if (item_index < 0 || (u32)item_index >= bin->m_items_free_index)
                return;  // invalid pointer

            // Mark the item as free in the binmap
            u64* bm0 = (u64*)&bin->m_bin0;
            switch (bin->m_bin_level_count)
            {
                case 3: nbinmap24::clr(bm0, &bm0[1], &bm0[bin->m_bin2_offset], &bm0[bin->m_bin3_offset], bin->m_items_free_index, item_index); break;
                case 2: nbinmap18::clr(bm0, &bm0[1], &bm0[bin->m_bin2_offset], bin->m_items_free_index, item_index); break;
                case 1: nbinmap12::clr(bm0, &bm0[1], bin->m_items_free_index, item_index); break;
                case 0: nbinmap6::clr(bm0, bin->m_items_free_index, item_index); break;
            }

            // Decrease number of used items
            bin->m_items_count -= 1;
        }

    }  // namespace nbin
}  // namespace ncore

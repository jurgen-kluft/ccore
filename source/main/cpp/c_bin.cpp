#include "ccore/c_arena.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_binmap1.h"

#include "ccore/c_bin.h"

namespace ncore
{
    namespace nbin
    {
        // This is an allocation bin that can handle quite a lot of different allocation sizes.
        // It uses a hierarchical binmap to track used/free items and can grow both the committed
        // memory for the binmap as well as for the items themselves.
        // A certain part of the binmap is always committed, roughly 1 to 2 pages (page-size = 16KB)
        // depending on the maximum number of items.
        // One benefit of this bin is that it can handle up to 16M items (with 4 levels) and that
        // the memory overhead for the binmap is quite low, especially when the number of items
        // is high, basically tracking consumes 1 bit per item.
        // The behaviour of the binmap is such that allocations are always done from low to high,
        // so an allocation will always take the first free position that is lowest in the binmap.

        struct bin_t
        {
            u32 m_items_committed_pages;   // (unit = pages) number of pages committed for items
            u32 m_items_free_index;        // current 'bit' used to allocate next free item
            u32 m_items_count;             // number of items currently in use
            u32 m_items_capacity;          // maximum number of items for this bin
            u32 m_items_commit_threshold;  // threshold of when to commit +1 page for items
            u16 m_item_size;               // size of one item
            u8  m_items_pages_offset;      // (unit = pages, relative to base address) offset from base for where items start
            u8  m_pagesize_shift;          // page-size = 1 << pagesize_shift
            u8  m_bin_committed_pages;     // (unit = pages) number of pages committed for bin, including binmap
            u8  m_bin_level_count;         // binmap, number of levels
            // u16 m_bin1_offset;            // (unit = number of u64, relative to &m_bin0), always == 1
            u16 m_bin2_offset;  // (unit = number of u64, relative to &m_bin0)
            u16 m_bin3_offset;  // (unit = number of u64, relative to &m_bin0)
            u64 m_bin0;         // binmap level 0 (top level)
        };

        bin_t* make_bin(u16 item_size, u32 max_items)
        {
            const s32 page_size       = v_alloc_get_page_size();
            const s8  page_size_shift = v_alloc_get_page_size_shift();

            const int_t items_memory_size = math::alignUp((int_t)item_size * max_items, page_size);

            nbinmap::layout_t layout;
            nbinmap::compute(max_items, layout);

            // bin header total size = 'sizeof(bin_t) + full binmap data' (excluding size for items)
            int_t bin_hdr_total_size = sizeof(bin_t);
            bin_hdr_total_size += ((layout.m_bin1 + 63) >> 6) * sizeof(u64);
            bin_hdr_total_size += ((layout.m_bin2 + 63) >> 6) * sizeof(u64);
            bin_hdr_total_size += ((layout.m_bin3 + 63) >> 6) * sizeof(u64);
            bin_hdr_total_size = math::alignUp(bin_hdr_total_size, page_size);

            bin_t* bin = (bin_t*)v_alloc_reserve(bin_hdr_total_size + items_memory_size);

            // bin header commit size includes bin_t and most of the binmap data but not all
            int_t bin_hdr_commit_size = sizeof(bin_t);
            bin_hdr_commit_size += (((layout.m_bin1 + 63) >> 6) * sizeof(u64));
            bin_hdr_commit_size += (layout.m_bin3 > 0) ? (((layout.m_bin2 + 63) >> 6) * sizeof(u64)) : 0;
            bin_hdr_commit_size += (layout.m_bin2 > 0 || layout.m_bin3 > 0) ? 8 * sizeof(u64) : 0;
            bin_hdr_commit_size = math::alignUp(bin_hdr_commit_size, page_size);

            // where do the items start relative to the reserved region, calculate the offset in pages
            const u8  items_pages_offset    = (u8)(bin_hdr_total_size >> page_size_shift);
            const u16 items_pages_committed = 1;

            // if items start right after the bin_hdr_commit_size, then we can combine the commit
            if (items_pages_offset == (bin_hdr_commit_size >> page_size_shift))
            {
                if (!v_alloc_commit(bin, bin_hdr_commit_size + (items_pages_committed << page_size_shift)))
                {
                    v_alloc_release(bin, bin_hdr_total_size + items_memory_size);
                    return nullptr;
                }
            }
            else
            {
                // if we arrive here, this means that either bin2 or bin3 is partially committed and
                // will 'grow' later when needed.
                // but we will have to commit part of the header and separately commit the first page
                // for items.
                if (!v_alloc_commit(bin, bin_hdr_commit_size))
                {
                    v_alloc_release(bin, bin_hdr_total_size + items_memory_size);
                    return nullptr;
                }
                if (!v_alloc_commit(bin + bin_hdr_total_size, items_pages_committed << page_size_shift))
                {
                    v_alloc_release(bin, bin_hdr_total_size + items_memory_size);
                    return nullptr;
                }
            }

            bin->m_items_committed_pages = items_pages_committed;
            bin->m_items_free_index      = 0;
            bin->m_items_count           = 0;
            bin->m_items_capacity        = max_items;
            bin->m_item_size             = item_size;
            bin->m_items_pages_offset    = items_pages_offset;
            bin->m_pagesize_shift        = page_size_shift;
            bin->m_bin_committed_pages   = (u8)(bin_hdr_commit_size >> page_size_shift);
            bin->m_bin_level_count       = layout.m_levels;
            bin->m_bin2_offset           = 1 + (u16)((layout.m_bin1 + 63) >> 6);
            bin->m_bin3_offset           = bin->m_bin2_offset + (u32)((layout.m_bin2 + 63) >> 6);
            bin->m_bin0                  = D_U64_MAX;

            // What is the item index that when reached we should commit +1 page
            switch (bin->m_bin_level_count)
            {
                case 3:
                {
                    const u32 bin_binx               = ((u32)sizeof(bin_t) - (u32)sizeof(u64)) + ((u32)bin->m_bin3_offset * sizeof(u64));
                    const u32 items_commit_threshold = (u32)(bin_hdr_commit_size - bin_binx) * 8;
                    bin->m_items_commit_threshold    = math::min(items_commit_threshold, bin->m_items_capacity);
                }
                break;

                case 2:
                {
                    const u32 bin_binx               = ((u32)sizeof(bin_t) - (u32)sizeof(u64)) + ((u32)bin->m_bin2_offset * sizeof(u64));
                    const u32 items_commit_threshold = (u32)(bin_hdr_commit_size - bin_binx) * 8;
                    bin->m_items_commit_threshold    = math::min(items_commit_threshold, bin->m_items_capacity);
                }
                break;

                case 1:
                case 0:
                    bin->m_items_commit_threshold = bin->m_items_capacity;  // all committed
                    break;
            }

            return bin;
        }

        void destroy(bin_t* bin)
        {
            const int_t bin_hdr_reserved_size  = (bin->m_items_pages_offset << bin->m_pagesize_shift);
            const int_t bin_hdr_committed_size = (bin->m_bin_committed_pages << bin->m_pagesize_shift);
            const int_t items_reserved_size    = math::alignUp((int_t)bin->m_item_size * bin->m_items_capacity, (1 << bin->m_pagesize_shift));
            const int_t items_committed_size   = (bin->m_items_committed_pages << bin->m_pagesize_shift);
            const int_t total_reserved_size    = bin_hdr_reserved_size + items_reserved_size;

            byte* base_address = (byte*)bin;

            // decommit pages of header + pages of items
            v_alloc_decommit(base_address, bin_hdr_committed_size);
            v_alloc_decommit(base_address + bin_hdr_reserved_size, items_committed_size);

            // release the whole reserved region
            v_alloc_release(base_address, total_reserved_size);
        }

        void* alloc(bin_t* bin)
        {
            if (bin->m_items_count >= bin->m_items_capacity)
                return nullptr;  // bin is full

            byte* items = (byte*)bin + (bin->m_items_pages_offset << bin->m_pagesize_shift);
            u64*  bm0   = (u64*)&bin->m_bin0;
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
                if (bin->m_items_free_index >= bin->m_items_commit_threshold)
                {
                    // commit +1 page for binmap
                    if (!v_alloc_commit((byte*)bin + (bin->m_bin_committed_pages << bin->m_pagesize_shift), 1 << bin->m_pagesize_shift))
                        return nullptr;
                    bin->m_bin_committed_pages += 1;
                    // recalculate the new commit threshold
                    bin->m_items_commit_threshold += (((u32)1 << bin->m_pagesize_shift) * 8);
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
                byte* items_end = (byte*)bin + ((bin->m_items_pages_offset + bin->m_items_committed_pages) << bin->m_pagesize_shift);
                if (item + bin->m_item_size > items_end)
                {
                    // commit one more page
                    if (!v_alloc_commit(items_end, (1 << bin->m_pagesize_shift)))
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
            const byte* items      = (const byte*)bin + ((int_t)bin->m_items_pages_offset << bin->m_pagesize_shift);
            const s32   item_index = (s32)(((const byte*)ptr - items) / bin->m_item_size);
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

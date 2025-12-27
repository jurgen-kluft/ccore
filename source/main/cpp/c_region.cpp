#include "ccore/c_arena.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_binmap1.h"

#include "ccore/c_region.h"

namespace ncore
{
    // virtual memory functions (from c_arena.h):
    //    s32   v_alloc_get_page_size();
    //    void* v_alloc_reserve(int_t size);
    //    bool  v_alloc_commit(void* addr, int_t size);
    //    bool  v_alloc_decommit(void* addr, int_t extra_size);
    //    bool  v_alloc_release(void* addr, int_t size);

    namespace nregion
    {
        /*
        What if we just use large bins with a binmap to fully track used/free items?
        Let's see how that would work for a 16 byte item:

        Address Space = 64 MB (max 256 MB)
        Item Size = 16 bytes
        Number of Items = 64MB / 16 bytes = 4M items        | 256MB / 16 bytes = 16M items
        -----------------------------------------------------------------------------------
        Binmap Level 3 = 4M bits = u64[65536] = 512KB       | 16M bits = u64[262144] = 2MB
        Binmap Level 2 = 64K / 64 = u64[1024] = 8KB         | 256K / 64 = u64[4096] = 32KB
        Binmap Level 1 = 1K / 64 = u64[16] = 128 bytes      | 4K / 64 = u64[64] = 512 bytes
        Binmap Level 0 = 16 / 64 = u64[1] = 8 bytes         | 64 / 64 = u64[1] = 8 bytes

        So what would be best is if we can have Binmap3 managed in such a way that it commits more pages when growing.
        Binmap level 0, 1 and 2 can be part of one page that is always committed.

        Conclusion: What we thus need is a mechanic that can commit more pages to binmap level 3 as needed.

        Benefits: Allocation is always picking a free item closest to the start of the region, thus better locality.
                  Fragmentation is reduced as we always fill from the start.


        This can be applied to other item sizes:
        - 32 bytes   , bin2/bin1/bin0 = 2M   items = 2M (256KB) + 32K (4KB) + 512 (8 bytes)
        - 64 bytes   , bin2/bin1/bin0 = 1M   items = 128KB + 16K (2KB) + 256 (8 bytes)
        - 128 bytes  , bin2/bin1/bin0 = 512K items = 64KB + 8K (1KB) + 128 (8 bytes)
        - 256 bytes  , bin2/bin1/bin0 = 256K items = 32KB + 4K (512 bytes) + 64 (8 bytes)
        - 512 bytes  , bin2/bin1/bin0 = 128K items = 16KB + 2K (256 bytes) + 32 (8 bytes)
        - 1024 bytes , bin2/bin1/bin0 = 64K  items = 8KB + 1K (128 bytes) + 16 (8 bytes)
        - 2048 bytes , bin2/bin1/bin0 = 32K  items = 4KB + 512 (64 bytes) + 8 (8 bytes)
        - 4096 bytes , bin2/bin1/bin0 = 16K  items = 2KB + 256 (32 bytes) + 4 (8 bytes)

        With a page-size of 16KB, at 512 bytes and beyond we are not able to 'commit' more pages for level 3.

        We can even handle non-power-of-two sizes, like:
        - 24 bytes, although it is not a power-of-2 size, we can still be almost as fast as a power-of-2 size if we do a custom division
        - 48 bytes
          etc.., the only requirement is that the size is 'power-of-two * 3^N', so that we can still do fast division by shifting and multiplying.
        */

        // bin_t = 32 bytes
        struct bin_t
        {
            u32 m_items_pages_committed;  // (unit = pages) number of pages committed for items
            u32 m_free_index;             // current 'bit' used to allocate next free item
            u32 m_num_items;              // number of items currently in use
            u32 m_max_items;              // maximum number of items for this bin
            u16 m_item_size;              // size of one item
            u8  m_items_pages_offset;     // (unit = pages, relative to base address) offset from base for where items start
            u8  m_bin_levels;             // binmap, number of levels
            u8  m_pagesize_shift;         // page-size = 1 << pagesize_shift
            u16 m_bin3_offset;            // (unit = number of u64, relative to right after this structure), 0 means no bin3
            u16 m_bin2_offset;            // (unit = number of u64, relative to right after this structure), 0 means no bin2
            u64 m_bin0;
        };

        bin_t* make_bin(u16 item_size, u32 max_items)
        {
            const s32 page_size       = v_alloc_get_page_size();
            const s8  page_size_shift = math::ilog2(page_size);

            int_t items_memory_size = item_size * max_items;
            items_memory_size       = math::alignUp(items_memory_size, page_size);

            nbinmap::layout_t layout;
            nbinmap::compute(max_items, layout);

            // full bin is 'sizeof(bin_t) + full binmap data' (excluding memory for items)
            int_t full_bin_memory_size = sizeof(bin_t);
            full_bin_memory_size += ((layout.m_bin1 + 63) >> 6) * sizeof(u64);
            full_bin_memory_size += ((layout.m_bin2 + 63) >> 6) * sizeof(u64);
            full_bin_memory_size += ((layout.m_bin3 + 63) >> 6) * sizeof(u64);
            full_bin_memory_size = math::alignUp(full_bin_memory_size, page_size);

            bin_t* bin = (bin_t*)v_alloc_reserve(full_bin_memory_size + items_memory_size);

            // commit size includes bin_t and most of the binmap data but not all
            int_t commit_size = sizeof(bin_t) + (((layout.m_bin1 + 63) >> 6) * sizeof(u64));
            if (layout.m_bin3 > 0)
            {
                // Do make sure we have some bin3 u64's, like 256 bits
                commit_size += (((layout.m_bin2 + 63) >> 6) * sizeof(u64));
                commit_size += 8 * sizeof(u64);
            }
            else
            {
                // Do make sure we have some bin2 u64's, like 256 bits
                commit_size += 8 * sizeof(u64);
            }
            commit_size = math::alignUp(commit_size, page_size);

            // where do the items start relative to the reserved region, calculate the offset in pages
            const u8  items_pages_offset    = (u8)(math::alignUp(full_bin_memory_size, page_size) >> page_size_shift);
            const u16 items_pages_committed = 1;

            // if items start right after the commit_size, then we can combine the commit
            if (items_pages_offset == (commit_size >> page_size_shift))
            {
                if (v_alloc_commit(bin, commit_size + (items_pages_committed << page_size_shift)))
                {
                    v_alloc_release(bin, full_bin_memory_size + items_memory_size);
                    return nullptr;
                }
            }
            else
            {
                // if we arrive here, this means that either bin2 or bin3 is partially committed and
                // will 'grow' later when needed.
                // but we will have to commit part of the header and separately commit the first page
                // for items.
                if (!v_alloc_commit(bin, commit_size))
                {
                    v_alloc_release(bin, full_bin_memory_size + items_memory_size);
                    return nullptr;
                }
                if (!v_alloc_commit(bin + full_bin_memory_size, items_pages_committed << page_size_shift))
                {
                    v_alloc_release(bin, full_bin_memory_size + items_memory_size);
                    return nullptr;
                }
            }
            bin->m_items_pages_committed = items_pages_committed;
            bin->m_free_index            = 0;
            bin->m_num_items             = 0;
            bin->m_max_items             = max_items;
            bin->m_item_size             = item_size;
            bin->m_items_pages_offset    = items_pages_offset;
            bin->m_pagesize_shift        = page_size_shift;
            bin->m_bin_levels            = layout.m_levels;
            bin->m_bin3_offset           = layout.m_bin3 > 0 ? (bin->m_bin2_offset + (u32)((layout.m_bin2 + 63) >> 6)) : 0;
            bin->m_bin2_offset           = layout.m_bin2 > 0 ? (1 + (u16)((layout.m_bin1 + 63) >> 6)) : 0;
            bin->m_bin0                  = D_U64_MAX;

            return bin;
        }

        void* alloc(bin_t* bin)
        {
            byte* items = (byte*)bin + (bin->m_items_pages_offset << bin->m_pagesize_shift);
            u64*  bm0   = (u64*)&bin->m_bin0;
            if (bin->m_num_items < bin->m_free_index)
            {
                s32 item_index = -1;
                // We should have a free item in the binmap, where is it?
                switch (bin->m_bin_levels)
                {
                    case 3: item_index = nbinmap24::find_and_set(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bm0 + bin->m_bin3_offset, bin->m_max_items); break;
                    case 2: item_index = nbinmap18::find_and_set(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bin->m_max_items); break;
                    case 1: item_index = nbinmap12::find_and_set(bm0, bm0 + 1, bin->m_max_items); break;
                    case 0: item_index = nbinmap6::find_and_set(bm0, bin->m_max_items); break;
                }
                bin->m_num_items += 1;
                byte* item = items + (item_index * bin->m_item_size);
                return item;
            }
            else
            {
                switch (bin->m_bin_levels)
                {
                    case 3: nbinmap24::tick_used_lazy(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bm0 + bin->m_bin3_offset, bin->m_max_items, bin->m_free_index); break;
                    case 2: nbinmap18::tick_used_lazy(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bin->m_max_items, bin->m_free_index); break;
                    case 1: nbinmap12::tick_used_lazy(bm0, bm0 + 1, bin->m_max_items, bin->m_free_index); break;
                    case 0: break;
                }
                const s32 item_index = bin->m_free_index++;
                byte*     item       = items + (item_index * bin->m_item_size);

                // Do we need to commit more pages for items?
                byte* items_end = (byte*)bin + ((bin->m_items_pages_offset + bin->m_items_pages_committed) << bin->m_pagesize_shift);
                if (item + bin->m_item_size > items_end)
                {
                    // commit one more page
                    if (v_alloc_commit(items_end, (1 << bin->m_pagesize_shift)))
                    {
                        return nullptr;
                    }
                    bin->m_items_pages_committed += 1;
                }

                bin->m_num_items += 1;
                return item;
            }
        }

    }  // namespace nregion
}  // namespace ncore

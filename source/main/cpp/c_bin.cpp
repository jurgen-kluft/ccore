#include "ccore/c_arena.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_binmap1.h"

#include "ccore/c_bin.h"

namespace ncore
{
    namespace nbin
    {
        // sizeof(bin_t) = 48 bytes (must be multiple of 8 bytes for alignment)
        struct bin_t
        {
            byte* m_items;                // pointer to the items array
            byte* m_items_end;            // pointer to the items array end
            u32   m_items_maximum_pages;  // (unit = pages) maximum number of pages for items
            u32   m_items_free_index;     // current 'bit' used to allocate next free item
            u32   m_items_count;          // number of items currently in use
            u32   m_items_capacity;       // maximum number of items the bin can hold
            u16   m_item_sizeof;          // sizeof(item)
            u8    m_pagesize_shift;       // page-size = 1 << pagesize_shift
            u8    m_bin_level_count;      // binmap, number of levels
            u16   m_bin_committed_pages;  // (unit = pages) number of pages committed for bin, including binmap
            u16   m_bin_maximum_pages;    // (unit = pages) maximum number of pages for bin, including binmap
            u32   m_bin_commit_index;     // max index of when to commit +1 page for binmap
            u16   m_bin2_offset;          // (unit = number of u64, relative to &m_bin0)
            u16   m_bin3_offset;          // (unit = number of u64, relative to &m_bin0)
        };
        // memory layout:
        //       - bin_t
        //       - binmap level 0 (u64)
        //       - binmap level 1 (u64[], 512 bytes)
        //       - binmap level 2 (u64[], 32 KiB)
        //       - binmap level 3 (u64[], 2 MiB)
        // {pages for binmap level 2 or 3}
        // {page aligned}
        //       - item[]

        bin_t* make_bin(u16 item_size, u32 max_items)
        {
            ASSERT((sizeof(bin_t) & 7) == 0);  // sizeof(bin_t) must be 8-byte aligned

            const s32 page_size       = v_alloc_get_page_size();
            const s8  page_size_shift = v_alloc_get_page_size_shift();

            ASSERT(item_size < (page_size >> 1));

            // align the maximum number of items to a multiple of 64 (for binmap)
            max_items = math::alignUp(max_items, 64);

            // compute bin_t struct and most of the binmap
            nbinmap::layout64_t layout;
            nbinmap::compute(max_items, layout);

            const u32 binmap_level0_offset = (sizeof(bin_t) >> 3);
            const u32 binmap_level1_offset = binmap_level0_offset + layout.m_bin0;
            const u32 binmap_level2_offset = (layout.m_bin2 > 0) ? binmap_level1_offset + layout.m_bin1 : binmap_level1_offset;
            const u32 binmap_level3_offset = (layout.m_bin3 > 0) ? binmap_level2_offset + layout.m_bin2 : binmap_level2_offset;

            // sizeof(bin_t) + data for binmap
            const int_t bin_maximum_size  = math::alignUp(sizeof(bin_t) + nbinmap::sizeof_data(layout) * sizeof(u64), page_size);
            const int_t items_memory_size = math::alignUp((int_t)item_size * max_items, page_size);
            byte*       base_address      = (byte*)v_alloc_reserve(bin_maximum_size + items_memory_size);

            const int_t bin_commit_size = math::alignUp((binmap_level3_offset + 1) * sizeof(u64), page_size);
            if (!v_alloc_commit(base_address, bin_commit_size))
            {
                v_alloc_release(base_address, bin_maximum_size + items_memory_size);
                return nullptr;
            }

            const u16 item_pages_committed = 1;
            if (!v_alloc_commit(base_address + bin_maximum_size, (int_t)item_pages_committed << page_size_shift))
            {
                v_alloc_release(base_address, bin_maximum_size + items_memory_size);
                return nullptr;
            }

            bin_t* bin                 = (bin_t*)base_address;
            bin->m_items               = (byte*)(base_address + bin_maximum_size);
            bin->m_items_end           = bin->m_items + (item_pages_committed << page_size_shift);
            bin->m_items_maximum_pages = (u32)(items_memory_size >> page_size_shift);
            bin->m_items_free_index    = 0;
            bin->m_items_count         = 0;
            bin->m_items_capacity      = (u32)(items_memory_size / item_size);
            bin->m_item_sizeof         = item_size;
            bin->m_pagesize_shift      = page_size_shift;
            bin->m_bin_committed_pages = (u16)(bin_commit_size >> page_size_shift);
            bin->m_bin_maximum_pages   = (u16)(bin_maximum_size >> page_size_shift);
            bin->m_bin_level_count     = layout.m_levels;
            bin->m_bin2_offset         = (u16)(binmap_level2_offset - binmap_level0_offset);  // make it relative to level 0
            bin->m_bin3_offset         = (u16)(binmap_level3_offset - binmap_level0_offset);  // make it relative to level 0

            u64* bin0 = (u64*)(bin + 1);
            *bin0     = D_U64_MAX;

            const u32 bin_index_commit = (bin_commit_size - ((binmap_level0_offset + bin->m_bin3_offset) * sizeof(u64))) * 8;
            bin->m_bin_commit_index    = math::min(bin_index_commit, bin->m_items_capacity);

            return bin;
        }

        void destroy(bin_t* bin)
        {
            byte* base_address = (byte*)bin;

            const int_t bin_committed_size   = (int_t)bin->m_bin_committed_pages << bin->m_pagesize_shift;
            const int_t items_reserved_size  = (int_t)bin->m_items_maximum_pages << bin->m_pagesize_shift;
            const int_t items_committed_size = (int_t)(bin->m_items_end - bin->m_items);
            const int_t total_reserved_size  = (int_t)(bin->m_items - base_address) + items_reserved_size;

            // decommit pages of header + pages of items
            v_alloc_decommit(bin->m_items, items_committed_size);
            v_alloc_decommit(base_address, bin_committed_size);

            // release the whole reserved region
            v_alloc_release(base_address, total_reserved_size);
        }

        u32 size(bin_t const* bin) { return bin->m_items_count; }
        u32 capacity(bin_t const* bin) { return bin->m_items_capacity; }
        u32 highwater_mark(bin_t const* bin) { return bin->m_items_free_index; }

        void* alloc(bin_t* bin)
        {
            if (bin->m_items_count >= bin->m_items_capacity)
                return nullptr;  // bin is full

            byte* items = bin->m_items;
            u64*  bm0   = (u64*)(bin + 1);
            if (bin->m_items_count < bin->m_items_free_index)
            {
                s32 item_index = -1;

                // We should have a free item in the binmap, where is it?
                switch (bin->m_bin_level_count)
                {
                    case 3: item_index = nbinmap24::find_and_set(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bm0 + bin->m_bin3_offset, bin->m_items_free_index); break;
                    case 2: item_index = nbinmap18::find_and_set(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bin->m_items_free_index); break;
                    case 1: item_index = nbinmap12::find_and_set(bm0, bm0 + 1, bin->m_items_free_index); break;
                    case 0: item_index = nbinmap6::find_and_set(bm0, bin->m_items_free_index); break;
                }

                bin->m_items_count += 1;
                byte* item = items + (item_index * bin->m_item_sizeof);
                return item;
            }
            else
            {
                // Before touching the binmap, make sure we have committed enough memory for it
                if (bin->m_items_free_index >= bin->m_bin_commit_index)
                {
                    const u32 page_size = (1 << bin->m_pagesize_shift);
                    if (!v_alloc_commit((byte*)bin + (bin->m_bin_committed_pages << bin->m_pagesize_shift), page_size))
                        return nullptr;
                    bin->m_bin_committed_pages += 1;
                    bin->m_bin_commit_index += (page_size << 3);
                }

                const u32 items_capacity = bin->m_items_capacity;
                switch (bin->m_bin_level_count)
                {
                    case 3: nbinmap24::tick_lazy(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bm0 + bin->m_bin3_offset, items_capacity, bin->m_items_free_index); break;
                    case 2: nbinmap18::tick_lazy(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, items_capacity, bin->m_items_free_index); break;
                    case 1: nbinmap12::tick_lazy(bm0, bm0 + 1, items_capacity, bin->m_items_free_index); break;
                    case 0: break;
                }
                const s32 item_index = bin->m_items_free_index++;
                byte*     item       = items + (item_index * bin->m_item_sizeof);

                // Do we need to commit more pages for items?
                if ((item + bin->m_item_sizeof) > bin->m_items_end)
                {
                    const u32 page_size = (1 << bin->m_pagesize_shift);
                    if (!v_alloc_commit(bin->m_items_end, page_size))
                    {
                        return nullptr;
                    }
                    bin->m_items_end += page_size;
                }

                bin->m_items_count += 1;
                return item;
            }
        }

        void free(bin_t* bin, void* ptr)
        {
            const byte* items      = bin->m_items;
            const s32   item_index = (s32)(((const byte*)ptr - items) / bin->m_item_sizeof);
            if (item_index < 0 || (u32)item_index >= bin->m_items_free_index)
                return;  // invalid pointer

            // Mark the item as free in the binmap
            u64* bm0 = (u64*)(bin + 1);
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

        // convert a pointer to an index within the bin
        u32 ptr2idx(bin_t const* bin, void* ptr)
        {
            const byte* items = bin->m_items;
            if (ptr < (void*)items)
                return D_U32_MAX;  // invalid pointer
            const u32 index = (u32)(((const byte*)ptr - items) / bin->m_item_sizeof);
            if (index >= bin->m_items_free_index)
                return D_U32_MAX;  // invalid pointer
            return index;
        }

        // convert an index to a pointer within the bin
        void* idx2ptr(bin_t* bin, u32 index)
        {
            if (index >= bin->m_items_free_index)
                return nullptr;  // invalid index
            byte* items = bin->m_items;
            byte* item  = items + (index * bin->m_item_sizeof);
            return item;
        }

        // highest index of free item in the bin
        s32 highest_free(bin_t const* bin)
        {
            const u32 items_max = bin->m_items_free_index;
            s32       hi        = -1;
            u64*      bm0       = (u64*)(bin + 1);
            switch (bin->m_bin_level_count)
            {
                case 3: hi = nbinmap24::find_last(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bm0 + bin->m_bin3_offset, items_max); break;
                case 2: hi = nbinmap18::find_last(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, items_max); break;
                case 1: hi = nbinmap12::find_last(bm0, bm0 + 1, items_max); break;
                case 0: hi = nbinmap6::find_last(bm0, items_max); break;
            }
            return hi;
        }

    }  // namespace nbin

    namespace nbin16
    {
        // sizeof(bin_t) = 40 bytes (must be multiple of 8 bytes for alignment)
        struct bin_t
        {
            byte* m_items;                  // pointer to items
            byte* m_items_end;              // pointer to end of items where we need to commit more pages
            u32   m_items_committed_pages;  // (unit = pages) number of pages committed for items
            u32   m_items_maximum_pages;    // (unit = pages) maximum number of pages for items
            u32   m_items_free_index;       // current 'bit' used to allocate next free item
            u32   m_items_count;            // number of items currently in use
            u32   m_items_capacity;         // maximum number of items the bin can hold
            u16   m_item_sizeof;            // sizeof(item)
            u8    m_pagesize_shift;         // page-size = 1 << pagesize_shift
            u8    m_bin_level_count;        // binmap, number of levels
            u16   m_bin2_offset;            // (unit = number of u64, relative to &m_bin0)
            u16   m_padding0;               // padding for 8 byte alignment
        };
        // memory layout:
        //       - bin_t (40 bytes)
        //       - binmap level 0 (u64, 8 bytes)
        //       - binmap level 1 (u64[16], 128 bytes)
        //       - binmap level 2 (u64[1024], 8 KiB)
        // {page aligned}
        //       - item[]

        bin_t* make_bin(u16 item_size, u32 max_items)
        {
            ASSERT(max_items <= 65536);
            ASSERT((sizeof(bin_t) & 7) == 0);  // sizeof(bin_t) must be 8-byte aligned

            const s32 page_size       = v_alloc_get_page_size();
            const s8  page_size_shift = v_alloc_get_page_size_shift();

            ASSERT(item_size < (page_size >> 1));

            // align the maximum number of items to a multiple of 64 (for binmap)
            max_items = math::alignUp(max_items, 64);

            const int_t items_memory_size = math::alignUp((int_t)item_size * max_items, page_size);

            // compute bin_t struct and most of the binmap
            nbinmap::layout64_t layout;
            nbinmap::compute(max_items, layout);

            const u32 binmap_level0_offset = (sizeof(bin_t) >> 3);
            const u32 binmap_level1_offset = binmap_level0_offset + layout.m_bin0;
            const u32 binmap_level2_offset = (layout.m_bin2 > 0) ? binmap_level1_offset + layout.m_bin1 : binmap_level1_offset;

            // sizeof(bin_t) + data for binmap
            const int_t bin_maximum_size = math::alignUp(sizeof(bin_t) + ((1 + 16 + 1024) * sizeof(u64)), page_size);
            byte*       base_address     = (byte*)v_alloc_reserve(bin_maximum_size + items_memory_size);
            const int_t bin_commit_size  = bin_maximum_size;
            if (!v_alloc_commit(base_address, bin_commit_size))
            {
                v_alloc_release(base_address, bin_maximum_size + items_memory_size);
                return nullptr;
            }

            const u16 item_pages_committed = 1;
            if (!v_alloc_commit(base_address + bin_maximum_size, (int_t)item_pages_committed << page_size_shift))
            {
                v_alloc_release(base_address, bin_maximum_size + items_memory_size);
                return nullptr;
            }

            bin_t* bin                   = (bin_t*)base_address;
            bin->m_items                 = base_address + bin_maximum_size;
            bin->m_items_end             = bin->m_items + ((int_t)item_pages_committed << page_size_shift);
            bin->m_items_committed_pages = item_pages_committed;
            bin->m_items_maximum_pages   = (u32)(items_memory_size >> page_size_shift);
            bin->m_items_free_index      = 0;
            bin->m_items_count           = 0;
            bin->m_items_capacity        = (u32)(((int_t)bin->m_items_maximum_pages << page_size_shift) / item_size);
            bin->m_item_sizeof           = item_size;
            bin->m_pagesize_shift        = page_size_shift;
            bin->m_bin_level_count       = layout.m_levels;
            bin->m_bin2_offset           = (u16)(binmap_level2_offset - binmap_level0_offset);  // make it relative to level 0

            u64* bin0 = (u64*)(bin + 1);
            *bin0     = D_U64_MAX;

            return bin;
        }

        void destroy(bin_t* bin)
        {
            const u32   page_size            = v_alloc_get_page_size();
            const int_t bin_maximum_size     = math::alignUp(sizeof(bin_t) + ((1 + 16 + 1024) * sizeof(u64)), page_size);
            const int_t items_reserved_size  = (int_t)bin->m_items_maximum_pages << bin->m_pagesize_shift;
            const int_t items_committed_size = (bin->m_items_committed_pages << bin->m_pagesize_shift);
            const int_t total_reserved_size  = bin_maximum_size + items_reserved_size;

            byte* base_address = (byte*)bin;

            // decommit pages of header + pages of items
            v_alloc_decommit(base_address, bin_maximum_size + items_committed_size);

            // release the whole reserved region
            v_alloc_release(base_address, total_reserved_size);
        }

        u32 size(bin_t const* bin) { return bin->m_items_count; }
        u32 capacity(bin_t const* bin) { return (bin->m_items_maximum_pages << bin->m_pagesize_shift) / bin->m_item_sizeof; }
        u32 highwater_mark(bin_t const* bin) { return bin->m_items_free_index; }

        void* alloc(bin_t* bin)
        {
            if (bin->m_items_count >= bin->m_items_capacity)
                return nullptr;  // bin is full

            byte* items = bin->m_items;
            u64*  bm0   = (u64*)(bin + 1);
            if (bin->m_items_count < bin->m_items_free_index)
            {
                s32 item_index = -1;

                // We should have a free item in the binmap, where is it?
                switch (bin->m_bin_level_count)
                {
                    case 2: item_index = nbinmap18::find_and_set(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bin->m_items_capacity); break;
                    case 1: item_index = nbinmap12::find_and_set(bm0, bm0 + 1, bin->m_items_capacity); break;
                    case 0: item_index = nbinmap6::find_and_set(bm0, bin->m_items_capacity); break;
                }
                ASSERT(item_index >= 0);

                bin->m_items_count += 1;
                return items + (item_index * bin->m_item_sizeof);
            }
            else
            {
                switch (bin->m_bin_level_count)
                {
                    case 2: nbinmap18::tick_lazy(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, bin->m_items_capacity, bin->m_items_free_index); break;
                    case 1: nbinmap12::tick_lazy(bm0, bm0 + 1, bin->m_items_capacity, bin->m_items_free_index); break;
                    case 0: break;
                }
                const s32 item_index = bin->m_items_free_index++;
                byte*     item       = items + (item_index * bin->m_item_sizeof);

                // Do we need to commit more pages for items?
                if ((item + bin->m_item_sizeof) > bin->m_items_end)
                {
                    // commit one more page
                    const u32 page_size = (1 << bin->m_pagesize_shift);
                    if (!v_alloc_commit(bin->m_items_end, page_size))
                    {
                        return nullptr;
                    }
                    bin->m_items_committed_pages += 1;
                    bin->m_items_end += page_size;
                }

                bin->m_items_count += 1;
                return item;
            }
        }

        void free(bin_t* bin, void* ptr)
        {
            const byte* items      = bin->m_items;
            const s32   item_index = (s32)(((const byte*)ptr - items) / bin->m_item_sizeof);
            if (item_index < 0 || (u32)item_index >= bin->m_items_free_index)
                return;  // invalid pointer

            // Mark the item as free in the binmap
            u64* bm0 = (u64*)(bin + 1);
            switch (bin->m_bin_level_count)
            {
                case 2: nbinmap18::clr(bm0, &bm0[1], &bm0[bin->m_bin2_offset], bin->m_items_free_index, item_index); break;
                case 1: nbinmap12::clr(bm0, &bm0[1], bin->m_items_free_index, item_index); break;
                case 0: nbinmap6::clr(bm0, bin->m_items_free_index, item_index); break;
            }

            // Decrease number of used items
            bin->m_items_count -= 1;
        }

        // convert a pointer to an index within the bin
        i32 ptr2idx(bin_t const* bin, void* ptr)
        {
            const byte* items = bin->m_items;
            if (ptr < items)
                return -1;  // invalid pointer
            const u32 index = (u32)(((const byte*)ptr - items) / bin->m_item_sizeof);
            if (index >= bin->m_items_free_index)
                return -1;  // invalid index
            return (i32)index;
        }

        // convert an index to a pointer within the bin
        void* idx2ptr(bin_t* bin, u16 index)
        {
            if (index >= bin->m_items_free_index)
                return nullptr;  // invalid index
            return bin->m_items + (index * bin->m_item_sizeof);
        }

        // highest index of free item in the bin
        s32 highest_free(bin_t const* bin)
        {
            const u32 items_max = bin->m_items_free_index;
            s32       hi        = -1;
            u64*      bm0       = (u64*)(bin + 1);
            switch (bin->m_bin_level_count)
            {
                case 2: hi = nbinmap18::find_last(bm0, bm0 + 1, bm0 + bin->m_bin2_offset, items_max); break;
                case 1: hi = nbinmap12::find_last(bm0, bm0 + 1, items_max); break;
                case 0: hi = nbinmap6::find_last(bm0, items_max); break;
            }
            return hi;
        }

    }  // namespace nbin16

}  // namespace ncore

#include "ccore/c_arena.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_binmap1.h"

#include "ccore/c_bin.h"

namespace ncore
{
    struct mem_t
    {
        byte* m_base;             // base address of the virtual memory for this memory
        u32   m_reserved_pages;   // number of pages reserved for this memory
        u32   m_committed_pages;  // number of pages committed for this memory
    };

    static void s_highest_access(mem_t& m, u64 offset, u8 const pagesize_shift)
    {
        if (m.m_base == nullptr)
            return;

        u64 const pages_end = (offset + 1) >> pagesize_shift;
        if (pages_end > m.m_committed_pages)
        {
            u64 const new_committed_pages = pages_end;
            if (new_committed_pages <= m.m_reserved_pages)
            {
                v_alloc_commit(m.m_base + ((u64)m.m_committed_pages << pagesize_shift), (new_committed_pages - m.m_committed_pages) << pagesize_shift);
                m.m_committed_pages = (u32)new_committed_pages;
            }
        }
    }

    namespace nbin
    {
        // sizeof(bin_t) = 48 bytes (must be multiple of 8 bytes for alignment)
        struct bin_t
        {
            mem_t m_items;             // memory for items
            u32   m_items_free_index;  // current 'bit' used to allocate next free item
            u32   m_items_count;       // number of items currently in use
            u32   m_items_capacity;    // maximum number of items the bin can hold
            u16   m_item_sizeof;       // sizeof(item)
            u8    m_pagesize_shift;    // page-size = 1 << pagesize_shift
            u8    m_bin_level_count;   // binmap, number of levels
            u64   m_bin0;              // level 0 of binmap, always present, stored
            u64   m_bin1[64];          // level 1 of binmap, always present, stored
            mem_t m_bin2;              // level 2 of binmap, optional
            mem_t m_bin3;              // level 3 of binmap, optional
        };
        // memory layout:
        //       - bin_t
        // {page aligned}
        // {pages for binmap level 2, u64[], max 32 KiB}
        // {page aligned}
        // {pages for binmap level 3, u64[], max 2 MiB}
        // {page aligned}
        // {pages for items}

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

            const u32 binmap_level2_num_pages = (u32)(math::alignUp((u64)layout.m_bin2 * sizeof(u64), page_size) >> page_size_shift);
            const u32 binmap_level3_num_pages = (u32)(math::alignUp((u64)layout.m_bin3 * sizeof(u64), page_size) >> page_size_shift);

            // sizeof(bin_t) + binmap level 2 pages + binmap level 3 pages
            const int_t bin_maximum_size  = math::alignUp(sizeof(bin_t) + (binmap_level2_num_pages + binmap_level3_num_pages) * page_size, page_size);
            const int_t items_memory_size = math::alignUp((int_t)item_size * max_items, page_size);
            byte*       base_address      = (byte*)v_alloc_reserve(bin_maximum_size + items_memory_size);

            const u16 item_pages_committed = 1;
            if (!v_alloc_commit(base_address + bin_maximum_size, (int_t)item_pages_committed << page_size_shift))
            {
                v_alloc_release(base_address, bin_maximum_size + items_memory_size);
                return nullptr;
            }

            bin_t* bin                     = (bin_t*)base_address;
            bin->m_items.m_base            = (byte*)(base_address + bin_maximum_size);
            bin->m_items.m_committed_pages = 0;
            bin->m_items.m_reserved_pages  = (u32)(items_memory_size >> page_size_shift);
            bin->m_items_free_index        = 0;
            bin->m_items_count             = 0;
            bin->m_items_capacity          = (u32)(items_memory_size / item_size);
            bin->m_item_sizeof             = item_size;
            bin->m_pagesize_shift          = page_size_shift;
            bin->m_bin_level_count         = layout.m_levels;
            bin->m_bin2.m_base             = (u8*)(bin + 1);
            bin->m_bin3.m_base             = bin->m_bin2.m_base + (binmap_level2_num_pages << page_size_shift);

            bin->m_bin0 = D_U64_MAX;

            // const u32 bin_index_commit = (u32)((bin_commit_size - ((binmap_level0_offset + bin->m_bin3_offset) * sizeof(u64))) * 8);
            // bin->m_bin_commit_index    = math::min(bin_index_commit, bin->m_items_capacity);

            // Commit pages for items, bin2 and bin3 if needed
            const u64 initial_num_items = 64;

            // Just make sure we have 64 items committed for the items
            s_highest_access(bin->m_items, initial_num_items * item_size, page_size_shift);
            s_highest_access(bin->m_bin2, 1 * sizeof(u64), page_size_shift);
            s_highest_access(bin->m_bin3, 1 * sizeof(u64), page_size_shift);

            return bin;
        }

        void destroy(bin_t* bin)
        {
            byte* base_address = (byte*)bin;

            const int_t items_reserved_size = (int_t)bin->m_items.m_reserved_pages << bin->m_pagesize_shift;
            const int_t total_reserved_size = (int_t)(bin->m_items.m_base - base_address) + items_reserved_size;

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

            byte* items = bin->m_items.m_base;
            u64*  bm0   = &bin->m_bin0;
            u64*  bm1   = bin->m_bin1;
            if (bin->m_items_count < bin->m_items_free_index)
            {
                s32 item_index = -1;

                // We should have a free item in the binmap, where is it?
                switch (bin->m_bin_level_count)
                {
                    case 3: item_index = nbinmap24::find_and_set(bm0, bm1, (u64*)bin->m_bin2.m_base, (u64*)bin->m_bin3.m_base, bin->m_items_free_index); break;
                    case 2: item_index = nbinmap18::find_and_set(bm0, bm1, (u64*)bin->m_bin2.m_base, bin->m_items_free_index); break;
                    case 1: item_index = nbinmap12::find_and_set(bm0, bm1, bin->m_items_free_index); break;
                    case 0: item_index = nbinmap6::find_and_set(bm0, bin->m_items_free_index); break;
                }

                bin->m_items_count += 1;
                byte* item = items + (item_index * bin->m_item_sizeof);
                return item;
            }
            else
            {
                // Before touching the binmap, make sure we have committed enough memory for it
                switch (bin->m_bin_level_count)
                {
                    case 3:
                        s_highest_access(bin->m_bin2, (bin->m_items_free_index + 63) >> 12, bin->m_pagesize_shift);
                        s_highest_access(bin->m_bin3, (bin->m_items_free_index + 63) >> 6, bin->m_pagesize_shift);
                        break;
                    case 2: s_highest_access(bin->m_bin2, (bin->m_items_free_index + 63) >> 6, bin->m_pagesize_shift); break;
                }

                const u32 items_capacity = bin->m_items_capacity;
                switch (bin->m_bin_level_count)
                {
                    case 3: nbinmap24::tick_lazy(bm0, bm1, (u64*)bin->m_bin2.m_base, (u64*)bin->m_bin3.m_base, items_capacity, bin->m_items_free_index); break;
                    case 2: nbinmap18::tick_lazy(bm0, bm1, (u64*)bin->m_bin2.m_base, items_capacity, bin->m_items_free_index); break;
                    case 1: nbinmap12::tick_lazy(bm0, bm1, items_capacity, bin->m_items_free_index); break;
                    case 0: break;
                }
                const s32 item_index = bin->m_items_free_index++;
                byte*     item       = items + (item_index * bin->m_item_sizeof);

                // Do we need to commit more pages for items?
                s_highest_access(bin->m_items, (item_index * bin->m_item_sizeof), bin->m_pagesize_shift);

                bin->m_items_count += 1;
                return item;
            }
        }

        void free(bin_t* bin, void* ptr)
        {
            const byte* items      = bin->m_items.m_base;
            const s32   item_index = (s32)(((const byte*)ptr - items) / bin->m_item_sizeof);
            if (item_index < 0 || (u32)item_index >= bin->m_items_free_index)
                return;  // invalid pointer

            // Mark the item as free in the binmap
            u64* bm0 = &bin->m_bin0;
            u64* bm1 = bin->m_bin1;
            switch (bin->m_bin_level_count)
            {
                case 3: nbinmap24::clr(bm0, bm1, (u64*)bin->m_bin2.m_base, (u64*)bin->m_bin3.m_base, bin->m_items_free_index, item_index); break;
                case 2: nbinmap18::clr(bm0, bm1, (u64*)bin->m_bin2.m_base, bin->m_items_free_index, item_index); break;
                case 1: nbinmap12::clr(bm0, bm1, bin->m_items_free_index, item_index); break;
                case 0: nbinmap6::clr(bm0, bin->m_items_free_index, item_index); break;
            }

            // Decrease number of used items
            bin->m_items_count -= 1;
        }

        // convert a pointer to an index within the bin
        u32 ptr2idx(bin_t const* bin, void* ptr)
        {
            const byte* items = bin->m_items.m_base;
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
            byte* items = bin->m_items.m_base;
            byte* item  = items + (index * bin->m_item_sizeof);
            return item;
        }

        // highest index of free item in the bin
        s32 highest_free(bin_t const* bin)
        {
            const u32  items_max = bin->m_items_free_index;
            s32        hi        = -1;
            u64 const* bm0       = &bin->m_bin0;
            u64 const* bm1       = bin->m_bin1;
            switch (bin->m_bin_level_count)
            {
                case 3: hi = nbinmap24::find_last(bm0, bm1, (u64*)bin->m_bin2.m_base, (u64*)bin->m_bin3.m_base, items_max); break;
                case 2: hi = nbinmap18::find_last(bm0, bm1, (u64*)bin->m_bin2.m_base, items_max); break;
                case 1: hi = nbinmap12::find_last(bm0, bm1, items_max); break;
                case 0: hi = nbinmap6::find_last(bm0, items_max); break;
            }
            return hi;
        }

    }  // namespace nbin

    namespace nbin16
    {
        // sizeof(bin_t) = 184 bytes (must be multiple of 8 bytes for alignment)
        struct bin_t
        {
            mem_t m_items;             // pointer to items
            u32   m_items_free_index;  // current 'bit' used to allocate next free item
            u32   m_items_count;       // number of items currently in use
            u32   m_items_capacity;    // maximum number of items the bin can hold
            u16   m_item_sizeof;       // sizeof(item)
            u8    m_pagesize_shift;    // page-size = 1 << pagesize_shift
            u8    m_bin_level_count;   // binmap, number of levels
            u64   m_bin0;              // level 0 of binmap, always present, stored
            u64   m_bin1[16];          // level 1 of binmap, always present, stored
            mem_t m_bin2;              // level 2 of binmap
        };
        // memory layout:
        //       - bin_t (184 bytes)
        // {binmap level 2, u64[1024] max 8 KiB)
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

            const u32 binmap_level2_size = layout.m_bin2 * sizeof(u64);

            // sizeof(bin_t) + binmap level 2
            const int_t bin_maximum_size = math::alignUp(sizeof(bin_t) + binmap_level2_size, page_size);
            byte*       base_address     = (byte*)v_alloc_reserve(bin_maximum_size + items_memory_size);
            const int_t base_commit_size = page_size;
            if (!v_alloc_commit(base_address, base_commit_size))
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

            bin_t* bin                     = (bin_t*)base_address;
            bin->m_items.m_base            = base_address + bin_maximum_size;
            bin->m_items.m_committed_pages = item_pages_committed;
            bin->m_items.m_reserved_pages  = (u32)(items_memory_size >> page_size_shift);
            bin->m_items_free_index        = 0;
            bin->m_items_count             = 0;
            bin->m_items_capacity          = (u32)(((int_t)bin->m_items.m_reserved_pages << page_size_shift) / item_size);
            bin->m_item_sizeof             = item_size;
            bin->m_pagesize_shift          = page_size_shift;
            bin->m_bin_level_count         = layout.m_levels;
            bin->m_bin2.m_base             = base_address;
            bin->m_bin2.m_committed_pages  = 1;
            bin->m_bin2.m_reserved_pages   = bin_maximum_size >> page_size_shift;
            bin->m_bin0                    = D_U64_MAX;

            return bin;
        }

        void destroy(bin_t* bin)
        {
            const u32   page_size            = (u32)1 << bin->m_pagesize_shift;
            const int_t bin_maximum_size     = math::alignUp(sizeof(bin_t) + ((1 + 16 + 1024) * sizeof(u64)), page_size);
            const int_t items_reserved_size  = (int_t)bin->m_items.m_reserved_pages << bin->m_pagesize_shift;
            const int_t items_committed_size = (bin->m_items.m_committed_pages << bin->m_pagesize_shift);
            const int_t total_reserved_size  = bin_maximum_size + items_reserved_size;

            byte* base_address = (byte*)bin;

            // decommit pages of header + pages of items
            v_alloc_decommit(base_address, bin_maximum_size + items_committed_size);

            // release the whole reserved region
            v_alloc_release(base_address, total_reserved_size);
        }

        u32 size(bin_t const* bin) { return bin->m_items_count; }
        u32 capacity(bin_t const* bin) { return (bin->m_items.m_reserved_pages << bin->m_pagesize_shift) / bin->m_item_sizeof; }
        u32 highwater_mark(bin_t const* bin) { return bin->m_items_free_index; }

        void* alloc(bin_t* bin)
        {
            if (bin->m_items_count >= bin->m_items_capacity)
                return nullptr;  // bin is full

            byte* items = bin->m_items.m_base;
            u64*  bm0   = &bin->m_bin0;
            u64*  bm1   = bin->m_bin1;
            if (bin->m_items_count < bin->m_items_free_index)
            {
                s32 item_index = -1;

                switch (bin->m_bin_level_count)
                {
                    case 2: s_highest_access(bin->m_bin2, (bin->m_items_free_index + 63) >> 6, bin->m_pagesize_shift); break;
                }

                // We should have a free item in the binmap, where is it?
                switch (bin->m_bin_level_count)
                {
                    case 2: item_index = nbinmap18::find_and_set(bm0, bm1, (u64*)bin->m_bin2.m_base, bin->m_items_capacity); break;
                    case 1: item_index = nbinmap12::find_and_set(bm0, bm1, bin->m_items_capacity); break;
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
                    case 2: nbinmap18::tick_lazy(bm0, bm1, (u64*)bin->m_bin2.m_base, bin->m_items_capacity, bin->m_items_free_index); break;
                    case 1: nbinmap12::tick_lazy(bm0, bm1, bin->m_items_capacity, bin->m_items_free_index); break;
                    case 0: break;
                }
                const s32 item_index = bin->m_items_free_index++;
                byte*     item       = items + (item_index * bin->m_item_sizeof);

                s_highest_access(bin->m_items, (item_index * bin->m_item_sizeof), bin->m_pagesize_shift);

                bin->m_items_count += 1;
                return item;
            }
        }

        void free(bin_t* bin, void* ptr)
        {
            const byte* items      = bin->m_items.m_base;
            const s32   item_index = (s32)(((const byte*)ptr - items) / bin->m_item_sizeof);
            if (item_index < 0 || (u32)item_index >= bin->m_items_free_index)
                return;  // invalid pointer

            // Mark the item as free in the binmap
            u64* bm0 = &bin->m_bin0;
            u64* bm1 = bin->m_bin1;
            switch (bin->m_bin_level_count)
            {
                case 2: nbinmap18::clr(bm0, bm1, (u64*)bin->m_bin2.m_base, bin->m_items_free_index, item_index); break;
                case 1: nbinmap12::clr(bm0, bm1, bin->m_items_free_index, item_index); break;
                case 0: nbinmap6::clr(bm0, bin->m_items_free_index, item_index); break;
            }

            // Decrease number of used items
            bin->m_items_count -= 1;
        }

        // convert a pointer to an index within the bin
        i32 ptr2idx(bin_t const* bin, void* ptr)
        {
            const byte* items = bin->m_items.m_base;
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
            return bin->m_items.m_base + (index * bin->m_item_sizeof);
        }

        // highest index of free item in the bin
        s32 highest_free(bin_t const* bin)
        {
            const u32  items_max = bin->m_items_free_index;
            s32        hi        = -1;
            u64 const* bm0       = &bin->m_bin0;
            u64 const* bm1       = bin->m_bin1;
            switch (bin->m_bin_level_count)
            {
                case 2: hi = nbinmap18::find_last(bm0, bm1, (u64 const*)bin->m_bin2.m_base, items_max); break;
                case 1: hi = nbinmap12::find_last(bm0, bm1, items_max); break;
                case 0: hi = nbinmap6::find_last(bm0, items_max); break;
            }
            return hi;
        }

    }  // namespace nbin16

}  // namespace ncore

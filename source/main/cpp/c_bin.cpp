#include "ccore/c_arena.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_binmap1.h"

#include "ccore/c_bin.h"

namespace ncore
{
    void bin_setup(bin32_t* bin, u16 item_size, u32 max_items)
    {
        const s32 page_size = v_alloc_get_page_size();
        ASSERT(item_size < (page_size >> 1));

        // align the maximum number of items to a multiple of 64 (for binmap)
        max_items = math::alignUp(max_items, 64);

        const u64   initial_num_items = 64;
        const int_t items_memory_size = (int_t)item_size * max_items;
        bin->m_items                  = narena::new_arena(items_memory_size, initial_num_items * item_size);
        bin->m_items_count            = 0;
        bin->m_item_sizeof            = item_size;

        // compute binmap size layout and allocate arenas accordingly
        nbinmap::layout64_t layout;
        nbinmap::compute(max_items, layout);

        bin->m_bin_level_count = layout.m_levels;
        if (layout.m_bin3 > 0)
        {
            const u32 bin012_size    = (layout.m_bin0 + layout.m_bin1 + layout.m_bin2);
            const u32 bin012_commit  = (layout.m_bin0 + layout.m_bin1 + ((initial_num_items + 63) >> 12));
            bin->m_bin_level2_offset = layout.m_bin0 + layout.m_bin1;
            bin->m_bin               = narena::new_arena(bin012_size * sizeof(u64), bin012_commit * sizeof(u64));
            bin->m_bin3              = narena::new_arena(layout.m_bin3 * sizeof(u64), ((initial_num_items + 63) >> 6));
        }
        else
        {
            const u32 bin012_size    = (layout.m_bin0 + layout.m_bin1 + layout.m_bin2);
            const u32 bin012_commit  = (layout.m_bin0 + layout.m_bin1 + ((initial_num_items + 63) >> 6));
            bin->m_bin_level2_offset = layout.m_bin0 + layout.m_bin1;
            bin->m_bin               = narena::new_arena(bin012_size * sizeof(u64), bin012_commit * sizeof(u64));
            bin->m_bin3              = nullptr;
        }

        u64* bin0 = (u64*)narena::base_ptr(bin->m_bin);
        bin0[0]   = D_U64_MAX;  // mark all items as free in the binmap
    }

    void bin_destroy(bin32_t* bin)
    {
        if (bin->m_items != nullptr)
            narena::destroy(bin->m_items);
        if (bin->m_bin != nullptr)
            narena::destroy(bin->m_bin);
    }

    u32 bin_size(bin32_t const* bin) { return bin->m_items_count; }
    u32 bin_capacity(bin32_t const* bin) { return narena::reserved_size(bin->m_items) / bin->m_item_sizeof; }
    u32 bin_highwater_mark(bin32_t const* bin) { return narena::current_pos(bin->m_items) / bin->m_item_sizeof; }

    void* bin_alloc(bin32_t* bin)
    {
        const u32 items_capacity = bin_capacity(bin);
        if (bin->m_items_count >= items_capacity)
            return nullptr;  // bin is full

        byte* items = narena::base_ptr(bin->m_items);

        u32 item_free_index = narena::current_pos(bin->m_items) / bin->m_item_sizeof;

        u64* bm0 = (u64*)narena::base_ptr(bin->m_bin);
        u64* bm1 = bm0 + 1;
        u64* bm2 = bm0 + bin->m_bin_level2_offset;
        u64* bm3 = bin->m_bin3 != nullptr ? (u64*)narena::base_ptr(bin->m_bin3) : nullptr;

        if (bin->m_items_count < item_free_index)
        {
            s32 item_index = -1;

            // We should have a free item in the binmap, where is it?
            switch (bin->m_bin_level_count)
            {
                case 3: item_index = nbinmap24::find_and_remove(bm0, bm1, bm2, bm3, item_free_index); break;
                case 2: item_index = nbinmap18::find_and_remove(bm0, bm1, bm2, item_free_index); break;
                case 1: item_index = nbinmap12::find_and_remove(bm0, bm1, item_free_index); break;
                case 0: item_index = nbinmap6::find_and_remove(bm0, item_free_index); break;
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
                    narena::commit(bin->m_bin, (bin->m_bin_level2_offset + ((item_free_index + 1 + 63) >> 12)) * sizeof(u64));
                    narena::commit(bin->m_bin3, ((item_free_index + 1 + 63) >> 6) * sizeof(u64));
                    break;
                case 2:
                    narena::commit(bin->m_bin, (bin->m_bin_level2_offset + ((item_free_index + 1 + 63) >> 6)) * sizeof(u64));
                    // we have no bin3
                    break;
            }

            switch (bin->m_bin_level_count)
            {
                case 3: nbinmap24::tick_lazy(bm0, bm1, bm2, bm3, items_capacity, item_free_index); break;
                case 2: nbinmap18::tick_lazy(bm0, bm1, bm2, items_capacity, item_free_index); break;
                case 1: nbinmap12::tick_lazy(bm0, bm1, items_capacity, item_free_index); break;
                case 0: break;
            }

            bin->m_items_count += 1;

            void* item = narena::alloc(bin->m_items, bin->m_item_sizeof);
            return item;
        }
    }

    void bin_free(bin32_t* bin, void* ptr)
    {
        const u32 item_free_index = narena::current_pos(bin->m_items) / bin->m_item_sizeof;

        const byte* items      = narena::base_ptr(bin->m_items);
        const s32   item_index = (s32)(((const byte*)ptr - items) / bin->m_item_sizeof);
        if (item_index < 0 || (u32)item_index >= item_free_index)
            return;  // invalid pointer

        // Mark the item as free in the binmap
        u64* bm0 = (u64*)narena::base_ptr(bin->m_bin);
        u64* bm1 = bm0 + 1;
        u64* bm2 = bm0 + bin->m_bin_level2_offset;
        u64* bm3 = bin->m_bin3 != nullptr ? (u64*)narena::base_ptr(bin->m_bin3) : nullptr;

        switch (bin->m_bin_level_count)
        {
            case 3: nbinmap24::clr(bm0, bm1, bm2, bm3, item_free_index, item_index); break;
            case 2: nbinmap18::clr(bm0, bm1, bm2, item_free_index, item_index); break;
            case 1: nbinmap12::clr(bm0, bm1, item_free_index, item_index); break;
            case 0: nbinmap6::clr(bm0, item_free_index, item_index); break;
        }

        // Decrease number of used items
        bin->m_items_count -= 1;
    }

    // convert a pointer to an index within the bin
    u32 bin_ptr2idx(bin32_t const* bin, void* ptr)
    {
        const byte* items = narena::base_ptr(bin->m_items);
        if (ptr < (void*)items)
            return D_U32_MAX;  // invalid pointer
        const u32 item_free_index = narena::current_pos(bin->m_items) / bin->m_item_sizeof;
        const u32 index = (u32)(((const byte*)ptr - items) / bin->m_item_sizeof);
        if (index >= item_free_index)
            return D_U32_MAX;  // invalid pointer
        return index;
    }

    // convert an index to a pointer within the bin
    void* bin_idx2ptr(bin32_t* bin, u32 index)
    {
        const u32 item_free_index = narena::current_pos(bin->m_items) / bin->m_item_sizeof;
        if (index >= item_free_index)
            return nullptr;  // invalid index
        byte* items = narena::base_ptr(bin->m_items);
        byte* item  = items + (index * bin->m_item_sizeof);
        return item;
    }

    // highest index of free item in the bin
    s32 bin_highest_free(bin32_t const* bin)
    {
        const u32 item_free_index = narena::current_pos(bin->m_items) / bin->m_item_sizeof;
        s32       hi        = -1;

        u64* bm0 = (u64*)narena::base_ptr(bin->m_bin);
        u64* bm1 = bm0 + 1;
        u64* bm2 = bm0 + bin->m_bin_level2_offset;
        u64* bm3 = bin->m_bin3 != nullptr ? (u64*)narena::base_ptr(bin->m_bin3) : nullptr;

        switch (bin->m_bin_level_count)
        {
            case 3: hi = nbinmap24::find_last(bm0, bm1, bm2, bm3, item_free_index); break;
            case 2: hi = nbinmap18::find_last(bm0, bm1, bm2, item_free_index); break;
            case 1: hi = nbinmap12::find_last(bm0, bm1, item_free_index); break;
            case 0: hi = nbinmap6::find_last(bm0, item_free_index); break;
        }
        return hi;
    }

    // ----------------------------------------------------------------------------------------------------------------------
    // bin16 implementation
    // ----------------------------------------------------------------------------------------------------------------------

    void bin_setup(bin16_t* bin, u16 item_size, u32 max_items)
    {
        ASSERT(max_items <= 65536);

        const s32 page_size = v_alloc_get_page_size();

        ASSERT(item_size < (page_size >> 1));

        // align the maximum number of items to a multiple of 64 (for binmap)
        max_items = math::alignUp(max_items, 64);

        const int_t items_memory_size = (int_t)item_size * max_items;
        bin->m_items                  = narena::new_arena(items_memory_size, items_memory_size);
        bin->m_items_count            = 0;
        bin->m_item_sizeof            = item_size;

        // compute bin16_t struct and most of the binmap
        nbinmap::layout64_t layout;
        nbinmap::compute(max_items, layout);

        bin->m_bin_level_count = layout.m_levels;
        bin->m_bin             = narena::new_arena(nbinmap::sizeof_data(layout) * sizeof(u64), layout.m_bin2 * sizeof(u64));

        u64* bin0 = (u64*)narena::base_ptr(bin->m_bin);
        *bin0     = D_U64_MAX;
    }

    void bin_destroy(bin16_t* bin)
    {
        if (bin->m_items != nullptr)
            narena::destroy(bin->m_items);
        if (bin->m_bin != nullptr)
            narena::destroy(bin->m_bin);
    }

    u32 bin_size(bin16_t const* bin) { return bin->m_items_count; }
    u32 bin_capacity(bin16_t const* bin) 
    { 
        const u32 max_items = (u32)(narena::reserved_size(bin->m_items) / bin->m_item_sizeof); 
        return max_items > 65536 ? 65536 : max_items;  
    }
    u32 bin_highwater_mark(bin16_t const* bin) { return (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof); }

    void* bin_alloc(bin16_t* bin)
    {
        const u32 items_capacity = (u32)(narena::reserved_size(bin->m_items) / bin->m_item_sizeof);
        if (bin->m_items_count >= items_capacity)
            return nullptr;  // bin is full

        byte* items = narena::base_ptr(bin->m_items);

        u64* bm0 = (u64*)narena::base_ptr(bin->m_bin);
        u64* bm1 = bm0 + 1;
        u64* bm2 = bm1 + 16;

        const u32 item_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);

        if (bin->m_items_count < item_free_index)
        {
            s32 item_index = -1;

            // We should have a free item in the binmap, where is it?
            switch (bin->m_bin_level_count)
            {
                case 2: item_index = nbinmap18::find_and_remove(bm0, bm1, bm2, item_free_index); break;
                case 1: item_index = nbinmap12::find_and_remove(bm0, bm1, item_free_index); break;
                case 0: item_index = nbinmap6::find_and_remove(bm0, item_free_index); break;
            }
            ASSERT(item_index >= 0);

            bin->m_items_count += 1;
            return items + (item_index * bin->m_item_sizeof);
        }
        else
        {
            // Before touching the binmap, make sure we have committed enough memory for it
            switch (bin->m_bin_level_count)
            {
                case 2: narena::commit(bin->m_bin, 1 + 16 + ((item_free_index + 1 + 63) >> 6)); break;
            }

            switch (bin->m_bin_level_count)
            {
                case 2: nbinmap18::tick_lazy(bm0, bm1, bm2, items_capacity, item_free_index); break;
                case 1: nbinmap12::tick_lazy(bm0, bm1, items_capacity, item_free_index); break;
                case 0: break;
            }

            bin->m_items_count += 1;
            void* item = narena::alloc(bin->m_items, bin->m_item_sizeof);
            return item;
        }
    }

    void bin_free(bin16_t* bin, void* ptr)
    {
        const u32 item_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        
        const byte* items      = narena::base_ptr(bin->m_items);
        const s32   item_index = (s32)(((const byte*)ptr - items) / bin->m_item_sizeof);
        if (item_index < 0 || (u32)item_index >= item_free_index)
            return;  // invalid pointer

        // Mark the item as free in the binmap
        u64* bm0 = (u64*)narena::base_ptr(bin->m_bin);
        u64* bm1 = bm0 + 1;
        u64* bm2 = bm1 + 16;

        switch (bin->m_bin_level_count)
        {
            case 2: nbinmap18::clr(bm0, bm1, bm2, item_free_index, item_index); break;
            case 1: nbinmap12::clr(bm0, bm1, item_free_index, item_index); break;
            case 0: nbinmap6::clr(bm0, item_free_index, item_index); break;
        }

        // Decrease number of used items
        bin->m_items_count -= 1;
    }

    // convert a pointer to an index within the bin
    i32 bin_ptr2idx(bin16_t const* bin, void* ptr)
    {
        const byte* items = narena::base_ptr(bin->m_items);
        if (ptr < items)
            return -1;  // invalid pointer
        const u32 index = (u32)(((const byte*)ptr - items) / bin->m_item_sizeof);
        const u32 item_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        if (index >= item_free_index)
            return -1;  // invalid index
        return (i32)index;
    }

    // convert an index to a pointer within the bin
    void* bin_idx2ptr(bin16_t* bin, u16 index)
    {
        const u32 item_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        if (index >= item_free_index)
            return nullptr;  // invalid index
        return narena::base_ptr(bin->m_items) + (index * bin->m_item_sizeof);
    }

    // highest index of free item in the bin
    s32 bin_highest_free(bin16_t const* bin)
    {
        const u32 item_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        s32       hi        = -1;

        u64* bm0 = (u64*)narena::base_ptr(bin->m_bin);
        u64* bm1 = bm0 + 1;
        u64* bm2 = bm1 + 16;

        switch (bin->m_bin_level_count)
        {
            case 2: hi = nbinmap18::find_last(bm0, bm1, bm2, item_free_index); break;
            case 1: hi = nbinmap12::find_last(bm0, bm1, item_free_index); break;
            case 0: hi = nbinmap6::find_last(bm0, item_free_index); break;
        }
        return hi;
    }

}  // namespace ncore

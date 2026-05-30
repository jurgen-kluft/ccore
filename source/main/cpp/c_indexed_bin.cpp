#include "ccore/c_arena.h"
#include "ccore/c_statevec.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_indexed_bin.h"

namespace ncore
{
    // ASCII font Collosal
    // 8888888 888888b.  8888888 888b    888  d888   .d8888b.
    //   888   888  "88b   888   8888b   888 d8888  d88P  Y88b
    //   888   888  .88P   888   88888b  888   888  888
    //   888   8888888K.   888   888Y88b 888   888  888d888b.
    //   888   888  "Y88b  888   888 Y88b888   888  888P "Y88b
    //   888   888    888  888   888  Y88888   888  888    888
    //   888   888   d88P  888   888   Y8888   888  Y88b  d88P
    // 8888888 8888888P" 8888888 888    Y888 8888888 "Y8888P"

    void bin_setup(ibin16_t* bin, u16 element_size)
    {
        ASSERT(element_size > 0 && element_size <= 1024);  // element size should be reasonable

        const u32 max_elements = 65535;  // maximum number of elements is 65535 (u16 indices)

        bin->m_items = narena::new_arena((uint_t)element_size * max_elements, 0);
        bin->m_tags  = narena::new_arena((uint_t)sizeof(u16) * max_elements, 0);

        bin->m_items_count = 0;             // number of items_array currently in use
        bin->m_item_sizeof = element_size;  // sizeof(item)

        bin->m_binmap = narena::new_arena((1 + 1 + 16 + 16 + 1024) * sizeof(u64), (1 + 1 + 16 + 16) * sizeof(u64));

        u64* used0 = (u64*)narena::base_ptr(bin->m_binmap);
        u64* free0 = used0 + 1;
        u64* used1 = free0 + 1;
        u64* free1 = used1 + 16;
        u64* bin2  = free1 + 16;

        nstatevec18::setup_used_lazy(free0, free1, used0, used1, bin2, max_elements);
    }

    void bin_destroy(ibin16_t* bin)
    {
        if (bin->m_tags != nullptr)
            narena::destroy(bin->m_tags);
        if (bin->m_items != nullptr)
            narena::destroy(bin->m_items);
        if (bin->m_binmap != nullptr)
            narena::destroy(bin->m_binmap);
    }

    // resize the bin to be able to hold new_max_elements
    void bin_commit(ibin16_t* bin, u32 num_elements)
    {
        ASSERT(num_elements > 0 && num_elements < 65536);  // maximum number of elements is 65535 (u16 indices)

        narena::commit(bin->m_items, num_elements * bin->m_item_sizeof);
        narena::commit(bin->m_tags, num_elements * sizeof(u16));
        narena::commit(bin->m_binmap, (1 + 1 + 16 + 16 + ((num_elements + 63) / 64)) * sizeof(u64));
    }

    i32 bin_alloc(ibin16_t* bin, u16 tag)
    {
        if (bin->m_items_count >= 65535)
            return -1;  // bin is full

        u64* used0 = narena::base_ptr_as<u64>(bin->m_binmap);
        u64* free0 = used0 + 1;
        u64* used1 = free0 + 1;
        u64* free1 = used1 + 16;
        u64* bin2  = free1 + 16;

        const u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);

        if (bin->m_items_count < items_free_index)
        {
            const s32 item_index = nstatevec18::alloc(free0, free1, used0, used1, bin2, items_free_index);
            ASSERT(item_index >= 0 && (u32)item_index < items_free_index);
            u16* tags_array        = narena::base_ptr_as<u16>(bin->m_tags);
            tags_array[item_index] = tag;
            bin->m_items_count += 1;
            return item_index;
        }
        else
        {
            const s32 item_index = (s32)items_free_index;  // next item index to allocate

            // We need to make sure there is enough committed space in the binmap arena
            narena::commit(bin->m_binmap, (1 + 1 + 16 + 16 + ((items_free_index + 1 + 63) / 64)) * sizeof(u64));
            nstatevec18::tick_used_lazy(free0, free1, used0, used1, bin2, items_free_index, item_index);

            narena::alloc(bin->m_items, bin->m_item_sizeof);

            narena::commit(bin->m_tags, (items_free_index + 1) * sizeof(u16));
            narena::commit(bin->m_binmap, (1 + 1 + 16 + 16 + ((items_free_index + 63) / 64)) * sizeof(u64));

            u16* tags_array        = narena::base_ptr_as<u16>(bin->m_tags);
            tags_array[item_index] = tag;
            bin->m_items_count += 1;
            return item_index;
        }
    }

    // get the tag associated with an item index
    i32 bin_get_tag(ibin16_t const * bin, u32 item_index)
    {
        const u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        if (item_index < items_free_index)
        {
            const u16* tags = narena::base_ptr_as<u16>(bin->m_tags);
            return tags[item_index];
        }
        return -1;
    }

    void bin_set_tag(ibin16_t* bin, u32 item_index, u16 tag)
    {
        const u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        if (item_index < items_free_index)
        {
            u16* tags        = narena::base_ptr_as<u16>(bin->m_tags);
            tags[item_index] = tag;
        }
    }

    void bin_free(ibin16_t* bin, u32 item_index)
    {
        const u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        ASSERT(item_index < items_free_index);  // invalid index

        // mark the item as free in the binmap, and unmark the used bit
        // also decrease the item count, but do not move any items_array (no compaction)

        u64* used0 = narena::base_ptr_as<u64>(bin->m_binmap);
        u64* free0 = used0 + 1;
        u64* used1 = free0 + 1;
        u64* free1 = used1 + 16;
        u64* bin2  = free1 + 16;

        nstatevec18::set_free(free0, free1, used0, used1, bin2, items_free_index, item_index);
        bin->m_items_count -= 1;
    }

    i32 bin_compact(ibin16_t* bin, u32& out_item_index)
    {
        u16*  tags_array  = narena::base_ptr_as<u16>(bin->m_tags);
        byte* items_array = narena::base_ptr(bin->m_items);

        u64* used0 = narena::base_ptr_as<u64>(bin->m_binmap);
        u64* free0 = used0 + 1;
        u64* used1 = free0 + 1;
        u64* free1 = used1 + 16;
        u64* bin2  = free1 + 16;

        const u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        const s32 first_free_index = nstatevec18::find_free(free0, free1, used0, used1, bin2, items_free_index);
        if (first_free_index < 0 || (u32)first_free_index >= items_free_index)
            return -1;  // no free item found, so no compaction possible

        const s32 last_used_index = nstatevec18::find_used_last(free0, free1, used0, used1, bin2, items_free_index);
        ASSERT(last_used_index >= 0 && (u32)last_used_index < items_free_index);

        if (first_free_index >= last_used_index)
            return -1;

        // mark this last item in the duomap as free
        nstatevec18::set_free(free0, free1, used0, used1, bin2, items_free_index, last_used_index);

        if (first_free_index < last_used_index)
        {
            // move the item data
            byte* dst_item = items_array + ((uint_t)first_free_index * bin->m_item_sizeof);
            byte* src_item = items_array + ((uint_t)last_used_index * bin->m_item_sizeof);
            g_memcpy(dst_item, src_item, bin->m_item_sizeof);

            // the moved item now occupies the hole again, so restore that slot as used
            nstatevec18::set_used(free0, free1, used0, used1, bin2, items_free_index, first_free_index);

            // return the owner index of the item that was moved to fill the hole
            tags_array[first_free_index] = tags_array[last_used_index];
            out_item_index               = (u32)first_free_index;
            return last_used_index;
        }

        return -1;  // no swap performed
    }

    void* bin_idx2ptr(ibin16_t const * bin, u32 index)
    {
        const u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        if (index < items_free_index)
        {
            return narena::base_ptr(bin->m_items) + ((int_t)index * bin->m_item_sizeof);
        }
        return nullptr;
    }

    i32 bin_ptr2idx(ibin16_t const * bin, void const * p)
    {
        const byte* ptr              = (const byte*)p;
        const byte* base             = narena::base_ptr(bin->m_items);
        const u32   items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        const byte* end              = base + (items_free_index * bin->m_item_sizeof);
        return (ptr >= base && ptr < end) ? (i32)((ptr - base) / bin->m_item_sizeof) : -1;
    }

    u32 bin_size(ibin16_t const * bin) { return bin->m_items_count; }

    // ASCII font Collosal
    // 8888888 888888b.  8888888 888b    888  .d8888b.   .d8888b.
    //   888   888  "88b   888   8888b   888 d88P  Y88b d88P  Y88b
    //   888   888  .88P   888   88888b  888      .d88P        888
    //   888   8888888K.   888   888Y88b 888     8888"       .d88P
    //   888   888  "Y88b  888   888 Y88b888      "Y8b.  .od888P"
    //   888   888    888  888   888  Y88888 888    888 d88P"
    //   888   888   d88P  888   888   Y8888 Y88b  d88P 888"
    // 8888888 8888888P" 8888888 888    Y888  "Y8888P"  888888888

    const u32 cBIN32_MAX_ELEMENTS = 1 << 24;  // maximum number of elements in the ibin32 (limited by u32 indices)

    void bin_setup(ibin32_t* bin, u16 element_size)
    {
        ASSERT(element_size > 0 && element_size <= 1024);  // element size should be reasonable

        const u32 max_elements = cBIN32_MAX_ELEMENTS;  // maximum number of elements is 16 MILLION

        bin->m_items = narena::new_arena((uint_t)element_size * max_elements, 0);
        bin->m_tags  = narena::new_arena((uint_t)sizeof(u32) * max_elements, 0);

        bin->m_items_count = 0;             // number of items_array currently in use
        bin->m_item_sizeof = element_size;  // sizeof(item)

        bin->m_binmap   = narena::new_arena((1 + 1 + 64 + 64 + 4096) * sizeof(u64), 8);
        bin->m_binmap2u = narena::new_arena(4096 * sizeof(u64), 8);
        bin->m_binmap3  = narena::new_arena(4096 * 64 * sizeof(u64), 8);

        u64* free0 = narena::base_ptr_as<u64>(bin->m_binmap);
        u64* used0 = free0 + 1;
        u64* free1 = used0 + 1;
        u64* used1 = free1 + 64;
        u64* free2 = used1 + 64;
        u64* used2 = narena::base_ptr_as<u64>(bin->m_binmap2u);
        u64* bin3  = narena::base_ptr_as<u64>(bin->m_binmap3);

        nstatevec24::setup_used_lazy(free0, free1, used0, used1, free2, used2, bin3, max_elements);
    }

    void bin_destroy(ibin32_t* bin)
    {
        if (bin->m_tags != nullptr)
            narena::destroy(bin->m_tags);
        if (bin->m_items != nullptr)
            narena::destroy(bin->m_items);
        if (bin->m_binmap != nullptr)
            narena::destroy(bin->m_binmap);
        if (bin->m_binmap2u != nullptr)
            narena::destroy(bin->m_binmap2u);
        if (bin->m_binmap3 != nullptr)
            narena::destroy(bin->m_binmap3);
    }

    // resize the bin to be able to hold new_max_elements
    void bin_commit(ibin32_t* bin, u32 num_elements)
    {
        ASSERT(num_elements > 0 && num_elements < cBIN32_MAX_ELEMENTS);  // maximum number of elements is 16 million

        narena::commit(bin->m_items, num_elements * bin->m_item_sizeof);
        narena::commit(bin->m_tags, num_elements * sizeof(u32));
        narena::commit(bin->m_binmap, (1 + 1 + 64 + 64 + ((num_elements + 63) / 64)) * sizeof(u64));
        narena::commit(bin->m_binmap2u, ((num_elements + (1 << 12) - 1) >> 12) * sizeof(u64));
        narena::commit(bin->m_binmap3, ((num_elements + (1 << 6) - 1) >> 6) * sizeof(u64));
    }

    i32 bin_alloc(ibin32_t* bin, u32 tag)
    {
        if (bin->m_items_count >= cBIN32_MAX_ELEMENTS)
            return -1;  // bin is full

        u64* free0 = narena::base_ptr_as<u64>(bin->m_binmap);
        u64* used0 = free0 + 1;
        u64* free1 = used0 + 1;
        u64* used1 = free1 + 64;
        u64* free2 = used1 + 64;
        u64* used2 = narena::base_ptr_as<u64>(bin->m_binmap2u);
        u64* bin3  = narena::base_ptr_as<u64>(bin->m_binmap3);

        u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);

        if (bin->m_items_count < items_free_index)
        {
            const s32 item_index = nstatevec24::alloc(free0, free1, free2, used0, used1, used2, bin3, items_free_index);
            ASSERT(item_index >= 0 && (u32)item_index < items_free_index);
            u32* tags_array        = narena::base_ptr_as<u32>(bin->m_tags);
            tags_array[item_index] = tag;
            bin->m_items_count += 1;
            return item_index;
        }
        else
        {
            const s32 item_index = (s32)items_free_index;
            items_free_index++;

            // We need to make sure there is enough committed space in the binmap arena
            bin_commit(bin, items_free_index);  // this will also commit the necessary binmap space
            nstatevec24::tick_used_lazy(free0, free1, free2, used0, used1, used2, bin3, items_free_index, item_index);

            narena::alloc(bin->m_items, bin->m_item_sizeof);

            u32* tags_array        = narena::base_ptr_as<u32>(bin->m_tags);
            tags_array[item_index] = tag;
            bin->m_items_count += 1;
            return item_index;
        }
    }

    // get the tag associated with an item index
    i32 bin_get_tag(ibin32_t const * bin, u32 item_index)
    {
        const u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        if (item_index < items_free_index)
        {
            const u32* tags = narena::base_ptr_as<u32>(bin->m_tags);
            return tags[item_index];
        }
        return -1;
    }

    void bin_set_tag(ibin32_t* bin, u32 item_index, u32 tag)
    {
        const u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        if (item_index < items_free_index)
        {
            u32* tags        = narena::base_ptr_as<u32>(bin->m_tags);
            tags[item_index] = tag;
        }
    }

    void bin_free(ibin32_t* bin, u32 item_index)
    {
        const u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        ASSERT(item_index < items_free_index);  // invalid index

        // mark the item as free in the binmap, and unmark the used bit
        // also decrease the item count, but do not move any items_array (no compaction)

        u64* free0 = narena::base_ptr_as<u64>(bin->m_binmap);
        u64* used0 = free0 + 1;
        u64* free1 = used0 + 1;
        u64* used1 = free1 + 64;
        u64* free2 = used1 + 64;
        u64* used2 = narena::base_ptr_as<u64>(bin->m_binmap2u);
        u64* bin3  = narena::base_ptr_as<u64>(bin->m_binmap3);

        nstatevec24::set_free(free0, free1, free2, used0, used1, used2, bin3, items_free_index, item_index);
        bin->m_items_count -= 1;
    }

    i32 bin_compact(ibin32_t* bin, u32& out_item_index)
    {
        u32*  tags_array  = narena::base_ptr_as<u32>(bin->m_tags);
        byte* items_array = narena::base_ptr(bin->m_items);

        u64* free0 = narena::base_ptr_as<u64>(bin->m_binmap);
        u64* used0 = free0 + 1;
        u64* free1 = used0 + 1;
        u64* used1 = free1 + 64;
        u64* free2 = used1 + 64;
        u64* used2 = narena::base_ptr_as<u64>(bin->m_binmap2u);
        u64* bin3  = narena::base_ptr_as<u64>(bin->m_binmap3);

        const u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);

        const s32 first_free_index = nstatevec24::find_free(free0, free1, free2, used0, used1, used2, bin3, items_free_index);
        if (first_free_index < 0 || (u32)first_free_index >= items_free_index)
            return -1;  // no free item found, so no compaction possible

        const s32 last_used_index = nstatevec24::find_used_last(free0, free1, free2, used0, used1, used2, bin3, items_free_index);
        ASSERT(last_used_index >= 0 && (u32)last_used_index < items_free_index);

        if (first_free_index >= last_used_index)
            return -1;

        // mark this last item in the duomap as free
        nstatevec24::set_free(free0, free1, free2, used0, used1, used2, bin3, items_free_index, last_used_index);

        if (first_free_index < last_used_index)
        {
            // move the item data
            byte* dst_item = items_array + ((uint_t)first_free_index * bin->m_item_sizeof);
            byte* src_item = items_array + ((uint_t)last_used_index * bin->m_item_sizeof);
            g_memcpy(dst_item, src_item, bin->m_item_sizeof);

            // the moved item now occupies the hole again, so restore that slot as used
            nstatevec24::set_used(free0, free1, free2, used0, used1, used2, bin3, items_free_index, first_free_index);

            // return the owner index of the item that was moved to fill the hole
            tags_array[first_free_index] = tags_array[last_used_index];
            out_item_index               = (u32)first_free_index;
            return last_used_index;
        }

        return -1;  // no swap performed
    }

    void* bin_idx2ptr(ibin32_t const * bin, u32 index)
    {
        const u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);
        if (index < items_free_index)
        {
            return narena::base_ptr(bin->m_items) + ((int_t)index * bin->m_item_sizeof);
        }
        return nullptr;
    }

    i32 bin_ptr2idx(ibin32_t const * bin, void const * p)
    {
        const u32 items_free_index = (u32)(narena::current_pos(bin->m_items) / bin->m_item_sizeof);

        const byte* ptr  = (const byte*)p;
        const byte* base = narena::base_ptr(bin->m_items);
        const byte* end  = base + (items_free_index * bin->m_item_sizeof);
        return (ptr >= base && ptr < end) ? (i32)((ptr - base) / bin->m_item_sizeof) : -1;
    }

    u32 bin_size(ibin32_t const * bin) { return bin->m_items_count; }

}  // namespace ncore

#include "ccore/c_arena.h"
#include "ccore/c_duomap1.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_indexed_bin.h"

namespace ncore
{

    // indexed bin16
    // Note: uses nduomap18 to track free and used items

    void bin_setup(indexed_bin16_t* bin, u16 element_size)
    {
        ASSERT(element_size > 0 && element_size <= 1024);  // element size should be reasonable

        const u32 max_elements = 65535;  // maximum number of elements is 65535 (u16 indices)

        bin->m_items = narena::new_arena((uint_t)element_size * max_elements, 0);
        bin->m_owner = narena::new_arena((uint_t)sizeof(u16) * max_elements, 0);

        bin->m_items_count      = 0;             // number of items currently in use
        bin->m_item_sizeof      = element_size;  // sizeof(item)
        bin->m_items_free_index = 0;             // index of the first free slot in the items array

        bin->m_binmap = narena::new_arena((1 + 1 + 16 + 16 + 1024) * sizeof(u64), (1 + 1 + 16 + 16) * sizeof(u64));

        u64* used0 = (u64*)narena::base_ptr(bin->m_binmap);
        u64* free0 = used0 + 1;
        u64* used1 = free0 + 1;
        u64* free1 = used1 + 16;
        u64* bin2  = free1 + 16;

        nduomap18::setup_used_lazy(free0, free1, used0, used1, bin2, max_elements);
    }

    void bin_destroy(indexed_bin16_t* bin)
    {
        if (bin->m_owner != nullptr)
            narena::destroy(bin->m_owner);
        if (bin->m_items != nullptr)
            narena::destroy(bin->m_items);
        if (bin->m_binmap != nullptr)
            narena::destroy(bin->m_binmap);
    }

    // resize the bin to be able to hold new_max_elements
    void bin_commit(indexed_bin16_t* bin, u32 num_elements)
    {
        ASSERT(num_elements > 0 && num_elements < 65536);  // maximum number of elements is 65535 (u16 indices)

        narena::commit(bin->m_items, num_elements * bin->m_item_sizeof);
        narena::commit(bin->m_owner, num_elements * sizeof(u16));
        narena::commit(bin->m_binmap, (1 + 1 + 16 + 16 + ((num_elements + 63) / 64)) * sizeof(u64));
    }

    i32 bin_alloc(indexed_bin16_t* bin, u16 owner_index)
    {
        if (bin->m_items_count >= 65535)
            return -1;  // bin is full

        // 1. if we are compacted, just do an allocation at the end of the array's
        //    - clear the free bit
        //    - set the used bit
        // 2. else find an empty slot by querying the free binmap
        //    - clear the free bit
        //    - set the used bit
        u64* used0 = (u64*)narena::base_ptr(bin->m_binmap);
        u64* free0 = used0 + 1;
        u64* used1 = free0 + 1;
        u64* free1 = used1 + 16;
        u64* bin2  = free1 + 16;

        if (bin->m_items_count < bin->m_items_free_index)
        {
            const s32 item_index = nduomap18::alloc(free0, free1, used0, used1, bin2, bin->m_items_free_index);
            ASSERT(item_index >= 0 && (u32)item_index < bin->m_items_free_index);
            u16* owner_array        = narena::base_ptr_as<u16>(bin->m_owner);
            owner_array[item_index] = owner_index;
            bin->m_items_count += 1;
            return item_index;
        }
        else
        {
            const s32 item_index = (s32)bin->m_items_free_index++;

            // We need to make sure there is enough committed space in the binmap arena
            narena::commit(bin->m_binmap, (1 + 1 + 16 + 16 + ((bin->m_items_free_index + 1 + 63) / 64)) * sizeof(u64));
            nduomap18::tick_used_lazy(free0, free1, used0, used1, bin2, bin->m_items_free_index, item_index);

            narena::alloc(bin->m_items, bin->m_item_sizeof);
            narena::commit(bin->m_owner, bin->m_items_free_index * sizeof(u16));
            narena::commit(bin->m_binmap, (1 + 1 + 16 + 16 + ((bin->m_items_free_index + 63) / 64)) * sizeof(u64));

            u16* owner_array        = narena::base_ptr_as<u16>(bin->m_owner);
            owner_array[item_index] = owner_index;
            bin->m_items_count += 1;
            return item_index;
        }
    }

    void bin_free_normal(indexed_bin16_t* bin, u32 item_index)
    {
        ASSERT(item_index < bin->m_items_count);  // invalid index

        // mark the item as free in the binmap, and unmark the used bit
        // also decrease the item count, but do not move any items (no compaction)

        u64* used0 = (u64*)narena::base_ptr(bin->m_binmap);
        u64* free0 = used0 + 1;
        u64* used1 = free0 + 1;
        u64* free1 = used1 + 16;
        u64* bin2  = free1 + 16;

        nduomap18::set_free(free0, free1, used0, used1, bin2, bin->m_items_free_index, item_index);

        bin->m_items_count -= 1;
    }

    // free item at 'index' and return index of item that was moved
    // to fill the hole (likely last item), or -1 if no swap performed
    // returns -2 if error (invalid index)
    i32 bin_free_compact(indexed_bin16_t* bin, u32 item_index)
    {
        if (item_index < bin->m_items_count)
        {
            u16*  owner_index_array = narena::base_ptr_as<u16>(bin->m_owner);
            byte* items             = narena::base_ptr(bin->m_items);

            u64* used0 = (u64*)narena::base_ptr(bin->m_binmap);
            u64* free0 = used0 + 1;
            u64* used1 = free0 + 1;
            u64* free1 = used1 + 16;
            u64* bin2  = free1 + 16;

            const s32 last_index = nduomap18::find_used_last(free0, free1, used0, used1, bin2, bin->m_items_free_index);
            ASSERT(last_index >= 0 && (u32)last_index < bin->m_items_free_index);

            // mark this last item in the binmap as free
            nduomap18::set_free(free0, free1, used0, used1, bin2, bin->m_items_free_index, last_index);

            // decrease number of used items
            bin->m_items_count -= 1;

            if (item_index != (u32)last_index)
            {
                // move the item data
                byte* dst_item = items + ((uint_t)item_index * bin->m_item_sizeof);
                byte* src_item = items + ((uint_t)last_index * bin->m_item_sizeof);
                g_memcpy(dst_item, src_item, bin->m_item_sizeof);

                // return the owner index of the item that was moved to fill the hole
                owner_index_array[item_index] = owner_index_array[last_index];
                return (i32)owner_index_array[item_index];
            }

            return -1;  // no swap performed
        }
        return -2;  // error
    }

    i32 bin_compact(indexed_bin16_t* bin, u32& item_index)
    {
        // find the first free slot using the free binmap
        // find the last used slot using the used binmap
        // if the first free slot is before the last used slot, move the item from the
        // last used slot to the first free slot, and update the binmaps and owner index array
        // return the owner index of the item that was moved to fill the hole, and update
        // item_index to the new index of the item

        return -1;  // no swap performed
    }

    void* bin_idx2ptr(indexed_bin16_t const * bin, u32 index)
    {
        if (index < bin->m_items_count)
        {
            return narena::base_ptr(bin->m_items) + ((int_t)index * bin->m_item_sizeof);
        }
        return nullptr;
    }

    i32 bin_ptr2idx(indexed_bin16_t const * bin, void const * p)
    {
        const byte* ptr  = (const byte*)p;
        const byte* base = narena::base_ptr(bin->m_items);
        const byte* end  = base + (bin->m_items_count * bin->m_item_sizeof);
        return (ptr >= base && ptr < end) ? (i32)((ptr - base) / bin->m_item_sizeof) : -1;
    }

    u32 bin_size(indexed_bin16_t const * bin) { return bin->m_items_count; }

}  // namespace ncore

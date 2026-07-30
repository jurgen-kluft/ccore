#include "ccore/c_bitvec.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"

#include "ccore/c_chunk_bin.h"

namespace ncore
{

    //  .d8888b.  888    888 888     888 888b    888 888    d8P
    // d88P  Y88b 888    888 888     888 8888b   888 888   d8P
    // 888    888 888    888 888     888 88888b  888 888  d8P
    // 888        8888888888 888     888 888Y88b 888 888d88K
    // 888        888    888 888     888 888 Y88b888 8888888b
    // 888    888 888    888 888     888 888  Y88888 888  Y88b
    // Y88b  d88P 888    888 Y88b. .d88P 888   Y8888 888   Y88b
    //  "Y8888P"  888    888  "Y88888P"  888    Y888 888    Y88b

    const u16 cINVALID_CHUNK_INDEX = 0xFFFF;

    // Chunk
    // - max items <= (1 << 10) = 1024
    struct cchunk_t
    {
        u16 m_free_index;  // highwater mark free index
        u16 m_item_count;  // number of items currently allocated in this chunk
        u16 m_prev;        // previous chunk in list
        u16 m_next;        // next chunk in list
        u32 m_layer0;      // bitvec layer0
        // u32 m_layer1[]; // bitvec layer1, size = m_chunk_max_items / 32
    };

    struct cbin_t
    {
        void* m_address_base;
        u32   m_address_size_in_pages;
        u32   m_bin_size_in_pages;
        u32   m_total_items_count;
        u16   m_chunk_max_count;
        u16   m_chunk_free_index;
        u16   m_chunk_max_items;
        u16   m_sizeof_item;
        u16   m_chunk_sizeof;
        u16   m_chunk_free_list_head;
        u16   m_chunk_active_list_head;
        u8    m_chunk_size_shift;
        u8    m_page_size_shift;
        b8    m_ownership;
    };

    static inline byte* s_chunks(cbin_t* bin) { return (byte*)bin + sizeof(cbin_t); }

    static inline cchunk_t* s_get_chunk(cbin_t* bin, u32 chunk_index)
    {
        // cchunk_t is a variable-sized struct, so we calculate the address of the chunk based
        // on the chunk index and the number of u32 entries per chunk.
        return (cchunk_t*)(s_chunks(bin) + (chunk_index * bin->m_chunk_sizeof));
    }

    static inline void s_push_to_list(cbin_t* bin, u16* list_head, u16 chunk_index)
    {
        cchunk_t* chunk = s_get_chunk(bin, chunk_index);
        chunk->m_prev   = cINVALID_CHUNK_INDEX;
        chunk->m_next   = *list_head;

        if (*list_head != cINVALID_CHUNK_INDEX)
        {
            s_get_chunk(bin, *list_head)->m_prev = chunk_index;
        }

        *list_head = chunk_index;
    }

    static inline u16 s_pop_from_list(cbin_t* bin, u16* list_head)
    {
        if (*list_head == cINVALID_CHUNK_INDEX)
        {
            return cINVALID_CHUNK_INDEX;
        }

        u16       chunk_index = *list_head;
        cchunk_t* chunk       = s_get_chunk(bin, chunk_index);

        *list_head = chunk->m_next;

        if (chunk->m_next != cINVALID_CHUNK_INDEX)
        {
            s_get_chunk(bin, chunk->m_next)->m_prev = cINVALID_CHUNK_INDEX;
        }

        chunk->m_prev = cINVALID_CHUNK_INDEX;
        chunk->m_next = cINVALID_CHUNK_INDEX;

        return chunk_index;
    }

    static inline void s_remove_from_list(cbin_t* bin, u16* list_head, u16 chunk_index)
    {
        ASSERT(chunk_index != cINVALID_CHUNK_INDEX);

        cchunk_t* chunk      = s_get_chunk(bin, chunk_index);
        u16 const prev_index = chunk->m_prev;
        u16 const next_index = chunk->m_next;

        if (prev_index != cINVALID_CHUNK_INDEX)
        {
            s_get_chunk(bin, prev_index)->m_next = next_index;
        }
        else
        {
            ASSERT(*list_head == chunk_index);
            *list_head = next_index;
        }

        if (next_index != cINVALID_CHUNK_INDEX)
        {
            s_get_chunk(bin, next_index)->m_prev = prev_index;
        }

        chunk->m_prev = cINVALID_CHUNK_INDEX;
        chunk->m_next = cINVALID_CHUNK_INDEX;
    }
    static inline void s_chunk_init(cbin_t* bin, cchunk_t* chunk)
    {
        chunk->m_free_index = 0;
        chunk->m_item_count = 0;                 // no items allocated yet
        chunk->m_prev       = cINVALID_CHUNK_INDEX;
        chunk->m_next       = cINVALID_CHUNK_INDEX;
        u32* layer0         = &chunk->m_layer0;  // bitvec data starts right after the chunk header
        u32* layer1         = layer0 + 1;        // layer1 starts right after layer0

        // TODO setup_used_lazy

        nbitvec10::set_all_free(layer0, layer1, bin->m_chunk_max_items);
    }

    static void* s_chunk_alloc_item(cbin_t* bin, cchunk_t* chunk, u32 chunk_index)
    {
        u32* layer0 = &chunk->m_layer0;  // bitvec data starts right after the chunk header
        u32* layer1 = layer0 + 1;        // layer1 starts right after layer0

        // TODO tick_used_lazy and use count + free index to optimize free slot search

        s32 free_item_index = -1;
        free_item_index     = nbitvec10::find_free_and_remove(layer0, layer1, bin->m_chunk_max_items);

        if (free_item_index >= 0)
        {
            chunk->m_item_count += 1;

            const u32 chunk_size    = (u32)1 << bin->m_chunk_size_shift;
            byte*     chunk_address = (byte*)bin->m_address_base + ((uint_t)chunk_index * chunk_size);
            return chunk_address + ((uint_t)free_item_index * bin->m_sizeof_item);
        }

        return nullptr;
    }

    // free an item back to the chunk, this is called when an item is freed
    static void s_chunk_free_item(cbin_t* bin, cchunk_t* chunk, u32 chunk_index, void* item_ptr)
    {
        const u32 chunk_size = (u32)1 << bin->m_chunk_size_shift;

        // Add the item back to the free list of the chunk
        byte*     chunk_address = (byte*)bin->m_address_base + ((uint_t)chunk_index * chunk_size);
        const u16 item_index    = (u16)(((byte*)item_ptr - chunk_address) / bin->m_sizeof_item);

        // Mark this item as free in the bitvec
        u32* layer0 = &chunk->m_layer0;
        u32* layer1 = layer0 + 1;  // layer1 starts right after layer0
        nbitvec10::set_free(layer0, layer1, bin->m_chunk_max_items, item_index);
        chunk->m_item_count -= 1;  // decrease item count

        // TODO: Find the top free index for the chunk, and update the free index if necessary
        // Note: Currently there is no function in nbitvec10 that can lower our free index
    }

    static void s_commit_chunk_memory(cbin_t* bin, u32 chunk_index)
    {
        const u32 chunk_size    = (u32)1 << bin->m_chunk_size_shift;
        byte*     chunk_address = (byte*)bin->m_address_base + ((uint_t)chunk_index * chunk_size);
        v_alloc_commit(chunk_address, chunk_size);
    }

    static void s_decommit_chunk_memory(cbin_t* bin, u32 chunk_index)
    {
        const u32 chunk_size    = (u32)1 << bin->m_chunk_size_shift;
        byte*     chunk_address = (byte*)bin->m_address_base + ((uint_t)chunk_index * chunk_size);
        v_alloc_decommit(chunk_address, chunk_size);
    }

    //        d8888 888      888      .d88888b.   .d8888b.
    //       d88888 888      888     d88P" "Y88b d88P  Y88b
    //      d88P888 888      888     888     888 888    888
    //     d88P 888 888      888     888     888 888
    //    d88P  888 888      888     888     888 888
    //   d88P   888 888      888     888     888 888    888
    //  d8888888888 888      888     Y88b. .d88P Y88b  d88P
    // d88P     888 88888888 88888888 "Y88888P"   "Y8888P"

    void* bin_alloc(cbin_t* bin)
    {
        cchunk_t* active_chunk       = nullptr;
        s32       active_chunk_index = -1;

        if (bin->m_chunk_active_list_head != cINVALID_CHUNK_INDEX)
        {
            // Get the active chunk from the head of the active chunk list
            active_chunk_index = bin->m_chunk_active_list_head;
            active_chunk       = s_get_chunk(bin, active_chunk_index);
        }
        else if (bin->m_chunk_free_list_head != cINVALID_CHUNK_INDEX)
        {
            active_chunk_index = s_pop_from_list(bin, &bin->m_chunk_free_list_head);
            active_chunk       = s_get_chunk(bin, active_chunk_index);
            s_chunk_init(bin, active_chunk);
            s_commit_chunk_memory(bin, active_chunk_index);
            s_push_to_list(bin, &bin->m_chunk_active_list_head, active_chunk_index);
        }
        else
        {
            // No active or free chunks, need to allocate a new chunk
            if (bin->m_chunk_free_index >= bin->m_chunk_max_count)
            {
                // No more space for new chunks
                return nullptr;
            }

            active_chunk_index = bin->m_chunk_free_index++;
            active_chunk       = s_get_chunk(bin, active_chunk_index);
            s_chunk_init(bin, active_chunk);
            s_commit_chunk_memory(bin, active_chunk_index);
            s_push_to_list(bin, &bin->m_chunk_active_list_head, active_chunk_index);
        }

        ASSERT(active_chunk != nullptr && active_chunk_index >= 0 && active_chunk_index < bin->m_chunk_free_index);
        void* item = s_chunk_alloc_item(bin, active_chunk, (u32)active_chunk_index);
        ASSERT(item != nullptr);

        if (active_chunk->m_item_count >= bin->m_chunk_max_items)
        {
            // This chunk is now full, remove it from the active chunk list
            s_remove_from_list(bin, &bin->m_chunk_active_list_head, (u16)active_chunk_index);
        }

        bin->m_total_items_count += 1;
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

    void bin_free(cbin_t* bin, void* ptr)
    {
        const uint_t address_size = (uint_t)bin->m_address_size_in_pages << bin->m_page_size_shift;
        ASSERT(ptr != nullptr && ptr >= bin->m_address_base && ptr < (byte*)bin->m_address_base + address_size);

        const u8 chunk_size_shift = bin->m_chunk_size_shift;

        // Find the chunk this item belongs to
        const u32 chunk_index = (u32)((uint_t)((byte*)ptr - (byte*)bin->m_address_base) >> chunk_size_shift);
        cchunk_t* chunk       = s_get_chunk(bin, chunk_index);

        const bool chunk_was_full = (chunk->m_item_count >= bin->m_chunk_max_items);

        // Free the item from the chunk
        s_chunk_free_item(bin, chunk, chunk_index, ptr);

        bin->m_total_items_count -= 1;

        if (chunk_was_full)
        {
            // This chunk was full before, now it has a free item, add it back to the active chunk list
            s_push_to_list(bin, &bin->m_chunk_active_list_head, chunk_index);
        }
        else if (chunk->m_item_count == 0)
        {
            // This chunk is now empty, move it to the free list and decommit its backing pages.
            s_remove_from_list(bin, &bin->m_chunk_active_list_head, (u16)chunk_index);
            s_push_to_list(bin, &bin->m_chunk_free_list_head, chunk_index);
            s_decommit_chunk_memory(bin, chunk_index);
        }
    }

    //  .d8888b.  8888888888 88888888888 888     888 8888888b.
    // d88P  Y88b 888            888     888     888 888   Y88b
    // Y88b.      888            888     888     888 888    888
    //  "Y888b.   8888888        888     888     888 888   d88P
    //     "Y88b. 888            888     888     888 8888888P"
    //       "888 888            888     888     888 888
    // Y88b  d88P 888            888     Y88b. .d88P 888
    //  "Y8888P"  8888888888     888      "Y88888P"  888

    const u32 s_max_items_per_chunk = 1024;
    const u32 s_min_chunk_size      = 16 * cKB;
    const u32 s_max_chunk_size      = 64 * cKB;

    struct cbin_layout_t
    {
        u32 m_chunk_size;         // size of each chunk in bytes
        u32 m_items_per_chunk;    // number of items per chunk
        u32 m_chunk_layer1_size;  // size of layer1 bitvec in bytes
        u32 m_chunk_sizeof;       // size of chunk_t + layer1 in bytes
        u32 m_max_chunk_count;    // maximum number of chunks that can be allocated
        u32 m_required_pages;     // required pages for the bin structure including chunk array
    };

    static void s_setup_bin(cbin_t* bin, cbin_layout_t const& layout, void* base_address, uint_t base_size, u16 item_sizeof)
    {
        bin->m_chunk_max_count  = (u16)layout.m_max_chunk_count;
        bin->m_chunk_size_shift = (u8)math::ilog2(layout.m_chunk_size);
        bin->m_chunk_sizeof     = (u16)layout.m_chunk_sizeof;

        bin->m_chunk_free_index  = 0;
        bin->m_total_items_count = 0;
        bin->m_chunk_max_items   = (u16)(layout.m_items_per_chunk);
        bin->m_sizeof_item       = item_sizeof;
        bin->m_chunk_free_list_head   = cINVALID_CHUNK_INDEX;
        bin->m_chunk_active_list_head = cINVALID_CHUNK_INDEX;
        if (base_address != nullptr)
        {
            bin->m_address_base = base_address;
            bin->m_ownership    = false;
        }
        else
        {
            bin->m_address_base = v_alloc_reserve(base_size);
            ASSERT(bin->m_address_base != nullptr);
            bin->m_ownership    = true;
        }
        bin->m_page_size_shift       = v_alloc_get_page_size_shift();
        bin->m_address_size_in_pages = (u32)(base_size >> bin->m_page_size_shift);
        bin->m_bin_size_in_pages     = layout.m_required_pages;
    }

    static void s_bin_calculate_size(uint_t base_size, u16 item_sizeof, cbin_layout_t& layout)
    {
        ASSERT(item_sizeof <= (32 * cKB));

        // Calculate the chunk size based on the item size and maximum items per chunk
        u32 chunk_size = item_sizeof * s_max_items_per_chunk;
        chunk_size     = math::floorpo2(chunk_size);                                   // round down to nearest power of two
        chunk_size     = math::clamp(chunk_size, s_min_chunk_size, s_max_chunk_size);  // clamp to min/max chunk size

        const u32 items_per_chunk = math::min(chunk_size / item_sizeof, s_max_items_per_chunk);
        ASSERT(items_per_chunk >= 2 && items_per_chunk <= s_max_items_per_chunk);

        // Calculate the number of u32s needed for layer1, then calculate the size of a chunk_t structure including layer1
        ASSERT((sizeof(cchunk_t) & 3) == 0);  // ensure chunk struct is a multiple of u32
        const u32 chunk_layer1 = (items_per_chunk + 31) / 32;
        const u16 chunk_sizeof = sizeof(cchunk_t) + (u16)(chunk_layer1 * sizeof(u32));

        // Calculate the maximum number of chunks based on the reserved size and chunk size
        const u32 max_chunk_count = (u32)(base_size / chunk_size);
        ASSERT(max_chunk_count > 0 && max_chunk_count <= cINVALID_CHUNK_INDEX);

        // Calculate the total size needed for the bin structure and the chunk array
        const u8 page_size_shift = v_alloc_get_page_size_shift();
        const uint_t page_size   = (uint_t)1 << page_size_shift;
        uint_t required_size     = sizeof(cbin_t) + ((uint_t)max_chunk_count * (uint_t)chunk_sizeof);
        required_size            = math::alignUp(required_size, page_size);
        const uint_t required_pages = required_size >> page_size_shift;
        ASSERT(required_pages > 0 && required_pages <= 0xFFFFFFFFULL);

        layout.m_chunk_size        = chunk_size;
        layout.m_items_per_chunk   = items_per_chunk;
        layout.m_chunk_layer1_size = chunk_layer1 * sizeof(u32);
        layout.m_chunk_sizeof      = chunk_sizeof;
        layout.m_max_chunk_count   = max_chunk_count;
        layout.m_required_pages    = (u32)required_pages;
    }

    u32 bin_calculate_size(uint_t base_size, u16 item_sizeof)
    {
        cbin_layout_t layout;
        s_bin_calculate_size(base_size, item_sizeof, layout);
        return layout.m_required_pages;
    }

    cbin_t* bin_setup(void* bin_address, u32 bin_size_in_pages, void* base_address, uint_t base_size, u16 item_sizeof)
    {
        ASSERT(bin_address != nullptr);
        ASSERT(item_sizeof <= (32 * cKB));

        cbin_layout_t layout;
        s_bin_calculate_size(base_size, item_sizeof, layout);
        ASSERTS(bin_size_in_pages >= layout.m_required_pages, "Error: bin_size is too small for the requested base_size and item_sizeof");

        cbin_t* bin = (cbin_t*)bin_address;
        g_memclr(bin, sizeof(cbin_t));
        s_setup_bin(bin, layout, base_address, base_size, item_sizeof);

        return bin;
    }

    u32 bin_size(cbin_t const* bin)
    {
        // The global item count
        return bin->m_total_items_count;
    }

    // 8888888b.  8888888888 .d8888b. 88888888888 8888888b.   .d88888b. Y88b   d88P
    // 888  "Y88b 888       d88P  Y88b    888     888   Y88b d88P" "Y88b Y88b d88P
    // 888    888 888       Y88b.         888     888    888 888     888  Y88o88P
    // 888    888 8888888    "Y888b.      888     888   d88P 888     888   Y888P
    // 888    888 888           "Y88b.    888     8888888P"  888     888    888
    // 888    888 888             "888    888     888 T88b   888     888    888
    // 888  .d88P 888       Y88b  d88P    888     888  T88b  Y88b. .d88P    888
    // 8888888P"  8888888888 "Y8888P"     888     888   T88b  "Y88888P"     888

    void bin_destroy(cbin_t* bin)
    {
        if (bin->m_address_base != nullptr && bin->m_ownership)
        {
            const uint_t address_size = (uint_t)bin->m_address_size_in_pages << bin->m_page_size_shift;
            v_alloc_release(bin->m_address_base, address_size);
        }

        g_memclr(bin, sizeof(cbin_t));
    }

}  // namespace ncore

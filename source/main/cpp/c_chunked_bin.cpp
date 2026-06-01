#include "ccore/c_arena.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"

#include "ccore/c_chunked_bin.h"

namespace ncore
{
    const u32 c_list_null_index = 0xFFFFFFFF;

    // template functions to manipulate a doubly-linked list using u32 indices, where 0xFFFFFFFF means null
    // Note: these functions require the type to have the first two members to be for the next and prev
    // indices, e.g.:
    // struct item_t
    // {
    //     u32 m_next;
    //     u32 m_prev;
    //     // other members...
    // };
    // or:
    // struct item_t
    // {
    //     u64 m_dlist; // next and prev indices packed in a u64, where the lower 32 bits are the next index and the upper 32 bits are the prev index
    //     // other members...
    // };
    //

#define DLIST_NEXT 0
#define DLIST_PREV 1

    static inline void s_dlist_insert(void* array, u32 item_size, u32& head, u32& item_count, u32 index)
    {
        u32* item        = (u32*)((byte*)array + index * item_size);
        item[DLIST_NEXT] = head;
        item[DLIST_PREV] = c_list_null_index;
        if (head != c_list_null_index)
        {
            u32* head_item        = (u32*)((byte*)array + head * item_size);
            head_item[DLIST_PREV] = index;
        }
        head = index;
        ++item_count;
    }

    static inline void s_dlist_remove(void* array, u32 item_size, u32& head, u32& item_count, u32 index)
    {
        u32*      item = (u32*)((byte*)array + index * item_size);
        const u32 next = item[DLIST_NEXT];
        const u32 prev = item[DLIST_PREV];
        if (next != c_list_null_index)
        {
            u32* next_item        = (u32*)((byte*)array + next * item_size);
            next_item[DLIST_PREV] = prev;
        }
        if (prev != c_list_null_index)
        {
            u32* prev_item        = (u32*)((byte*)array + prev * item_size);
            prev_item[DLIST_NEXT] = next;
        }
        else
        {
            head = next;
        }
        --item_count;
    }

    static inline u32 s_dlist_pop(void* array, u32 item_size, u32& head, u32& item_count)
    {
        if (head != c_list_null_index)
        {
            const u32 item     = head;
            u32*      item_ptr = (u32*)((byte*)array + item * item_size);
            head               = item_ptr[DLIST_NEXT];
            if (head != c_list_null_index)
            {
                u32* head_ptr        = (u32*)((byte*)array + head * item_size);
                head_ptr[DLIST_PREV] = c_list_null_index;
            }
            --item_count;
            return item;
        }
        return c_list_null_index;
    }

    // Chunk
    // - max items < 32768, so that we can use a u16 for the free list and item count
    struct cchunk_t
    {
        u32 m_next;        // next chunk in the free/active chunk list, or 0xFFFFFFFF if this is the last chunk in the list
        u32 m_prev;        // previous chunk in the free/active chunk list, or 0xFFFFFFFF if this is the first chunk in the list
        u16 m_free_head;   // free bit vector for items in this chunk, 1 means free, 0 means used
        u16 m_free_index;  // index of the first free item in this chunk (relative to the chunk)
        u16 m_item_count;  // number of items currently allocated in this chunk
        u16 m_padding;     // padding for alignment
    };

    static inline void s_chunk_init(cchunk_t* chunk)
    {
        chunk->m_next       = c_list_null_index;
        chunk->m_prev       = c_list_null_index;
        chunk->m_free_head  = 0xFFFF;  // all items are free
        chunk->m_free_index = 0;       // start allocating from the first item
        chunk->m_item_count = 0;       // no items allocated yet
        chunk->m_padding    = 0;       // padding for alignment
    }

    static void* s_chunk_alloc_item(cbin_t* bin, cchunk_t* chunk, u32 chunk_index)
    {
        // Calculate the address of this item
        byte* chunk_address = (byte*)bin->m_address_base + ((uint_t)chunk_index << bin->m_chunk_size_shift);
        if (chunk->m_free_head != 0xFFFF)
        {
            // There is a free item in this chunk, pop it from the list
            const u16 free_item_index = chunk->m_free_head;
            if (free_item_index != 0xFFFF)
            {
                // Pop the free item from the free list
                u16* item          = (u16*)(chunk_address + ((uint_t)free_item_index * bin->m_item_sizeof));
                chunk->m_free_head = *item;  // update free head to the next free item
                return item;
            }
        }
        else if (chunk->m_free_index < bin->m_max_items_per_chunk)
        {
            // Get the index of the free item in this chunk
            const u32 item_index = chunk->m_free_index++;
            return chunk_address + ((uint_t)item_index * bin->m_item_sizeof);
        }
        return nullptr;
    }

    // free an item back to the chunk, this is called when an item is freed
    static void s_chunk_free_item(cbin_t* bin, cchunk_t* chunk, u32 chunk_index, void* item_ptr)
    {
        // Add the item back to the free list of the chunk
        byte*     chunk_address = (byte*)bin->m_address_base + ((uint_t)chunk_index << bin->m_chunk_size_shift);
        const u16 item_index    = (u16)(((byte*)item_ptr - chunk_address) / bin->m_item_sizeof);

        // Insert this item back to the free list of the chunk
        u16* item = (u16*)(chunk_address + (item_index * bin->m_item_sizeof));
        ASSERT((byte*)item == (byte*)item_ptr);   // sanity check
        *item              = chunk->m_free_head;  // point to current free head
        chunk->m_free_head = item_index;          // update free head to this item
    }

    static void s_commit_chunk_memory(cbin_t* bin, u32 chunk_index)
    {
        byte* chunk_address = (byte*)bin->m_address_base + ((uint_t)chunk_index << bin->m_chunk_size_shift);
        v_alloc_commit(chunk_address, uint_t(1) << bin->m_chunk_size_shift);
    }

    static void s_decommit_chunk_memory(cbin_t* bin, u32 chunk_index)
    {
        byte* chunk_address = (byte*)bin->m_address_base + ((uint_t)chunk_index << bin->m_chunk_size_shift);
        v_alloc_decommit(chunk_address, uint_t(1) << bin->m_chunk_size_shift);
    }

    u32 bin_size(cbin_t const * bin)
    {
        // The global item count
        return bin->m_total_items_count;
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
        u32       active_chunk_index = c_list_null_index;

        cchunk_t* chunk_array = narena::base_ptr_as<cchunk_t>(bin->m_chunks);

        if (bin->m_chunk_active_list_head != c_list_null_index)
        {
            // Get the active chunk from the head of the active chunk list
            active_chunk_index = bin->m_chunk_active_list_head;
            active_chunk       = chunk_array + active_chunk_index;
        }
        else if (bin->m_chunk_free_list_head != c_list_null_index)
        {
            active_chunk_index = s_dlist_pop(chunk_array, sizeof(cchunk_t), bin->m_chunk_free_list_head, bin->m_chunk_free_list_size);
            active_chunk       = chunk_array + active_chunk_index;
            s_chunk_init(active_chunk);
            s_commit_chunk_memory(bin, active_chunk_index);
        }
        else
        {
            // No free chunk, need to allocate a new chunk
            const u32 chunk_free_index = narena::current_pos(bin->m_chunks) / sizeof(cchunk_t);

            if ((uint_t(chunk_free_index + 1) << bin->m_chunk_size_shift) > bin->m_address_size)
            {
                // No more space for new chunks
                return nullptr;
            }

            active_chunk_index = chunk_free_index;
            active_chunk       = g_allocate<cchunk_t>(bin->m_chunks);
            s_chunk_init(active_chunk);
            bin->m_chunk_count += 1;

            // This chunk is now active, add it to the active chunk list
            s_dlist_insert(chunk_array, sizeof(cchunk_t), bin->m_chunk_active_list_head, bin->m_chunk_active_list_size, active_chunk_index);

            s_commit_chunk_memory(bin, active_chunk_index);
        }

        void* item = s_chunk_alloc_item(bin, active_chunk, active_chunk_index);

        if (active_chunk->m_free_head == 0xFFFF && active_chunk->m_free_index >= bin->m_max_items_per_chunk)
        {
            // This chunk is now full, remove it from the active chunk list
            s_dlist_remove(chunk_array, sizeof(cchunk_t), bin->m_chunk_active_list_head, bin->m_chunk_active_list_size, active_chunk_index);
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
        ASSERT(ptr != nullptr && ptr >= bin->m_address_base && ptr < (byte*)bin->m_address_base + bin->m_address_size);

        cchunk_t* chunk_array = narena::base_ptr_as<cchunk_t>(bin->m_chunks);

        // Find the chunk this item belongs to
        const u32 chunk_index = (u32)((uint_t)((byte*)ptr - (byte*)bin->m_address_base) >> bin->m_chunk_size_shift);
        cchunk_t* chunk       = chunk_array + chunk_index;

        const bool chunk_was_full = (chunk->m_item_count >= bin->m_max_items_per_chunk);

        // Insert this item back to the free list of the chunk
        s_chunk_free_item(bin, chunk, chunk_index, ptr);

        // TODO
        // See if we can move 'bin->m_chunk_free_index' back, this requires
        // chunks to be in a doubly linked list!

        bin->m_total_items_count -= 1;

        if (chunk_was_full)
        {
            // This chunk was full before, now it has a free item, add it back to the active chunk list
            s_dlist_remove(chunk_array, sizeof(cchunk_t), bin->m_chunk_full_list_head, bin->m_chunk_full_list_size, chunk_index);
            s_dlist_insert(chunk_array, sizeof(cchunk_t), bin->m_chunk_active_list_head, bin->m_chunk_active_list_size, chunk_index);
        }
        else if (chunk->m_item_count == 0)
        {
            // This chunk is now empty, move it to the full list
            s_dlist_remove(chunk_array, sizeof(cchunk_t), bin->m_chunk_active_list_head, bin->m_chunk_active_list_size, chunk_index);
            s_dlist_insert(chunk_array, sizeof(cchunk_t), bin->m_chunk_full_list_head, bin->m_chunk_full_list_size, chunk_index);
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

    void bin_setup(cbin_t* bin, uint_t reserved_size, u32 chunk_size, u32 item_sizeof)
    {
        // item size must be at least 4 bytes to be able to use the free list in the chunk struct
        ASSERT(item_sizeof >= 4);

        // chunk size must be a power of 2
        ASSERT((1) << math::ilog2(chunk_size) == chunk_size);

        // reserved size must be a multiple of chunk size
        ASSERT((reserved_size & (chunk_size - 1)) == 0);

        // At least 4 items per chunk to be able to use the free_head and free_index in the chunk struct
        ASSERT(chunk_size / item_sizeof >= 4);

        bin->m_address_base = v_alloc_reserve(reserved_size);
        bin->m_address_size = reserved_size;

        bin->m_chunk_free_list_head   = c_list_null_index;
        bin->m_chunk_active_list_head = c_list_null_index;
        bin->m_chunk_full_list_head   = c_list_null_index;

        bin->m_chunk_free_list_size   = 0;
        bin->m_chunk_active_list_size = 0;
        bin->m_chunk_full_list_size   = 0;

        bin->m_total_items_count   = 0;
        bin->m_max_items_per_chunk = chunk_size / item_sizeof;
        bin->m_item_sizeof         = item_sizeof;
        bin->m_chunk_size_shift    = math::ilog2(chunk_size);

        const u32 chunk_count_max = reserved_size / chunk_size;
        bin->m_chunks             = narena::new_arena(chunk_count_max * sizeof(cchunk_t), 0);
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
        if (bin->m_chunks != nullptr)
        {
            narena::destroy(bin->m_chunks);
            bin->m_chunks = nullptr;
        }
        if (bin->m_address_base != nullptr)
        {
            v_alloc_decommit(bin->m_address_base, bin->m_address_size);
            bin->m_address_base = nullptr;
        }
    }

}  // namespace ncore

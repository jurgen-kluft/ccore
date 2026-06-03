#include "ccore/c_arena.h"
#include "ccore/c_bitvec.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"

#include "ccore/c_chunk_bin.h"

namespace ncore
{
    const u16 c_index16_null = 0xFFFF;

    // template functions to manipulate a doubly-linked list using u16 indices, where c_index16_null means null
    // Note: these functions require the type to have the first two members to be for the next and prev
    // indices, e.g.:
    // struct item_t
    // {
    //     u16 m_next;
    //     u16 m_prev;
    //     // other members...
    // };
    // or:
    // struct item_t
    // {
    //     u32 m_dlist; // next and prev indices packed in a u32, where the lower 16 bits are the next index and the upper 16 bits are the prev index
    //     // other members...
    // };
    //

    // 888      8888888 .d8888b. 88888888888
    // 888        888  d88P  Y88b    888
    // 888        888  Y88b.         888
    // 888        888   "Y888b.      888
    // 888        888      "Y88b.    888
    // 888        888        "888    888
    // 888        888  Y88b  d88P    888
    // 88888888 8888888 "Y8888P"     888

#define DLIST_NEXT 0
#define DLIST_PREV 1

    static inline void s_dlist_insert(void* array, u32 item_size, u16& head, u16& item_count, u16 index)
    {
        u16* item        = (u16*)((byte*)array + index * item_size);
        item[DLIST_NEXT] = head;
        item[DLIST_PREV] = c_index16_null;
        if (head != c_index16_null)
        {
            u16* head_item        = (u16*)((byte*)array + head * item_size);
            head_item[DLIST_PREV] = index;
        }
        head = index;
        ++item_count;
    }

    static inline void s_dlist_remove(void* array, u32 item_size, u16& head, u16& item_count, u16 index)
    {
        u16*      item = (u16*)((byte*)array + index * item_size);
        const u16 next = item[DLIST_NEXT];
        const u16 prev = item[DLIST_PREV];
        if (next != c_index16_null)
        {
            u16* next_item        = (u16*)((byte*)array + next * item_size);
            next_item[DLIST_PREV] = prev;
        }
        if (prev != c_index16_null)
        {
            u16* prev_item        = (u16*)((byte*)array + prev * item_size);
            prev_item[DLIST_NEXT] = next;
        }
        else
        {
            head = next;
        }
        --item_count;
    }

    static inline u16 s_dlist_pop(void* array, u32 item_size, u16& head, u16& item_count)
    {
        if (head != c_index16_null)
        {
            const u16 item     = head;
            u16*      item_ptr = (u16*)((byte*)array + item * item_size);
            head               = item_ptr[DLIST_NEXT];
            if (head != c_index16_null)
            {
                u16* head_ptr        = (u16*)((byte*)array + head * item_size);
                head_ptr[DLIST_PREV] = c_index16_null;
            }
            --item_count;
            return item;
        }
        return c_index16_null;
    }

    //  .d8888b.  888    888 888     888 888b    888 888    d8P
    // d88P  Y88b 888    888 888     888 8888b   888 888   d8P
    // 888    888 888    888 888     888 88888b  888 888  d8P
    // 888        8888888888 888     888 888Y88b 888 888d88K
    // 888        888    888 888     888 888 Y88b888 8888888b
    // 888    888 888    888 888     888 888  Y88888 888  Y88b
    // Y88b  d88P 888    888 Y88b. .d88P 888   Y8888 888   Y88b
    //  "Y8888P"  888    888  "Y88888P"  888    Y888 888    Y88b

    // Chunk
    // - max items <= 32768, so that we can use a u16 for the free list and item count
    struct cchunk_t
    {
        u16 m_next;             // next chunk in the free/active chunk list, or c_index16_null if this is the last chunk in the list
        u16 m_prev;             // previous chunk in the free/active chunk list, or c_index16_null if this is the first chunk in the list
        u16 m_item_count;       // number of items currently allocated in this chunk
        u16 m_pages_committed;  // number of pages currently committed in this chunk, used for lazy commit/decommit
        u64 m_layer0;           // nbitvec12, layer 0
    };

    static inline void s_chunk_init(cbin_t* bin, cchunk_t* chunk, u64* layer1)
    {
        chunk->m_next            = c_index16_null;
        chunk->m_prev            = c_index16_null;
        chunk->m_item_count      = 0;  // no items allocated yet
        chunk->m_pages_committed = 0;  // no pages committed yet
        if (layer1 != nullptr)
        {
            nbitvec12::set_all_free(&chunk->m_layer0, layer1, bin->m_max_items_per_chunk);
        }
        else
        {
            nbitvec6::setup(&chunk->m_layer0, bin->m_max_items_per_chunk);
        }
    }

    static void* s_chunk_alloc_item(cbin_t* bin, cchunk_t* chunk, u32 chunk_index)
    {
        s32 free_item_index = -1;
        if (bin->m_layer1 != nullptr)
        {
            u64* layer1     = narena::base_ptr_as<u64>(bin->m_layer1) + (chunk_index * bin->m_layer1_num_u64);
            free_item_index = nbitvec12::find_free_and_remove(&chunk->m_layer0, layer1, bin->m_max_items_per_chunk);
        }
        else
        {
            free_item_index = nbitvec6::find_free_and_remove(&chunk->m_layer0, bin->m_max_items_per_chunk);
        }

        if (free_item_index >= 0)
        {
            chunk->m_item_count += 1;

            const u32 chunk_size    = (u32)1 << bin->m_chunk_size_shift;
            byte*     chunk_address = (byte*)bin->m_address_base + ((uint_t)chunk_index * chunk_size);
            return chunk_address + ((uint_t)free_item_index * bin->m_item_sizeof);
        }

        return nullptr;
    }

    // free an item back to the chunk, this is called when an item is freed
    static void s_chunk_free_item(cbin_t* bin, cchunk_t* chunk, u32 chunk_index, void* item_ptr)
    {
        const u32 chunk_size = (u32)1 << bin->m_chunk_size_shift;

        // Add the item back to the free list of the chunk
        byte*     chunk_address = (byte*)bin->m_address_base + ((uint_t)chunk_index * chunk_size);
        const u16 item_index    = (u16)(((byte*)item_ptr - chunk_address) / bin->m_item_sizeof);

        // Mark this item as free in the bitvec
        u64* layer1 = bin->m_layer1 != nullptr ? narena::base_ptr_as<u64>(bin->m_layer1) + (chunk_index * bin->m_layer1_num_u64) : nullptr;
        if (layer1 != nullptr)
        {
            nbitvec12::set_free(&chunk->m_layer0, layer1, bin->m_max_items_per_chunk, item_index);
        }
        else
        {
            nbitvec6::set_free(&chunk->m_layer0, bin->m_max_items_per_chunk, item_index);
        }
        chunk->m_item_count -= 1;  // decrease item count
    }

    static void s_commit_chunk_memory(cbin_t* bin, u32 chunk_index)
    {
        const u32 chunk_size      = (u32)1 << bin->m_chunk_size_shift;
        const u32 chunk_num_pages = chunk_size >> v_alloc_get_page_size_shift();

        cchunk_t* chunk_array    = narena::base_ptr_as<cchunk_t>(bin->m_chunks);
        cchunk_t* chunk          = chunk_array + chunk_index;
        chunk->m_pages_committed = chunk_num_pages;

        byte* chunk_address = (byte*)bin->m_address_base + ((uint_t)chunk_index * chunk_size);
        v_alloc_commit(chunk_address, chunk_size);
    }

    static void s_decommit_chunk_memory(cbin_t* bin, u32 chunk_index)
    {
        const u32 chunk_size = (u32)1 << bin->m_chunk_size_shift;

        cchunk_t* chunk_array    = narena::base_ptr_as<cchunk_t>(bin->m_chunks);
        cchunk_t* chunk          = chunk_array + chunk_index;
        chunk->m_pages_committed = 0;

        byte* chunk_address = (byte*)bin->m_address_base + ((uint_t)chunk_index * chunk_size);
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
        u32       active_chunk_index = c_index16_null;

        cchunk_t* chunk_array = narena::base_ptr_as<cchunk_t>(bin->m_chunks);

        if (bin->m_chunk_active_list_head != c_index16_null)
        {
            // Get the active chunk from the head of the active chunk list
            active_chunk_index = bin->m_chunk_active_list_head;
            active_chunk       = chunk_array + active_chunk_index;
        }
        else if (bin->m_chunk_free_list_head != c_index16_null)
        {
            active_chunk_index = s_dlist_pop(chunk_array, sizeof(cchunk_t), bin->m_chunk_free_list_head, bin->m_chunk_free_list_size);
            active_chunk       = chunk_array + active_chunk_index;
            u64* layer1        = (bin->m_layer1_num_u64 > 0) ? narena::base_ptr_as<u64>(bin->m_layer1) + (active_chunk_index * bin->m_layer1_num_u64) : nullptr;
            s_chunk_init(bin, active_chunk, layer1);
            s_dlist_insert(chunk_array, sizeof(cchunk_t), bin->m_chunk_active_list_head, bin->m_chunk_active_list_size, active_chunk_index);
            s_commit_chunk_memory(bin, active_chunk_index);
        }
        else
        {
            // No free chunk, need to allocate a new chunk
            const u32 chunk_free_index = (u32)(narena::current_pos(bin->m_chunks) / sizeof(cchunk_t));

            if (chunk_free_index >= bin->m_chunk_max_count)
            {
                // No more space for new chunks
                return nullptr;
            }

            u64* new_layer1 = (bin->m_layer1_num_u64 > 0) ? g_allocate_array<u64>(bin->m_layer1, bin->m_layer1_num_u64) : nullptr;

            active_chunk_index = chunk_free_index;
            active_chunk       = g_allocate<cchunk_t>(bin->m_chunks);

            // layer1 for this chunk should be at the expected position
            ASSERT(new_layer1 == ((bin->m_layer1_num_u64 > 0) ? narena::base_ptr_as<u64>(bin->m_layer1) + (active_chunk_index * bin->m_layer1_num_u64) : nullptr));

            s_chunk_init(bin, active_chunk, new_layer1);

            // This chunk is now active, add it to the active chunk list
            s_dlist_insert(chunk_array, sizeof(cchunk_t), bin->m_chunk_active_list_head, bin->m_chunk_active_list_size, active_chunk_index);

            s_commit_chunk_memory(bin, active_chunk_index);
        }

        void* item = s_chunk_alloc_item(bin, active_chunk, active_chunk_index);

        if (item == nullptr)
            return nullptr;

        if (active_chunk->m_item_count >= bin->m_max_items_per_chunk)
        {
            // This chunk is now full, remove it from the active chunk list
            s_dlist_remove(chunk_array, sizeof(cchunk_t), bin->m_chunk_active_list_head, bin->m_chunk_active_list_size, active_chunk_index);
            s_dlist_insert(chunk_array, sizeof(cchunk_t), bin->m_chunk_full_list_head, bin->m_chunk_full_list_size, active_chunk_index);
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

        cchunk_t* chunk_array      = narena::base_ptr_as<cchunk_t>(bin->m_chunks);
        const u8  chunk_size_shift = bin->m_chunk_size_shift;

        // Find the chunk this item belongs to
        const u32 chunk_index = (u32)((uint_t)((byte*)ptr - (byte*)bin->m_address_base) >> chunk_size_shift);
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
            // This chunk is now empty, move it to the free list and decommit its backing pages.
            s_dlist_remove(chunk_array, sizeof(cchunk_t), bin->m_chunk_active_list_head, bin->m_chunk_active_list_size, chunk_index);
            s_dlist_insert(chunk_array, sizeof(cchunk_t), bin->m_chunk_free_list_head, bin->m_chunk_free_list_size, chunk_index);
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

    // - sizeof(item) >=    8 B && sizeof(item) <=  128 B -> chunk-size =   4 KiB
    // - sizeof(item)  >  128 B && sizeof(item) <=  256 B -> chunk-size =   8 KiB
    // - sizeof(item)  >  256 B && sizeof(item) <=  512 B -> chunk-size =  16 KiB
    // - sizeof(item)  >  512 B && sizeof(item) <=  1 KiB -> chunk-size =  32 KiB
    // - sizeof(item)  >  1 KiB && sizeof(item) <=  2 KiB -> chunk-size =  64 KiB
    // - sizeof(item)  >  2 KiB && sizeof(item) <=  4 KiB -> chunk-size = 128 KiB
    // - sizeof(item)  >  4 KiB && sizeof(item) <=  8 KiB -> chunk-size = 256 KiB
    // - sizeof(item)  >  8 KiB && sizeof(item) <= 16 KiB -> chunk-size = 512 KiB
    // - sizeof(item)  > 16 KiB && sizeof(item) <= 32 KiB -> chunk-size =   1 MiB

    // clang-format off
    const static u8 s_cchunk_configs[] = {
         12, 12, 12, // 1<<0, 1<<1, 1<<2
         12, 12, 12, // 1<<3, 1<<4, 1<<5,
         12, 12, 13, // 1<<6, 1<<7, 1<<8,
         14, 15, 16, // 1<<9, 1<<10, 1<<11,
         17, 18, 19, // 1<<12, 1<<13, 1<<14,
         20, 21,     // 1<<15, 1<<16
    };
    // clang-format on

    void bin_setup(cbin_t* bin, void* base_address, uint_t reserved_size, u16 item_sizeof)
    {
        // Note: Item size >= 8 B and <= 32 KiB
        ASSERT(item_sizeof <= (32 * cKB));

        const u8 page_size_shift = v_alloc_get_page_size_shift();

        // Find the appropriate chunk size based on the item size, this is done by
        // looking up the 's_cchunk_configs' array, which has predefined chunk size shifts
        // for different item size ranges.
        const u8 item_size_shift  = math::max((u8)math::ilog2(math::ceilpo2(item_sizeof)), (u8)3);
        const u8 chunk_size_shift = math::max(s_cchunk_configs[item_size_shift], page_size_shift);

        ASSERT(chunk_size_shift > 0);
        const u32 chunk_size = (u32)1 << chunk_size_shift;

        const u32 items_per_chunk = chunk_size / item_sizeof;
        ASSERT(items_per_chunk >= 4 && items_per_chunk <= (64 * 64));

        // Calculate the number of u64s we need for layer1
        const u32 layer1_num_u64 = items_per_chunk > 64 ? (items_per_chunk + 63) / 64 : 0;
        bin->m_layer1_num_u64    = (u16)layer1_num_u64;

        // the maximum number of chunks is calculated based on the reserved
        // size and the calculated chunk size, but must be < 65536.
        const u32 max_chunk_count = (u32)(reserved_size / chunk_size);
        ASSERT(max_chunk_count > 0 && max_chunk_count < 65536);

        // If chunks need layer 1 then initialize the arena for this
        bin->m_layer1 = (layer1_num_u64 > 0) ? narena::new_arena((uint_t)max_chunk_count * layer1_num_u64 * sizeof(u64), 0) : nullptr;

        if (base_address != nullptr)
        {
            bin->m_address_base = base_address;
            bin->m_address_size = reserved_size;
            bin->m_ownership    = false;
        }
        else
        {
            bin->m_address_base = v_alloc_reserve(reserved_size);
            bin->m_address_size = reserved_size;
            bin->m_ownership    = true;
        }

        bin->m_chunk_free_list_head   = c_index16_null;
        bin->m_chunk_active_list_head = c_index16_null;
        bin->m_chunk_full_list_head   = c_index16_null;

        bin->m_chunk_free_list_size   = 0;
        bin->m_chunk_active_list_size = 0;
        bin->m_chunk_full_list_size   = 0;
        bin->m_chunk_max_count        = (u16)max_chunk_count;
        bin->m_chunks                 = narena::new_arena(max_chunk_count * sizeof(cchunk_t), 0);
        bin->m_chunk_size_shift       = chunk_size_shift;

        bin->m_total_items_count   = 0;
        bin->m_max_items_per_chunk = (u16)(items_per_chunk);
        bin->m_item_sizeof         = (u16)1 << item_size_shift;
    }

    void bin_setup(cbin_t* bin, uint_t reserved_size, u16 item_sizeof) { bin_setup(bin, nullptr, reserved_size, item_sizeof); }

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
        if (bin->m_chunks != nullptr)
        {
            narena::destroy(bin->m_chunks);
        }
        if (bin->m_layer1 != nullptr)
        {
            narena::destroy(bin->m_layer1);
        }
        if (bin->m_address_base != nullptr)
        {
            if (bin->m_ownership)
            {
                v_alloc_release(bin->m_address_base, bin->m_address_size);
            }
        }

        g_memclr(bin, sizeof(cbin_t));
    }

}  // namespace ncore

#include "ccore/c_arena.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_bindex.h"

namespace ncore
{
    namespace nbindex16
    {
        struct bindex_t
        {
            u16*  m_owner_index_array;      // pointer to index array (u16[])
            u16*  m_owner_index_array_end;  // pointer to end of (committed) index array
            byte* m_items;                  // pointer to items
            byte* m_items_end;              // pointer to end of items where we need to commit more pages
            byte* m_items_end_max;          // pointer to end of maximum items memory
            u32   m_items_count;            // number of items currently in use
            u16   m_item_sizeof;            // sizeof(item)
            u8    m_pagesize_shift;         // page-size = 1 << pagesize_shift
            u8    m_padding0;               // padding for 8 byte alignment
        };

        bindex_t* make_bin(u16 element_size)
        {
            const u32 max_elements = 65535;  // maximum number of elements is 65535 (u16 indices)

            ASSERT(max_elements > 0 && max_elements < 65536);  // maximum number of elements is 65535 (u16 indices)
            ASSERT(element_size > 0 && element_size <= 1024);  // element size should be reasonable

            const s32 page_size       = v_alloc_get_page_size();
            const u8  page_size_shift = v_alloc_get_page_size_shift();

            const int_t index_array_size  = math::alignUp((int_t)sizeof(bindex_t) + (int_t)sizeof(u16) * max_elements, page_size);
            const int_t items_memory_size = math::alignUp((int_t)element_size * max_elements, page_size);

            byte* base_address = (byte*)v_alloc_reserve(index_array_size + items_memory_size);
            if (!base_address)
                return nullptr;

            const u32 index_pages_committed = 1;
            if (!v_alloc_commit(base_address, (int_t)index_pages_committed << page_size_shift))
            {
                v_alloc_release(base_address, index_array_size + items_memory_size);
                return nullptr;
            }

            const u32 item_pages_committed = 1;
            if (!v_alloc_commit(base_address + index_array_size, (int_t)item_pages_committed << page_size_shift))
            {
                v_alloc_release(base_address, index_array_size + items_memory_size);
                return nullptr;
            }

            bindex_t* bin                = (bindex_t*)base_address;
            bin->m_owner_index_array     = (u16*)(bin + 1);
            bin->m_owner_index_array_end = bin->m_owner_index_array + ((((int_t)index_pages_committed << page_size_shift) - (int_t)sizeof(bindex_t)) / sizeof(u16));
            bin->m_items                 = base_address + (int_t)index_array_size;
            bin->m_items_end             = bin->m_items + ((int_t)item_pages_committed << page_size_shift);
            bin->m_items_end_max         = bin->m_items + items_memory_size;
            bin->m_items_count           = 0;
            bin->m_item_sizeof           = element_size;
            bin->m_pagesize_shift        = page_size_shift;

            return bin;
        }

        void destroy(bindex_t* bin)
        {
            byte*       base_address        = (byte*)bin;
            const int_t total_reserved_size = (int_t)bin->m_items_end_max - (int_t)base_address;
            v_alloc_release(base_address, total_reserved_size);
        }

        // resize the bin to be able to hold new_max_elements
        void commit(bindex_t* bin, u32 num_elements)
        {
            ASSERT(num_elements > 0 && num_elements < 65536);  // maximum number of elements is 65535 (u16 indices)

            const u8  page_size_shift = bin->m_pagesize_shift;
            const s32 page_size       = (1 << page_size_shift);

            // calculate how many pages to commit for index array
            const int_t current_index_array_size       = math::alignUp((int_t)bin->m_owner_index_array_end - (int_t)bin->m_owner_index_array, page_size);
            const int_t required_index_array_size      = math::alignUp((int_t)sizeof(bindex_t) + (int_t)sizeof(u16) * num_elements, page_size);
            const int_t size_to_commit_for_index_array = (required_index_array_size > current_index_array_size) ? (required_index_array_size - current_index_array_size) : 0;
            if (size_to_commit_for_index_array > 0)
            {
                if (!v_alloc_commit((byte*)bin->m_owner_index_array_end, size_to_commit_for_index_array))
                {
                    return;  // failed to commit more memory for index array
                }
                bin->m_owner_index_array_end = (u16*)((byte*)bin + required_index_array_size);
            }

            // calculate how many pages to commit for items
            const int_t current_items_size       = math::alignUp((int_t)bin->m_items_end - (int_t)bin->m_items, page_size);
            const int_t required_items_size      = math::alignUp((int_t)num_elements * (int_t)bin->m_item_sizeof, page_size);
            const u32   size_to_commit_for_items = (required_items_size > current_items_size) ? (required_items_size - current_items_size) : 0;
            if (size_to_commit_for_items > 0)
            {
                if (!v_alloc_commit(bin->m_items_end, size_to_commit_for_items))
                {
                    return;  // failed to commit more memory for items
                }
                bin->m_items_end += size_to_commit_for_items;
            }
        }

        i32 alloc(bindex_t* bin, u16 owner_index)
        {
            if (bin->m_items_count >= 65535)
                return -1;  // bin is full

            const u32 index = bin->m_items_count;
            bin->m_items_count += 1;

            // check if we need to commit more pages for the index array
            if (&bin->m_owner_index_array[index] >= bin->m_owner_index_array_end)
            {
                // commit one more page for the index array
                const u32 page_size = ((u32)1 << bin->m_pagesize_shift);
                if (!v_alloc_commit((byte*)bin->m_owner_index_array_end, page_size))
                {
                    bin->m_items_count -= 1;
                    return -1;
                }
                bin->m_owner_index_array_end += (page_size / sizeof(u16));
            }

            // check if we need to commit more pages for the items
            if ((bin->m_items + ((int_t)bin->m_items_count * bin->m_item_sizeof)) >= bin->m_items_end)
            {
                // commit one more page for the items
                const u32 page_size = ((u32)1 << bin->m_pagesize_shift);
                if (!v_alloc_commit(bin->m_items_end, page_size))
                {
                    bin->m_items_count -= 1;
                    return -1;
                }
                bin->m_items_end += page_size;
            }

            bin->m_owner_index_array[index] = owner_index;
            return (i32)index;
        }

        // free item at 'index' and return index of item that was moved
        // to fill the hole (likely last item), or -1 if no swap performed
        i32 free(bindex_t* bin, u32 item_index)
        {
            if (item_index < bin->m_items_count)
            {
                u16*  owner_index_array = bin->m_owner_index_array;
                byte* items             = bin->m_items;

                // decrease number of used items
                bin->m_items_count -= 1;

                const u32 last_index = bin->m_items_count;
                if (item_index != last_index)
                {
                    // move the item data
                    byte* dst_item = items + ((int_t)item_index * bin->m_item_sizeof);
                    byte* src_item = items + ((int_t)last_index * bin->m_item_sizeof);
                    g_memcpy(dst_item, src_item, bin->m_item_sizeof);

                    // return the owner index of the item that was moved to fill the hole
                    owner_index_array[item_index] = owner_index_array[last_index];
                    return (i32)owner_index_array[item_index];
                }

                // TODO : consider decommitting pages for index array and items

                return -1;  // no swap performed
            }
            return -2;  // error
        }

        void* idx2ptr(bindex_t const* bin, u32 index)
        {
            if (index < bin->m_items_count)
            {
                return bin->m_items + ((int_t)index * bin->m_item_sizeof);
            }
            return nullptr;  // invalid index
        }

        i32 ptr2idx(bindex_t const* bin, void const* p)
        {
            const byte* ptr  = (const byte*)p;
            const byte* base = bin->m_items;
            const byte* end  = bin->m_items + ((int_t)bin->m_items_count * bin->m_item_sizeof);
            return (ptr >= base && ptr < end) ? (i32)((ptr - base) / bin->m_item_sizeof) : -1;
        }

        u32 size(bindex_t const* bin) { return bin->m_items_count; }
    }  // namespace nbindex16
}  // namespace ncore

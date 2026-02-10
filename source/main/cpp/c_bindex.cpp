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
            u16*  m_index_array;                  // pointer to index array (u16[])
            u32   m_index_commit_index;           // max index of when to commit +1 page for index array
            u32   m_index_array_committed_pages;  // (unit = pages) number of pages committed for index array
            u32   m_index_array_maximum_pages;    // (unit = pages) maximum number of pages for index array
            byte* m_items;                        // pointer to items
            byte* m_items_end;                    // pointer to end of items where we need to commit more pages
            u32   m_items_committed_pages;        // (unit = pages) number of pages committed for items
            u32   m_items_maximum_pages;          // (unit = pages) maximum number of pages for items
            u32   m_items_count;                  // number of items currently in use
            u32   m_items_capacity;               // maximum number of items the bin can hold
            u16   m_item_sizeof;                  // sizeof(item)
            u8    m_pagesize_shift;               // page-size = 1 << pagesize_shift
            u8    m_padding0;                     // padding for 8 byte alignment
        };

        bindex_t* make_bin(u16 element_size, u32 max_elements)
        {
            ASSERT(max_elements <= 65536);  // maximum number of elements is 65536 (u16 indices)
            ASSERT(element_size <= 1024);   // element size should be reasonable

            const s32 page_size       = v_alloc_get_page_size();
            const u8  page_size_shift = v_alloc_get_page_size_shift();

            const int_t index_array_size  = math::alignUp(sizeof(bindex_t) + (int_t)sizeof(u16) * max_elements, page_size);
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

            bindex_t* bin                      = (bindex_t*)base_address;
            bin->m_index_array                 = (u16*)(bin + 1);
            bin->m_index_commit_index          = ((((int_t)index_pages_committed << page_size_shift) - (int_t)bin->m_index_array) / sizeof(u16));
            bin->m_index_array_committed_pages = index_pages_committed;
            bin->m_index_array_maximum_pages   = (u32)(index_array_size >> page_size_shift);
            bin->m_items                       = base_address + (int_t)index_array_size;
            bin->m_items_end                   = bin->m_items + ((int_t)item_pages_committed << page_size_shift);
            bin->m_items_committed_pages       = item_pages_committed;
            bin->m_items_maximum_pages         = (u32)(items_memory_size >> page_size_shift);
            bin->m_items_count                 = 0;
            bin->m_items_capacity              = (u32)(((int_t)bin->m_items_maximum_pages << page_size_shift) / element_size);
            bin->m_item_sizeof                 = (u16)element_size;
            bin->m_pagesize_shift              = page_size_shift;

            return bin;
        }

        void destroy(bindex_t* bin)
        {
            const u32   page_size                 = v_alloc_get_page_size();
            const int_t index_array_reserved_size = (int_t)bin->m_index_array_maximum_pages << bin->m_pagesize_shift;
            const int_t items_reserved_size       = (int_t)bin->m_items_maximum_pages << bin->m_pagesize_shift;
            const int_t total_reserved_size       = index_array_reserved_size + items_reserved_size;

            byte* base_address = (byte*)bin;

            // release the whole reserved region
            v_alloc_release(base_address, total_reserved_size);
        }

        i32 alloc(bindex_t* bin)
        {
            if (bin->m_items_count >= bin->m_items_capacity)
                return -1;  // bin is full

            u16*  index_array = bin->m_index_array;
            byte* items       = bin->m_items;

            if (bin->m_items_count >= bin->m_index_commit_index)
            {
                // commit one more page for the index array
                const u32 page_size = (1 << bin->m_pagesize_shift);
                if (!v_alloc_commit((byte*)bin + (bin->m_index_array_committed_pages << bin->m_pagesize_shift), page_size))
                {
                    bin->m_items_count -= 1;
                    return -1;
                }
                bin->m_index_array_committed_pages += 1;
                bin->m_index_commit_index += (page_size / sizeof(u16));
            }

            // We assume that there are no gaps in the index bin, so 'count' is always the next free index
            // The user is responsible for calling 'remove' to remove gaps from the array, so a deallocate
            // should be followed by a remove to keep the bin compact.

            const u32 index    = bin->m_items_count;
            index_array[index] = (u16)index;
            bin->m_items_count += 1;

            return (i32)index;
        }

        i32 free(bindex_t* bin, u32 index)
        {
            if (index < bin->m_items_count)
            {
                u16*  index_array = bin->m_index_array;
                byte* items       = bin->m_items;

                // decrease number of used items
                bin->m_items_count -= 1;

                const u32 last_index = bin->m_items_count;
                if (index != last_index)
                {
                    // fill hole by swapping the last item into it
                    index_array[index] = index_array[last_index];
                    // move the item data
                    byte* dst_item = items + (index * bin->m_item_sizeof);
                    byte* src_item = items + (last_index * bin->m_item_sizeof);
                    g_memcpy(dst_item, src_item, bin->m_item_sizeof);
                    return (i32)last_index;  // return index used to fill hole
                }
            }
            return -1;  // no swap performed
        }

        // (swap) remove an index (return index used to fill the gap)
        i32 remove(bindex_t* bin, u32 index)
        {
            if (index >= bin->m_items_count)
                return -1;  // invalid index
        }

        u32 size(bindex_t const* bin) { return bin->m_items_count; }
        u32 capacity(bindex_t const* bin) { return bin->m_items_capacity; }
    }  // namespace nbindex16
}  // namespace ncore

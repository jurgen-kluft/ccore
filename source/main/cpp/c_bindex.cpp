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
            u16*  m_owner_index_array;                  // pointer to index array (u16[])
            u32   m_owner_index_commit_index;           // max index of when to commit +1 page for index array
            u32   m_owner_index_array_committed_pages;  // (unit = pages) number of pages committed for index array
            u32   m_owner_index_array_maximum_pages;    // (unit = pages) maximum number of pages for index array
            byte* m_items;                              // pointer to items
            byte* m_items_end;                          // pointer to end of items where we need to commit more pages
            u32   m_items_committed_pages;              // (unit = pages) number of pages committed for items
            u32   m_items_maximum_pages;                // (unit = pages) maximum number of pages for items
            u32   m_items_count;                        // number of items currently in use
            u32   m_items_capacity;                     // maximum number of items the bin can hold
            u16   m_item_sizeof;                        // sizeof(item)
            u8    m_pagesize_shift;                     // page-size = 1 << pagesize_shift
            u8    m_padding0;                           // padding for 8 byte alignment
        };

        bindex_t* make_bin(u16 element_size, u32 max_elements)
        {
            ASSERT(max_elements > 0 && max_elements < 65536);  // maximum number of elements is 65535 (u16 indices)
            ASSERT(element_size > 0 && element_size <= 1024);  // element size should be reasonable

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

            bindex_t* bin                            = (bindex_t*)base_address;
            bin->m_owner_index_array                 = (u16*)(bin + 1);
            bin->m_owner_index_commit_index          = (u32)((((int_t)index_pages_committed << page_size_shift) - (int_t)sizeof(bindex_t)) / sizeof(u16));
            bin->m_owner_index_array_committed_pages = index_pages_committed;
            bin->m_owner_index_array_maximum_pages   = (u32)(index_array_size >> page_size_shift);
            bin->m_items                             = base_address + (int_t)index_array_size;
            bin->m_items_end                         = bin->m_items + ((int_t)item_pages_committed << page_size_shift);
            bin->m_items_committed_pages             = item_pages_committed;
            bin->m_items_maximum_pages               = (u32)(items_memory_size >> page_size_shift);
            bin->m_items_count                       = 0;
            bin->m_items_capacity                    = (u32)(((int_t)bin->m_items_maximum_pages << page_size_shift) / element_size);
            bin->m_item_sizeof                       = (u16)element_size;
            bin->m_pagesize_shift                    = page_size_shift;

            return bin;
        }

        void destroy(bindex_t* bin)
        {
            const int_t index_array_reserved_size = (int_t)bin->m_owner_index_array_maximum_pages << bin->m_pagesize_shift;
            const int_t items_reserved_size       = (int_t)bin->m_items_maximum_pages << bin->m_pagesize_shift;
            const int_t total_reserved_size       = index_array_reserved_size + items_reserved_size;

            byte* base_address = (byte*)bin;

            // release the whole reserved region
            v_alloc_release(base_address, total_reserved_size);
        }

        i32 alloc(bindex_t* bin, u16 owner_index)
        {
            if (bin->m_items_count >= bin->m_items_capacity)
                return -1;  // bin is full

            const u32 index = bin->m_items_count;
            bin->m_items_count += 1;

            // check if we need to commit more pages for the index array
            if (bin->m_items_count >= bin->m_owner_index_commit_index)
            {
                // commit one more page for the index array
                const u32 page_size = ((u32)1 << bin->m_pagesize_shift);
                if (!v_alloc_commit((byte*)bin + ((int_t)bin->m_owner_index_array_committed_pages << bin->m_pagesize_shift), page_size))
                {
                    bin->m_items_count -= 1;
                    return -1;
                }
                bin->m_owner_index_array_committed_pages += 1;
                bin->m_owner_index_commit_index += (page_size / sizeof(u16));
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
                bin->m_items_committed_pages += 1;
            }

            // We assume that there are no gaps in the index bin, so 'count' is always the next free index
            // The user is responsible for calling 'remove' to remove gaps from the array, so a deallocate
            // should be followed by a remove to keep the bin compact.
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
                    byte* dst_item = items + (item_index * bin->m_item_sizeof);
                    byte* src_item = items + (last_index * bin->m_item_sizeof);
                    g_memcpy(dst_item, src_item, bin->m_item_sizeof);

                    // return the owner index of the item that was moved to fill the hole
                    owner_index_array[item_index] = owner_index_array[last_index];
                    return (i32)owner_index_array[item_index];
                }
                return -1;  // no swap performed
            }
            return -2;  // error
        }

        void* idx2ptr(bindex_t const* bin, u32 index)
        {
            if (index < bin->m_items_count)
            {
                return bin->m_items + (index * bin->m_item_sizeof);
            }
            return nullptr;  // invalid index
        }

        i32 ptr2idx(bindex_t const* bin, void const* p)
        {
            const byte* ptr  = (const byte*)p;
            const byte* base = bin->m_items;
            const byte* end  = bin->m_items + (bin->m_items_count * bin->m_item_sizeof);
            return (ptr >= base && ptr < end) ? (i32)((ptr - base) / bin->m_item_sizeof) : -1;
        }

        u32 size(bindex_t const* bin) { return bin->m_items_count; }
        u32 capacity(bindex_t const* bin) { return bin->m_items_capacity; }
    }  // namespace nbindex16
}  // namespace ncore

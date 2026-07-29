#include "ccore/c_arena.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"
#include "ccore/c_bitvec.h"

#include "ccore/c_bin.h"

namespace ncore
{
    struct ibin32_t
    {
        u32 m_items_count;
        u32 m_items_capacity;
        u32 m_items_highwater;
        u16 m_item_sizeof;
        u8  m_page_size_shift;
        u8  m_bin0_offset8;
        u32 m_bin2_offset;
        u32 m_bin3_offset;
        u32 m_items_offset;
        u32 m_reserved_pages;
        u32 m_bin_committed_pages;
        u32 m_items_committed_pages;
    };

    struct bin32_layout_t
    {
        u32 m_bin0;
        u32 m_bin1;
        u32 m_bin2;
        u32 m_bin3;
    };

    static inline ibin32_t*       s_bin32(bin32_t* bin) { return (ibin32_t*)bin->m_memory; }
    static inline ibin32_t const* s_bin32(bin32_t const* bin) { return (ibin32_t const*)bin->m_memory; }
    static inline byte*           s_bin32_base(bin32_t* bin) { return (byte*)bin->m_memory; }
    static inline byte const*     s_bin32_base(bin32_t const* bin) { return (byte const*)bin->m_memory; }
    static inline u32             s_bin32_bin0_offset(ibin32_t const* ibin) { return (u32)ibin->m_bin0_offset8; }

    static inline u64*        s_bin32_bm0(ibin32_t* ibin) { return (u64*)((byte*)ibin + s_bin32_bin0_offset(ibin)); }
    static inline u64 const*  s_bin32_bm0(ibin32_t const* ibin) { return (u64 const*)((byte const*)ibin + s_bin32_bin0_offset(ibin)); }
    static inline u64*        s_bin32_bm1(ibin32_t* ibin) { return (u64*)((byte*)ibin + s_bin32_bin0_offset(ibin) + (u32)sizeof(u64)); }
    static inline u64 const*  s_bin32_bm1(ibin32_t const* ibin) { return (u64 const*)((byte const*)ibin + s_bin32_bin0_offset(ibin) + (u32)sizeof(u64)); }
    static inline u64*        s_bin32_bm2(ibin32_t* ibin) { return (u64*)((byte*)ibin + ibin->m_bin2_offset); }
    static inline u64 const*  s_bin32_bm2(ibin32_t const* ibin) { return (u64 const*)((byte const*)ibin + ibin->m_bin2_offset); }
    static inline u64*        s_bin32_bm3(ibin32_t* ibin) { return (u64*)((byte*)ibin + ibin->m_bin3_offset); }
    static inline u64 const*  s_bin32_bm3(ibin32_t const* ibin) { return (u64 const*)((byte const*)ibin + ibin->m_bin3_offset); }
    static inline byte*       s_bin32_items(ibin32_t* ibin) { return (byte*)ibin + ibin->m_items_offset; }
    static inline byte const* s_bin32_items(ibin32_t const* ibin) { return (byte const*)ibin + ibin->m_items_offset; }
    static inline u32         s_bin32_reserved_size(ibin32_t const* ibin) { return ibin->m_reserved_pages << ibin->m_page_size_shift; }
    static inline u32         s_bin32_committed_bin_end(ibin32_t const* ibin) { return ibin->m_bin_committed_pages << ibin->m_page_size_shift; }
    static inline u32         s_bin32_items_committed_size(ibin32_t const* ibin) { return ibin->m_items_committed_pages << ibin->m_page_size_shift; }

    static bin32_layout_t s_bin32_normalize_layout(nbitvec::layout64_t const& raw_layout)
    {
        bin32_layout_t layout = {1, 1, 1, 1};

        switch (raw_layout.m_levels)
        {
            case 3:
                layout.m_bin0 = (u32)raw_layout.m_bin0;
                layout.m_bin1 = (u32)raw_layout.m_bin1;
                layout.m_bin2 = raw_layout.m_bin2;
                layout.m_bin3 = raw_layout.m_bin3;
                break;
            case 2:
                layout.m_bin0 = 1;
                layout.m_bin1 = (u32)raw_layout.m_bin0;
                layout.m_bin2 = (u32)raw_layout.m_bin1;
                layout.m_bin3 = raw_layout.m_bin2;
                break;
            case 1:
                layout.m_bin0 = 1;
                layout.m_bin1 = 1;
                layout.m_bin2 = (u32)raw_layout.m_bin0;
                layout.m_bin3 = (u32)raw_layout.m_bin1;
                break;
            case 0:
            default:
                layout.m_bin0 = 1;
                layout.m_bin1 = 1;
                layout.m_bin2 = 1;
                layout.m_bin3 = (u32)raw_layout.m_bin0;
                break;
        }

        layout.m_bin0 = math::max(layout.m_bin0, 1u);
        layout.m_bin1 = math::max(layout.m_bin1, 1u);
        layout.m_bin2 = math::max(layout.m_bin2, 1u);
        layout.m_bin3 = math::max(layout.m_bin3, 1u);
        return layout;
    }

    struct bin16_layout_t
    {
        u32 m_bin0;
        u32 m_bin1;
        u32 m_bin2;
    };

    static bin16_layout_t s_bin16_normalize_layout(nbitvec::layout64_t const& raw_layout)
    {
        bin16_layout_t layout = {1, 1, 1};

        switch (raw_layout.m_levels)
        {
            case 2:
                layout.m_bin0 = (u32)raw_layout.m_bin0;
                layout.m_bin1 = (u32)raw_layout.m_bin1;
                layout.m_bin2 = raw_layout.m_bin2;
                break;
            case 1:
                layout.m_bin0 = 1;
                layout.m_bin1 = (u32)raw_layout.m_bin0;
                layout.m_bin2 = (u32)raw_layout.m_bin1;
                break;
            case 0:
            default:
                layout.m_bin0 = 1;
                layout.m_bin1 = 1;
                layout.m_bin2 = (u32)raw_layout.m_bin0;
                break;
        }

        layout.m_bin0 = math::max(layout.m_bin0, 1u);
        layout.m_bin1 = math::max(layout.m_bin1, 1u);
        layout.m_bin2 = math::max(layout.m_bin2, 1u);
        return layout;
    }

    static bool s_bin32_commit_bin3(bin32_t* bin, ibin32_t* ibin, u32 required_items)
    {
        const u32 required_words = math::max((required_items + 63) >> 6, 1u);
        const u32 required_end   = ibin->m_bin3_offset + (required_words * (u32)sizeof(u64));
        const u32 committed_end  = s_bin32_committed_bin_end(ibin);
        if (required_end <= committed_end)
            return true;

        const u8  page_size_shift = ibin->m_page_size_shift;
        const u32 page_size_bytes = (u32)1 << page_size_shift;

        const u32 required_commit_end = math::alignUp(required_end, page_size_bytes);
        if (required_commit_end > ibin->m_items_offset)
            return false;

        const u32 extra_size = required_commit_end - committed_end;
        if (!v_alloc_commit(s_bin32_base(bin) + committed_end, extra_size))
            return false;
        ibin->m_bin_committed_pages = (u32)(required_commit_end >> page_size_shift);
        return true;
    }

    static bool s_bin32_commit_items(bin32_t* bin, ibin32_t* ibin, u32 required_items)
    {
        const u8  page_size_shift = ibin->m_page_size_shift;
        const u32 page_size_bytes = (u32)1 << page_size_shift;
        const u32 required_size   = required_items * (u32)ibin->m_item_sizeof;
        const u32 committed_pages = (u32)(math::alignUp(required_size, page_size_bytes) >> page_size_shift);
        if (committed_pages <= ibin->m_items_committed_pages)
            return true;
        const u32 extra_pages = committed_pages - ibin->m_items_committed_pages;
        if (!v_alloc_commit(s_bin32_base(bin) + ibin->m_items_offset + s_bin32_items_committed_size(ibin), extra_pages << page_size_shift))
            return false;
        ibin->m_items_committed_pages = committed_pages;
        return true;
    }

    //  .d8888b.  8888888888 88888888888 888     888 8888888b.
    // d88P  Y88b 888            888     888     888 888   Y88b
    // Y88b.      888            888     888     888 888    888
    //  "Y888b.   8888888        888     888     888 888   d88P
    //     "Y88b. 888            888     888     888 8888888P"
    //       "888 888            888     888     888 888
    // Y88b  d88P 888            888     Y88b. .d88P 888
    //  "Y8888P"  8888888888     888      "Y88888P"  888

    void bin_setup(bin32_t* bin, u16 item_size, u32 max_items)
    {
        ASSERT(item_size < (v_alloc_get_page_size() >> 1));

        // align the maximum number of items to a multiple of 64 (for binmap)
        max_items = math::alignUp(max_items, 64);

        const u32 initial_num_items = 64;
        const u8  page_size_shift   = v_alloc_get_page_size_shift();
        const u32 page_size_bytes   = (u32)1 << page_size_shift;

        nbitvec::layout64_t raw_layout;
        nbitvec::compute(max_items, raw_layout);
        const bin32_layout_t layout = s_bin32_normalize_layout(raw_layout);
        ASSERT(layout.m_bin0 == 1u);

        const u32 header_size = math::alignUp((u32)sizeof(ibin32_t), (u32)alignof(u64));
        ASSERT(header_size <= 0xFFu);
        const u32 bin0_offset       = header_size;
        const u32 bin1_offset       = bin0_offset + (layout.m_bin0 * (u32)sizeof(u64));
        const u32 bin2_offset       = bin1_offset + (layout.m_bin1 * (u32)sizeof(u64));
        const u32 bin3_offset       = bin2_offset + (layout.m_bin2 * (u32)sizeof(u64));
        const u32 bin3_size         = layout.m_bin3 * (u32)sizeof(u64);
        const u32 items_offset      = math::alignUp(bin3_offset + bin3_size, page_size_bytes);
        const u32 reserve_size      = math::alignUp(items_offset + ((u32)item_size * max_items), page_size_bytes);
        const u32 meta_commit_size  = math::alignUp(bin3_offset, page_size_bytes);
        const u32 bin3_initial_size = initial_num_items >> 3;
        const u32 bin_commit_size   = math::alignUp(bin3_offset + bin3_initial_size, page_size_bytes);
        const u32 items_commit_size = math::alignUp((u32)item_size * initial_num_items, page_size_bytes);

        bin->m_memory = v_alloc_reserve(reserve_size);
        ASSERT(bin->m_memory != nullptr);
        if (bin->m_memory == nullptr)
            return;

        if (!v_alloc_commit(bin->m_memory, meta_commit_size))
        {
            v_alloc_release(bin->m_memory, reserve_size);
            bin->m_memory = nullptr;
            ASSERT(false);
            return;
        }

        if (bin_commit_size > meta_commit_size && !v_alloc_commit((byte*)bin->m_memory + meta_commit_size, bin_commit_size - meta_commit_size))
        {
            v_alloc_release(bin->m_memory, reserve_size);
            bin->m_memory = nullptr;
            ASSERT(false);
            return;
        }

        if (!v_alloc_commit((byte*)bin->m_memory + items_offset, items_commit_size))
        {
            v_alloc_release(bin->m_memory, reserve_size);
            bin->m_memory = nullptr;
            ASSERT(false);
            return;
        }

        ASSERT(bin0_offset <= 0xFFFFFFFFu);
        ASSERT(bin2_offset <= 0xFFFFFFFFu);
        ASSERT(bin3_offset <= 0xFFFFFFFFu);
        ASSERT(items_offset <= 0xFFFFFFFFu);

        ibin32_t* ibin                = s_bin32(bin);
        ibin->m_items_count           = 0;
        ibin->m_items_capacity        = max_items;
        ibin->m_items_highwater       = 0;
        ibin->m_item_sizeof           = item_size;
        ibin->m_page_size_shift       = page_size_shift;
        ibin->m_bin0_offset8          = (u8)bin0_offset;
        ibin->m_bin2_offset           = (u32)bin2_offset;
        ibin->m_bin3_offset           = (u32)bin3_offset;
        ibin->m_items_offset          = (u32)items_offset;
        ibin->m_reserved_pages        = (u32)(reserve_size >> page_size_shift);
        ibin->m_bin_committed_pages   = (u32)(bin_commit_size >> page_size_shift);
        ibin->m_items_committed_pages = (u32)(items_commit_size >> page_size_shift);

        nbitvec24::setup_used_lazy(s_bin32_bm0(ibin), s_bin32_bm1(ibin), s_bin32_bm2(ibin), s_bin32_bm3(ibin), max_items);
    }

    void bin_destroy(bin32_t* bin)
    {
        if (bin->m_memory != nullptr)
        {
            ibin32_t* ibin = s_bin32(bin);
            v_alloc_release(bin->m_memory, s_bin32_reserved_size(ibin));
            bin->m_memory = nullptr;
        }
    }

    u32 bin_size(bin32_t const* bin) { return s_bin32(bin)->m_items_count; }
    u32 bin_capacity(bin32_t const* bin) { return s_bin32(bin)->m_items_capacity; }
    u32 bin_highwater_mark(bin32_t const* bin) { return s_bin32(bin)->m_items_highwater; }

    void* bin_alloc(bin32_t* bin)
    {
        ibin32_t* ibin           = s_bin32(bin);
        const u32 items_capacity = bin_capacity(bin);
        if (ibin->m_items_count >= items_capacity)
            return nullptr;  // bin is full

        byte* items = s_bin32_items(ibin);

        const u32 item_free_index = ibin->m_items_highwater;

        u64* bm0 = s_bin32_bm0(ibin);
        u64* bm1 = s_bin32_bm1(ibin);
        u64* bm2 = s_bin32_bm2(ibin);
        u64* bm3 = s_bin32_bm3(ibin);

        if (ibin->m_items_count < item_free_index)
        {
            s32 item_index = -1;

            // We should have a free item in the binmap, where is it?
            item_index = nbitvec24::find_free_and_remove(bm0, bm1, bm2, bm3, item_free_index);

            ibin->m_items_count += 1;
            byte* item = items + ((u32)item_index * ibin->m_item_sizeof);
            return item;
        }
        else
        {
            if (!s_bin32_commit_bin3(bin, ibin, item_free_index + 1))
                return nullptr;

            if (!s_bin32_commit_items(bin, ibin, item_free_index + 1))
                return nullptr;

            nbitvec24::tick_used_lazy(bm0, bm1, bm2, bm3, items_capacity, item_free_index);

            ibin->m_items_count += 1;
            ibin->m_items_highwater += 1;
            return items + (item_free_index * (u32)ibin->m_item_sizeof);
        }
    }

    void bin_free(bin32_t* bin, void* ptr)
    {
        ibin32_t* ibin            = s_bin32(bin);
        const u32 item_free_index = ibin->m_items_highwater;

        const byte* items      = s_bin32_items(ibin);
        const s32   item_index = (s32)(((const byte*)ptr - items) / ibin->m_item_sizeof);
        if (item_index < 0 || (u32)item_index >= item_free_index)
            return;  // invalid pointer

        // Mark the item as free in the binmap
        u64* bm0 = s_bin32_bm0(ibin);
        u64* bm1 = s_bin32_bm1(ibin);
        u64* bm2 = s_bin32_bm2(ibin);
        u64* bm3 = s_bin32_bm3(ibin);

        nbitvec24::set_free(bm0, bm1, bm2, bm3, item_free_index, item_index);

        // Decrease number of used items
        ibin->m_items_count -= 1;
    }

    // convert a pointer to an index within the bin
    u32 bin_ptr2idx(bin32_t const* bin, void* ptr)
    {
        ibin32_t const* ibin  = s_bin32(bin);
        const byte*     items = s_bin32_items(ibin);
        if (ptr < (void*)items)
            return D_U32_MAX;  // invalid pointer
        const u32 item_free_index = ibin->m_items_highwater;
        const u32 index           = (u32)(((const byte*)ptr - items) / ibin->m_item_sizeof);
        if (index >= item_free_index)
            return D_U32_MAX;  // invalid pointer
        return index;
    }

    // convert an index to a pointer within the bin
    void* bin_idx2ptr(bin32_t* bin, u32 index)
    {
        ibin32_t* ibin            = s_bin32(bin);
        const u32 item_free_index = ibin->m_items_highwater;
        if (index >= item_free_index)
            return nullptr;  // invalid index
        byte* items = s_bin32_items(ibin);
        byte* item  = items + (index * (u32)ibin->m_item_sizeof);
        return item;
    }

    // highest index of free item in the bin
    s32 bin_highest_free(bin32_t const* bin)
    {
        ibin32_t const* ibin            = s_bin32(bin);
        const u32       item_free_index = ibin->m_items_highwater;
        s32             hi              = -1;

        u64 const* bm0 = s_bin32_bm0(ibin);
        u64 const* bm1 = s_bin32_bm1(ibin);
        u64 const* bm2 = s_bin32_bm2(ibin);
        u64 const* bm3 = s_bin32_bm3(ibin);

        hi = nbitvec24::find_free_last(bm0, bm1, bm2, bm3, item_free_index);
        return hi;
    }

    // ----------------------------------------------------------------------------------------------------------------------
    // bin16 implementation
    // ----------------------------------------------------------------------------------------------------------------------
    struct ibin16_t
    {
        u32 m_items_count;      // number of items currently in use
        u32 m_items_capacity;   // maximum number of items the bin can hold
        u32 m_items_highwater;  // highest number of items that have been in the bin
        u16 m_item_sizeof;      // sizeof(item)
        u8  m_page_size_shift;
        u8  m_bin0_offset;
        u32 m_bin2_offset;
        u32 m_items_offset;
        u32 m_reserved_pages;
        u32 m_committed_pages;
    };

    static inline ibin16_t*       s_bin16(bin16_t* bin) { return (ibin16_t*)bin->m_memory; }
    static inline ibin16_t const* s_bin16(bin16_t const* bin) { return (ibin16_t const*)bin->m_memory; }
    static inline byte*           s_bin16_base(bin16_t* bin) { return (byte*)bin->m_memory; }
    static inline u64*            s_bin16_bm0(ibin16_t* ibin) { return (u64*)((byte*)ibin + ibin->m_bin0_offset); }
    static inline u64*            s_bin16_bm1(ibin16_t* ibin) { return (u64*)((byte*)ibin + ibin->m_bin0_offset + 8); }
    static inline u64*            s_bin16_bm2(ibin16_t* ibin) { return (u64*)((byte*)ibin + ibin->m_bin2_offset); }
    static inline byte*           s_bin16_items(ibin16_t* ibin) { return (byte*)ibin + ibin->m_items_offset; }
    static inline byte const*     s_bin16_items(ibin16_t const* ibin) { return (byte const*)ibin + ibin->m_items_offset; }
    static inline u32             s_bin16_reserved_size(ibin16_t const* ibin) { return ibin->m_reserved_pages << ibin->m_page_size_shift; }
    static inline u32             s_bin16_committed_size(ibin16_t const* ibin) { return ibin->m_committed_pages << ibin->m_page_size_shift; }

    static bool s_bin16_commit(bin16_t* bin, ibin16_t* ibin, u32 required_size)
    {
        const u8  page_size_shift = ibin->m_page_size_shift;
        const u32 committed_pages = (u32)(math::alignUp(required_size, (u32)1 << page_size_shift) >> page_size_shift);
        if (committed_pages <= ibin->m_committed_pages)
            return true;
        if (committed_pages > ibin->m_reserved_pages)
            return false;
        const u32 extra_pages = committed_pages - ibin->m_committed_pages;
        if (!v_alloc_commit(s_bin16_base(bin) + s_bin16_committed_size(ibin), extra_pages << page_size_shift))
            return false;
        ibin->m_committed_pages = committed_pages;
        return true;
    }

    //  .d8888b.  8888888888 88888888888 888     888 8888888b.
    // d88P  Y88b 888            888     888     888 888   Y88b
    // Y88b.      888            888     888     888 888    888
    //  "Y888b.   8888888        888     888     888 888   d88P
    //     "Y88b. 888            888     888     888 8888888P"
    //       "888 888            888     888     888 888
    // Y88b  d88P 888            888     Y88b. .d88P 888
    //  "Y8888P"  8888888888     888      "Y88888P"  888

    void bin_setup(bin16_t* bin, u16 item_size, u32 max_items)
    {
        ASSERT(max_items <= 65536);
        ASSERT(item_size < (v_alloc_get_page_size() >> 1));

        // align the maximum number of items to a multiple of 64 (for binmap)
        max_items = math::alignUp(max_items, 64);

        // compute bin16_t struct and most of the binmap
        nbitvec::layout64_t raw_layout;
        nbitvec::compute(max_items, raw_layout);
        const bin16_layout_t layout = s_bin16_normalize_layout(raw_layout);

        const u32 initial_num_items = 64;
        const u8  page_size_shift   = v_alloc_get_page_size_shift();
        const u32 page_size_bytes   = (u32)1 << page_size_shift;
        const u32 header_size       = (u32)math::alignUp((u32)sizeof(ibin16_t), (u32)alignof(u64));
        ASSERT(layout.m_bin0 == 1u);
        const u32 bin0_offset       = header_size;
        const u32 bin1_offset       = header_size + (layout.m_bin0 * (u32)sizeof(u64));
        const u32 bin2_offset       = bin1_offset + (layout.m_bin1 * (u32)sizeof(u64));
        const u32 items_offset      = math::alignUp(bin2_offset + (layout.m_bin2 * (u32)sizeof(u64)), (u32)alignof(void*));
        const u32 reserve_size      = math::alignUp(items_offset + ((u32)item_size * max_items), page_size_bytes);
        const u32 commit_size       = math::alignUp(items_offset + ((u32)item_size * initial_num_items), page_size_bytes);

        bin->m_memory = v_alloc_reserve(reserve_size);
        ASSERT(bin->m_memory != nullptr);
        if (bin->m_memory == nullptr)
            return;
        if (!v_alloc_commit(bin->m_memory, commit_size))
        {
            v_alloc_release(bin->m_memory, reserve_size);
            bin->m_memory = nullptr;
            ASSERT(false);
            return;
        }

        ASSERT(bin1_offset <= 0xFFFFFFFFu);
        ASSERT(bin2_offset <= 0xFFFFFFFFu);
        ASSERT(items_offset <= 0xFFFFFFFFu);

        ibin16_t* ibin          = s_bin16(bin);
        ibin->m_items_count     = 0;
        ibin->m_items_capacity  = max_items;
        ibin->m_items_highwater = 0;
        ibin->m_item_sizeof     = item_size;
        ibin->m_page_size_shift = page_size_shift;
        ibin->m_bin0_offset     = (u8)bin0_offset;
        ibin->m_bin2_offset     = (u32)bin2_offset;
        ibin->m_items_offset    = (u32)items_offset;
        ibin->m_reserved_pages  = (u32)(reserve_size >> page_size_shift);
        ibin->m_committed_pages = (u32)(commit_size >> page_size_shift);

        u64* bm0 = s_bin16_bm0(ibin);
        u64* bm1 = s_bin16_bm1(ibin);
        u64* bm2 = s_bin16_bm2(ibin);
        nbitvec18::setup_used_lazy(bm0, bm1, bm2, max_items);
    }

    void bin_destroy(bin16_t* bin)
    {
        if (bin->m_memory != nullptr)
        {
            ibin16_t* ibin = s_bin16(bin);
            v_alloc_release(bin->m_memory, s_bin16_reserved_size(ibin));
            bin->m_memory = nullptr;
        }
    }

    u32 bin_size(bin16_t const* bin)
    {
        ibin16_t const* ibin = s_bin16(bin);
        return ibin->m_items_count;
    }

    u32 bin_capacity(bin16_t const* bin) { return s_bin16(bin)->m_items_capacity; }

    u32 bin_highwater_mark(bin16_t const* bin) { return s_bin16(bin)->m_items_highwater; }

    //        d8888 888      888      .d88888b.   .d8888b.
    //       d88888 888      888     d88P" "Y88b d88P  Y88b
    //      d88P888 888      888     888     888 888    888
    //     d88P 888 888      888     888     888 888
    //    d88P  888 888      888     888     888 888
    //   d88P   888 888      888     888     888 888    888
    //  d8888888888 888      888     Y88b. .d88P Y88b  d88P
    // d88P     888 88888888 88888888 "Y88888P"   "Y8888P"
    void* bin_alloc(bin16_t* bin)
    {
        ibin16_t* ibin = s_bin16(bin);
        if (ibin->m_items_count >= ibin->m_items_capacity)
            return nullptr;  // bin is full

        byte*     items           = s_bin16_items(ibin);
        const u32 item_free_index = ibin->m_items_highwater;

        u64* bm0 = s_bin16_bm0(ibin);
        u64* bm1 = s_bin16_bm1(ibin);
        u64* bm2 = s_bin16_bm2(ibin);

        if (ibin->m_items_count < item_free_index)
        {
            s32 item_index = -1;

            // We should have a free item in the binmap, where is it?
            item_index = nbitvec18::find_free_and_remove(bm0, bm1, bm2, item_free_index);
            ASSERT(item_index >= 0);

            ibin->m_items_count += 1;
            return items + (item_index * ibin->m_item_sizeof);
        }
        else
        {
            if (!s_bin16_commit(bin, ibin, ibin->m_items_offset + ((item_free_index + 1) * (u32)ibin->m_item_sizeof)))
                return nullptr;

            nbitvec18::tick_used_lazy(bm0, bm1, bm2, ibin->m_items_capacity, item_free_index);

            ibin->m_items_count += 1;
            ibin->m_items_highwater += 1;
            return items + (item_free_index * (u32)ibin->m_item_sizeof);
        }
    }

    // 8888888888 8888888b.  8888888888 8888888888
    // 888        888   Y88b 888        888
    // 888        888    888 888        888
    // 8888888    888   d88P 8888888    8888888
    // 888        8888888P"  888        888
    // 888        888 T88b   888        888
    // 888        888  T88b  888        888
    // 888        888   T88b 8888888888 8888888888

    void bin_free(bin16_t* bin, void* ptr)
    {
        ibin16_t* ibin            = s_bin16(bin);
        byte*     items           = s_bin16_items(ibin);
        const u32 item_free_index = ibin->m_items_highwater;
        const s32 item_index      = (s32)(((const byte*)ptr - items) / ibin->m_item_sizeof);
        if (item_index < 0 || (u32)item_index >= item_free_index)
            return;  // invalid pointer

        u64* bm0 = s_bin16_bm0(ibin);
        u64* bm1 = s_bin16_bm1(ibin);
        u64* bm2 = s_bin16_bm2(ibin);

        nbitvec18::set_free(bm0, bm1, bm2, item_free_index, item_index);

        // Decrease number of used items
        ibin->m_items_count -= 1;
    }

    u32 bin_item_sizeof(bin16_t const* bin)
    {
        ibin16_t const* ibin = s_bin16(bin);
        return ibin->m_item_sizeof;
    }

    // convert a pointer to an index within the bin
    i32 bin_ptr2idx(bin16_t const* bin, void* ptr)
    {
        ibin16_t const* ibin = s_bin16(bin);

        byte const* items = s_bin16_items(ibin);
        if (ptr < items)
            return -1;  // invalid pointer
        const u32 index           = (u32)(((const byte*)ptr - items) / ibin->m_item_sizeof);
        const u32 item_free_index = ibin->m_items_highwater;
        if (index >= item_free_index)
            return -1;  // invalid index
        return (i32)index;
    }

    // convert an index to a pointer within the bin
    void* bin_idx2ptr(bin16_t* bin, u16 index)
    {
        ibin16_t* ibin = s_bin16(bin);

        const u32 item_free_index = ibin->m_items_highwater;
        if (index >= item_free_index)
            return nullptr;  // invalid index
        byte* items = s_bin16_items(ibin);
        return items + (index * ibin->m_item_sizeof);
    }

    // highest index of free item in the bin
    s32 bin_highest_free(bin16_t const* bin)
    {
        ibin16_t*  ibin = (ibin16_t*)s_bin16(bin);
        u64 const* bm0  = s_bin16_bm0(ibin);
        u64 const* bm1  = s_bin16_bm1(ibin);
        u64 const* bm2  = s_bin16_bm2(ibin);

        const u32 item_free_index = ibin->m_items_highwater;
        const s32 hi              = nbitvec18::find_free_last(bm0, bm1, bm2, item_free_index);
        return hi;
    }

}  // namespace ncore

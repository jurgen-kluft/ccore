#ifndef __CCORE_SEGMENT_H__
#define __CCORE_SEGMENT_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

namespace ncore
{
    namespace nsegment
    {
        typedef i32  node_t;
        const node_t cINVALID_NODE = -1;

        struct chain_t;

        struct dlnode_t
        {
            u16 m_next;
            u16 m_prev;
        };

        struct allocator_t
        {
            byte*     m_base_address;            // base address of the reserved address space for this allocator
            u32       m_total_minsize_segments;  // total number of minsize segments in the address space
            u8        m_segment_minsize_shift;   // minimum size (1u << m_segment_minsize_shift) for a segment
            u8        m_segment_maxsize_shift;   // maximum size (1u << m_segment_maxsize_shift) for a segment
            u8        m_pagesize_shift;          // page size (1u << m_pagesize_shift)
            u8        m_bookkeeping_num_pages;   // bookkeeping num pages
            u16       m_free_list_heads[32];     // free list heads for each size class (0 = smallest, 31 = largest)
            chain_t*  m_chain;                   // m_chain[max_nodes]
            dlnode_t* m_free;                    // m_free[max_nodes]
        };

        // Note: @address_space_num_pages MUST be a power of two
        void initialize(allocator_t* allocator, u64 address_space_size = 128 * cGB, u64 segment_min_size = 8 * cMB, u64 segment_max_size = 1 * cGB);
        void teardown(allocator_t* allocator);

        // Note: size should be a power-of-two number of pages
        node_t alloc_node(allocator_t* allocator, u64 size);
        void   dealloc_node(allocator_t* allocator, node_t node);

        // return the virtual address of a node from its index including the number of pages available
        void* get_address(allocator_t* allocator, node_t node, u32& num_pages);

        // Grow or shrink the physically backed prefix of an allocated node to an absolute page target.
        // Targets must fit the node and the 24-bit committed-page counter.
        void commit(allocator_t* allocator, node_t node, u32 target_pages);    // target_pages >= current committed pages
        void decommit(allocator_t* allocator, node_t node, u32 target_pages);  // target_pages <= current committed pages

    }  // namespace nsegment
}  // namespace ncore

#endif  // __CCORE_SEGMENT_H__

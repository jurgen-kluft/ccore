#ifndef __CCORE_VMEM_SMALL_BLOCK_H__
#define __CCORE_VMEM_SMALL_BLOCK_H__
#include "ccore/c_target.h"
#ifdef USE_PRAGMA_ONCE
#    pragma once
#endif

#include "ccore/c_allocator.h"
#include "ccore/c_binmap1.h"

namespace ncore
{
    // Regions of 64KB, for allocating 16, 32, 64, 128 or 256 byte items
    struct sbnil_t
    {
        u64      m_empty0;  // unused
        sbnil_t *m_prev;
        sbnil_t *m_next;
    };

    // 64KB, 4096 items
    struct sb16_t
    {
        u64     m_bin0;  // binmap for free/used items, 16 byte items
        sb16_t *m_prev;
        sb16_t *m_next;
    };

    // 64KB, 2048 items
    struct sb32_t
    {
        u64     m_bin0;  // binmap for free/used items, 32 byte items
        sb32_t *m_prev;
        sb32_t *m_next;
    };

    // 64KB, 1024 items
    struct sb64_t
    {
        u64     m_bin0;  // binmap for free/used items, 64 byte items
        sb64_t *m_prev;
        sb64_t *m_next;
    };

    // 64KB, 512 items
    struct sb128_t
    {
        u64      m_bin0;  // binmap for free/used items, 128 byte items
        sb128_t *m_prev;
        sb128_t *m_next;
    };

    // 64KB, 256 items
    struct sb256_t
    {
        u64      m_bin0;  // binmap for free/used items, 256 byte items
        sb256_t *m_prev;
        sb256_t *m_next;
    };

    // 64KB, 128 items
    struct sb512_t
    {
        u64      m_bin0;  // binmap for free/used items, 512 byte items
        sb512_t *m_prev;
        sb512_t *m_next;
    };

    // All the above structures are allocated from the sbarena itself, since
    // they are exactly 32 bytes.
    //
    // The binmap array for each size is also one of the following sizes:
    // - 16 byte items:  4096 bits = 512 bytes
    // - 32 byte items:  2048 bits = 256 bytes
    // - 64 byte items:  1024 bits = 128 bytes
    // - 128 byte items:  512 bits =  64 bytes
    // - 256 byte items:  256 bits =  32 bytes
    // - 512 byte items:  128 bits =  16 bytes
    //
    // So these can also be allocated from the sbarena itself.
    // We just need to initialize sbarena, since the 512 items depend on
    // the 16 byte items, and vice versa. Same for the 256 <-> 32 byte items, etc.
    //
    // Example: doing an alloc16() may need to allocate a new sb16_t structure,
    // which may need to allocate something from alloc512(), which may need to allocate
    // a new sb512_t structure, which may need to allocate something from alloc16(), etc.
    // So we need to be careful to avoid infinite recursion.

    // small block arena (64 bytes)
    struct sbarena_t
    {
        sbarena_t();
        arena_t *m_arena;             // dedicated arena for 64KB blocks
        sbnil_t *m_free_list;         // list of free 64KB blocks
        sb16_t  *m_sb16_active_list;  // list of active 16 byte item blocks
        sb32_t  *m_sb32_active_list;
        sb64_t  *m_sb64_active_list;
        sb128_t *m_sb128_active_list;
        sb256_t *m_sb256_active_list;
        sb512_t *m_sb512_active_list;
        i32      m_alloc16_count;
        i32      m_alloc32_count;
        i32      m_alloc64_count;
        i32      m_alloc128_count;
        i32      m_alloc256_count;
        i32      m_alloc512_count;
        i32      m_alloc16_free_index;
        i32      m_alloc32_free_index;
        i32      m_alloc64_free_index;
        i32      m_alloc128_free_index;
        i32      m_alloc256_free_index;
        i32      m_alloc512_free_index;
    };

    void sbarena_construct(sbarena_t *sbarena, int_t reserve_size, int_t commit_size);
    void sbarena_destruct(sbarena_t *sbarena);

    void *sbarena_alloc16(sbarena_t *sbarena);
    void *sbarena_alloc32(sbarena_t *sbarena);
    void *sbarena_alloc64(sbarena_t *sbarena);
    void *sbarena_alloc128(sbarena_t *sbarena);
    void  sbarena_free(sbarena_t *sbarena, void *ptr);

}  // namespace ncore

#endif  // __CCORE_VMEM_SMALL_BLOCK_H__

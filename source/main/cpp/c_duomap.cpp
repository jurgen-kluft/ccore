#include "ccore/c_allocator.h"
#include "ccore/c_integer.h"
#include "ccore/c_duomap.h"

namespace ncore
{
    // Note: Size has to be passed to each function since most of the time
    //       the user knows the amount of bits they are going to use and
    //       it makes no sense to store the size in the structure itself.

    //           16             = 32 + 32 + 64 = 128 bits max
    //        32 * 64           = 256, 512, 1K, 2K bits max (num binnodes = 0, num leafnodes = 1)
    //       32 * 2048          = 4K, 8K, 16K, 32K, 64K bits max (num binnodes = 1, num leafnodes = 32)
    //     32 * 32 * 2048       = 128K, 256K, 512K, 1M, 2M bits max (num binnodes = 32, num leafnodes = 512)
    //   32 * 32 * 32 * 2048    = 4M, 8M, 16M, 32M, 64M bits max (num binnodes = 1024, num leafnodes = 16384)
    // 32 * 32 * 32 * 32 * 2048 = 128M, 256M, 512M, 1G, 2G bits max (num binnodes = 32768, num leafnodes = 524288)

    // If size <= 128 then only use bintree_t (fully allocated)
    // Else If size <= (32 * 64 = 2K) then use one bitmap_t (fully allocated)
    // Else If size <= (32 * 32 * 2048 = 64K), 2 levels, (can be partially allocated, some children may be null)
    // Else If size <= (32 * 32 * 32 * 2048 = 2M), 3 levels (can be partially allocated, some children may be null)
    // Else If size <= (32 * 32 * 32 * 32 * 2048 = 64M), 4 levels (can be partially allocated, some children may be null)

    // If bin0 = 1 (no   zeros) and bin1 = 1 (some ones) then the leaf is all 1, so we can delete it
    // If bin0 = 0 (some zeros) and bin1 = 0 (no   ones) then the leaf is all 0, so we can delete it

    struct node_t;

    struct tree_t
    {
        u32     m_map0;
        u32     m_map1;
        node_t* m_children;
    };

    struct node_t
    {
        u32     m_map0;
        u32     m_map1;
        node_t* m_node;
    };

    struct bitmap_t
    {
        u32  m_map0;
        u32  m_map1;
        u64* m_bitmap;  // array of N u64 bitmaps (children), 4 / 8 / 12 / 16 / 20 / 24 / 28 / 32
    };

    void setup(alloc_t* allocator, bintree_t* bt, u32 const maxbits)
    {
        if (maxbits <= 128)
        {
            bt->m_bitmap[0] = 0;
            bt->m_bitmap[1] = 0;
        }
        else if (maxbits <= 2048)
        {
            bitmap_t* node = (bitmap_t*)bt;
            node->m_map0   = 0;
            node->m_map1   = 0;
            u32 const N    = (((maxbits + 63) >> 6) + 3) >> 2;
            node->m_bitmap = g_allocate_array_and_clear<u64>(allocator, N * 4);
        }
        else if (maxbits <= 65536)
        {
            node_t* node = (node_t*)bt;
            node->m_map0 = 0;
            node->m_map1 = 0;
            node->m_node = g_allocate_array_and_clear<bitmap_t>(allocator, 32);
        }
        else
        {
            node_t* node = (node_t*)bt;
            node->m_map0 = 0;
            node->m_map1 = 0;
            node->m_node = g_allocate_array_and_clear<node_t>(allocator, 32);
        }
    }

    void clear(alloc_t* allocator, bintree_t* bt, u32 const maxbits);

    void set(alloc_t* allocator, bintree_t* bt, u32 const maxbits, u32 bit)
    {
        if (maxbits <= 128)
        {
            u32 const index = bit >> 6;
            u32 const mask  = 1 << (bit & 63);
            bt->m_bitmap[index] |= mask;
        }
        else
        {
            node_t* node = (node_t*)bt;

            // 5 : 5 : 5 : 5 : 10
            s8 levels = (math::mostSignificantBit(maxbits - 1) / 5);
            if (levels >= 2)
            {
                levels -= 2;

                s8 ls[4];
                s8 l = 0;
                switch (levels)
                {
                    case 3: ls[l++] = bit_range & 31; bit_range >>= 5;  // fall through
                    case 2: ls[l++] = bit_range & 31; bit_range >>= 5;  // fall through
                    case 1: ls[l++] = bit_range & 31; bit_range >>= 5;  // fall through
                    case 0: ls[l++] = bit_range & 31;                   // fall through
                }

                while (levels > 0)
                {
                    u32 const index = ls[levels - 1];
                    if (node->m_node[index].m_node == nullptr)
                    {
                        node->m_node = g_allocate_array_and_clear<node_t>(allocator, 32);
                    }
                    node = &((node_t*)node->m_node)[index];
                    levels -= 1;
                }
            }

            bitmap_t* bitmap = (bitmap_t*)node;
            if (bitmap->m_bitmap == nullptr)
            {
                bitmap->m_bitmap = g_allocate_array_and_clear<u64>(allocator, 32);
            }

            u32 const index = 
        }
    }

};  // namespace ncore

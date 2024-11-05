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
    // Else If size <= (32 * 64 = 2K) then use one binleaf_t (fully allocated)
    // Else If size <= (32 * 32 * 2048 = 64K), 2 levels, (can be partially allocated, some children may be null)
    // Else If size <= (32 * 32 * 32 * 2048 = 2M), 3 levels (can be partially allocated, some children may be null)
    // Else If size <= (32 * 32 * 32 * 32 * 2048 = 64M), 4 levels (can be partially allocated, some children may be null)

    // If bin0 = 1 (no zeros) and bin1 = 1 (ones) then the leaf is all 1, so we can delete it
    // If bin0 = 1 (no zeros) and bin1 = 0 (no ones) then we are uninitialized and the child pointer is not valid
    // If bin0 = 0 (zeros) and bin1 = 0 (no ones) then the leaf is all 0, so we can delete it

    const u32 N = 32;

    // N is dynamic: 2(32) / 4(64) / 8(128) / 16(256) / 32(512) nodes
    struct binnode_t
    {
        u32        m_bin0[N];  // Track '0' bits
        u32        m_bin1[N];  // Track '1' bits
        binnode_t* m_children[N];
    };

    // Can have any number of bins, 4 / 8 / 16 / 32
    struct bitmap_t
    {
        u64 m_bins[32];
    };

    // We have multiple 'types' 
    struct duotree1_t
    {
        u32       m_bin0;
        u32       m_bin1;
        bitmap_t* m_leaf;
    };

    // One binnode with N children
    struct duotree2_t
    {
        u32        m_bin0;
        u32        m_bin1;
        binnode_t* m_node;
    };



};  // namespace ncore

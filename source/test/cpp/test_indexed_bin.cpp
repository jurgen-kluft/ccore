#include "ccore/c_target.h"
#include "ccore/c_allocator.h"
#include "ccore/c_indexed_bin.h"
#include "ccore/c_memory.h"
#include "ccore/c_random.h"

#include "cunittest/cunittest.h"

using namespace ncore;

struct ibin_item_t
{
    u32 value;
    u32 payload;
};

// UNITTEST_SUITE_BEGIN(indexed_bin)
// {
//     UNITTEST_FIXTURE(bin16)
//     {
//         UNITTEST_FIXTURE_SETUP() {}
//         UNITTEST_FIXTURE_TEARDOWN() {}

//         UNITTEST_ALLOCATOR;

//         UNITTEST_TEST(create_destroy_1)
//         {
//             ibin16_t bin;
//             bin_setup(&bin, 64);
//             bin_destroy(&bin);
//         }

//         UNITTEST_TEST(create_destroy_2)
//         {
//             ibin16_t bin;
//             bin_setup(&bin, 64);
//             bin_destroy(&bin);
//         }

//         UNITTEST_TEST(a_few_alloc_free)
//         {
//             ibin16_t bin;
//             bin_setup(&bin, 64);
//             const u32 num_allocs = 1000;
//             i32       ptrs[num_allocs];
//             i32*      indices = g_allocate_array<i32>(Allocator, num_allocs);
//             for (u32 i = 0; i < num_allocs; ++i)
//             {
//                 ptrs[i] = bin_alloc(&bin, (u16)i);
//                 CHECK_NOT_EQUAL(-1, ptrs[i]);
//             }
//             for (u32 i = 0; i < num_allocs; ++i)
//             {
//                 const u16 tag = bin_get_tag(&bin, ptrs[i]);
//                 CHECK_EQUAL((u16)i, tag);
//                 bin_free(&bin, ptrs[i]);
//             }
//             g_deallocate_array(Allocator, indices);
//             bin_destroy(&bin);
//         }

//         UNITTEST_TEST(a_lot_more_alloc_free)
//         {
//             ibin16_t bin;
//             bin_setup(&bin, 64);

//             const u32 num_allocs = 50000;
//             i32*      ptrs       = g_allocate_array<i32>(Allocator, num_allocs);
//             u16*      indices    = g_allocate_array<u16>(Allocator, num_allocs);

//             for (u32 i = 0; i < num_allocs; ++i)
//             {
//                 indices[i] = (u16)i;
//                 ptrs[i]    = bin_alloc(&bin, (u16)i);
//                 CHECK_NOT_EQUAL(-1, ptrs[i]);
//             }

//             xor_random_t rnd(0x1234567890abcdef);

//             // 'randomly' shuffle indices
//             for (u32 i = 0; i < num_allocs; ++i)
//             {
//                 u32 const a = rnd.rand32() % num_allocs;
//                 if (a != i)
//                     nmem::swap(indices[a], indices[i]);
//             }

//             for (u32 i = 0; i < num_allocs; ++i)
//             {
//                 u16 index = indices[i];
//                 const u16 tag = bin_get_tag(&bin, ptrs[index]);
//                 CHECK_EQUAL(index, tag);
//                 bin_free(&bin, ptrs[index]);
//             }

//             g_deallocate_array(Allocator, ptrs);
//             g_deallocate_array(Allocator, indices);

//             bin_destroy(&bin);
//         }
//     }

//     UNITTEST_FIXTURE(bin32)
//     {
//         UNITTEST_FIXTURE_SETUP() {}
//         UNITTEST_FIXTURE_TEARDOWN() {}

//         UNITTEST_TEST(create_destroy)
//         {
//             ibin32_t bin;
//             bin_setup(&bin, sizeof(ibin_item_t));
//             bin_destroy(&bin);
//         }

//         UNITTEST_TEST(a_few_alloc_free_with_ptr_roundtrip)
//         {
//             ibin32_t bin;
//             bin_setup(&bin, sizeof(ibin_item_t));

//             const u32 num_allocs = 256;
//             i32       indices[num_allocs];

//             for (u32 i = 0; i < num_allocs; ++i)
//             {
//                 const u32 tag = 0x10000u + i;
//                 indices[i]    = bin_alloc(&bin, tag);
//                 CHECK_NOT_EQUAL(-1, indices[i]);

//                 ibin_item_t* item = (ibin_item_t*)bin_idx2ptr(&bin, (u32)indices[i]);
//                 CHECK_NOT_NULL(item);

//                 item->value   = i;
//                 item->payload = i * 3;

//                 CHECK_EQUAL((i32)tag, bin_get_tag(&bin, (u32)indices[i]));
//                 CHECK_EQUAL(indices[i], bin_ptr2idx(&bin, item));
//             }

//             CHECK_EQUAL(num_allocs, bin_size(&bin));

//             for (u32 i = 0; i < num_allocs; ++i)
//             {
//                 ibin_item_t* item = (ibin_item_t*)bin_idx2ptr(&bin, (u32)indices[i]);
//                 CHECK_NOT_NULL(item);
//                 CHECK_EQUAL(i, item->value);
//                 CHECK_EQUAL(i * 3, item->payload);

//                 bin_free(&bin, (u32)indices[i]);
//             }

//             CHECK_EQUAL(0u, bin_size(&bin));
//             bin_destroy(&bin);
//         }

//         UNITTEST_TEST(compact_moves_last_item_into_first_hole)
//         {
//             ibin32_t bin;
//             bin_setup(&bin, sizeof(ibin_item_t));

//             i32 indices[4];
//             for (u32 i = 0; i < 4; ++i)
//             {
//                 indices[i] = bin_alloc(&bin, 100u + i);
//                 CHECK_EQUAL((i32)i, indices[i]);

//                 ibin_item_t* item = (ibin_item_t*)bin_idx2ptr(&bin, i);
//                 CHECK_NOT_NULL(item);
//                 item->value   = 1000u + i;
//                 item->payload = 2000u + i;
//             }

//             bin_free(&bin, 1);
//             CHECK_EQUAL(3u, bin_size(&bin));

//             u32 new_index  = 0xffffffffu;
//             i32 moved_from = bin_compact(&bin, new_index);

//             CHECK_EQUAL(3, moved_from);
//             CHECK_EQUAL(1u, new_index);
//             CHECK_EQUAL(3u, bin_size(&bin));
//             CHECK_EQUAL(103, bin_get_tag(&bin, new_index));

//             ibin_item_t* compacted = (ibin_item_t*)bin_idx2ptr(&bin, new_index);
//             CHECK_NOT_NULL(compacted);
//             CHECK_EQUAL(1003u, compacted->value);
//             CHECK_EQUAL(2003u, compacted->payload);

//             CHECK_EQUAL((i32)new_index, bin_ptr2idx(&bin, compacted));

//             bin_destroy(&bin);
//         }
//     }
// }
// UNITTEST_SUITE_END

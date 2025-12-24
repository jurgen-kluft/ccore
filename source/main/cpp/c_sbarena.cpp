#include "ccore/c_binmap1.h"
#include "ccore/c_vmem.h"
#include "ccore/c_sbarena.h"
#include "ccore/c_math.h"
#include "ccore/c_memory.h"

namespace ncore
{
    sbarena_t::sbarena_t()
        : m_arena(nullptr)
        , m_free_list(nullptr)
        , m_sb16_active_list(nullptr)
        , m_sb32_active_list(nullptr)
        , m_sb64_active_list(nullptr)
        , m_sb128_active_list(nullptr)
        , m_sb256_active_list(nullptr)
        , m_sb512_active_list(nullptr)
    {
    }

    void sbarena_construct(sbarena_t* sbarena, int_t reserve_size, int_t commit_size) { sbarena->m_arena = narena::create(reserve_size, commit_size); }

    void sbarena_destruct(sbarena_t*& sbarena)
    {
        narena::destroy(sbarena->m_arena);
        sbarena = nullptr;
    }

    void* sbarena_alloc16(sbarena_t* sbarena)
    {
        const i32 itemsize = 16;
        const i32 offset   = sizeof(sb16_t) + ((65536 / itemsize) / 8);
        const i32 count    = (65536 - offset) / itemsize;

        sb16_t* block16 = nullptr;
        if (sbarena->m_sb16_active_list == nullptr)
        {
            // Any free 64KB blocks?
            if (sbarena->m_free_list != nullptr)
            {
                sbnil_t* block       = sbarena->m_free_list;
                sbarena->m_free_list = block->m_next;
                block16              = (sb16_t*)block;
            }
            else
            {
                block16 = (sb16_t*)narena::alloc(sbarena->m_arena, 65536);
            }
            sbarena->m_sb16_active_list = block16;

            block16->m_next             = nullptr;
            block16->m_prev             = nullptr;
            block16->m_bin0             = 0;  // all items free
        }
        block16 = sbarena->m_sb16_active_list;
        u64*      bin1    = (u64*)((byte*)block16 + sizeof(sb16_t));
        const s32 index   = nbinmap12::find_and_set(&block16->m_bin0, bin1, count);
    }

}  // namespace ncore

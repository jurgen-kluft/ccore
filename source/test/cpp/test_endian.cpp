#include "ccore/c_endian.h"
#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(test_endian)
{
    UNITTEST_FIXTURE(main)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(_u16)
        {
			u16 a = 0x1234;
			u16 le = nendian_le::read_u16((const u8*)&a);
			u16 be = nendian_be::read_u16((const u8*)&a);
			CHECK_EQUAL(0x1234, a);
			CHECK_EQUAL(0x1234, le);
			CHECK_EQUAL(0x3412, be);
        }

        UNITTEST_TEST(_s16)
        {
			s16 a = 0x1234;
			s16 le = nendian_le::read_s16((const u8*)&a);
			s16 be = nendian_be::read_s16((const u8*)&a);
			CHECK_EQUAL(0x1234, a);
			CHECK_EQUAL(0x1234, le);
			CHECK_EQUAL(0x3412, be);
        }

        UNITTEST_TEST(_u32)
        {
			u32 a = 0x12345678;
			u32 le = nendian_le::read_u32((const u8*)&a);
            u32 be = nendian_be::read_u32((const u8*)&a);
			CHECK_EQUAL((u32)0x12345678, a);
			CHECK_EQUAL((u32)0x12345678, le);
			CHECK_EQUAL((u32)0x78563412, be);
        }

        UNITTEST_TEST(_s32)
        {
			s32 a = 0x12345678;
			s32 le = nendian_le::read_s32((const u8*)&a);
            s32 be = nendian_be::read_s32((const u8*)&a);
			CHECK_EQUAL(0x12345678, a);
			CHECK_EQUAL(0x12345678, le);
			CHECK_EQUAL(0x78563412, be);
        }

        UNITTEST_TEST(_u64)
        {
			u64 a = D_CONSTANT_U64(0x90ABCDEF12345678);
			u64 le = nendian_le::read_u64((const u8*)&a);
            u64 be = nendian_be::read_u64((const u8*)&a);
			CHECK_EQUAL(D_CONSTANT_U64(0x90ABCDEF12345678), a);
			CHECK_EQUAL(D_CONSTANT_U64(0x90ABCDEF12345678), le);
			CHECK_EQUAL(D_CONSTANT_U64(0x78563412EFCDAB90), be);
        }

        UNITTEST_TEST(_s64)
        {
			s64 a = D_CONSTANT_U64(0x90ABCDEF12345678);
			s64 le = nendian_le::read_u64((const u8*)&a);
            s64 be = nendian_be::read_u64((const u8*)&a);
			CHECK_EQUAL(D_CONSTANT_S64(0x90ABCDEF12345678), a);
			CHECK_EQUAL(D_CONSTANT_S64(0x90ABCDEF12345678), le);
			CHECK_EQUAL(D_CONSTANT_S64(0x78563412EFCDAB90), be);
        }
	}
}
UNITTEST_SUITE_END

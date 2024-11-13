#include "ccore/c_math.h"
#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(integer)
{
	UNITTEST_FIXTURE(main)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		UNITTEST_TEST(Min)
		{
			u8 au8 = 1, bu8 = 2, cu8 = 0;
			CHECK_EQUAL(1, math::g_min(au8,bu8));
			CHECK_EQUAL(0, math::g_min(au8,cu8));

			u16 au16 = 1, bu16 = 2, cu16 = 0;
			CHECK_EQUAL(1, math::g_min(au16,bu16));
			CHECK_EQUAL(0, math::g_min(au16,cu16));

			u32 au32 = 1, bu32 = 2, cu32 = 0;
			CHECK_EQUAL(1, math::g_min(au32,bu32));
			CHECK_EQUAL(0, math::g_min(au32,cu32));

			s32 as32 = 1, bs32 = 2, cs32 = 0;
			CHECK_EQUAL(1, math::g_min(as32,bs32));
			CHECK_EQUAL(0, math::g_min(as32,cs32));

			u64 au64 = 1, bu64 = 2, cu64 = 0;
			CHECK_EQUAL(1, math::g_min(au64,bu64));
			CHECK_EQUAL(0, math::g_min(au64,cu64));
		}

		UNITTEST_TEST(Max)
		{
			u8 au8 = 1, bu8 = 2, cu8 = 0;
			CHECK_EQUAL(2, math::g_max(au8,bu8));
			CHECK_EQUAL(1, math::g_max(au8,cu8));

			u16 au16 = 1, bu16 = 2, cu16 = 0;
			CHECK_EQUAL(2, math::g_max(au16,bu16));
			CHECK_EQUAL(1, math::g_max(au16,cu16));

			u32 au32 = 1, bu32 = 2, cu32 = 0;
			CHECK_EQUAL(2, math::g_max(au32,bu32));
			CHECK_EQUAL(1, math::g_max(au32,cu32));

			u32 as32 = 1, bs32 = 2, cs32 = 0;
			CHECK_EQUAL(2, math::g_max(as32,bs32));
			CHECK_EQUAL(1, math::g_max(as32,cs32));

			u64 au64 = 1, bu64 = 2, cu64 = 0;
			CHECK_EQUAL(2, math::g_max(au64,bu64));
			CHECK_EQUAL(1, math::g_max(au64,cu64));
		}

		UNITTEST_TEST(Clamp)
		{
			u8 au8 = 0, bu8 = 1, cu8 = 2;
			CHECK_EQUAL(1, math::g_clamp(au8,bu8,cu8));
			CHECK_EQUAL(1, math::g_clamp(bu8,bu8,cu8));

			u16 au16 = 0, bu16 = 1, cu16 = 2;
			CHECK_EQUAL(1, math::g_clamp(au16,bu16,cu16));
			CHECK_EQUAL(2, math::g_clamp(cu16,bu16,cu16));

			u32 au32 = 0, bu32 = 1, cu32 = 2;
			CHECK_EQUAL(1, math::g_clamp(au32,bu32,cu32));
			CHECK_EQUAL(2, math::g_clamp(au32,cu32,cu32));

			u64 au64 = 0, bu64 = 1, cu64 = 2;
			CHECK_EQUAL(1, math::g_clamp(au64,bu64,cu64));
			CHECK_EQUAL(1, math::g_clamp(bu64,au64,cu64));
		}

		UNITTEST_TEST(Align)
		{
			CHECK_EQUAL(16, math::g_align(17, 16));
			CHECK_EQUAL(256, math::g_align(300, 256));
			CHECK_EQUAL(1024, math::g_align(2011, 1024));
		}

		UNITTEST_TEST(AlignDown)
		{
			CHECK_EQUAL(16, math::g_alignDown(17, 16));
			CHECK_EQUAL(256, math::g_alignDown(300, 256));
			CHECK_EQUAL(1024, math::g_alignDown(2011, 1024));
		}

		UNITTEST_TEST(AlignUp)
		{
			CHECK_EQUAL(16, math::g_alignUp(12, 16));
			CHECK_EQUAL(32, math::g_alignUp(17, 16));
			CHECK_EQUAL(256, math::g_alignUp(200, 256));
			CHECK_EQUAL(512, math::g_alignUp(300, 256));
			CHECK_EQUAL(2048, math::g_alignUp(2011, 1024));
		}

		UNITTEST_TEST(IsAligned)
		{
			CHECK_TRUE(math::g_isAligned(16, 16));
			CHECK_TRUE(math::g_isAligned(256, 256));
			CHECK_TRUE(math::g_isAligned(1024, 1024));
		}

		UNITTEST_TEST(Abs)
		{
			s32 a = 0, b = 1, c = -2;
			CHECK_EQUAL(0, math::g_abs(a));
			CHECK_EQUAL(1, math::g_abs(b));
			CHECK_EQUAL(2, math::g_abs(c));
		}

		UNITTEST_TEST(Sqr)
		{
			s32 a = 0, b = 1, c = -2;
			CHECK_EQUAL(0, math::g_sqr(a));
			CHECK_EQUAL(1, math::g_sqr(b));
			CHECK_EQUAL(4, math::g_sqr(c));

			u32 au = 0, bu = 1, cu = 2;
			CHECK_EQUAL(0, math::g_sqr(au));
			CHECK_EQUAL(1, math::g_sqr(bu));
			CHECK_EQUAL(4, math::g_sqr(cu));
		}

		UNITTEST_TEST(Mod)
		{
			s32 a = 3, b = 50, c = -2;
			CHECK_EQUAL(3, math::g_mod(a,b));
			CHECK_EQUAL(0, math::g_mod(b,c));
			CHECK_EQUAL(2, math::g_mod(b,a));

			// WII: truncates towards zero

			CHECK_EQUAL(-2, math::g_mod(c,a));
			c = -22;
			a = 7;
			CHECK_EQUAL(-1, math::g_mod(c,a));
		}

		UNITTEST_TEST(Gcd)
		{
			s32 a = 12, b = 50, c = -2;
			CHECK_EQUAL(2, math::g_gcd(a,b));
			CHECK_EQUAL(-2, math::g_gcd(b,c));
			CHECK_EQUAL(-2, math::g_gcd(c,a));
		}

		UNITTEST_TEST(Average)
		{
			s32 a = 13, b = 50, c = -200;
			CHECK_TRUE(math::g_average(a,b) >= 31);
			CHECK_TRUE(math::g_average(a,b) <= 32);
			CHECK_TRUE(math::g_average(b,c) == -75);
			CHECK_TRUE(math::g_average(c,a) <= -93);
			CHECK_TRUE(math::g_average(c,a) >= -94);
		}

		UNITTEST_TEST(IsPowerOf2)
		{
			s32 a = 13, b = 64;
			CHECK_FALSE(math::g_ispo2(a));
			CHECK_TRUE(math::g_ispo2(b));
		}

		UNITTEST_TEST(CeilPower2)
		{
			u32 a = 13, b = 64;
			CHECK_EQUAL(16, math::g_ceilpo2(a));
			CHECK_EQUAL(64, math::g_ceilpo2(b));
		}

		UNITTEST_TEST(FloorPower2)
		{
			u32 a = 13, b = 64;
			CHECK_EQUAL(8, math::g_floorpo2(a));
			CHECK_EQUAL(64, math::g_floorpo2(b));
		}

		UNITTEST_TEST(BitReverse)
		{
			u32 a = 13, b = 64;
			CHECK_EQUAL(D_CONSTANT_U32(2952790016), math::g_bitReverse(a));
			CHECK_EQUAL(33554432, math::g_bitReverse(b));
		}

		UNITTEST_TEST(CountBits)
		{
			u32 a = 13, b = 64;
			CHECK_EQUAL(3, math::g_countBits(a));
			CHECK_EQUAL(1, math::g_countBits(b));
		}

		UNITTEST_TEST(Log2_s32)
        {
            s32 a = 13, b = 64, c = 1;
            CHECK_EQUAL(3, math::g_ilog2(a));
            CHECK_EQUAL(6, math::g_ilog2(b));
            CHECK_EQUAL(0, math::g_ilog2(c));
        }

		UNITTEST_TEST(Log2_u32)
        {
            u32 a = 0x1, b = 0x10000, c = 0x80000000;
            CHECK_EQUAL(0, math::g_ilog2(a));
            CHECK_EQUAL(16, math::g_ilog2(b));
            CHECK_EQUAL(31, math::g_ilog2(c));
        }

		UNITTEST_TEST(CountTrailingZeros16)
		{
			u16 a = 13, b = 64, c = 0, d = -1;
			CHECK_EQUAL(0, math::g_countTrailingZeros(a));
			CHECK_EQUAL(6, math::g_countTrailingZeros(b));
			CHECK_EQUAL(16, math::g_countTrailingZeros(c));
			CHECK_EQUAL(0, math::g_countTrailingZeros(d));
		}
		UNITTEST_TEST(CountTrailingZeros32)
		{
			u32 a = 13, b = 64, c = 0, d = -1;
			CHECK_EQUAL(0, math::g_countTrailingZeros(a));
			CHECK_EQUAL(6, math::g_countTrailingZeros(b));
			CHECK_EQUAL(32, math::g_countTrailingZeros(c));
			CHECK_EQUAL(0, math::g_countTrailingZeros(d));
		}
		UNITTEST_TEST(CountTrailingZeros64)
		{
			u64 a = 13, b = 64, c = 0, d = -1, e = 0xffff000000000000UL;
			CHECK_EQUAL( 0, math::g_countTrailingZeros(a));
			CHECK_EQUAL( 6, math::g_countTrailingZeros(b));
			CHECK_EQUAL(64, math::g_countTrailingZeros(c));
			CHECK_EQUAL( 0, math::g_countTrailingZeros(d));
			CHECK_EQUAL(48, math::g_countTrailingZeros(e));
		}

		UNITTEST_TEST(CountLeadingZeros16)
		{
			u16 a = 13, b = 64, c = 0, d = -1;
			CHECK_EQUAL(12, math::g_countLeadingZeros(a));
			CHECK_EQUAL(9, math::g_countLeadingZeros(b));
			CHECK_EQUAL(16, math::g_countLeadingZeros(c));
			CHECK_EQUAL( 0, math::g_countLeadingZeros(d));
		}
		UNITTEST_TEST(CountLeadingZeros32)
		{
			u32 a = 13, b = 64, c = 0, d = -1;
			CHECK_EQUAL(28, math::g_countLeadingZeros(a));
			CHECK_EQUAL(25, math::g_countLeadingZeros(b));
			CHECK_EQUAL(32, math::g_countLeadingZeros(c));
			CHECK_EQUAL( 0, math::g_countLeadingZeros(d));
		}
		UNITTEST_TEST(CountLeadingZeros64)
		{
			u64 a = 13, b = 64, c = 0, d = -1, e = 0x0000ffff00000000UL;
			CHECK_EQUAL(32+28, math::g_countLeadingZeros(a));
			CHECK_EQUAL(32+25, math::g_countLeadingZeros(b));
			CHECK_EQUAL(32+32, math::g_countLeadingZeros(c));
			CHECK_EQUAL(0, math::g_countLeadingZeros(d));
			CHECK_EQUAL(16, math::g_countLeadingZeros(e));
		}

		UNITTEST_TEST(LeastSignificantOneBit)
		{
			u32 a = 12, b = 64, c = 0;
			CHECK_EQUAL(4, math::g_leastSignificantOneBit(a));
			CHECK_EQUAL(64, math::g_leastSignificantOneBit(b));
			CHECK_EQUAL(0, math::g_leastSignificantOneBit(c));
		}

		UNITTEST_TEST(MostSignificantOneBit)
		{
			u32 a = 12, b = 64, c = 0;
			CHECK_EQUAL(8, math::g_mostSignificantOneBit(a));
			CHECK_EQUAL(64, math::g_mostSignificantOneBit(b));
			CHECK_EQUAL(0, math::g_mostSignificantOneBit(c));
		}

		UNITTEST_TEST(LeastSignificantBit)
		{
			u32 a = 12, b = 64, c = 0;
			CHECK_EQUAL(2, math::g_leastSignificantBit(a));
			CHECK_EQUAL(6, math::g_leastSignificantBit(b));
			CHECK_EQUAL(32, math::g_leastSignificantBit(c));
		}

		UNITTEST_TEST(MostSignificantBit)
		{
			u32 a = 12, b = 64, c = 0;
			CHECK_EQUAL(3, math::g_mostSignificantBit(a));
			CHECK_EQUAL(6, math::g_mostSignificantBit(b));
			CHECK_EQUAL(32, math::g_mostSignificantBit(c));
		}

		UNITTEST_TEST(FindFirstBit)
		{
			u32 a = 12, b = 64, c = 0;
			CHECK_EQUAL(2, math::g_findFirstBit(a));
			CHECK_EQUAL(6, math::g_findFirstBit(b));
			CHECK_EQUAL(32, math::g_findFirstBit(c));
		}

		UNITTEST_TEST(FindLastBit_u16)
		{
			u16 a = 12, b = 64, c = 0;
			CHECK_EQUAL(15-3 , math::g_findLastBit(a));
			CHECK_EQUAL(15-6 , math::g_findLastBit(b));
			CHECK_EQUAL(16, math::g_findLastBit(c));
		}

		UNITTEST_TEST(FindLastBit_u32)
		{
			u32 a = 12, b = 64, c = 0;
			CHECK_EQUAL(31-3 , math::g_findLastBit(a));
			CHECK_EQUAL(31-6 , math::g_findLastBit(b));
			CHECK_EQUAL(32, math::g_findLastBit(c));
		}

		UNITTEST_TEST(FindLastBit_u64)
		{
			u64 a = 12, b = 64, c = 0;
			CHECK_EQUAL(63-3 , math::g_findLastBit(a));
			CHECK_EQUAL(63-6 , math::g_findLastBit(b));
			CHECK_EQUAL(64, math::g_findLastBit(c));
		}

		UNITTEST_TEST(Rol32)
		{
			u32 a = 12, b = 64, c = 0 , m = 5;
			CHECK_EQUAL(24, math::g_rol32(a,1));
			CHECK_EQUAL(64, math::g_rol32(b,0));
			CHECK_EQUAL(0, math::g_rol32(c,m));
			CHECK_EQUAL(12, math::g_rol32(a,b));
			CHECK_EQUAL(0x000000FF, math::g_rol32(0xF000000F,4));
		}

		UNITTEST_TEST(Ror32)
		{
			u32 a = 12, b = 64, c = 0 , m = 5;
			CHECK_EQUAL(6, math::g_ror32(a,1));
			CHECK_EQUAL(64, math::g_ror32(b,0));
			CHECK_EQUAL(0, math::g_ror32(c,m));
			CHECK_EQUAL(12, math::g_ror32(a,b));
			CHECK_EQUAL(0xFF000000, math::g_ror32(0xF000000F,4));
		}
	}
}
UNITTEST_SUITE_END

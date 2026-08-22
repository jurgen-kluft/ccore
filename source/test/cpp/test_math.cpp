#include "ccore/c_math.h"
#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(math)
{
	UNITTEST_FIXTURE(main)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		UNITTEST_TEST(Min)
		{
			u8 au8 = 1, bu8 = 2, cu8 = 0;
			CHECK_EQUAL(1, math::min(au8,bu8));
			CHECK_EQUAL(0, math::min(au8,cu8));

			u16 au16 = 1, bu16 = 2, cu16 = 0;
			CHECK_EQUAL(1, math::min(au16,bu16));
			CHECK_EQUAL(0, math::min(au16,cu16));

			u32 au32 = 1, bu32 = 2, cu32 = 0;
			CHECK_EQUAL(au32, math::min(au32,bu32));
			CHECK_EQUAL(cu32, math::min(au32,cu32));

			s32 as32 = 1, bs32 = 2, cs32 = 0;
			CHECK_EQUAL(as32, math::min(as32,bs32));
			CHECK_EQUAL(cs32, math::min(as32,cs32));

			u64 au64 = 1, bu64 = 2, cu64 = 0;
			CHECK_EQUAL(au64, math::min(au64,bu64));
			CHECK_EQUAL(cu64, math::min(au64,cu64));
		}

		UNITTEST_TEST(Max)
		{
			u8 au8 = 1, bu8 = 2, cu8 = 0;
			CHECK_EQUAL(2, math::max(au8,bu8));
			CHECK_EQUAL(1, math::max(au8,cu8));

			u16 au16 = 1, bu16 = 2, cu16 = 0;
			CHECK_EQUAL(2, math::max(au16,bu16));
			CHECK_EQUAL(1, math::max(au16,cu16));

			u32 au32 = 1, bu32 = 2, cu32 = 0;
			CHECK_EQUAL(bu32, math::max(au32,bu32));
			CHECK_EQUAL(au32, math::max(au32,cu32));

			u32 as32 = 1, bs32 = 2, cs32 = 0;
			CHECK_EQUAL(bs32, math::max(as32,bs32));
			CHECK_EQUAL(as32, math::max(as32,cs32));

			u64 au64 = 1, bu64 = 2, cu64 = 0;
			CHECK_EQUAL(bu64, math::max(au64,bu64));
			CHECK_EQUAL(au64, math::max(au64,cu64));
		}

		UNITTEST_TEST(Clamp)
		{
			u8 au8 = 0, bu8 = 1, cu8 = 2;
			CHECK_EQUAL(1, math::clamp(au8,bu8,cu8));
			CHECK_EQUAL(1, math::clamp(bu8,bu8,cu8));

			u16 au16 = 0, bu16 = 1, cu16 = 2;
			CHECK_EQUAL(1, math::clamp(au16,bu16,cu16));
			CHECK_EQUAL(2, math::clamp(cu16,bu16,cu16));

			u32 au32 = 0, bu32 = 1, cu32 = 2;
			CHECK_EQUAL((u32)1, math::clamp(au32,bu32,cu32));
			CHECK_EQUAL((u32)2, math::clamp(au32,cu32,cu32));

			u64 au64 = 0, bu64 = 1, cu64 = 2;
			CHECK_EQUAL((u64)1, math::clamp(au64,bu64,cu64));
			CHECK_EQUAL((u64)1, math::clamp(bu64,au64,cu64));
		}

		UNITTEST_TEST(Align)
		{
			CHECK_EQUAL(16, math::align(17, 16));
			CHECK_EQUAL(256, math::align(300, 256));
			CHECK_EQUAL(1024, math::align(2011, 1024));
		}

		UNITTEST_TEST(AlignDown)
		{
			CHECK_EQUAL(16, math::alignDown(17, 16));
			CHECK_EQUAL(256, math::alignDown(300, 256));
			CHECK_EQUAL(1024, math::alignDown(2011, 1024));
		}

		UNITTEST_TEST(AlignUp)
		{
			CHECK_EQUAL(16, math::alignUp(12, 16));
			CHECK_EQUAL(32, math::alignUp(17, 16));
			CHECK_EQUAL(256, math::alignUp(200, 256));
			CHECK_EQUAL(512, math::alignUp(300, 256));
			CHECK_EQUAL(2048, math::alignUp(2011, 1024));
		}

		UNITTEST_TEST(IsAligned)
		{
			CHECK_TRUE(math::isAligned(16, 16));
			CHECK_TRUE(math::isAligned(256, 256));
			CHECK_TRUE(math::isAligned(1024, 1024));
		}

		UNITTEST_TEST(Abs)
		{
			s32 a = 0, b = 1, c = -2;
			CHECK_EQUAL(0, math::abs(a));
			CHECK_EQUAL(1, math::abs(b));
			CHECK_EQUAL(2, math::abs(c));
		}

		UNITTEST_TEST(Sqr)
		{
			s32 a = 0, b = 1, c = -2;
			CHECK_EQUAL(0, math::sqr(a));
			CHECK_EQUAL(1, math::sqr(b));
			CHECK_EQUAL(4, math::sqr(c));

			u32 au = 0, bu = 1, cu = 2;
			CHECK_EQUAL((u32)0, math::sqr(au));
			CHECK_EQUAL((u32)1, math::sqr(bu));
			CHECK_EQUAL((u32)4, math::sqr(cu));
		}

		UNITTEST_TEST(Mod)
		{
			s32 a = 3, b = 50, c = -2;
			CHECK_EQUAL(3, math::mod(a,b));
			CHECK_EQUAL(0, math::mod(b,c));
			CHECK_EQUAL(2, math::mod(b,a));

			// WII: truncates towards zero

			CHECK_EQUAL(-2, math::mod(c,a));
			c = -22;
			a = 7;
			CHECK_EQUAL(-1, math::mod(c,a));
		}

		UNITTEST_TEST(Gcd)
		{
			s32 a = 12, b = 50, c = -2;
			CHECK_EQUAL(2, math::gcd(a,b));
			CHECK_EQUAL(-2, math::gcd(b,c));
			CHECK_EQUAL(-2, math::gcd(c,a));
		}

		UNITTEST_TEST(Average)
		{
			s32 a = 13, b = 50, c = -200;
			CHECK_TRUE(math::average(a,b) >= 31);
			CHECK_TRUE(math::average(a,b) <= 32);
			CHECK_TRUE(math::average(b,c) == -75);
			CHECK_TRUE(math::average(c,a) <= -93);
			CHECK_TRUE(math::average(c,a) >= -94);
		}

		UNITTEST_TEST(IsPowerOf2)
		{
			s32 a = 13, b = 64;
			CHECK_FALSE(math::ispo2(a));
			CHECK_TRUE(math::ispo2(b));
		}

		UNITTEST_TEST(CeilPower2)
		{
			u32 a = 13, b = 64, c = 1, d = 2, e = 3, f = 63, g = 65, h = 0x7fffffff;
			CHECK_EQUAL((u32)16, math::ceilpo2(a));
			CHECK_EQUAL((u32)64, math::ceilpo2(b));
			CHECK_EQUAL((u32)1, math::ceilpo2(c));
			CHECK_EQUAL((u32)2, math::ceilpo2(d));
			CHECK_EQUAL((u32)4, math::ceilpo2(e));
			CHECK_EQUAL((u32)64, math::ceilpo2(f));
			CHECK_EQUAL((u32)128, math::ceilpo2(g));
			CHECK_EQUAL((u32)0x80000000, math::ceilpo2(h));
		}

		UNITTEST_TEST(FloorPower2)
		{
			u32 a = 13, b = 64;
			CHECK_EQUAL((u32)8, math::floorpo2(a));
			CHECK_EQUAL((u32)64, math::floorpo2(b));
		}

		UNITTEST_TEST(CountBits)
		{
			u32 a = 13, b = 64;
			CHECK_EQUAL(3, math::countBits(a));
			CHECK_EQUAL(1, math::countBits(b));
		}

		UNITTEST_TEST(Log2_s32)
        {
            s32 a = 13, b = 64, c = 1;
            CHECK_EQUAL(3, math::ilog2(a));
            CHECK_EQUAL(6, math::ilog2(b));
            CHECK_EQUAL(0, math::ilog2(c));
        }

		UNITTEST_TEST(Log2_u32)
        {
            u32 a = 0x1, b = 0x10000, c = 0x80000000;
            CHECK_EQUAL(0, math::ilog2(a));
            CHECK_EQUAL(16, math::ilog2(b));
            CHECK_EQUAL(31, math::ilog2(c));
        }

		UNITTEST_TEST(CountTrailingZeros16)
		{
			u16 a = 13, b = 64, c = 0, d = -1;
			CHECK_EQUAL(0, math::countTrailingZeros(a));
			CHECK_EQUAL(6, math::countTrailingZeros(b));
			CHECK_EQUAL(16, math::countTrailingZeros(c));
			CHECK_EQUAL(0, math::countTrailingZeros(d));
		}
		UNITTEST_TEST(CountTrailingZeros32)
		{
			u32 a = 13, b = 64, c = 0, d = -1;
			CHECK_EQUAL(0, math::countTrailingZeros(a));
			CHECK_EQUAL(6, math::countTrailingZeros(b));
			CHECK_EQUAL(32, math::countTrailingZeros(c));
			CHECK_EQUAL(0, math::countTrailingZeros(d));
		}
		UNITTEST_TEST(CountTrailingZeros64)
		{
			u64 a = 13, b = 64, c = 0, d = -1, e = 0xffff000000000000UL;
			CHECK_EQUAL( 0, math::countTrailingZeros(a));
			CHECK_EQUAL( 6, math::countTrailingZeros(b));
			CHECK_EQUAL(64, math::countTrailingZeros(c));
			CHECK_EQUAL( 0, math::countTrailingZeros(d));
			CHECK_EQUAL(48, math::countTrailingZeros(e));
		}

		UNITTEST_TEST(CountLeadingZeros16)
		{
			u16 a = 13, b = 64, c = 0, d = -1;
			CHECK_EQUAL(12, math::countLeadingZeros(a));
			CHECK_EQUAL(9, math::countLeadingZeros(b));
			CHECK_EQUAL(16, math::countLeadingZeros(c));
			CHECK_EQUAL( 0, math::countLeadingZeros(d));
		}
		UNITTEST_TEST(CountLeadingZeros32)
		{
			u32 a = 13, b = 64, c = 0, d = -1;
			CHECK_EQUAL(28, math::countLeadingZeros(a));
			CHECK_EQUAL(25, math::countLeadingZeros(b));
			CHECK_EQUAL(32, math::countLeadingZeros(c));
			CHECK_EQUAL( 0, math::countLeadingZeros(d));
		}
		UNITTEST_TEST(CountLeadingZeros64)
		{
			u64 a = 13, b = 64, c = 0, d = -1, e = 0x0000ffff00000000UL;
			CHECK_EQUAL(32+28, math::countLeadingZeros(a));
			CHECK_EQUAL(32+25, math::countLeadingZeros(b));
			CHECK_EQUAL(32+32, math::countLeadingZeros(c));
			CHECK_EQUAL(0, math::countLeadingZeros(d));
			CHECK_EQUAL(16, math::countLeadingZeros(e));
		}

		UNITTEST_TEST(LeastSignificantOneBit)
		{
			u32 a = 12, b = 64, c = 0;
			CHECK_EQUAL((u32)4, math::leastSignificantOneBit(a));
			CHECK_EQUAL((u32)64, math::leastSignificantOneBit(b));
			CHECK_EQUAL((u32)0, math::leastSignificantOneBit(c));
		}

		UNITTEST_TEST(MostSignificantOneBit)
		{
			u32 a = 12, b = 64, c = 0;
			CHECK_EQUAL((u32)8, math::mostSignificantOneBit(a));
			CHECK_EQUAL((u32)64, math::mostSignificantOneBit(b));
			CHECK_EQUAL((u32)0, math::mostSignificantOneBit(c));
		}

		UNITTEST_TEST(LeastSignificantBit)
		{
			u32 a = 12, b = 64, c = 0;
			CHECK_EQUAL(2, math::leastSignificantBit(a));
			CHECK_EQUAL(6, math::leastSignificantBit(b));
			CHECK_EQUAL(32, math::leastSignificantBit(c));
		}

		UNITTEST_TEST(MostSignificantBit)
		{
			u32 a = 12, b = 64, c = 0;
			CHECK_EQUAL(3, math::mostSignificantBit(a));
			CHECK_EQUAL(6, math::mostSignificantBit(b));
			CHECK_EQUAL(32, math::mostSignificantBit(c));
		}

		UNITTEST_TEST(FindFirstBit)
		{
			u32 a = 12, b = 64, c = 0;
			CHECK_EQUAL(2, math::findFirstBit(a));
			CHECK_EQUAL(6, math::findFirstBit(b));
			CHECK_EQUAL(-1, math::findFirstBit(c));
		}

		UNITTEST_TEST(FindLastBit_u16)
		{
			u16 a = 12, b = 64, c = 0;
			CHECK_EQUAL(3 , math::findLastBit(a));
			CHECK_EQUAL(6 , math::findLastBit(b));
            CHECK_EQUAL(-1, math::findLastBit(c));
		}

		UNITTEST_TEST(FindLastBit_u32)
		{
			u32 a = 12, b = 64, c = 0;
			CHECK_EQUAL(3 , math::findLastBit(a));
			CHECK_EQUAL(6 , math::findLastBit(b));
            CHECK_EQUAL(-1, math::findLastBit(c));
		}

		UNITTEST_TEST(FindLastBit_u64)
		{
			u64 a = 12, b = 64, c = 0;
			CHECK_EQUAL(3 , math::findLastBit(a));
			CHECK_EQUAL(6 , math::findLastBit(b));
            CHECK_EQUAL(-1, math::findLastBit(c));
		}

		UNITTEST_TEST(Rol32)
		{
			u32 a = 12, b = 64, c = 0 , m = 5;
			CHECK_EQUAL((u32)24, math::rol32(a,1));
			CHECK_EQUAL((u32)64, math::rol32(b,0));
			CHECK_EQUAL((u32)0, math::rol32(c,m));
			CHECK_EQUAL((u32)12, math::rol32(a,b));
			CHECK_EQUAL((u32)0x000000FF, math::rol32(0xF000000F,4));
		}

		UNITTEST_TEST(Ror32)
		{
			u32 a = 12, b = 64, c = 0 , m = 5;
			CHECK_EQUAL((u32)6, math::ror32(a,1));
			CHECK_EQUAL((u32)64, math::ror32(b,0));
			CHECK_EQUAL((u32)0, math::ror32(c,m));
			CHECK_EQUAL((u32)12, math::ror32(a,b));
			CHECK_EQUAL((u32)0xFF000000, math::ror32(0xF000000F,4));
		}
	}

	UNITTEST_FIXTURE(ni32)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		UNITTEST_TEST(BasicOperations)
		{
			CHECK_EQUAL((i32)2, math::ni32::abs(-2));
			CHECK_EQUAL((i32)-3, math::ni32::min(-3, 4));
			CHECK_EQUAL((i32)4, math::ni32::max(-3, 4));
			CHECK_EQUAL((i32)0, math::ni32::clamp(-1, 0, 10));
			CHECK_EQUAL((i32)10, math::ni32::clamp(11, 0, 10));
			CHECK_EQUAL((i32)5, math::ni32::average(4, 7));
			CHECK_EQUAL((i32)0x7fffffff, math::ni32::average(0x7fffffff, 0x7fffffff));
			CHECK_EQUAL((i32)49, math::ni32::sqr(-7));
			CHECK_EQUAL((i32)50, math::ni32::map(5, 0, 10, 0, 100));
		}

		UNITTEST_TEST(Sort)
		{
			i32 a = 8, b = -2, c = 4;
			math::ni32::sort(a, b);
			CHECK_EQUAL((i32)-2, a);
			CHECK_EQUAL((i32)8, b);
			math::ni32::sort(a, b, c);
			CHECK_EQUAL((i32)-2, a);
			CHECK_EQUAL((i32)4, b);
			CHECK_EQUAL((i32)8, c);
		}

		UNITTEST_TEST(Interpolation)
		{
			CHECK_EQUAL((i32)0, math::ni32::lerp(0, 100, 0));
			CHECK_EQUAL((i32)50, math::ni32::lerp(0, 100, 128));
			CHECK_EQUAL((i32)100, math::ni32::lerp(0, 100, 256));
			CHECK_EQUAL((i32)360, math::ni32::lerp_angle(350, 10, 128));
			CHECK_EQUAL((i32)0, math::ni32::smoothstep(0, 100, 0));
			CHECK_EQUAL((i32)15, math::ni32::smoothstep(0, 100, 64));
			CHECK_EQUAL((i32)50, math::ni32::smoothstep(0, 100, 128));
			CHECK_EQUAL((i32)100, math::ni32::smoothstep(0, 100, 256));
		}
	}

	UNITTEST_FIXTURE(ni64)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		UNITTEST_TEST(BasicOperations)
		{
			CHECK_EQUAL((i64)2, math::ni64::abs((i64)-2));
			CHECK_EQUAL((i64)-3, math::ni64::min((i64)-3, 4));
			CHECK_EQUAL((i64)4, math::ni64::max((i64)-3, 4));
			CHECK_EQUAL((i64)0, math::ni64::clamp((i64)-1, 0, 10));
			CHECK_EQUAL((i64)10, math::ni64::clamp((i64)11, 0, 10));
			CHECK_EQUAL((i64)5, math::ni64::average((i64)4, 7));
			CHECK_EQUAL((i64)49, math::ni64::sqr((i64)-7));
			CHECK_EQUAL((i64)50, math::ni64::map((i64)5, 0, 10, 0, 100));
		}

		UNITTEST_TEST(Sort)
		{
			i64 a = 8, b = -2, c = 4;
			math::ni64::sort(a, b);
			CHECK_EQUAL((i64)-2, a);
			CHECK_EQUAL((i64)8, b);
			math::ni64::sort(a, b, c);
			CHECK_EQUAL((i64)-2, a);
			CHECK_EQUAL((i64)4, b);
			CHECK_EQUAL((i64)8, c);
		}

		UNITTEST_TEST(Interpolation)
		{
			CHECK_EQUAL((i64)0, math::ni64::lerp(0, 100, 0));
			CHECK_EQUAL((i64)50, math::ni64::lerp(0, 100, 32768));
			CHECK_EQUAL((i64)100, math::ni64::lerp(0, 100, 65536));
			CHECK_EQUAL((i64)360, math::ni64::lerp_angle(350, 10, 32768));
			CHECK_EQUAL((i64)0, math::ni64::smoothstep(0, 100, 0));
			CHECK_EQUAL((i64)15, math::ni64::smoothstep(0, 100, 64, 8));
			CHECK_EQUAL((i64)50, math::ni64::smoothstep(0, 100, 128, 8));
			CHECK_EQUAL((i64)100, math::ni64::smoothstep(0, 100, 256, 8));
		}
	}

	UNITTEST_FIXTURE(nf32)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		const f32 cCheckEpsilon = 0.0001f;

		UNITTEST_TEST(BasicOperations)
		{
			CHECK_CLOSE(2.0f, math::nf32::abs(-2.0f), cCheckEpsilon);
			CHECK_CLOSE(-3.0f, math::nf32::min(-3.0f, 4.0f), cCheckEpsilon);
			CHECK_CLOSE(4.0f, math::nf32::max(-3.0f, 4.0f), cCheckEpsilon);
			CHECK_CLOSE(0.0f, math::nf32::clamp(-1.0f, 0.0f, 10.0f), cCheckEpsilon);
			CHECK_CLOSE(10.0f, math::nf32::clamp(11.0f, 0.0f, 10.0f), cCheckEpsilon);
			CHECK_CLOSE(5.0f, math::nf32::average(4.0f, 6.0f), cCheckEpsilon);
			CHECK_CLOSE(49.0f, math::nf32::sqr(-7.0f), cCheckEpsilon);
			CHECK_CLOSE(50.0f, math::nf32::map(5.0f, 0.0f, 10.0f, 0.0f, 100.0f), cCheckEpsilon);
		}

		UNITTEST_TEST(SortAndRounding)
		{
			f32 a = 8.0f, b = -2.0f, c = 4.0f;
			math::nf32::sort(a, b);
			CHECK_CLOSE(-2.0f, a, cCheckEpsilon);
			CHECK_CLOSE(8.0f, b, cCheckEpsilon);
			math::nf32::sort(a, b, c);
			CHECK_CLOSE(-2.0f, a, cCheckEpsilon);
			CHECK_CLOSE(4.0f, b, cCheckEpsilon);
			CHECK_CLOSE(8.0f, c, cCheckEpsilon);
			CHECK_CLOSE(-2.0f, math::nf32::floor(-1.5f), cCheckEpsilon);
			CHECK_CLOSE(2.0f, math::nf32::ceil(1.5f), cCheckEpsilon);
			CHECK_CLOSE(2.0f, math::nf32::round(1.5f), cCheckEpsilon);
		}

		UNITTEST_TEST(Interpolation)
		{
			CHECK_CLOSE(2.5f, math::nf32::lerp(0.0f, 10.0f, 0.25f), cCheckEpsilon);
			CHECK_CLOSE(0.15625f, math::nf32::smooth_step(0.0f, 1.0f, 0.25f), cCheckEpsilon);
			CHECK_CLOSE(1.0f, math::nf32::smooth_step(0.0f, 1.0f, 2.0f), cCheckEpsilon);
			CHECK_CLOSE(1.0f, math::nf32::slerp(1.0f, 1.0f, 0.5f), cCheckEpsilon);
		}

		UNITTEST_TEST(Functions)
		{
			CHECK_CLOSE(0.0f, math::nf32::sin(0.0f), cCheckEpsilon);
			CHECK_CLOSE(1.0f, math::nf32::cos(0.0f), cCheckEpsilon);
			CHECK_CLOSE(1.0f, math::nf32::tan(math::PI * 0.25f), cCheckEpsilon);
			CHECK_CLOSE(0.7853982f, math::nf32::atan(1.0f), cCheckEpsilon);
			CHECK_CLOSE(0.5235988f, math::nf32::asin(0.5f), cCheckEpsilon);
			CHECK_CLOSE(0.0f, math::nf32::atan2(0.0f, 1.0f), cCheckEpsilon);
			CHECK_CLOSE(0.0f, math::nf32::tanh(0.0f), cCheckEpsilon);
			CHECK_CLOSE(1.0f, math::nf32::exp(0.0f), cCheckEpsilon);
			CHECK_CLOSE(2.0f, math::nf32::sqrt(4.0f), cCheckEpsilon);
			CHECK_CLOSE(8.0f, math::nf32::pow(2.0f, 3.0f), cCheckEpsilon);
			CHECK_CLOSE(1.0f, math::nf32::log(math::nf32::exp(1.0f)), cCheckEpsilon);
			CHECK_CLOSE(2.0f, math::nf32::log10(100.0f), cCheckEpsilon);
			CHECK_CLOSE(1.0f, math::nf32::fmod(5.0f, 2.0f), cCheckEpsilon);
			CHECK_CLOSE(6.0f, math::nf32::ldexp(0.75f, 3), cCheckEpsilon);
			CHECK_CLOSE(2.0f, math::nf32::fabs(-2.0f), cCheckEpsilon);
		}

		UNITTEST_TEST(Decompose)
		{
			i32 exponent = 0;
			f32 integer = 0.0f;
			CHECK_CLOSE(0.75f, math::nf32::frexp(6.0f, &exponent), cCheckEpsilon);
			CHECK_EQUAL(3, exponent);
			CHECK_CLOSE(0.25f, math::nf32::modf(2.25f, &integer), cCheckEpsilon);
			CHECK_CLOSE(2.0f, integer, cCheckEpsilon);
		}
	}

	UNITTEST_FIXTURE(nf64)
	{
		UNITTEST_FIXTURE_SETUP() {}
		UNITTEST_FIXTURE_TEARDOWN() {}

		const f64 cCheckEpsilon = 0.0000001;

		UNITTEST_TEST(BasicOperations)
		{
			CHECK_CLOSE(2.0, math::nf64::abs(-2.0), cCheckEpsilon);
			CHECK_CLOSE(-3.0, math::nf64::min(-3.0, 4.0), cCheckEpsilon);
			CHECK_CLOSE(4.0, math::nf64::max(-3.0, 4.0), cCheckEpsilon);
			CHECK_CLOSE(0.0, math::nf64::clamp(-1.0, 0.0, 10.0), cCheckEpsilon);
			CHECK_CLOSE(10.0, math::nf64::clamp(11.0, 0.0, 10.0), cCheckEpsilon);
			CHECK_CLOSE(5.0, math::nf64::average(4.0, 6.0), cCheckEpsilon);
			CHECK_CLOSE(49.0, math::nf64::sqr(-7.0), cCheckEpsilon);
			CHECK_CLOSE(50.0, math::nf64::map(5.0, 0.0, 10.0, 0.0, 100.0), cCheckEpsilon);
		}

		UNITTEST_TEST(SortAndRounding)
		{
			f64 a = 8.0, b = -2.0, c = 4.0;
			math::nf64::sort(a, b);
			CHECK_CLOSE(-2.0, a, cCheckEpsilon);
			CHECK_CLOSE(8.0, b, cCheckEpsilon);
			math::nf64::sort(a, b, c);
			CHECK_CLOSE(-2.0, a, cCheckEpsilon);
			CHECK_CLOSE(4.0, b, cCheckEpsilon);
			CHECK_CLOSE(8.0, c, cCheckEpsilon);
			CHECK_CLOSE(-2.0, math::nf64::floor(-1.5), cCheckEpsilon);
			CHECK_CLOSE(2.0, math::nf64::ceil(1.5), cCheckEpsilon);
			CHECK_CLOSE(2.0, math::nf64::round(1.5), cCheckEpsilon);
		}

		UNITTEST_TEST(Interpolation)
		{
			CHECK_CLOSE(2.5, math::nf64::lerp(0.0, 10.0, 0.25), cCheckEpsilon);
			CHECK_CLOSE(0.15625, math::nf64::smooth_step(0.0, 1.0, 0.25), cCheckEpsilon);
			CHECK_CLOSE(1.0, math::nf64::smooth_step(0.0, 1.0, 1.0), cCheckEpsilon);
			CHECK_CLOSE(1.0, math::nf64::slerp(1.0, 1.0, 0.5), cCheckEpsilon);
		}

		UNITTEST_TEST(Functions)
		{
			CHECK_CLOSE(0.0, math::nf64::sin(0.0), cCheckEpsilon);
			CHECK_CLOSE(1.0, math::nf64::cos(0.0), cCheckEpsilon);
			CHECK_CLOSE(1.0, math::nf64::tan(math::PI * 0.25), cCheckEpsilon);
			CHECK_CLOSE(0.7853981634, math::nf64::atan(1.0), cCheckEpsilon);
			CHECK_CLOSE(0.5235987756, math::nf64::asin(0.5), cCheckEpsilon);
			CHECK_CLOSE(0.0, math::nf64::atan2(0.0, 1.0), cCheckEpsilon);
			CHECK_CLOSE(0.0, math::nf64::tanh(0.0), cCheckEpsilon);
			CHECK_CLOSE(1.0, math::nf64::exp(0.0), cCheckEpsilon);
			CHECK_CLOSE(2.0, math::nf64::sqrt(4.0), cCheckEpsilon);
			CHECK_CLOSE(8.0, math::nf64::pow(2.0, 3.0), cCheckEpsilon);
			CHECK_CLOSE(1.0, math::nf64::log(math::nf64::exp(1.0)), cCheckEpsilon);
			CHECK_CLOSE(2.0, math::nf64::log10(100.0), cCheckEpsilon);
			CHECK_CLOSE(1.0, math::nf64::fmod(5.0, 2.0), cCheckEpsilon);
			CHECK_CLOSE(6.0, math::nf64::ldexp(0.75, 3), cCheckEpsilon);
			CHECK_CLOSE(2.0, math::nf64::fabs(-2.0), cCheckEpsilon);
		}

		UNITTEST_TEST(Decompose)
		{
			i32 exponent = 0;
			f64 integer = 0.0;
			CHECK_CLOSE(0.75, math::nf64::frexp(6.0, &exponent), cCheckEpsilon);
			CHECK_EQUAL(3, exponent);
			CHECK_CLOSE(0.25, math::nf64::modf(2.25, &integer), cCheckEpsilon);
			CHECK_CLOSE(2.0, integer, cCheckEpsilon);
		}
	}
}
UNITTEST_SUITE_END

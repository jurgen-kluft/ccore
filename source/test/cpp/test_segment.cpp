#include "ccore/c_target.h"
#include "ccore/c_segment.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(segment)
{
    UNITTEST_FIXTURE(operations)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(basic_alloc_and_free) {}
    }
}
UNITTEST_SUITE_END

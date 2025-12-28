#include "ccore/c_target.h"
#include "ccore/c_bin.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(bin)
{
    UNITTEST_FIXTURE(init)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        UNITTEST_TEST(init)
        {
            nbin::bin_t *bin = nbin::make_bin(64, 65536);


            nbin::destroy(bin);
        }
    }
}
UNITTEST_SUITE_END

#include "ccore/c_target.h"
#include "ccore/c_error.h"
#include "ccore/c_callback.h"

#include "cunittest/cunittest.h"

using namespace ncore;

UNITTEST_SUITE_BEGIN(error)
{
    UNITTEST_FIXTURE(handler)
    {
        UNITTEST_FIXTURE_SETUP() {}
        UNITTEST_FIXTURE_TEARDOWN() {}

        enum TestErrors
        {
            cTestError1 = 0,
            cTestError2 = 1,
            cTestError3 = 2,
            cTestError4 = 3,
            cTestError5 = 4,
            cTestError6 = 5,
            cTestError7 = 6,
            cTestError8 = 7,
            cTestError9 = 8,
            cTestErrorCount,
        };

        static s64         gTestErrorBase      = 0;
        static const char* gTestErrorStrings[] = {
          "Error 1", "Error 2", "Error 3", "Error 4", "Error 5", "Error 6", "Error 7", "Error 8", "Error 9",
        };

        static const char* test_error_to_string(error_t error)
        {
            error = (error - gTestErrorBase);
            if (error >= cTestError1 && error < cTestErrorCount)
            {
                return gTestErrorStrings[error];
            }
            return "Unknown Test Error";
        }

        UNITTEST_TEST(register_error_handler)
        {
            gTestErrorBase = nerror::insert_handler(test_error_to_string, cTestErrorCount);
            CHECK(gTestErrorBase < -nerror::cCoreNumErrors);

            for (s64 i = 0; i < cTestErrorCount; ++i)
            {
                const char* error_str  = nerror::to_string(gTestErrorBase + i);
                CHECK_EQUAL(gTestErrorStrings[i], error_str);
            }
        }
    }
}
UNITTEST_SUITE_END

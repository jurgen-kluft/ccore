#include "ccore/c_target.h"
#include "ccore/c_allocator.h"

#include "cunittest/cunittest.h"

// include the system header for malloc/free
#include <stdlib.h>

UNITTEST_SUITE_LIST

namespace ncore
{
    // Our own assert handler
    class UnitTestAssertHandler : public ncore::asserthandler_t
    {
    public:
        UnitTestAssertHandler() { NumberOfAsserts = 0; }

        virtual bool handle_assert(u32& flags, const char* fileName, s32 lineNumber, const char* exprString, const char* messageString)
        {
            UnitTest::ReportAssert(exprString, fileName, lineNumber);
            NumberOfAsserts++;
            return false;
        }

        ncore::s32 NumberOfAsserts;
    };

    class UnitTestAllocator : public UnitTest::TestAllocator
    {
    public:
        int mNumAllocations;

        UnitTestAllocator()
            : mNumAllocations(0)
        {
        }

        virtual void* Allocate(unsigned int size, unsigned int alignment)
        {
            mNumAllocations++;
            void* ptr = malloc(size);
            return ptr;
        }
        virtual unsigned int Deallocate(void* ptr, int* status)
        {
            --mNumAllocations;
            free(ptr);
            return 0;
        }
    };

    class TestAllocator : public alloc_t
    {
        UnitTest::TestAllocator* mAllocator;

    public:
        TestAllocator(UnitTestAllocator* allocator)
            : mAllocator(allocator)
        {
        }

        virtual void* v_allocate(u32 size, u32 alignment) { return mAllocator->Allocate(size, alignment); }
        virtual u32   v_deallocate(void* mem) { return mAllocator->Deallocate(mem); }
        virtual void  v_release() {}
    };
} // namespace ncore

bool gRunUnitTest(UnitTest::TestReporter& reporter, UnitTest::TestContext& context)
{

#ifdef TARGET_DEBUG
    ncore::UnitTestAssertHandler assertHandler;
    ncore::gSetAssertHandler(&assertHandler);
#endif

    ncore::UnitTestAllocator unittestAllocator;
    context.mAllocator = &unittestAllocator;

    ncore::TestAllocator testAllocator(&unittestAllocator);

    int r = UNITTEST_SUITE_RUN(context, reporter, cUnitTest);
    if (unittestAllocator.mNumAllocations != 0)
    {
        reporter.reportFailure(__FILE__, __LINE__, "cunittest", "memory leaks detected!");
        r = -1;
    }

    ncore::gSetAssertHandler(nullptr);
    return r == 0;
}

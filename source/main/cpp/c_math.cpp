#include "ccore/c_memory.h"

#ifdef TARGET_MAC
#    include <math.h>
#endif

#ifdef TARGET_PC
#    include <math.h>
#endif

namespace ncore
{
    namespace math
    {
        f32 atanf(f32 v) { return ::atanf(v); }
        f32 cosf(f32 v) { return ::cosf(v); }
        f32 sinf(f32 v) { return ::sinf(v); }
        f32 tanf(f32 v) { return ::tanf(v); }
        f32 tanhf(f32 v) { return ::tanhf(v); }
        f32 frexpf(f32 v, i32* p) { return ::frexpf(v, p); }
        f32 modff(f32 v, f32* p) { return ::modff(v, p); }
        f32 ceilf(f32 v) { return ::ceilf(v); }
        f32 fabsf(f32 v) { return ::fabsf(v); }
        f32 floorf(f32 v) { return ::floorf(v); }

        f32 acosf(f32 v) { return ::acosf(v); }
        f32 asinf(f32 v) { return ::asinf(v); }
        f32 atan2f(f32 v, f32 w) { return ::atan2f(v, w); }
        f32 coshf(f32 v) { return ::coshf(v); }
        f32 sinhf(f32 v) { return ::sinhf(v); }
        f32 expf(f32 v) { return ::expf(v); }
        f32 ldexpf(f32 v, i32 w) { return ::ldexpf(v, w); }
        f32 logf(f32 v) { return ::logf(v); }
        f32 log10f(f32 v) { return ::log10f(v); }
        f32 powf(f32 v, f32 w) { return ::powf(v, w); }
        f32 sqrtf(f32 v) { return ::sqrtf(v); }
        f32 fmodf(f32 v, f32 w) { return ::fmodf(v, w); }
    }  // namespace math
}  // namespace ncore

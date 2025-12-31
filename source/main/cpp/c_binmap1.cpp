#include "ccore/c_math.h"

#include "ccore/c_binmap1.h"

namespace ncore
{
    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

#define D_INVERT(T, value) ((T) ~(value))

    // binmaps with a single level

    namespace nbinmap6
    {
        typedef u64 bin0type;

        static constexpr bin0type bin0constant = (bin0type) ~(bin0type)0;

        void set(bin0type* bin0, u32 maxbits, u32 bit) { *bin0 |= ((bin0type)1 << bit); }
        void clr(bin0type* bin0, u32 maxbits, u32 bit) { *bin0 &= D_INVERT(bin0type, (bin0type)1 << bit); }
        bool get(bin0type const* bin0, u32 maxbits, u32 bit) { return (*bin0 & ((bin0type)1 << bit)) != 0; }

        s32 find(bin0type const* bin0, u32 maxbits)
        {
            if (*bin0 == bin0constant)
                return -1;
            s32 const bit = math::findFirstBit(D_INVERT(bin0type, *bin0));
            return bit < (s32)maxbits ? bit : -1;
        }

        s32 find_and_set(bin0type* bin0, u32 maxbits)
        {
            if (*bin0 == bin0constant)
                return -1;
            s32 const bit = math::findFirstBit(D_INVERT(bin0type, *bin0));
            *bin0 |= ((bin0type)1 << bit);
            return bit < (s32)maxbits ? bit : -1;
        }
    }  // namespace nbinmap6

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    // binmaps with two levels

    template <typename bin0type, typename bin1type, u32 bin1shift>
    class binmap_bin0_bin1_t
    {
    public:
        template <typename T>
        static constexpr inline T invert(T value)
        {
            return (T)~value;
        }

        static constexpr u32 bin0bits = sizeof(bin0type) * 8;
        static constexpr u32 bin1bits = sizeof(bin1type) * 8;

        static constexpr bin0type bin0constant = (bin0type) ~(bin0type)0;
        static constexpr bin1type bin1constant = (bin1type) ~(bin1type)0;

        static void setup_free_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits) { *_bin0 = bin0constant; }
        static void tick_free_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin1
                {
                    const u32 bi1      = wi & (bin1bits - 1);
                    wi                 = wi >> bin1shift;
                    const bin1type wd1 = (bi1 == 0) ? bin1constant : _bin1[wi];
                    _bin1[wi]          = wd1 & invert((bin1type)1 << bi1);

                    // bin0
                    if (wd1 == bin1constant)
                    {
                        const u32      bi0 = wi & (bin0bits - 1);
                        const bin0type wd0 = (bi0 == 0) ? bin0constant : *_bin0;
                        *_bin0             = wd0 & invert((bin0type)1 << bi0);
                    }
                }
            }
        }

        static void setup_used_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits) { *_bin0 = bin0constant; }
        static void tick_used_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;
                // bin1
                {
                    const u32 bi1      = wi & (bin1bits - 1);
                    wi                 = wi >> bin1shift;
                    const bin1type wd1 = (bi1 == 0) ? bin1constant : (_bin1[wi] | invert((bin1type)1 << bi1));
                    _bin1[wi]          = wd1;

                    // bin0
                    if (wd1 == bin1constant)
                    {
                        const u32      bi0 = wi & (bin0bits - 1);
                        const bin0type wd0 = (bi0 == 0) ? bin0constant : (*_bin0 | invert((bin0type)1 << bi0));
                        *_bin0             = wd0;
                    }
                }
            }
        }

        static void clear(bin0type* _bin0, bin1type* _bin1, u32 maxbits)
        {
            u32 const size = (maxbits + (bin1bits - 1)) >> bin1shift;
            for (u32 i = 0; i < size; ++i)
                _bin1[i] = 0;
            *_bin0 = 0;
        }

        static void set(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const      i1 = bit >> bin1shift;
            u32 const      b1 = bit & (bin1bits - 1);
            bin1type const vo = _bin1[i1];                 // old
            bin1type const vn = vo | ((bin1type)1 << b1);  // new
            _bin1[i1]         = vn;
            if (vo != bin1constant && vn == bin1constant)
            {
                *_bin0 |= ((bin0type)1 << i1);
            }
        }

        static void clr(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const      i1 = bit >> bin1shift;
            u32 const      b1 = bit & (bin1bits - 1);
            bin1type const vo = _bin1[i1];  // old
            _bin1[i1]         = vo & invert((bin1type)1 << b1);
            if (vo == bin1constant)
                *_bin0 &= invert((bin0type)1 << i1);
        }

        static bool get(bin0type const* _bin0, bin1type const* _bin1, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i1 = bit >> bin1shift;
            u32 const b1 = bit & (bin1bits - 1);
            return (_bin1[i1] & ((bin1type)1 << b1)) != 0;
        }

        static s32 find(bin0type const* _bin0, bin1type const* _bin1, u32 maxbits)
        {
            if (*_bin0 == bin0constant)
                return -1;
            s32 const      b0  = math::findFirstBit(invert(*_bin0));
            bin1type const w1  = invert(_bin1[b0]);
            s32 const      bit = math::findFirstBit(w1) + (b0 << bin1shift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_and_set(bin0type* _bin0, bin1type* _bin1, u32 maxbits)
        {
            if (*_bin0 == bin0constant)
                return -1;

            s32 const i0 = math::findFirstBit(invert(*_bin0));
            ASSERT(_bin1[i0] != bin1constant);
            const s32      b1 = math::findFirstBit(invert(_bin1[i0]));
            bin1type const v1 = _bin1[i0] | ((bin1type)1 << b1);
            _bin1[i0]         = v1;
            if (v1 == bin1constant)
                *_bin0 |= ((bin0type)1 << i0);

            const s32 bit = b1 + (i0 << bin1shift);
            return bit < (s32)maxbits ? bit : -1;
        }
    };

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    namespace nbinmap10
    {
        constexpr u32 bin1shift = 5;

        void setup_free_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::setup_free_lazy(_bin0, _bin1, maxbits); }
        void tick_free_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::tick_free_lazy(_bin0, _bin1, maxbits, bit); }
        void setup_used_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::setup_used_lazy(_bin0, _bin1, maxbits); }
        void tick_used_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::tick_used_lazy(_bin0, _bin1, maxbits, bit); }
        void clear(bin0type* _bin0, bin1type* _bin1, u32 maxbits) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::clear(_bin0, _bin1, maxbits); }
        void set(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::set(_bin0, _bin1, maxbits, bit); }
        void clr(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::clr(_bin0, _bin1, maxbits, bit); }
        bool get(bin0type const* _bin0, bin1type const* _bin1, u32 maxbits, u32 bit) { return binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::get(_bin0, _bin1, maxbits, bit); }
        s32  find(bin0type const* _bin0, bin1type const* _bin1, u32 maxbits) { return binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::find(_bin0, _bin1, maxbits); }
        s32  find_and_set(bin0type* _bin0, bin1type* _bin1, u32 maxbits) { return binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::find_and_set(_bin0, _bin1, maxbits); }
    }  // namespace nbinmap10

    namespace nbinmap12
    {
        constexpr u32 bin1shift = 6;

        void setup_free_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::setup_free_lazy(_bin0, _bin1, maxbits); }
        void tick_free_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::tick_free_lazy(_bin0, _bin1, maxbits, bit); }
        void setup_used_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::setup_used_lazy(_bin0, _bin1, maxbits); }
        void tick_used_lazy(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::tick_used_lazy(_bin0, _bin1, maxbits, bit); }
        void clear(bin0type* _bin0, bin1type* _bin1, u32 maxbits) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::clear(_bin0, _bin1, maxbits); }
        void set(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::set(_bin0, _bin1, maxbits, bit); }
        void clr(bin0type* _bin0, bin1type* _bin1, u32 maxbits, u32 bit) { binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::clr(_bin0, _bin1, maxbits, bit); }
        bool get(bin0type const* _bin0, bin1type const* _bin1, u32 maxbits, u32 bit) { return binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::get(_bin0, _bin1, maxbits, bit); }
        s32  find(bin0type const* _bin0, bin1type const* _bin1, u32 maxbits) { return binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::find(_bin0, _bin1, maxbits); }
        s32  find_and_set(bin0type* _bin0, bin1type* _bin1, u32 maxbits) { return binmap_bin0_bin1_t<bin0type, bin1type, bin1shift>::find_and_set(_bin0, _bin1, maxbits); }
    }  // namespace nbinmap12

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // Below are the binmaps that have three levels.
    template <typename bin0type, typename bin1type, typename bin2type, u32 bin1shift, u32 bin2shift>
    class binmap_bin0_bin1_bin2_t
    {
    public:
        template <typename T>
        static constexpr inline T invert(T value)
        {
            return (T)~value;
        }

        static constexpr u32 bin0bits = sizeof(bin0type) * 8;
        static constexpr u32 bin1bits = sizeof(bin1type) * 8;
        static constexpr u32 bin2bits = sizeof(bin1type) * 8;

        static constexpr bin0type bin0constant = (bin0type) ~(bin0type)0;
        static constexpr bin1type bin1constant = (bin1type) ~(bin1type)0;
        static constexpr bin2type bin2constant = (bin2type) ~(bin2type)0;

        static void setup_free_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits) { *_bin0 = bin0constant; }
        static void tick_free_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;
                // bin2
                {
                    const u32 bi2      = wi & (bin2bits - 1);
                    wi                 = wi >> bin2shift;
                    const bin2type wd2 = (bi2 == 0) ? bin2constant : _bin2[wi];
                    _bin2[wi]          = wd2 & D_INVERT(bin2type, (bin2type)1 << bi2);

                    // bin1
                    if (wd2 == bin2constant)
                    {
                        const u32 bi1      = wi & (bin1bits - 1);
                        wi                 = wi >> bin1shift;
                        const bin1type wd1 = (bi1 == 0) ? bin1constant : _bin1[wi];
                        _bin1[wi]          = wd1 & D_INVERT(bin1type, (bin1type)1 << bi1);

                        // bin0
                        if (wd1 == bin1constant)
                        {
                            const u32      bi0 = wi & (bin0bits - 1);
                            const bin0type wd0 = (bi0 == 0) ? bin0constant : *_bin0;
                            *_bin0             = wd0 & D_INVERT(bin0type, (bin0type)1 << bi0);
                        }
                    }
                }
            }
        }

        static void setup_used_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits) { *_bin0 = bin0constant; }
        static void tick_used_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;
                // bin2
                {
                    const u32 bi2      = wi & (bin2bits - 1);
                    wi                 = wi >> bin2shift;
                    const bin2type wd2 = (bi2 == 0) ? bin2constant : (_bin2[wi] | D_INVERT(bin2type, (bin2type)1 << bi2));
                    _bin2[wi]          = wd2;

                    // bin1
                    if (wd2 == bin2constant)
                    {
                        const u32 bi1      = wi & (bin1bits - 1);
                        wi                 = wi >> bin1shift;
                        const bin1type wd1 = (bi1 == 0) ? bin1constant : (_bin1[wi] | D_INVERT(bin1type, (bin1type)1 << bi1));
                        _bin1[wi]          = wd1;

                        // bin0
                        if (wd1 == bin1constant)
                        {
                            const u32      bi0 = wi & (bin0bits - 1);
                            const bin0type wd0 = (bi0 == 0) ? bin0constant : (*_bin0 | D_INVERT(bin0type, (bin0type)1 << bi0));
                            *_bin0             = wd0;
                        }
                    }
                }
            }
        }

        static void clear(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits)
        {
            u32 wi = (maxbits + (bin2bits - 1)) >> bin2shift;
            for (u32 i = 0; i < wi; ++i)
                _bin2[i] = 0;
            wi = (wi + (bin1bits - 1)) >> bin1shift;
            for (u32 i = 0; i < wi; ++i)
                _bin1[i] = 0;
            *_bin0 = 0;
        }

        static void set(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const      i2 = bit >> bin2shift;
            u32 const      b2 = bit & (bin2bits - 1);
            bin2type const v2 = _bin2[i2] | ((bin2type)1 << b2);
            _bin2[i2]         = v2;
            if (v2 == bin2constant)
            {
                u32 const      i1 = i2 >> bin1shift;
                u32 const      b1 = i2 & (bin1bits - 1);
                bin1type const v1 = _bin1[i1] | ((bin1type)1 << b1);
                _bin1[i1]         = v1;
                if (v1 == bin1constant)
                {
                    u32 const      b0 = i1 & (bin0bits - 1);
                    bin0type const v0 = *_bin0;
                    *_bin0            = v0 | ((bin0type)1 << b0);
                }
            }
        }

        static void clr(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i2 = bit >> bin2shift;
            u32 const b2 = bit & (bin2bits - 1);
            u64 const v2 = _bin2[i2];
            _bin2[i2]    = v2 & D_INVERT(bin2type, (bin2type)1 << b2);
            if (v2 == bin2constant)
            {
                u32 const      i1 = i2 >> bin1shift;
                u32 const      b1 = i2 & (bin1bits - 1);
                bin1type const v1 = _bin1[i1];
                _bin1[i1]         = v1 & D_INVERT(bin1type, (bin1type)1 << b1);
                if (v1 == bin1constant)
                {
                    u32 const      b0 = i1 & (bin0bits - 1);
                    bin0type const v0 = *_bin0;
                    *_bin0            = v0 & D_INVERT(bin0type, (bin0type)1 << b0);
                }
            }
        }

        static bool get(bin0type const* _bin0, bin1type const* _bin1, bin2type const* _bin2, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> bin2shift;
            u32 const b = bit & (bin2bits - 1);
            return (_bin2[i] & ((bin2type)1 << b)) != 0;
        }

        static s32 find(bin0type const* _bin0, bin1type const* _bin1, bin2type const* _bin2, u32 maxbits)
        {
            if (*_bin0 == bin0constant)
                return -1;

            s32 const b0 = math::findFirstBit(D_INVERT(bin0type, *_bin0));

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(D_INVERT(bin1type, _bin1[w1]));

            s32 const w2 = b1 + (w1 << bin1shift);
            s32 const b2 = math::findFirstBit(D_INVERT(bin2type, _bin2[w2]));

            s32 const bit = b2 + (w2 << bin2shift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_and_set(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits)
        {
            if (*_bin0 == bin0constant)
                return -1;

            s32 const b0 = math::findFirstBit(D_INVERT(bin0type, *_bin0));

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(D_INVERT(bin1type, _bin1[w1]));

            s32 const w2 = b1 + (w1 << bin1shift);
            s32 const b2 = math::findFirstBit(D_INVERT(bin2type, _bin2[w2]));

            s32 const bit = b2 + (w2 << bin2shift);

            // set bit
            {
                // bin2
                {
                    _bin2[w2] = _bin2[w2] | ((bin2type)1 << b2);
                    if (_bin2[w2] != bin2constant)
                        return bit < (s32)maxbits ? bit : -1;
                }
                // bin1
                {
                    _bin1[w1] = _bin1[w1] | ((bin1type)1 << b1);
                    if (_bin1[w1] != bin1constant)
                        return bit < (s32)maxbits ? bit : -1;
                }
                // bin0
                {
                    *_bin0 = *_bin0 | ((bin0type)1 << b0);
                }
            }

            return bit < (s32)maxbits ? bit : -1;
        }
    };

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------
    namespace nbinmap15
    {
        // constexpr u32 bin0shift = 5;
        constexpr u32 bin1shift = 5;
        constexpr u32 bin2shift = 5;

        void setup_free_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::setup_free_lazy(_bin0, _bin1, _bin2, maxbits); }
        void tick_free_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::tick_free_lazy(_bin0, _bin1, _bin2, maxbits, bit); }
        void setup_used_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::setup_used_lazy(_bin0, _bin1, _bin2, maxbits); }
        void tick_used_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::tick_used_lazy(_bin0, _bin1, _bin2, maxbits, bit); }
        void clear(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::clear(_bin0, _bin1, _bin2, maxbits); }
        void set(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::set(_bin0, _bin1, _bin2, maxbits, bit); }
        void clr(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::clr(_bin0, _bin1, _bin2, maxbits, bit); }
        bool get(bin0type const* _bin0, bin1type const* _bin1, bin2type const* _bin2, u32 maxbits, u32 bit) { return binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::get(_bin0, _bin1, _bin2, maxbits, bit); }
        s32  find(bin0type const* _bin0, bin1type const* _bin1, bin2type const* _bin2, u32 maxbits) { return binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::find(_bin0, _bin1, _bin2, maxbits); }
        s32  find_and_set(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits) { return binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::find_and_set(_bin0, _bin1, _bin2, maxbits); }
    }  // namespace nbinmap15

    namespace nbinmap18
    {
        // constexpr u32 bin0shift = 6;
        constexpr u32 bin1shift = 6;
        constexpr u32 bin2shift = 6;

        void setup_free_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::setup_free_lazy(_bin0, _bin1, _bin2, maxbits); }
        void tick_free_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::tick_free_lazy(_bin0, _bin1, _bin2, maxbits, bit); }
        void setup_used_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::setup_used_lazy(_bin0, _bin1, _bin2, maxbits); }
        void tick_used_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::tick_used_lazy(_bin0, _bin1, _bin2, maxbits, bit); }
        void clear(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::clear(_bin0, _bin1, _bin2, maxbits); }
        void set(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::set(_bin0, _bin1, _bin2, maxbits, bit); }
        void clr(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::clr(_bin0, _bin1, _bin2, maxbits, bit); }
        bool get(bin0type const* _bin0, bin1type const* _bin1, bin2type const* _bin2, u32 maxbits, u32 bit) { return binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::get(_bin0, _bin1, _bin2, maxbits, bit); }
        s32  find(bin0type const* _bin0, bin1type const* _bin1, bin2type const* _bin2, u32 maxbits) { return binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::find(_bin0, _bin1, _bin2, maxbits); }
        s32  find_and_set(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, u32 maxbits) { return binmap_bin0_bin1_bin2_t<bin0type, bin1type, bin2type, bin1shift, bin2shift>::find_and_set(_bin0, _bin1, _bin2, maxbits); }
    }  // namespace nbinmap18

    // --------------------------------------------------------------------------------
    // --------------------------------------------------------------------------------

    // Below are the binmaps that have three levels.
    template <typename bin0type, typename bin1type, typename bin2type, typename bin3type, u32 bin1shift, u32 bin2shift, u32 bin3shift>
    class binmap_bin0_bin1_bin2_bin3_t
    {
    public:
        template <typename T>
        static constexpr inline T invert(T value)
        {
            return (T)~value;
        }

        static constexpr u32 bin0bits = sizeof(bin0type) * 8;
        static constexpr u32 bin1bits = sizeof(bin1type) * 8;
        static constexpr u32 bin2bits = sizeof(bin1type) * 8;
        static constexpr u32 bin3bits = sizeof(bin2type) * 8;

        static constexpr bin0type bin0constant = (bin0type) ~(bin0type)0;
        static constexpr bin1type bin1constant = (bin1type) ~(bin1type)0;
        static constexpr bin2type bin2constant = (bin2type) ~(bin2type)0;
        static constexpr bin3type bin3constant = (bin3type) ~(bin3type)0;

        static void setup_free_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, bin3type* _bin3, u32 maxbits) { *_bin0 = bin0constant; }
        static void tick_free_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, bin3type* _bin3, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin3
                {
                    const u32 bi3      = wi & (bin3bits - 1);
                    wi                 = wi >> bin3shift;
                    const bin3type wd3 = (bi3 == 0) ? bin3constant : _bin3[wi];
                    _bin3[wi]          = wd3 & D_INVERT(bin3type, (bin3type)1 << bi3);

                    // bin2
                    if (wd3 == bin3constant)
                    {
                        const u32 bi2      = wi & (bin2bits - 1);
                        wi                 = wi >> bin2shift;
                        const bin2type wd2 = (bi2 == 0) ? bin2constant : _bin2[wi];
                        _bin2[wi]          = wd2 & D_INVERT(bin2type, (bin2type)1 << bi2);

                        // bin1
                        if (wd2 == bin2constant)
                        {
                            const u32 bi1      = wi & (bin1bits - 1);
                            wi                 = wi >> bin1shift;
                            const bin1type wd1 = (bi1 == 0) ? bin1constant : _bin1[wi];
                            _bin1[wi]          = wd1 & D_INVERT(bin1type, (bin1type)1 << bi1);

                            // bin0
                            if (wd1 == bin1constant)
                            {
                                const u32      bi0 = wi & (bin0bits - 1);
                                const bin0type wd0 = (bi0 == 0) ? bin0constant : *_bin0;
                                *_bin0             = wd0 & D_INVERT(bin0type, (bin0type)1 << bi0);
                            }
                        }
                    }
                }
            }
        }

        static void setup_used_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, bin3type* _bin3, u32 maxbits) { *_bin0 = bin0constant; }
        static void tick_used_lazy(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, bin3type* _bin3, u32 maxbits, u32 bit)
        {
            if (bit < maxbits)
            {
                u32 wi = bit;

                // bin3
                {
                    const u32 bi3      = wi & (bin3bits - 1);
                    wi                 = wi >> bin3shift;
                    const bin3type wd3 = (bi3 == 0) ? bin3constant : (_bin3[wi] | D_INVERT(bin3type, (bin3type)1 << bi3));
                    _bin3[wi]          = wd3;

                    // bin2
                    if (wd3 == bin3constant)
                    {
                        const u32 bi2      = wi & (bin2bits - 1);
                        wi                 = wi >> bin2shift;
                        const bin2type wd2 = (bi2 == 0) ? bin2constant : (_bin2[wi] | D_INVERT(bin2type, (bin2type)1 << bi2));
                        _bin2[wi]          = wd2;

                        // bin1
                        if (wd2 == bin2constant)
                        {
                            const u32 bi1      = wi & (bin1bits - 1);
                            wi                 = wi >> bin1shift;
                            const bin1type wd1 = (bi1 == 0) ? bin1constant : (_bin1[wi] | D_INVERT(bin1type, (bin1type)1 << bi1));
                            _bin1[wi]          = wd1;

                            // bin0
                            if (wd1 == bin1constant)
                            {
                                const u32      bi0 = wi & (bin0bits - 1);
                                const bin0type wd0 = (bi0 == 0) ? bin0constant : (*_bin0 | D_INVERT(bin0type, (bin0type)1 << bi0));
                                *_bin0             = wd0;
                            }
                        }
                    }
                }
            }
        }

        static void clear(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, bin3type* _bin3, u32 maxbits)
        {
            u32 wi = (maxbits + (bin3bits - 1)) >> bin3shift;
            for (u32 i = 0; i < wi; ++i)
                _bin3[i] = 0;
            wi = (wi + (bin2bits - 1)) >> bin2shift;
            for (u32 i = 0; i < wi; ++i)
                _bin2[i] = 0;
            wi = (wi + (bin1bits - 1)) >> bin1shift;
            for (u32 i = 0; i < wi; ++i)
                _bin1[i] = 0;
            *_bin0 = 0;
        }

        static void set(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, bin3type* _bin3, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const      i3 = bit >> bin3shift;
            u32 const      b3 = bit & (bin3bits - 1);
            bin3type const v3 = _bin3[i3] | ((bin3type)1 << b3);
            _bin3[i3]         = v3;
            if (v3 == bin3constant)
            {
                u32 const      i2 = i3 >> bin2shift;
                u32 const      b2 = i3 & (bin2bits - 1);
                bin2type const v2 = _bin2[i2] | ((bin2type)1 << b2);
                _bin2[i2]         = v2;
                if (v2 == bin2constant)
                {
                    u32 const      i1 = i2 >> bin1shift;
                    u32 const      b1 = i2 & (bin1bits - 1);
                    bin1type const v1 = _bin1[i1] | ((bin1type)1 << b1);
                    _bin1[i1]         = v1;
                    if (v1 == bin1constant)
                    {
                        u32 const      b0 = i1 & (bin0bits - 1);
                        bin0type const v0 = *_bin0 | ((bin0type)1 << b0);
                        *_bin0            = v0;
                    }
                }
            }
        }

        static void clr(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, bin3type* _bin3, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i3 = bit >> bin3shift;
            u32 const b3 = bit & (bin3bits - 1);
            u64 const v3 = _bin3[i3];
            _bin3[i3]    = v3 & D_INVERT(bin3type, (bin3type)1 << b3);
            if (v3 == bin3constant)
            {
                u32 const i2 = i3 >> bin2shift;
                u32 const b2 = i3 & (bin2bits - 1);
                u64 const v2 = _bin2[i2];
                _bin2[i2]    = v2 & D_INVERT(bin2type, (bin2type)1 << b2);
                if (v2 == bin2constant)
                {
                    u32 const i1 = i2 >> bin1shift;
                    u32 const b1 = i2 & (bin1bits - 1);
                    u64 const v1 = _bin1[i1];
                    _bin1[i1]    = v1 & D_INVERT(bin1type, (bin1type)1 << b1);
                    if (v1 == bin1constant)
                    {
                        u32 const      b0 = i1 & (bin0bits - 1);
                        bin0type const v0 = *_bin0;
                        *_bin0            = v0 & D_INVERT(bin0type, (bin0type)1 << b0);
                    }
                }
            }
        }

        static bool get(bin0type const* _bin0, bin1type const* _bin1, bin2type const* _bin2, bin3type const* _bin3, u32 maxbits, u32 bit)
        {
            ASSERT(bit < maxbits);
            u32 const i = bit >> bin3shift;
            u32 const b = bit & (bin3bits - 1);
            return (_bin3[i] & ((bin3type)1 << b)) != 0;
        }

        static s32 find(bin0type const* _bin0, bin1type const* _bin1, bin2type const* _bin2, bin3type const* _bin3, u32 maxbits)
        {
            if (*_bin0 == bin0constant)
                return -1;

            s32 const b0 = math::findFirstBit(D_INVERT(bin0type, *_bin0));

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(D_INVERT(bin1type, _bin1[w1]));

            s32 const w2 = b1 + (w1 << bin1shift);
            s32 const b2 = math::findFirstBit(D_INVERT(bin2type, _bin2[w2]));

            s32 const w3 = b2 + (w2 << bin2shift);
            s32 const b3 = math::findFirstBit(D_INVERT(bin3type, _bin3[w3]));

            s32 const bit = b3 + (w3 << bin3shift);
            return bit < (s32)maxbits ? bit : -1;
        }

        static s32 find_and_set(bin0type* _bin0, bin1type* _bin1, bin2type* _bin2, bin3type* _bin3, u32 maxbits)
        {
            if (*_bin0 == bin0constant)
                return -1;

            s32 const b0 = math::findFirstBit(D_INVERT(bin0type, *_bin0));

            s32 const w1 = b0;
            s32 const b1 = math::findFirstBit(D_INVERT(bin1type, _bin1[w1]));

            s32 const w2 = b1 + (w1 << bin1shift);
            s32 const b2 = math::findFirstBit(D_INVERT(bin2type, _bin2[w2]));

            s32 const w3 = b2 + (w2 << bin2shift);
            s32 const b3 = math::findFirstBit(D_INVERT(bin3type, _bin3[w3]));

            s32 const bit = b3 + (w3 << bin3shift);

            // set bit
            {
                // bin3
                {
                    _bin3[w3] = _bin3[w3] | ((bin3type)1 << b3);
                    if (_bin3[w3] != bin3constant)
                        return bit < (s32)maxbits ? bit : -1;
                }
                // bin2
                {
                    _bin2[w2] = _bin2[w2] | ((bin2type)1 << b2);
                    if (_bin2[w2] != bin2constant)
                        return bit < (s32)maxbits ? bit : -1;
                }
                // bin1
                {
                    _bin1[w1] = _bin1[w1] | ((bin1type)1 << b1);
                    if (_bin1[w1] != bin1constant)
                        return bit < (s32)maxbits ? bit : -1;
                }
                // bin0
                {
                    *_bin0 = *_bin0 | ((bin0type)1 << b0);
                }
            }

            return bit < (s32)maxbits ? bit : -1;
        }
    };

    namespace nbinmap20
    {
        void setup_free_lazy(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 5, 5, 5>::setup_free_lazy(bin0, bin1, bin2, bin3, maxbits); }
        void tick_free_lazy(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 5, 5, 5>::tick_free_lazy(bin0, bin1, bin2, bin3, maxbits, bit); }
        void setup_used_lazy(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 5, 5, 5>::setup_used_lazy(bin0, bin1, bin2, bin3, maxbits); }
        void tick_used_lazy(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 5, 5, 5>::tick_used_lazy(bin0, bin1, bin2, bin3, maxbits, bit); }

        void clear(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 5, 5, 5>::clear(bin0, bin1, bin2, bin3, maxbits); }

        void set(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 5, 5, 5>::set(bin0, bin1, bin2, bin3, maxbits, bit); }
        void clr(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 5, 5, 5>::clr(bin0, bin1, bin2, bin3, maxbits, bit); }
        bool get(bin0type const* bin0, bin1type const* bin1, bin2type const* bin2, bin3type const* bin3, u32 maxbits, u32 bit)
        {
            return binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 5, 5, 5>::get(bin0, bin1, bin2, bin3, maxbits, bit);
        }
        s32 find(bin0type const* bin0, bin1type const* bin1, bin2type const* bin2, bin3type const* bin3, u32 maxbits) { return binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 5, 5, 5>::find(bin0, bin1, bin2, bin3, maxbits); }
        s32 find_and_set(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits) { return binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 5, 5, 5>::find_and_set(bin0, bin1, bin2, bin3, maxbits); }
    }  // namespace nbinmap20

    namespace nbinmap24
    {
        void setup_free_lazy(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 6, 6, 6>::setup_free_lazy(bin0, bin1, bin2, bin3, maxbits); }
        void tick_free_lazy(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 6, 6, 6>::tick_free_lazy(bin0, bin1, bin2, bin3, maxbits, bit); }
        void setup_used_lazy(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 6, 6, 6>::setup_used_lazy(bin0, bin1, bin2, bin3, maxbits); }
        void tick_used_lazy(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 6, 6, 6>::tick_used_lazy(bin0, bin1, bin2, bin3, maxbits, bit); }

        void clear(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 6, 6, 6>::clear(bin0, bin1, bin2, bin3, maxbits); }

        void set(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 6, 6, 6>::set(bin0, bin1, bin2, bin3, maxbits, bit); }
        void clr(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits, u32 bit) { binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 6, 6, 6>::clr(bin0, bin1, bin2, bin3, maxbits, bit); }
        bool get(bin0type const* bin0, bin1type const* bin1, bin2type const* bin2, bin3type const* bin3, u32 maxbits, u32 bit)
        {
            return binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 6, 6, 6>::get(bin0, bin1, bin2, bin3, maxbits, bit);
        }
        s32 find(bin0type const* bin0, bin1type const* bin1, bin2type const* bin2, bin3type const* bin3, u32 maxbits) { return binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 6, 6, 6>::find(bin0, bin1, bin2, bin3, maxbits); }
        s32 find_and_set(bin0type* bin0, bin1type* bin1, bin2type* bin2, bin3type* bin3, u32 maxbits) { return binmap_bin0_bin1_bin2_bin3_t<bin0type, bin1type, bin2type, bin3type, 6, 6, 6>::find_and_set(bin0, bin1, bin2, bin3, maxbits); }
    }  // namespace nbinmap24

    namespace nbinmap
    {
        // Note: maximum count is 16 Million (6 bits + 6 bits + 6 bits + 6 bits = 24 bits = 16 M)
        void compute(u32 number_of_bits, layout64_t& layout)
        {
            ASSERT(number_of_bits > 0 && number_of_bits <= 16 * 1024 * 1024);

            layout.m_maxbits = number_of_bits;
            layout.m_bin0    = 0;  // u64[N], N for level 1
            layout.m_bin1    = 0;  // u64[N], N for level 2
            layout.m_bin2    = 0;  // u64[N], N for level 3
            layout.m_bin3    = 0;  // u64[N], N for level 4

            u32 len = number_of_bits;
            // We can have a maximum of 4 levels, each level holds 6 bits (u64)
            layout.m_levels = (u16)(math::mostSignificantBit(len - 1) / 6);
            switch (layout.m_levels)
            {
                case 3: layout.m_bin3 = (len = 63) >> 6; len = (len + 63) >> 6;  // fall through
                case 2: layout.m_bin2 = (len = 63) >> 6; len = (len + 63) >> 6;  // fall through
                case 1: layout.m_bin1 = (len = 63) >> 6; len = (len + 63) >> 6;  // fall through
            }
            layout.m_bin0 = len;
        }

        void pointers(byte* ptr, layout64_t const& l, u64*& bin0, u64*& bin1, u64*& bin2, u64*& bin3)
        {
            bin0 = (u64*)ptr;
            bin1 = (u64*)(ptr + sizeof(u64));
            bin2 = bin1 + l.m_bin1;
            bin3 = bin2 + l.m_bin2;
        }

        u32 sizeof_data(layout64_t const& l) { return l.m_bin0 + l.m_bin1 + l.m_bin2 + l.m_bin3; }

        u32 sizeof_data(layout64_t const& l, u32 bit)
        {
            u32 size = l.m_bin0;
            switch (l.m_levels)
            {
                case 3: size += l.m_bin2;
                case 2: size += l.m_bin1;
                case 1: break;
            }
            size += (bit + 63) >> 6;
            return size;
        }

        // Note: maximum count is 1 Million (5 bits + 5 bits + 5 bits + 5 bits = 20 bits = 1 M)
        void compute(u32 number_of_bits, layout32_t& layout)
        {
            ASSERT(number_of_bits > 0 && number_of_bits <= 1 * 1024 * 1024);

            layout.m_maxbits = number_of_bits;
            layout.m_bin0    = 0;  // u32[N], N for level 1
            layout.m_bin1    = 0;  // u32[N], N for level 2
            layout.m_bin2    = 0;  // u32[N], N for level 3
            layout.m_bin3    = 0;  // u32[N], N for level 4

            u32 len = number_of_bits;
            // We can have a maximum of 4 levels, each level holds 5 bits (u32)
            layout.m_levels = (u16)(math::mostSignificantBit(len - 1) / 5);
            switch (layout.m_levels)
            {
                case 3: layout.m_bin3 = (len = 31) >> 5; len = (len + 31) >> 5;  // fall through
                case 2: layout.m_bin2 = (len = 31) >> 5; len = (len + 31) >> 5;  // fall through
                case 1: layout.m_bin1 = (len = 31) >> 5; len = (len + 31) >> 5;  // fall through
            }
            layout.m_bin0 = len;
        }

        void pointers(byte* ptr, layout32_t const& l, u32*& bin0, u32*& bin1, u32*& bin2, u32*& bin3)
        {
            bin0 = (u32*)ptr;
            bin1 = (u32*)(ptr + sizeof(u32));
            bin2 = bin1 + l.m_bin1;
            bin3 = bin2 + l.m_bin2;
        }

        u32 sizeof_data(layout32_t const& l) { return l.m_bin0 + l.m_bin1 + l.m_bin2 + l.m_bin3; }

        u32 sizeof_data(layout32_t const& l, u32 bit)
        {
            u32 size = l.m_bin0;
            switch (l.m_levels)
            {
                case 3: size += l.m_bin2;
                case 2: size += l.m_bin1;
                case 1: break;
            }
            size += (bit + 31) >> 5;
            return size;
        }

    }  // namespace nbinmap

};  // namespace ncore

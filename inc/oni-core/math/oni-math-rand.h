#include <oni-core/common/oni-common-typedef.h>

// TODO: drop this and remove the template function that uses it
#include <type_traits>


namespace oni {
    class Rand {
    public:
        Rand(u64 seed_a,
             u64 seed_b);

        template<class T>
        T
        next(T lowerBoundInclusive,
             T upperBoundExclusive) {
            static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>);
            assert(upperBoundExclusive > lowerBoundInclusive);
            auto n = next_u64();
            auto d = upperBoundExclusive - lowerBoundInclusive;
            // TODO: To avoid modulo operator see: https://arxiv.org/pdf/1805.10941.pdf
            // I think it will be: auto result = lowerBoundInclusive + ((n * d) >> 64); 
            // Although I would need a 128 bit uint to hold the intermediate results. 
            auto result = lowerBoundInclusive + (n % d);
            return result;
        }

        u32
        next_u32();

        u64
        next_u64();

        u8
        next_u8();

        u16
        next_u16();

        r32
        next_r32();

        r64
        next_r64();

        r32
        next_r32(r32 lowerBoundInclusive,
                 r32 upperBoundExclusive);

        r64
        next_r64(r64 lowerBoundInclusive,
                 r64 upperBoundExclusive);

        r32
        next_norm(r32 mean,
                  r32 stddev);

        r64
        next_norm(r64 mean,
                  r64 stddev);

    private:
        static u32
        rotl_u32(u32 x,
                 i32 k);

        static u64
        rotl_64(u64 x,
                u32 k);

    private:
        union _r32 {
            u32 _int{};
            r32 _float;
        };

        union _r64 {
            u64 _int{};
            r64 _float;
        };

    private:
        u64 mSeed_u64[2]{};
        u64 mSeed_u32[4]{};
        r64 mZ1_r64{0};
        r32 mZ1_r32{0};
        bool mGenerate_r32{false};
        bool mGenerate_r64{false};
    };
}

#include <oni-core/common/oni-common-typedef.h>


namespace oni {
    namespace math {
        class Rand {
        public:
            Rand(common::u64 seed_a,
                 common::u64 seed_b);

            template<class T>
            T
            next(T lowerBoundInclusive,
                 T upperBoundExclusive) {
                static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>);
                assert(upperBoundExclusive > lowerBoundInclusive);
                auto n = next_u64();
                auto d = upperBoundExclusive - lowerBoundInclusive;
                auto result = lowerBoundInclusive + (n % d);
                return result;
            }

            common::u32
            next_u32();

            common::u64
            next_u64();

            common::u8
            next_u8();

            common::u16
            next_u16();

            common::r32
            next_r32(common::r32 lowerBoundInclusive,
                     common::r32 upperBoundExclusive);

            common::r64
            next_r64(common::r64 lowerBoundInclusive,
                     common::r64 upperBoundExclusive);

            common::r32
            next_norm(common::r32 mean,
                      common::r32 stddev);

            common::r64
            next_norm(common::r64 mean,
                      common::r64 stddev);

        private:
            static common::u32
            rotl_u32(common::u32 x,
                     common::i32 k);

            static common::u64
            rotl_64(common::u64 x,
                    common::u32 k);

            common::r32
            _next_r32();

            common::r64
            _next_r64();

        private:
            union _r32 {
                common::u32 _int{};
                common::r32 _float;
            };

            union _r64 {
                common::u64 _int{};
                common::r64 _float;
            };

        private:
            common::u64 mSeed_u64[2]{};
            common::u64 mSeed_u32[4]{};
            common::r64 mZ1_r64{0};
            common::r32 mZ1_r32{0};
            bool mGenerate_r32{false};
            bool mGenerate_r64{false};
        };
    }
}
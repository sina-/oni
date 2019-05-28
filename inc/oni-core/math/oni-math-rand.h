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
                auto n = _next();
                auto d = upperBoundExclusive - lowerBoundInclusive;
                auto result = lowerBoundInclusive + (n % d);
                return result;
            }

            template<class T>
            T
            next() {
                static_assert(std::is_integral_v<T> && std::is_unsigned_v<T>);
                auto n = _next();
                auto d = std::numeric_limits<T>::max();
                auto result = n % d;
                return result;
            }

            common::r32
            next(common::r32 lowerBoundInclusive,
                 common::r32 upperBoundExclusive);

            common::r64
            next(common::r64 lowerBoundInclusive,
                 common::r64 upperBoundExclusive);

            common::r32
            next_norm(common::r32 mean,
                      common::r32 stddev);

            common::r64
            next_norm(common::r64 mean,
                      common::r64 stddev);

        private:
            common::u64
            rotl(common::u64 x,
                 common::u32 k);

            common::u64
            _next();

            common::r32
            _next_r32();

            common::r64
            _next_r64();

        private:
            common::u64 mSeed[2]{};
            common::r64 mZ1_r64{0};
            common::r32 mZ1_r32{0};
            bool mGenerate_r32{false};
            bool mGenerate_r64{false};
        };
    }
}
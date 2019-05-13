#include <random>

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace math {
        class Rand {
        public:
            Rand(common::u32 seed);

            common::i8
            nextInt8(common::i8 lowerBound,
                     common::i8 upperBound);

            common::u8
            nextUint8(common::u8 lowerBound,
                      common::u8 upperBound);

            common::i16
            nextInt16(common::i16 lowerBound,
                      common::i16 upperBound);

            common::u16
            nextUint16(common::u16 lowerBound,
                       common::u16 upperBound);

            common::i32
            nextInt32(common::i32 lowerBound,
                      common::i32 upperBound);

            common::u32
            nextUint32(common::u32 lowerBound,
                       common::u32 upperBound);

            common::r32
            nextReal32(common::r32 lowerBound,
                       common::r32 upperBound);

            common::r64
            nextReal64Normal(common::r64 mean,
                             common::r64 stddev);

        private:
            std::mt19937 mEngine{};
        };
    }
}
#include <random>

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace math {
        class Rand {
        public:
            Rand(common::uint32 seed);

            common::int8
            nextInt8(common::int8 lowerBound,
                     common::int8 upperBound);

            common::uint8
            nextUint8(common::uint8 lowerBound,
                      common::uint8 upperBound);

            common::int16
            nextInt16(common::int16 lowerBound,
                      common::int16 upperBound);

            common::uint16
            nextUint16(common::uint16 lowerBound,
                       common::uint16 upperBound);

            common::int32
            nextInt32(common::int32 lowerBound,
                      common::int32 upperBound);

            common::uint32
            nextUint32(common::uint32 lowerBound,
                       common::uint32 upperBound);

            common::real32
            nextReal32(common::real32 lowerBound,
                       common::real32 upperBound);

        private:
            std::mt19937 mEngine{};
        };
    }
}
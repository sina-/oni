#include <oni-core/math/rand.h>

namespace oni {
    namespace math {
        Rand::Rand(common::uint32 seed) {
            if (seed) {
                mEngine = std::mt19937(seed);
            } else {
                std::random_device rd;
                mEngine = std::mt19937(rd());
            }
        }

        common::int8
        Rand::nextInt8(common::int8 lowerBound,
                       common::int8 upperBound) {
            // TODO: This is probably not that fast! Benchmark it.
            std::uniform_int_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::uint8
        Rand::nextUint8(common::uint8 lowerBound,
                        common::uint8 upperBound) {
            std::uniform_int_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::int16
        Rand::nextInt16(common::int16 lowerBound,
                        common::int16 upperBound) {
            std::uniform_int_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::uint16
        Rand::nextUint16(common::uint16 lowerBound,
                         common::uint16 upperBound) {
            std::uniform_int_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::int32
        Rand::nextInt32(common::int32 lowerBound,
                        common::int32 upperBound) {
            std::uniform_int_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::uint32
        Rand::nextUint32(common::uint32 lowerBound,
                         common::uint32 upperBound) {
            std::uniform_int_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::real32
        Rand::nextReal32(common::real32 lowerBound,
                         common::real32 upperBound) {
            std::uniform_real_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::real64
        Rand::nextReal64Normal(common::real64 mean,
                               common::real64 stddev) {
            std::normal_distribution distribution(mean, stddev);
            return distribution(mEngine);
        }
    }
}

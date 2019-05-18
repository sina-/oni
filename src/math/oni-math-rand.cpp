#include <oni-core/math/oni-math-rand.h>

namespace oni {
    namespace math {
        Rand::Rand(common::u32 seed) {
            if (seed) {
                mEngine = std::mt19937(seed);
            } else {
                std::random_device rd;
                mEngine = std::mt19937(rd());
            }
        }

        common::i8
        Rand::next_i8(common::i8 lowerBound,
                      common::i8 upperBound) {
            // TODO: This is probably not that fast! Benchmark it.
            std::uniform_int_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::u8
        Rand::next_u8(common::u8 lowerBound,
                      common::u8 upperBound) {
            std::uniform_int_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::i16
        Rand::next_i16(common::i16 lowerBound,
                       common::i16 upperBound) {
            std::uniform_int_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::u16
        Rand::next_u16(common::u16 lowerBound,
                       common::u16 upperBound) {
            std::uniform_int_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::i32
        Rand::next_i32(common::i32 lowerBound,
                       common::i32 upperBound) {
            std::uniform_int_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::u32
        Rand::next_u32(common::u32 lowerBound,
                       common::u32 upperBound) {
            std::uniform_int_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::r32
        Rand::next_r32(common::r32 lowerBound,
                       common::r32 upperBound) {
            std::uniform_real_distribution distribution(lowerBound, upperBound);
            return distribution(mEngine);
        }

        common::r64
        Rand::nextNormal_r64(common::r64 mean,
                             common::r64 stddev) {
            std::normal_distribution distribution(mean, stddev);
            return distribution(mEngine);
        }
    }
}

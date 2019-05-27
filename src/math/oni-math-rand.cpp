#include <math.h>

#include <oni-core/math/oni-math-rand.h>

#include <cassert>
#include <random>

#include <oni-core/common/oni-common-const.h>


namespace oni {
    namespace math {
        Rand::Rand(common::u64 seed_a,
                   common::u64 seed_b) {
            if (seed_a && seed_b) {
                mSeed[0] = seed_a;
                mSeed[1] = seed_b;
            } else {
                std::random_device rd;
                mSeed[0] = rd();
                mSeed[1] = rd();
            }
        }

        common::r32
        Rand::next_r32(common::r32 lowerBoundInclusive,
                       common::r32 upperBoundExclusive) {
            assert(upperBoundExclusive > lowerBoundInclusive);
            auto d = upperBoundExclusive - lowerBoundInclusive;
            auto result = next<common::u32>() * ( 1.f / common::u32Max);
            result *= d;
            return lowerBoundInclusive + result;
        }

        common::r64
        Rand::next_r64(common::r64 lowerBoundInclusive,
                       common::r64 upperBoundExclusive) {
            assert(upperBoundExclusive > lowerBoundInclusive);
            auto d = upperBoundExclusive - lowerBoundInclusive;
            auto result = next<common::u64>() * (1. / common::u64Max);
            result *= d;
            return lowerBoundInclusive + result;
        }

        common::r32
        Rand::next_r32_norm(common::r32 mean,
                            common::r32 stddev) {
            // NOTE: Taken from https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
            mGenerate_r32 = !mGenerate_r32;
            if (!mGenerate_r32) {
                return mZ1_r32 * stddev + mean;
            }

            common::r32 u1;
            common::r32 u2;
            do {
                u1 = next<common::u32>() * (1.f / common::u32Max);
                u2 = next<common::u32>() * (1.f / common::u32Max);
            } while (u1 <= common::EP32);

            common::r32 z0 = std::sqrt(-2.0 * log(u1)) * std::cos(common::TWO_PI * u2);
            mZ1_r32 = std::sqrt(-2.0 * std::log(u1)) * std::sin(common::TWO_PI * u2);
            return z0 * stddev + mean;
        }

        common::r64
        Rand::next_r64_norm(common::r64 mean,
                            common::r64 stddev) {
            // NOTE: Taken from https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
            mGenerate_r64 = !mGenerate_r64;
            if (!mGenerate_r64) {
                return mZ1_r64 * stddev + mean;
            }

            common::r64 u1;
            common::r64 u2;
            do {
                u1 = next<common::u64>() * (1. / common::u64Max);
                u2 = next<common::u64>() * (1. / common::u64Max);
            } while (u1 <= common::EP64);

            common::r64 z0 = std::sqrt(-2.0 * std::log(u1)) * std::cos(common::TWO_PI * u2);
            mZ1_r64 = std::sqrt(-2.0 * std::log(u1)) * std::sin(common::TWO_PI * u2);
            return z0 * stddev + mean;
        }
    }
}

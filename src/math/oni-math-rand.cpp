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

        common::u64
        Rand::rotl(common::u64 x,
                   common::u32 k) {
            return (x << k) | (x >> (64 - k));
        }

        common::u64
        Rand::_next() {
            // NOTE: Taken from http://xoshiro.di.unimi.it/xoshiro128plus.c
            const common::u64 s0 = mSeed[0];
            common::u64 s1 = mSeed[1];
            const common::u64 result = s0 + s1;

            s1 ^= s0;
            mSeed[0] = rotl(s0, 24) ^ s1 ^ (s1 << 16); // a, b
            mSeed[1] = rotl(s1, 37); // c

            return result;
        }

        common::r32
        Rand::_next_r32() {
            common::r32 result;
            do {
                result = next<common::u32>() * (1.f / common::u32Max);
            } while (result <= common::EP32);

            return result;
        }

        common::r64
        Rand::_next_r64() {
            common::r64 result;
            do {
                result = next<common::u64>() * (1.f / common::u64Max);
            } while (result <= common::EP64);

            return result;
        }

        common::r32
        Rand::next(common::r32 lowerBoundInclusive,
                   common::r32 upperBoundExclusive) {
            assert(upperBoundExclusive > lowerBoundInclusive);
            auto d = upperBoundExclusive - lowerBoundInclusive;
            auto result = _next_r32();
            result *= d;
            return lowerBoundInclusive + result;
        }

        common::r64
        Rand::next(common::r64 lowerBoundInclusive,
                   common::r64 upperBoundExclusive) {
            assert(upperBoundExclusive > lowerBoundInclusive);
            auto d = upperBoundExclusive - lowerBoundInclusive;
            auto result = _next_r64();
            result *= d;
            return lowerBoundInclusive + result;
        }

        common::r32
        Rand::next_norm(common::r32 mean,
                        common::r32 stddev) {
            // NOTE: Taken from https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
            mGenerate_r32 = !mGenerate_r32;
            if (!mGenerate_r32) {
                return mZ1_r32 * stddev + mean;
            }

            common::r32 u1 = _next_r32();
            common::r32 u2 = _next_r32();

            common::r32 z0 = std::sqrt(-2.0 * log(u1)) * std::cos(common::TWO_PI * u2);
            mZ1_r32 = std::sqrt(-2.0 * std::log(u1)) * std::sin(common::TWO_PI * u2);
            return z0 * stddev + mean;
        }

        common::r64
        Rand::next_norm(common::r64 mean,
                        common::r64 stddev) {
            // NOTE: Taken from https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
            mGenerate_r64 = !mGenerate_r64;
            if (!mGenerate_r64) {
                return mZ1_r64 * stddev + mean;
            }

            common::r64 u1 = _next_r64();
            common::r64 u2 = _next_r64();

            common::r64 z0 = std::sqrt(-2.0 * std::log(u1)) * std::cos(common::TWO_PI * u2);
            mZ1_r64 = std::sqrt(-2.0 * std::log(u1)) * std::sin(common::TWO_PI * u2);
            return z0 * stddev + mean;
        }
    }
}

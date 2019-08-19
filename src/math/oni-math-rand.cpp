#include <oni-core/math/oni-math-rand.h>

#include <cassert>
#include <random>

#include <oni-core/common/oni-common-const.h>


namespace oni {
    Rand::Rand(u64 seed_a,
               u64 seed_b) {
        if (seed_a && seed_b) {
            mSeed_u64[0] = seed_a;
            mSeed_u64[1] = seed_b;
        } else {
            std::random_device rd;
            mSeed_u64[0] = rd();
            mSeed_u64[1] = rd();

            mSeed_u32[0] = rd();
            mSeed_u32[1] = rd();
            mSeed_u32[2] = rd();
            mSeed_u32[3] = rd();
        }
    }

    u32
    Rand::rotl_u32(u32 x,
                   i32 k) {
        return (x << k) | (x >> (32 - k));
    }

    u64
    Rand::rotl_64(u64 x,
                  u32 k) {
        return (x << k) | (x >> (64 - k));
    }

    u32
    Rand::next_u32() {
        // NOTE: Taken from http://xoshiro.di.unimi.it/xoshiro128plus.c
        const uint32_t result = mSeed_u32[0] + mSeed_u32[3];
        const uint32_t t = mSeed_u32[1] << 9u;

        mSeed_u32[2] ^= mSeed_u32[0];
        mSeed_u32[3] ^= mSeed_u32[1];
        mSeed_u32[1] ^= mSeed_u32[2];
        mSeed_u32[0] ^= mSeed_u32[3];

        mSeed_u32[2] ^= t;

        mSeed_u32[3] = rotl_u32(mSeed_u32[3], 11);

        return result;
    }

    u64
    Rand::next_u64() {
        // NOTE: Taken from http://xoshiro.di.unimi.it/xoroshiro128plus.c
        const u64 s0 = mSeed_u64[0];
        u64 s1 = mSeed_u64[1];
        const u64 result = s0 + s1;

        s1 ^= s0;
        mSeed_u64[0] = rotl_64(s0, 24) ^ s1 ^ (s1 << 16u); // a, b
        mSeed_u64[1] = rotl_64(s1, 37); // c

        return result;
    }

    u8
    Rand::next_u8() {
        u8 result = next_u32() >> 24u;
        return result;
    }

    u16
    Rand::next_u16() {
        u16 result = next_u32() >> 16u;
        return result;
    }

    r32
    Rand::next_r32() {
        u32 exponent = 128 - 1;
        exponent <<= 23u;

        auto mantissa = next_u32() >> 9u;
        auto result = _r32{};

        // NOTE: This is using 23 upper bits of 32 random bits to fill the mantissa
        // but it is doing so with exponent of 127, which is the float range of 1.0 to 2.0, and then
        // subtracting 1 from the result to get a random float between 0.0 and 1.0, BUT, there are
        // more floats between 0.0 and 1.0 than between 1.0 and 2.0, so some floats can never
        // be generated with this method. But this method is fast, it does not use any
        // float division. Check https://www.h-schmidt.net/FloatConverter/IEEE754.html to see how floats
        // change between 1.0 to 2.0 compared to 0.0 to 1.0
        // NOTE: This is also an interesting article that inspired the implementation:
        // https://randomascii.wordpress.com/2012/01/11/tricks-with-the-floating-point-format/
        result._int = exponent | mantissa;
        return result._float - 1;
    }

    r64
    Rand::next_r64() {
        u64 exponent = 1024 - 1;
        exponent <<= 52u;

        auto mantissa = next_u64() >> 12u;
        auto result = _r64{};

        result._int = exponent | mantissa;
        return result._float - 1;
    }

    r32
    Rand::next_r32(r32 lowerBoundInclusive,
                   r32 upperBoundExclusive) {
        assert(upperBoundExclusive > lowerBoundInclusive);
        auto d = upperBoundExclusive - lowerBoundInclusive;
        auto result = next_r32();
        result *= d;
        return lowerBoundInclusive + result;
    }

    r64
    Rand::next_r64(r64 lowerBoundInclusive,
                   r64 upperBoundExclusive) {
        assert(upperBoundExclusive > lowerBoundInclusive);
        auto d = upperBoundExclusive - lowerBoundInclusive;
        auto result = next_r64();
        result *= d;
        return lowerBoundInclusive + result;
    }

    r32
    Rand::next_norm(r32 mean,
                    r32 stddev) {
        // NOTE: Taken from https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
        mGenerate_r32 = !mGenerate_r32;
        if (!mGenerate_r32) {
            return mZ1_r32 * stddev + mean;
        }

        r32 u1 = next_r32();
        r32 u2 = next_r32();

        r32 z0 = std::sqrt(-2.0 * log(u1)) * std::cos(TWO_PI * u2);
        mZ1_r32 = std::sqrt(-2.0 * std::log(u1)) * std::sin(TWO_PI * u2);
        return z0 * stddev + mean;
    }

    r64
    Rand::next_norm(r64 mean,
                    r64 stddev) {
        // NOTE: Taken from https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform
        mGenerate_r64 = !mGenerate_r64;
        if (!mGenerate_r64) {
            return mZ1_r64 * stddev + mean;
        }

        r64 u1 = next_r64();
        r64 u2 = next_r64();

        r64 z0 = std::sqrt(-2.0 * std::log(u1)) * std::cos(TWO_PI * u2);
        mZ1_r64 = std::sqrt(-2.0 * std::log(u1)) * std::sin(TWO_PI * u2);
        return z0 * stddev + mean;
    }
}

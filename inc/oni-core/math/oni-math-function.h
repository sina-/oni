#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <cassert>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/common/oni-common-const.h>
#include <oni-core/component/oni-component-fwd.h>

namespace oni {
    struct vec2;

    inline r64
    toRadians(r64 degrees) noexcept {
        return degrees * (PI / 180.0f);
    }

    template<class T>
    inline T
    max(const T &x,
        const T &y) noexcept {
        if (x > y) {
            return x;
        }
        return y;
    }

    template<class T>
    inline T
    min(const T &x,
        const T &y) noexcept {
        if (x < y) {
            return x;
        }
        return y;
    }

    template<class T>
    inline void
    clip(T &n,
         const T &lower,
         const T &upper) noexcept {
        n = max(lower, min(n, upper));
    }

    template<class T>
    inline T
    clip(const T &n,
         const T &lower,
         const T &upper) noexcept {
        return max(lower, min(n, upper));
    }

    template<class T>
    inline void
    clipUpper(T &val,
              const T &upper) noexcept {
        if (val > upper) {
            val = upper;
        }
    }

    template<class T>
    inline void
    clipLower(T &val,
              const T &lower) noexcept {
        if (val < lower) {
            val = lower;
        }
    }

    template<class T>
    inline bool
    zeroClip(T &val) noexcept {
        if (val <= EP32) {
            val = 0;
            return true;
        }
        return false;
    }

    template<class T>
    inline i32
    sign(T n) noexcept {
        return (T(0) < n) - (T(0) > n);
    }

    inline i32
    findBin(const r32 position,
            const u16 binSize) {
        assert(binSize);
        auto result = std::floor(position / binSize);
        auto truncated = static_cast<i32>(result);
        return truncated;
    }

    inline r32
    binPos(const i64 index,
           const u16 binSize) noexcept {
        return binSize * index;
    }

    inline i32p
    pack_i32(const i32 x,
             const i32 y) noexcept {
        // NOTE: Cast to unsigned int adds max(std::uint32_t) + 1 when input is negative.
        // For example: std::unint32_t(-1) = -1 + max(std::uint32_t) + 1 = max(std::uint32_t)
        // and std::uint32_t(-max(std::int32_t)) = -max(std::int32_t) + max(std::uint32_t) + 1 = max(std::uint32_t) / 2 + 1
        // Notice that uint32_t = 2 * max(int32_t).
        // So it kinda shifts all the values in the reverse order from max(std::uint32_t) / 2 + 1 to max(std::uint32_t)
        // And that is why this function is bijective, which is an important property since it has to always map
        // unique inputs to a unique output.
        // There are other ways to do this: https://stackoverflow.com/a/13871379 (Cantor pairing function and Szudzik's improved implementation)
        // I could also just yank the numbers together and save it as a string.
        auto _x = static_cast<u64>(static_cast<u32>(x)) << 32u;
        auto _y = static_cast<u64>(static_cast<u32>(y));
        auto result = _x | _y;

        return result;
    }

    inline u32p
    pack_u32(const u32 x,
             const u32 y) noexcept {
        auto _x = static_cast<u64>(x) << 32u;
        auto _y = static_cast<u64>(y);
        auto result = _x | _y;
        return result;
    }

    inline u16p
    pack_u16(u16 x,
             u16 y) noexcept {
        auto _x = (static_cast<u32>(x) << 16u);
        auto _y = y;
        auto result = _x | _y;
        return result;
    }

    template<class T>
    inline T
    lerp(T x,
         T y,
         T t) noexcept {
        return (1 - t) * x + t * y;
    }

    template<class T>
    inline T
    abs(T x) {
        return std::abs(x);

        // This is wrong for 0.0. And (x < 0) is wrong for -0.0
        if (x <= 0) {
            return -1 * x;
        }
        return x;
    }

    template<class T>
    inline T
    pow(T x,
        i16 up) {
        if (up == 0) {
            return 1;
        }
        if (up < 0) {
            assert(false);
            return 1;
        }
        T result = x;
        while (--up) {
            result *= x;
        }
        return result;
    }

    template<class T>
    inline bool
    almost_Equal(T x,
                 T y) noexcept {
        return abs(x - y) <= EP32;
    }

    template<class T>
    inline bool
    almost_Zero(T x) noexcept {
        return abs(x) <= EP32;
    }

    template<class T>
    inline bool
    almost_Less(T x,
                T y) noexcept {
        return x - y <= EP32;
    }

    template<class T>
    inline bool
    almost_Greater(T x,
                   T y) noexcept {
        return x - y >= EP32;
    }

    template<class T>
    inline bool
    almost_Positive(T x) noexcept {
        return x >= EP32;
    }

    template<class T>
    inline bool
    almost_Negative(T x) noexcept {
        return x < 0;
    }

    template<class T>
    inline bool
    subAndZeroClip(T &x,
                   const T y) noexcept {
        x -= y;
        return zeroClip(x);
    }

    template<class T>
    constexpr std::underlying_type_t<T>
    enumCast(T value) noexcept {
        return static_cast<std::underlying_type_t<T>>(value);
    }

    void
    findAABB(const Quad &,
             AABB &);

    vec2
    headingVector(r32 heading);

    vec2
    headingVector(const Heading &);

    void
    translate(Quad &,
              const WorldP3D &);

    void
    translate(WorldP3D &,
              const WorldP3D &);

    void
    scale(Quad &,
          const Scale &);
}

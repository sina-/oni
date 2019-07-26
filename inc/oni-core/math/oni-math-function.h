#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <cassert>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/common/oni-common-const.h>

namespace oni {
    namespace component {
        struct AABB;
        struct Quad;
        struct Heading;
        union WorldP3D;
        union Scale;
    }
    namespace math {
        struct vec2;

        inline common::r64
        toRadians(common::r64 degrees) noexcept {
            return degrees * (common::PI / 180.0f);
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
            if (val <= common::EP32) {
                val = 0;
                return true;
            }
            return false;
        }

        template<class T>
        inline common::i32
        sign(T n) noexcept {
            return (T(0) < n) - (T(0) > n);
        }

        inline common::i64
        findBin(const common::r64 position,
                const common::u16 binSize) {
            assert(binSize);
            auto result = std::floor(position / binSize);
            auto truncated = static_cast<common::i64>(result);
            return truncated;
        }

        inline common::r32
        binPos(const common::i64 index,
               const common::u16 binSize) noexcept {
            return binSize * index;
        }

        inline common::i64p
        pack_i64(const common::i64 x,
                 const common::i64 y) noexcept {
            // NOTE: Cast to unsigned int adds max(std::uint32_t) + 1 when input is negative.
            // For example: std::unint32_t(-1) = -1 + max(std::uint32_t) + 1 = max(std::uint32_t)
            // and std::uint32_t(-max(std::int32_t)) = -max(std::int32_t) + max(std::uint32_t) + 1 = max(std::uint32_t) / 2 + 1
            // Notice that uint32_t = 2 * max(int32_t).
            // So it kinda shifts all the values in the reverse order from max(std::uint32_t) / 2 + 1 to max(std::uint32_t)
            // And that is why this function is bijective, which is an important property since it has to always map
            // unique inputs to a unique output.
            // There are other ways to do this: https://stackoverflow.com/a/13871379 (Cantor pairing function and Szudzik's improved implementation)
            // I could also just yank the numbers together and save it as a string.
            auto _x = static_cast<common::u64>(static_cast<common::u32>(x)) << 32;
            auto _y = static_cast<common::u64>(static_cast<common::u32>(y));
            auto result = _x | _y;

            return result;
        }

        inline common::u32p
        pack_u32(const common::u32 x,
                 const common::u32 y) noexcept {
            auto _x = static_cast<common::u64>(x) << 32;
            auto _y = static_cast<common::u64>(y);
            auto result = _x | _y;
            return result;
        }

        inline common::u16p
        pack_u16(common::u16 x,
                 common::u16 y) noexcept {
            return x << 16 | y;
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
            common::i16 up) {
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
            return abs(x - y) <= common::EP32;
        }

        template<class T>
        inline bool
        almost_Zero(T x) noexcept {
            return abs(x) <= common::EP32;
        }

        template<class T>
        inline bool
        almost_Less(T x,
                    T y) noexcept {
            return x - y <= common::EP32;
        }

        template<class T>
        inline bool
        almost_Greater(T x,
                       T y) noexcept {
            return x - y >= common::EP32;
        }

        template<class T>
        inline bool
        almost_Positive(T x) noexcept {
            return x >= common::EP32;
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
        findAABB(const component::Quad &, component::AABB&);

        math::vec2
        headingVector(common::r32 heading);

        math::vec2
        headingVector(const component::Heading&);

        void
        translate(component::Quad&, const component::WorldP3D&);

        void
        translate(component::WorldP3D&, const component::WorldP3D&);

        void
        scale(component::Quad&, const component::Scale&);
    }
}

#pragma once

#define _USE_MATH_DEFINES

#include <cmath>
#include <cassert>

#include <oni-core/common/typedefs.h>
#include <oni-core/common/consts.h>

namespace oni {
    namespace math {
        inline common::real64
        toRadians(common::real64 degrees) {
            return degrees * (common::PI / 180.0f);
        }

        template<class T>
        inline T
        max(const T &x,
            const T &y) {
            if (x > y) {
                return x;
            }
            return y;
        }

        template<class T>
        inline T
        min(const T &x,
            const T &y) {
            if (x < y) {
                return x;
            }
            return y;
        }

        template<class T>
        inline void
        clip(T &n,
             const T &lower,
             const T &upper) {
            n = max(lower, min(n, upper));
        }

        template<class T>
        inline T
        clip(const T &n,
             const T &lower,
             const T &upper) {
            return max(lower, min(n, upper));
        }

        template<class T>
        inline void
        clipUpper(T &val,
                  const T &upper) {
            if (val > upper) {
                val = upper;
            }
        }

        template<class T>
        inline void
        clipLower(T &val,
                  const T &lower) {
            if (val < lower) {
                val = lower;
            }
        }

        template<class T>
        inline void
        zeroClip(T &val) {
            if (val < 0) {
                val = 0;
            }
        }

        template<class T>
        inline common::int32
        sign(T n) {
            return (T(0) < n) - (T(0) > n);
        }

        inline common::int64
        findBin(const common::real64 position,
                const common::uint16 binSize) {
            auto result = std::floor(position / binSize);
            auto truncated = static_cast<common::int64>(result);
            return truncated;
        }

        inline common::real32
        binPos(const common::int64 index,
               const common::uint16 binSize) {
            return binSize * index;
        }

        inline common::Int64Pack
        packInt64(const common::int64 x,
                  const common::int64 y) {
            // NOTE: Cast to unsigned int adds max(std::uint32_t) + 1 when input is negative.
            // For example: std::unint32_t(-1) = -1 + max(std::uint32_t) + 1 = max(std::uint32_t)
            // and std::uint32_t(-max(std::int32_t)) = -max(std::int32_t) + max(std::uint32_t) + 1 = max(std::uint32_t) / 2 + 1
            // Notice that uint32_t = 2 * max(int32_t).
            // So it kinda shifts all the values in the reverse order from max(std::uint32_t) / 2 + 1 to max(std::uint32_t)
            // And that is why this function is bijective, which is an important property since it has to always map
            // unique inputs to a unique output.
            // There are other ways to do this: https://stackoverflow.com/a/13871379 (Cantor pairing function and Szudzik's improved implementation)
            // I could also just yank the numbers together and save it as a string.
            auto _x = static_cast<common::uint64>(static_cast<common::uint32>(x)) << 32;
            auto _y = static_cast<common::uint64>(static_cast<common::uint32>(y));
            auto result = _x | _y;

            return result;
        }

        inline common::UInt16Pack
        packUInt16(common::uint16 x,
                   common::uint16 y) {
            return x << 16 | y;
        }

        template<class T>
        inline T
        lerp(T a,
             T b,
             T t) {
            return (1 - t) * a + t * b;
        }

        template<class T>
        inline T
        abs(T a) {
            if (a < 0) {
                return -1 * a;
            }
            return a;
        }

        template<class T>
        inline T
        pow(T a,
            common::int16 up) {
            if (up == 0) {
                return 1;
            }
            if (up < 0) {
                assert(false);
                return 1;
            }
            T result = a;
            while (--up) {
                result *= a;
            }
            return result;
        }
    }
}

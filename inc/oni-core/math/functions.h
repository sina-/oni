#pragma once

#define _USE_MATH_DEFINES

#include <cmath>

#include <oni-core/common/typedefs.h>
#include <oni-core/common/consts.h>

namespace oni {
    namespace math {
        inline common::real64 toRadians(common::real64 degrees) {
            return degrees * (common::PI / 180.0f);
        }

        template<typename T>
        int sign(T val) {
            return (T(0) < val) - (val < T(0));
        }

        inline common::int64 findBin(const common::real64 position, const common::uint16 binSize) {
            auto result = floor(position / binSize);
            auto truncated = static_cast<common::int64>(result);
            return truncated;
        }

        inline common::real32 binPos(const common::int64 index, const common::uint16 binSize) {
            return binSize * index;
        }

        inline common::Int64Pack packInt64(const common::int64 x, const common::int64 y) {
            // NOTE: Cast to unsigned int adds max(std::uint32_t) + 1 when input is negative.
            // For example: std::unint32_t(-1) = -1 + max(std::uint32_t) + 1 = max(std::uint32_t)
            // and std::uint32_t(-max(std::int32_t)) = -max(std::int32_t) + max(std::uint32_t) + 1 = max(std::uint32_t) / 2 + 1
            // Notice that uint32_t = 2 * max(int32_t).
            // So it kinda shifts all the values in the reverse order from max(std::uint32_t) / 2 + 1 to max(std::uint32_t)
            // And that is why this function is bijective, which is an important property since it has to always map
            // unique inputs to a unique output.
            // There are other ways to do this: https://stackoverflow.com/a/13871379
            // I could also just yank the numbers together and save it as a string.
            auto _x = static_cast<common::uint64>(static_cast<common::uint32>(x)) << 32;
            auto _y = static_cast<common::uint64>(static_cast<common::uint32>(y));
            auto result = _x | _y;

            return result;
        }

        inline common::UInt16Pack packUInt16(common::uint16 x, common::uint16 y) {
            return x << 16 | y;
        }
    }
}

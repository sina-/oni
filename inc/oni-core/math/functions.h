#pragma once

#define _USE_MATH_DEFINES

#include <cmath>

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace math {
        inline common::real64 toRadians(common::real64 degrees) {
            return degrees * (M_PI / 180.0f);
        }

        inline common::int64 positionToIndex(const common::real64 position, const common::uint16 tileSize) {
            /**
             * Tiles in the world map fall under these indices:
             *
             * [-halfTileSize * 5, -halfTileSize * 3) -> -2
             * [-halfTileSize * 3, -halfTileSize * 1) -> -1
             * [-halfTileSize * 1, +halfTileSize * 1) -> +0
             * [+halfTileSize * 1, +halfTileSize * 3) -> +1
             * [+halfTileSize * 3, +halfTileSize * 5) -> +2
             *
             * For example for tile size 16:
             * [-40, -24) -> -2
             * [-24,  -8) -> -1
             * [ -8,  +8) -> +0
             * [ +8, +24) -> +1
             * [+24, +40) -> +2
             */
            auto result = floor(position / tileSize);
            auto truncated = static_cast<common::int64>(result);
            return truncated;
        }

        inline common::real32 indexToPosition(const common::int64 index, const common::uint16 tileSize) {
            return tileSize * index;
        }

        /**
         * Pack two unique int32 values into unique uint64.
         * @param x
         * @return pack uint64 value
         */
        inline common::packedInt32 packIntegers(const common::int64 x, const common::int64 y) {
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

    }
}

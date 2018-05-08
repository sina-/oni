#include <oni-core/entities/tile-world.h>
#include <oni-core/utils/oni-assert.h>

namespace oni {
    namespace entities {

        TileWorld::TileWorld() {
            mTileRegistry = std::make_unique<entt::DefaultRegistry>();
        }

        TileWorld::~TileWorld() = default;

        int TileWorld::getTileSize() const {
            return mTileSize;
        }

        bool TileWorld::tileExists(long tileCoords) const {
            return mCoordToTileLookup.find(tileCoords) != mCoordToTileLookup.end();
        }

        long TileWorld::packCoordinates(const math::vec2 &position) {
            // TODO: Need to use int64_t for return type and int32_t instead of int below
            //  when I have migrated to double precision in the math library
            int x = static_cast<int>(position.x) / mTileSize;
            int y = static_cast<int>(position.y) / mTileSize;

            // https://stackoverflow.com/a/827267
            long result = x << 32;
            result = result | static_cast<long>(static_cast<unsigned int>(y));

            return result;
        }

        math::vec2 TileWorld::unpackCoordinates(long coord) {
            auto x = static_cast<int>(coord >> 32) * mTileSize;
            auto y = static_cast<int>(coord & (0xFFFFFFFF)) * mTileSize;

            // TODO: This won't work with float for math::vec2. I need to migrate to doubles.
            return math::vec2{x, y};
        }

        math::vec4 TileWorld::getTileColor(const math::vec2 &position) {
            auto tileCoord = packCoordinates(position);
            auto result = mCoordToTileLookup.find(tileCoord);

            // TODO: It might also make sense to log a warning message and just tick the missing tile.
            ONI_DEBUG_ASSERT(result != mCoordToTileLookup.end());

            auto tileID = result->second;

            return mTileRegistry->get<math::vec4>(tileID);
        }

        void TileWorld::tick(const math::vec2 &position, unsigned int tickRadius) {
            // TODO: Make sure tiles adjacent to the given position are properly created and save into
            // mTileRegistry.

            auto tileCoordinates = packCoordinates(position);
            if (!tileExists(tileCoordinates)) {
                auto uCoordinates = unpackCoordinates(tileCoordinates);
                auto color = math::vec4{position.x, position.y, uCoordinates.x, uCoordinates.y};
                auto tile = mTileRegistry->create();

                mCoordToTileLookup.emplace(tileCoordinates, tile);

                mTileRegistry->assign<math::vec4>(tile, color);
            }
        }

    }
}
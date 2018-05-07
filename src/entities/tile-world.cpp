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

        entities::entityID TileWorld::getTileID(double tileCoords) const {
            auto result = mCoordToTileLookup.find(tileCoords);
            ONI_DEBUG_ASSERT(result != mCoordToTileLookup.end());

            return result->second;
        }

        bool TileWorld::tileExsists(long tileCoords) const {
            return mCoordToTileLookup.find(tileCoords) != mCoordToTileLookup.end();
        }

        long TileWorld::packCoords(const math::vec2 &position) {
            int x = static_cast<int>(position.x) / mTileSize;
            int y = static_cast<int>(position.y) / mTileSize;

            // https://stackoverflow.com/a/827267
            long result = x;
            result = result << 32;
            result = result | static_cast<long>(static_cast<unsigned int>(y));

            return result;
        }

        math::vec2 TileWorld::unpackCoords(long coord) {
            auto x = static_cast<int>(coord >> 32) * mTileSize;
            auto y = static_cast<int>(coord & (0xFFFFFFFF)) * mTileSize;

            // TODO: This won't work with float for math::vec2. I need to migrate to doubles.
            return math::vec2{x, y};
        }

        math::vec4 TileWorld::getTileColor(const math::vec2 &position) {
            auto tileCoord = packCoords(position);
            auto tileID = getTileID(tileCoord);

            return mTileRegistry->get<math::vec4>(tileID);
        }

        void TileWorld::tick(const math::vec2 &position, unsigned int distanceToRightSideOfScreen) {
            // TODO: Make sure tiles adjacent to the given position are properly created and save into
            // mTileRegistry.

            auto tileCoords = packCoords(position);
            if (!tileExsists(tileCoords)) {
                auto uCoords = unpackCoords(tileCoords);
                auto color = math::vec4{position.x, position.y, uCoords.x, uCoords.y};
                auto tile = mTileRegistry->create();

                mCoordToTileLookup.emplace(tileCoords, tile);

                mTileRegistry->assign<math::vec4>(tile, color);
            }
        }

    }
}
#include <ctime>

#include <oni-core/entities/tile-world.h>
#include <oni-core/utils/oni-assert.h>

namespace oni {
    namespace entities {

        TileWorld::TileWorld() {
            mTileRegistry = std::make_unique<entt::DefaultRegistry>();
            std::srand(std::time(nullptr));
        }

        TileWorld::~TileWorld() = default;

        int TileWorld::getTileSizeX() const {
            return mTileSizeX;
        }

        int TileWorld::getTileSizeY() const {
            return mTileSizeY;
        }

        bool TileWorld::tileExists(long tileCoordinates) const {
            return mCoordToTileLookup.find(tileCoordinates) != mCoordToTileLookup.end();
        }

        long TileWorld::packCoordinates(const math::vec2 &position) const {
            // TODO: Need to use int64_t for return type and int32_t instead of int below
            //  when I have migrated to double precision in the math library
            int x = getTileIndexX(position.x);
            int y = getTileIndexY(position.y);

            // https://stackoverflow.com/a/827267
            long result = x << 32;
            result = result | static_cast<long>(static_cast<unsigned int>(y));

            return result;
        }

        math::vec2 TileWorld::unpackCoordinates(long coord) const {
            auto x = static_cast<int>(coord >> 32) * mTileSizeX;
            auto y = static_cast<int>(coord & (0xFFFFFFFF)) * mTileSizeX;

            // TODO: This won't work with float for math::vec2. I need to migrate to doubles.
            return math::vec2{x, y};
        }

        math::vec4 TileWorld::getTileColor(const math::vec2 &position) const {
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
                auto R = (std::rand() % 255) / 255.0f;
                auto G = (std::rand() % 255) / 255.0f;
                auto B = (std::rand() % 255) / 255.0f;
                auto color = math::vec4{R, G, B, 1.0f};
                auto tile = mTileRegistry->create();

                mCoordToTileLookup.emplace(tileCoordinates, tile);

                mTileRegistry->assign<math::vec4>(tile, color);
            }
        }

        int TileWorld::getTileIndexX(float x) const {
            int _x = static_cast<int>(x) / mTileSizeX;
            return _x;
        }

        int TileWorld::getTileIndexY(float y) const {
            int _y = static_cast<int>(y) / mTileSizeY;
            return _y;
        }

    }
}
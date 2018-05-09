#include <ctime>

#include <oni-core/entities/tile-world.h>
#include <oni-core/utils/oni-assert.h>
#include <oni-core/components/geometry.h>

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

        common::packedTileCoordinates TileWorld::packCoordinates(const math::vec2 &position) const {
            // TODO: Need to use int64_t for return type and int32_t instead of int for getTileIndexX()
            //  when I have migrated to double precision in the math library
            int x = getTileIndexX(position.x);
            int y = getTileIndexY(position.y);

            // NOTE: Cast to unsigned int adds max(std::uint32_t) + 1 when input is negative.
            // For example: std::unint32_t(-1) = -1 + max(std::uint32_t) + 1 = max(std::uint32_t)
            // and std::uint32_t(-max(std::int32_t)) = -max(std::int32_t) + max(std::uint32_t) + 1 = max(std::uint32_t) / 2 + 1
            // Notice that uint32_t = 2 * max(int32_t).
            // So it kinda shifts all the values in the reverse order from max(std::uint32_t) / 2 + 1 to max(std::uint32_t)
            // And that is why this function is bijective, which is an important property since it has to always map
            // unique inputs to a unique output.
            // There are other ways to do this: https://stackoverflow.com/a/13871379
            // I could also just yank the numbers together and save it as a string.
            auto _x = std::uint64_t(std::uint32_t(x)) << 32;
            auto _y = std::uint64_t(std::uint32_t(y));
            auto result = _x | _y;

            return result;
        }

        math::vec2 TileWorld::unpackCoordinates(long coord) const {
            // TODO: This function is incorrect. Need to match it to packCoordinates function if I ever use it
            ONI_DEBUG_ASSERT(false);
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
            auto tilesInAlongX = getTileIndexX(tickRadius);
            auto tilesInAlongY = getTileIndexY(tickRadius);
            for (auto i = -tilesInAlongX; i <= tilesInAlongX; ++i) {
                for (auto j = -tilesInAlongY; j <= tilesInAlongY; ++j) {
                    auto tilePosition = math::vec2{position.x + i * mTileSizeX, position.y + j * mTileSizeY};
                    createTileIfMissing(tilePosition);
                }
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

        void TileWorld::createTileIfMissing(const math::vec2 &position) {
            auto packedCoordinates = packCoordinates(position);
            if (!tileExists(packedCoordinates)) {
                auto R = (std::rand() % 255) / 255.0f;
                auto G = (std::rand() % 255) / 255.0f;
                auto B = (std::rand() % 255) / 255.0f;
                auto color = math::vec4{R, G, B, 1.0f};

                auto tileIndexX = getTileIndexX(position.x);
                auto tileIndexY = getTileIndexX(position.y);
                auto tilePosX = tileIndexX * mTileSizeX;
                auto tilePosY = tileIndexY * mTileSizeY;
                auto positionInWorld = math::vec3{tilePosX, tilePosY, 1.0f};
                auto id = createSpriteStaticEntity(*mTileRegistry, color, math::vec2{mTileSizeX, mTileSizeY},
                                                   positionInWorld);

                mCoordToTileLookup.emplace(packedCoordinates, id);

            }
        }

        entt::DefaultRegistry &TileWorld::getRegistry() {
            return *mTileRegistry;
        }

    }
}
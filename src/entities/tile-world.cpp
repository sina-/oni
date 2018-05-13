#include <ctime>

#include <oni-core/entities/tile-world.h>
#include <oni-core/utils/oni-assert.h>
#include <oni-core/components/geometry.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/graphics/texture.h>
#include <oni-core/common/consts.h>


namespace oni {
    // TODO: save this as a tag in the world registry
    static const common::real32 GAME_UNIT_TO_PIXELS = 20.0f;

    namespace entities {

        TileWorld::TileWorld() :
                mTileSizeX{16}, mTileSizeY{16},
                mHalfTileSizeX{mTileSizeX / 2.0f},
                mHalfTileSizeY{mTileSizeX / 2.0f} {
            mTileRegistry = std::make_unique<entt::DefaultRegistry>();
            std::srand(std::time(nullptr));
        }

        TileWorld::~TileWorld() = default;

        common::uint16 TileWorld::getTileSizeX() const {
            return mTileSizeX;
        }

        common::uint16 TileWorld::getTileSizeY() const {
            return mTileSizeY;
        }

        common::int64 TileWorld::getTileXIndex(common::real64 x) const {
            /**
             * Tiles in the world map fall under these indices:
             *
             * [-mHalfTileSize * 5, -mHalfTileSize * 3) -> -2
             * [-mHalfTileSize * 3, -mHalfTileSize * 1) -> -1
             * [-mHalfTileSize * 1, +mHalfTileSize * 1) -> +0
             * [+mHalfTileSize * 1, +mHalfTileSize * 3) -> +1
             * [+mHalfTileSize * 3, +mHalfTileSize * 5) -> +2
             *
             * For example for tile size 16 and half size of 8:
             * [-40, -24) -> -2
             * [-24,  -8) -> -1
             * [ -8,  +8) -> +0
             * [ +8, +24) -> +1
             * [+24, +40) -> +2
             */
            auto result = floor(x / mTileSizeX + 0.5f);
            auto truncated = static_cast<common::int64>(result);
            return truncated;
        }

        common::int64 TileWorld::getTileYIndex(common::real64 y) const {
            auto result = floor(y / mTileSizeX + 0.5f);
            auto truncated = static_cast<common::int64>(result);
            return truncated;
        }

        common::real32 TileWorld::getTilePosForXIndex(common::int64 xIndex) const {
            return mTileSizeX * xIndex - mHalfTileSizeX;
        }

        common::real32 TileWorld::getTilePosForYIndex(common::int64 yIndex) const {
            return mTileSizeY * yIndex - mHalfTileSizeY;
        }


        bool TileWorld::tileExists(common::uint64 tileCoordinates) const {
            return mCoordToTileLookup.find(tileCoordinates) != mCoordToTileLookup.end();
        }

        bool TileWorld::skidTileExists(common::uint64 tileCoordinates) const {
            return mCoordToSkidLineLookup.find(tileCoordinates) != mCoordToSkidLineLookup.end();
        }

        common::packedTileCoordinates TileWorld::packCoordinates(const math::vec2 &position) const {
            auto x = getTileXIndex(position.x);
            auto y = getTileYIndex(position.y);

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

        math::vec2 TileWorld::unpackCoordinates(common::uint64 coord) const {
            // TODO: This function is incorrect. Need to match it to packCoordinates function if I ever use it
            ONI_DEBUG_ASSERT(false);
            //auto x = static_cast<int>(coord >> 32) * mTileSizeX;
            //auto y = static_cast<int>(coord & (0xFFFFFFFF)) * mTileSizeX;

            //return math::vec2{x, y};
            return math::vec2{};
        }

        void TileWorld::tick(const math::vec2 &position, common::uint16 tickRadius, const components::Car &car,
                             entt::DefaultRegistry &entityRegistry) {
            // TODO: Hardcoded +2 until I find a good way to calculate the exact number of tiles
            auto tilesAlongX = getTileXIndex(tickRadius) + 2;
            auto tilesAlongY = getTileYIndex(tickRadius) + 2;
            for (auto i = -tilesAlongX; i <= tilesAlongX; ++i) {
                for (auto j = -tilesAlongY; j <= tilesAlongY; ++j) {
                    auto tilePosition = math::vec2{position.x + i * mTileSizeX, position.y + j * mTileSizeY};
                    createTileIfMissing(tilePosition);
                }
            }

            // NOTE: Technically I should use slippingRear, but this gives better effect
            if (car.slippingFront) {
/*                auto carTireBRPlacement = entityRegistry.get<components::Placement>(mCarTireBREntity);
                auto carTireBLPlacement = entityRegistry.get<components::Placement>(mCarTireBLEntity);*/

/*                const auto &transformParentBL = entityRegistry.get<components::TransformParent>(mCarTireBLEntity);
                const auto &transformParentBR = entityRegistry.get<components::TransformParent>(mCarTireBREntity);*/

/*                auto skidMarkBLPos = transformParentBL.transform * carTireBLPlacement.position;
                auto skidMarkBRPos = transformParentBR.transform * carTireBRPlacement.position;*/

                auto carPos = car.position;
                auto packedCoordinates = packCoordinates(carPos);

                entityID skidEntity = 0;
                if (!skidTileExists(packedCoordinates)) {
                    // TODO: These are constant
                    auto skidWidthInPixels = static_cast<common::int32>(mTileSizeX * GAME_UNIT_TO_PIXELS + common::ep);
                    auto skidHeightInPixels = static_cast<common::int32>(mTileSizeY * GAME_UNIT_TO_PIXELS + common::ep);
                    auto skidDefaultPixel = oni::components::PixelRGBA{};

                    auto skidTexture = graphics::Texture::generate(skidWidthInPixels, skidHeightInPixels,
                                                                   skidDefaultPixel);
                    // END TODO
                    auto skidSize = math::vec2{mTileSizeX, mTileSizeY};
                    auto carTileX = getTileXIndex(car.position.x);
                    auto carTileY = getTileYIndex(car.position.y);
                    auto tilePosX = getTilePosForXIndex(carTileX);
                    auto tilePosY = getTilePosForYIndex(carTileY);
                    auto positionInWorld = math::vec3{tilePosX, tilePosY, 1.0f};
                    skidEntity = entities::createTexturedStaticEntity(entityRegistry, skidTexture,
                                                                      skidSize, positionInWorld);
                    mCoordToSkidLineLookup.emplace(packedCoordinates, skidEntity);
                } else {
                    skidEntity = mCoordToSkidLineLookup.at(packedCoordinates);
                }

                auto skidMarksTexture = entityRegistry.get<components::Texture>(skidEntity);
                auto skidMarksTexturePos = entityRegistry.get<components::Shape>(skidEntity).getPosition();

                auto skidPos = math::vec3{car.position.x, car.position.y, 1.0f};

                physics::Transformation::worldToLocalTextureTranslation(skidMarksTexturePos, GAME_UNIT_TO_PIXELS,
                                                                        skidPos);

                auto alpha = static_cast<unsigned char>((car.velocityAbsolute / car.maxVelocityAbsolute) * 255);
                // TODO: I can not generate geometrical shapes that are rotated. Until I have that I will stick to
                // squares.
                //auto width = static_cast<int>(carConfig.wheelRadius * GAME_UNIT_TO_PIXELS * 2);
                //auto height = static_cast<int>(carConfig.wheelWidth * GAME_UNIT_TO_PIXELS / 2);
                common::real32 height = 10.0f;
                common::real32 width = 10.0f;

                auto bits = graphics::Texture::generateBits(width, height, components::PixelRGBA{0, 0, 0, alpha});
                graphics::Texture::updateSubTexture(skidMarksTexture,
                                                    static_cast<GLint>(skidPos.x - width / 2),
                                                    static_cast<GLint>(skidPos.y - width / 2),
                                                    width, height, bits);
            }

        }

        void TileWorld::createTileIfMissing(const math::vec2 &position) {
            auto packedCoordinates = packCoordinates(position);
            if (!tileExists(packedCoordinates)) {
                const auto R = (std::rand() % 255) / 255.0f;
                const auto G = (std::rand() % 255) / 255.0f;
                const auto B = (std::rand() % 255) / 255.0f;
                const auto color = math::vec4{R, G, B, 1.0f};

                const auto tileIndexX = getTileXIndex(position.x);
                const auto tileIndexY = getTileXIndex(position.y);
                const auto tilePosX = getTilePosForXIndex(tileIndexX);
                const auto tilePosY = getTilePosForYIndex(tileIndexY);
                const auto positionInWorld = math::vec3{tilePosX, tilePosY, 1.0f};
                const auto id = createSpriteStaticEntity(*mTileRegistry, color, math::vec2{mTileSizeX, mTileSizeY},
                                                         positionInWorld);

                mCoordToTileLookup.emplace(packedCoordinates, id);

            }
        }

        entt::DefaultRegistry &TileWorld::getRegistry() {
            return *mTileRegistry;
        }

    }
}
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
            std::srand(std::time(nullptr));

            mSkidSize = math::vec2{mTileSizeX, mTileSizeY};
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
                             entt::DefaultRegistry &foregroundEntities, entt::DefaultRegistry &backgroundEntities) {
            // TODO: Hardcoded +2 until I find a good way to calculate the exact number of tiles
            auto tilesAlongX = getTileXIndex(tickRadius) + 2;
            auto tilesAlongY = getTileYIndex(tickRadius) + 2;
            for (auto i = -tilesAlongX; i <= tilesAlongX; ++i) {
                for (auto j = -tilesAlongY; j <= tilesAlongY; ++j) {
                    auto tilePosition = math::vec2{position.x + i * mTileSizeX, position.y + j * mTileSizeY};
                    createTileIfMissing(tilePosition, backgroundEntities);
                }
            }

            // NOTE: Technically I should use slippingRear, but this gives better effect
            if (car.slippingFront) {
                auto carTireRRPlacement = foregroundEntities.get<components::Placement>(car.tireRR);
                auto carTireRLPlacement = foregroundEntities.get<components::Placement>(car.tireRL);

                const auto &transformParentRR = foregroundEntities.get<components::TransformParent>(car.tireRR);
                const auto &transformParentRL = foregroundEntities.get<components::TransformParent>(car.tireRL);

                auto skidMarkRLPos = transformParentRL.transform * carTireRLPlacement.position;
                auto skidMarkRRPos = transformParentRR.transform * carTireRRPlacement.position;

                entityID skidEntityRL{0};
                entityID skidEntityRR{0};

                skidEntityRL = createSkidTileIfMissing(skidMarkRLPos.getXY(), foregroundEntities);
                skidEntityRR = createSkidTileIfMissing(skidMarkRRPos.getXY(), foregroundEntities);
                auto alpha = static_cast<unsigned char>((car.velocityAbsolute / car.maxVelocityAbsolute) * 255);

                updateSkidTexture(skidMarkRLPos, skidEntityRL, foregroundEntities, alpha);
                updateSkidTexture(skidMarkRRPos, skidEntityRR, foregroundEntities, alpha);
            }

        }

        void TileWorld::createTileIfMissing(const math::vec2 &position, entt::DefaultRegistry &backgroundEntities) {
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
                const auto id = createSpriteStaticEntity(backgroundEntities, color, math::vec2{mTileSizeX, mTileSizeY},
                                                         positionInWorld);

                mCoordToTileLookup.emplace(packedCoordinates, id);
            }
        }

        entities::entityID TileWorld::createSkidTileIfMissing(const math::vec2 &position,
                                                              entt::DefaultRegistry &foregroundEntities) {
            auto packedCoordinates = packCoordinates(position);
            entities::entityID entity{};
            if (!skidTileExists(packedCoordinates)) {
                auto tileX = getTileXIndex(position.x);
                auto tileY = getTileYIndex(position.y);
                auto tilePosX = getTilePosForXIndex(tileX);
                auto tilePosY = getTilePosForYIndex(tileY);
                auto positionInWorld = math::vec3{tilePosX, tilePosY, 1.0f};

                auto skidWidthInPixels = static_cast<common::uint16>(mTileSizeX * GAME_UNIT_TO_PIXELS + common::ep);
                auto skidHeightInPixels = static_cast<common::uint16>(mTileSizeY * GAME_UNIT_TO_PIXELS + common::ep);
                auto skidDefaultPixel = oni::components::PixelRGBA{};
                auto skidTexture = graphics::Texture::generate(skidWidthInPixels, skidHeightInPixels, skidDefaultPixel);

                entity = entities::createTexturedStaticEntity(foregroundEntities, skidTexture,
                                                              mSkidSize, positionInWorld);
                mCoordToSkidLineLookup.emplace(packedCoordinates, entity);
            } else {
                entity = mCoordToSkidLineLookup.at(packedCoordinates);
            }
            return entity;
        }

        void TileWorld::updateSkidTexture(const math::vec3 &position, entities::entityID skidTextureEntity,
                                          entt::DefaultRegistry &foregroundEntities, common::uint8 alpha) {
            auto skidMarksTexture = foregroundEntities.get<components::Texture>(skidTextureEntity);
            auto skidMarksTexturePos = foregroundEntities.get<components::Shape>(skidTextureEntity).getPosition();

            auto skidPos = position;
            physics::Transformation::worldToLocalTextureTranslation(skidMarksTexturePos, GAME_UNIT_TO_PIXELS, skidPos);

            // TODO: I can not generate geometrical shapes that are rotated. Until I have that I will stick to
            // squares.
            //auto width = static_cast<int>(carConfig.wheelRadius * GAME_UNIT_TO_PIXELS * 2);
            //auto height = static_cast<int>(carConfig.wheelWidth * GAME_UNIT_TO_PIXELS / 2);
            common::real32 height = 5.0f;
            common::real32 width = 5.0f;

            auto bits = graphics::Texture::generateBits(width, height, components::PixelRGBA{0, 0, 0, alpha});
            graphics::Texture::updateSubTexture(skidMarksTexture,
                                                static_cast<GLint>(skidPos.x - width / 2),
                                                static_cast<GLint>(skidPos.y - width / 2),
                                                width, height, bits);

        }
    }
}
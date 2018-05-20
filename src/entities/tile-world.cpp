#include <ctime>

#include <oni-core/entities/tile-world.h>
#include <oni-core/utils/oni-assert.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/graphics/texture.h>
#include <oni-core/common/consts.h>


namespace oni {
    // TODO: save this as a tag in the world registry
    static const common::real32 GAME_UNIT_TO_PIXELS = 20.0f;

    namespace entities {

        TileWorld::TileWorld() :
                mTileSizeX{32}, mTileSizeY{16},
                mHalfTileSizeX{mTileSizeX / 2.0f},
                mHalfTileSizeY{mTileSizeY / 2.0f},
                mSkidTileSizeX{64},
                mSkidTileSizeY{64},
                mHalfSkidTileSizeX{mSkidTileSizeX / 2.0f},
                mHalfSkidTileSizeY{mSkidTileSizeY / 2.0f} {
            std::srand(std::time(nullptr));

            mSkidTileSize = math::vec2{mSkidTileSizeX, mSkidTileSizeY};
        }

        TileWorld::~TileWorld() = default;

        common::uint16 TileWorld::getTileSizeX() const {
            return mTileSizeX;
        }

        common::uint16 TileWorld::getTileSizeY() const {
            return mTileSizeY;
        }

        common::int64 TileWorld::positionToIndex(const common::real64 position, const common::uint16 tileSize) const {
            /**
             * Tiles in the world map fall under these indices:
             *
             * [-mHalfTileSize * 5, -mHalfTileSize * 3) -> -2
             * [-mHalfTileSize * 3, -mHalfTileSize * 1) -> -1
             * [-mHalfTileSize * 1, +mHalfTileSize * 1) -> +0
             * [+mHalfTileSize * 1, +mHalfTileSize * 3) -> +1
             * [+mHalfTileSize * 3, +mHalfTileSize * 5) -> +2
             *
             * For example for tile size 16:
             * [-40, -24) -> -2
             * [-24,  -8) -> -1
             * [ -8,  +8) -> +0
             * [ +8, +24) -> +1
             * [+24, +40) -> +2
             */
            auto result = floor(position / tileSize + 0.5f);
            auto truncated = static_cast<common::int64>(result);
            return truncated;
        }

        common::real32 TileWorld::indexToPosition(const common::int64 index, const common::uint16 tileSize,
                                                  const common::real32 halfTileSize) const {
            return tileSize * index - halfTileSize;
        }

        bool TileWorld::tileExists(common::uint64 tileCoordinates) const {
            return mPackedTileIndicesToEntity.find(tileCoordinates) != mPackedTileIndicesToEntity.end();
        }

        bool TileWorld::skidTileExists(common::uint64 tileCoordinates) const {
            return mSkidPackedIndicesToEntity.find(tileCoordinates) != mSkidPackedIndicesToEntity.end();
        }

        common::packedInt32 TileWorld::packIntegers(const common::int64 x, const common::int64 y) const {
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

        math::vec2 TileWorld::unpackCoordinates(common::uint64 coord) const {
            // TODO: This function is incorrect. Need to match it to packIntegers function if I ever use it
            ONI_DEBUG_ASSERT(false);
            //auto x = static_cast<int>(coord >> 32) * mTileSizeX;
            //auto y = static_cast<int>(coord & (0xFFFFFFFF)) * mTileSizeX;

            //return math::vec2{x, y};
            return math::vec2{};
        }

        void TileWorld::tick(const math::vec2 &position, common::uint16 viewWidth, common::uint16 viewHeight,
                             const components::Car &car, entt::DefaultRegistry &foregroundEntities,
                             entt::DefaultRegistry &backgroundEntities) {
            auto halfNumTilesAlongX = static_cast<common::uint16>(viewWidth / (2.0f * mTileSizeX)) + 1;
            auto halfNumTilesAlongY = static_cast<common::uint16>(viewHeight / (2.0f * mTileSizeY)) + 1;
            for (auto i = -halfNumTilesAlongX; i <= halfNumTilesAlongX; ++i) {
                for (auto j = -halfNumTilesAlongY; j <= halfNumTilesAlongY; ++j) {
                    auto tileForPosition = math::vec2{position.x + i * mTileSizeX, position.y + j * mTileSizeY};
                    createTileIfMissing(tileForPosition, backgroundEntities);
                }
            }

            // NOTE: Technically I should use slippingRear, but this gives better effect
            if (car.slippingFront || true) {
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

        void TileWorld::createTileIfMissing(const math::vec2 &tileForPosition,
                                            entt::DefaultRegistry &backgroundEntities) {
            auto x = positionToIndex(tileForPosition.x, mTileSizeX);
            auto y = positionToIndex(tileForPosition.y, mTileSizeY);

            auto packedIndices = packIntegers(x, y);
            if (!tileExists(packedIndices)) {
                const auto R = (std::rand() % 255) / 255.0f;
                const auto G = (std::rand() % 255) / 255.0f;
                const auto B = (std::rand() % 255) / 255.0f;
                const auto color = math::vec4{R, G, B, 1.0f};

                const auto tileIndexX = positionToIndex(tileForPosition.x, mTileSizeX);
                const auto tileIndexY = positionToIndex(tileForPosition.y, mTileSizeY);
                const auto tilePosX = indexToPosition(tileIndexX, mTileSizeX, mHalfTileSizeX);
                const auto tilePosY = indexToPosition(tileIndexY, mTileSizeY, mHalfTileSizeY);
                const auto positionInWorld = math::vec3{tilePosX, tilePosY, 1.0f};
                const auto id = createSpriteStaticEntity(backgroundEntities, color, math::vec2{mTileSizeX, mTileSizeY},
                                                         positionInWorld);

                mPackedTileIndicesToEntity.emplace(packedIndices, id);
            }
        }

        entities::entityID TileWorld::createSkidTileIfMissing(const math::vec2 &position,
                                                              entt::DefaultRegistry &foregroundEntities) {
            auto x = positionToIndex(position.x, mSkidTileSizeX);
            auto y = positionToIndex(position.y, mSkidTileSizeY);
            auto packedIndices = packIntegers(x, y);
            entities::entityID entity{};
            if (!skidTileExists(packedIndices)) {
                auto tileX = positionToIndex(position.x, mSkidTileSizeX);
                auto tileY = positionToIndex(position.y, mSkidTileSizeY);
                auto tilePosX = indexToPosition(tileX, mSkidTileSizeX, mHalfSkidTileSizeX);
                auto tilePosY = indexToPosition(tileY, mSkidTileSizeY, mHalfSkidTileSizeY);
                auto positionInWorld = math::vec3{tilePosX, tilePosY, 1.0f};

                auto skidWidthInPixels = static_cast<common::uint16>(mSkidTileSizeX * GAME_UNIT_TO_PIXELS + common::ep);
                auto skidHeightInPixels = static_cast<common::uint16>(mSkidTileSizeY * GAME_UNIT_TO_PIXELS +
                                                                      common::ep);
                auto skidDefaultPixel = oni::components::PixelRGBA{};
                // TODO: This function generates a texture and loads it into video memory, meaning we can not
                // blend two layers of skid mark onto each other
                auto skidTexture = graphics::Texture::generate(skidWidthInPixels, skidHeightInPixels, skidDefaultPixel);

                entity = entities::createTexturedStaticEntity(foregroundEntities, skidTexture,
                                                              mSkidTileSize, positionInWorld);
                mSkidPackedIndicesToEntity.emplace(packedIndices, entity);
            } else {
                entity = mSkidPackedIndicesToEntity.at(packedIndices);
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
                                                static_cast<GLint>(skidPos.x - width / 2.0f),
                                                static_cast<GLint>(skidPos.y - height / 2.0f),
                                                width, height, bits);

        }
    }
}
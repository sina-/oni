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
                mTileSizeX{32}, mTileSizeY{32},
                mHalfTileSizeX{mTileSizeX / 2.0f},
                mHalfTileSizeY{mTileSizeY / 2.0f},
                mSkidTileSizeX{64}, mSkidTileSizeY{64},
                mHalfSkidTileSizeX{mSkidTileSizeX / 2.0f},
                mHalfSkidTileSizeY{mSkidTileSizeY / 2.0f},
                mChunkSizeX{mTileSizeX * 10 + mTileSizeX}, mChunkSizeY{mTileSizeY * 10 + mTileSizeY} {
            std::srand(std::time(nullptr));

            // NOTE: A road chunk is filled with road tiles, therefore road chunk size should be,
            // dividable by road tile size.
            ONI_DEBUG_ASSERT(mTileSizeX <= mChunkSizeX);
            ONI_DEBUG_ASSERT(mTileSizeY <= mChunkSizeY);
            ONI_DEBUG_ASSERT(mChunkSizeX % mTileSizeX == 0);
            ONI_DEBUG_ASSERT(mChunkSizeY % mTileSizeY == 0);

            // NOTE: If number of road tiles in a chunk is not an odd number it means there is no middle
            // tile. For convenience its good to have a middle tile.
            ONI_DEBUG_ASSERT((mChunkSizeX / mTileSizeX) % 2 == 1);
            ONI_DEBUG_ASSERT((mChunkSizeY / mTileSizeY) % 2 == 1);

            ONI_DEBUG_ASSERT(mChunkSizeX % 2 == 0);
            ONI_DEBUG_ASSERT(mChunkSizeY % 2 == 0);
        }

        TileWorld::~TileWorld() = default;

        common::int64 TileWorld::positionToIndex(const common::real64 position, const common::uint16 tileSize) const {
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
            auto result = floor(position / tileSize + 0.5f);
            auto truncated = static_cast<common::int64>(result);
            return truncated;
        }

        common::real32 TileWorld::indexToPosition(const common::int64 index, const common::uint16 tileSize,
                                                  const common::real32 halfTileSize) const {
            return tileSize * index - halfTileSize;
        }

        bool TileWorld::existsInMap(const common::uint64 packedIndices, const PackedIndiciesToEntity &map) const {
            return map.find(packedIndices) != map.end();
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

        void TileWorld::tick(const math::vec2 &position, const components::Car &car, entt::DefaultRegistry &foregroundEntities,
                                     entt::DefaultRegistry &backgroundEntities) {
            tickChunk(position, backgroundEntities);
            tickCars(car, foregroundEntities);
        }

        void TileWorld::tickCars(const components::Car &car, entt::DefaultRegistry &foregroundEntities) {
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

        void TileWorld::tickChunk(const math::vec2 &position, entt::DefaultRegistry &backgroundEntities) {
            const auto x = positionToIndex(position.x, mChunkSizeX);
            const auto y = positionToIndex(position.y, mChunkSizeY);

            // NOTE: We always create and fill chunks in the current location and 8 adjacent chunks.
            for (auto i = x - 1; i <= x + 1; ++i) {
                for (auto j = y - 1; j <= y + 1; ++j) {
                    const auto packedIndices = packIntegers(i, j);
                    if (!existsInMap(packedIndices, mPackedRoadChunkIndicesToEntity)) {
                        generateChunkOfTiles(i, j, backgroundEntities);
                        generateChunkOfRoads(i, j, backgroundEntities);

                        // TODO: create chunk entity
                        auto TEMPID = packedIndices;
                        mPackedRoadChunkIndicesToEntity.emplace(packedIndices, TEMPID);
                    }
                }
            }
        }

        void TileWorld::generateChunkOfRoads(const common::int64 xIndex, const common::int64 yIndex,
                                             entt::DefaultRegistry &backgroundEntities) {
            // NOTE: These locations are in world coordinate
            const auto firstTileX = xIndex * mChunkSizeX - mChunkSizeX / 2;
            const auto lastTileX = xIndex * mChunkSizeX + mChunkSizeX / 2;
            const auto firstTileY = yIndex * mChunkSizeY - mChunkSizeY / 2;
            const auto lastTileY = yIndex * mChunkSizeY + mChunkSizeY / 2;

            const auto roadTileSize = math::vec2{mTileSizeX, mTileSizeY};

            for (auto i = firstTileX; i < lastTileX; i += mTileSizeX) {
                for (auto j = firstTileY; j < lastTileY; j += mTileSizeY) {
                    const auto color = math::vec4{0.5f, 0.1f, 0.1f, 0.8f};

                    const auto positionInWorld = math::vec3{i, j, 1.0f};
                    const auto roadID = createSpriteStaticEntity(backgroundEntities, color, roadTileSize,
                                                                 positionInWorld);

                    const auto packedIndices = packIntegers(i, j);
                    mPackedRoadIndicesToEntity.emplace(packedIndices, roadID);
                }
            }
        }

        void TileWorld::generateChunkOfTiles(const common::int64 xIndex, const common::int64 yIndex,
                                             entt::DefaultRegistry &backgroundEntities) {

            const auto firstTileX = xIndex * mChunkSizeX - mChunkSizeX / 2;
            const auto lastTileX = xIndex * mChunkSizeX + mChunkSizeX / 2;
            const auto firstTileY = yIndex * mChunkSizeY - mChunkSizeY / 2;
            const auto lastTileY = yIndex * mChunkSizeY + mChunkSizeY / 2;

            const auto tileSize = math::vec2{mTileSizeX, mTileSizeY};

            for (auto i = firstTileX; i < lastTileX; i += mTileSizeX) {
                for (auto j = firstTileY; j < lastTileY; j += mTileSizeY) {
                    const auto R = (std::rand() % 255) / 255.0f;
                    const auto G = (std::rand() % 255) / 255.0f;
                    const auto B = (std::rand() % 255) / 255.0f;
                    const auto color = math::vec4{R, G, B, 1.0f};

                    const auto positionInWorld = math::vec3{i, j, 1.0f};

                    const auto packedIndices = packIntegers(i, j);
                    const auto tileID = createSpriteStaticEntity(backgroundEntities, color, tileSize,
                                                                 positionInWorld);

                    mPackedTileIndicesToEntity.emplace(packedIndices, tileID);
                }
            }
        }

        entities::entityID TileWorld::createSkidTileIfMissing(const math::vec2 &position,
                                                              entt::DefaultRegistry &foregroundEntities) {
            const auto x = positionToIndex(position.x, mSkidTileSizeX);
            const auto y = positionToIndex(position.y, mSkidTileSizeY);
            const auto packedIndices = packIntegers(x, y);

            entities::entityID skidTileID{};
            if (!existsInMap(packedIndices, mPackedSkidIndicesToEntity)) {
                const auto skidIndexX = positionToIndex(position.x, mSkidTileSizeX);
                const auto skidIndexY = positionToIndex(position.y, mSkidTileSizeY);
                const auto skidTilePosX = indexToPosition(skidIndexX, mSkidTileSizeX, mHalfSkidTileSizeX);
                const auto skidTilePosY = indexToPosition(skidIndexY, mSkidTileSizeY, mHalfSkidTileSizeY);
                const auto positionInWorld = math::vec3{skidTilePosX, skidTilePosY, 1.0f};
                const auto skidTileSize = math::vec2{mSkidTileSizeX, mSkidTileSizeY};

                const auto skidWidthInPixels = static_cast<common::uint16>(mSkidTileSizeX * GAME_UNIT_TO_PIXELS +
                                                                           common::ep);
                const auto skidHeightInPixels = static_cast<common::uint16>(mSkidTileSizeY * GAME_UNIT_TO_PIXELS +
                                                                            common::ep);
                const auto skidDefaultPixel = oni::components::PixelRGBA{};
                // TODO: This function generates a texture and loads it into video memory, meaning we can not
                // blend two layers of skid mark onto each other
                const auto skidTexture = graphics::Texture::generate(skidWidthInPixels, skidHeightInPixels,
                                                                     skidDefaultPixel);

                skidTileID = entities::createTexturedStaticEntity(foregroundEntities, skidTexture,
                                                                  skidTileSize, positionInWorld);
                mPackedSkidIndicesToEntity.emplace(packedIndices, skidTileID);
            } else {
                skidTileID = mPackedSkidIndicesToEntity.at(packedIndices);
            }
            return skidTileID;
        }

        void TileWorld::updateSkidTexture(const math::vec3 &position, entities::entityID skidTextureEntity,
                                          entt::DefaultRegistry &foregroundEntities, common::uint8 alpha) {
            const auto skidMarksTexture = foregroundEntities.get<components::Texture>(skidTextureEntity);
            const auto skidMarksTexturePos = foregroundEntities.get<components::Shape>(skidTextureEntity).getPosition();

            auto skidPos = position;
            physics::Transformation::worldToLocalTextureTranslation(skidMarksTexturePos, GAME_UNIT_TO_PIXELS, skidPos);

            // TODO: I can not generate geometrical shapes that are rotated. Until I have that I will stick to
            // squares.
            //auto width = static_cast<int>(carConfig.wheelRadius * GAME_UNIT_TO_PIXELS * 2);
            //auto height = static_cast<int>(carConfig.wheelWidth * GAME_UNIT_TO_PIXELS / 2);
            common::real32 height = 5.0f;
            common::real32 width = 5.0f;

            const auto bits = graphics::Texture::generateBits(width, height, components::PixelRGBA{0, 0, 0, alpha});
            graphics::Texture::updateSubTexture(skidMarksTexture,
                                                static_cast<GLint>(skidPos.x - width / 2.0f),
                                                static_cast<GLint>(skidPos.y - height / 2.0f),
                                                width, height, bits);

        }
    }
}
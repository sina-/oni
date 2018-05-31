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
                mTileSizeX{2}, mTileSizeY{2},
                //mHalfTileSizeX{mTileSizeX / 2.0f},
                //mHalfTileSizeY{mTileSizeY / 2.0f},
                mSkidTileSizeX{64}, mSkidTileSizeY{64},
                mHalfSkidTileSizeX{mSkidTileSizeX / 2.0f},
                mHalfSkidTileSizeY{mSkidTileSizeY / 2.0f},
                mTilesPerChunkX{11}, mTilesPerChunkY{11},
                mChunkSizeX{mTileSizeX * mTilesPerChunkX}, mChunkSizeY{mTileSizeY * mTilesPerChunkY},
                mHalfChunkSizeX{mChunkSizeX / 2}, mHalfChunkSizeY{mChunkSizeY / 2} {
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
            auto result = floor(position / tileSize);
            auto truncated = static_cast<common::int64>(result);
            return truncated;
        }

        common::real32 TileWorld::indexToPosition(const common::int64 index, const common::uint16 tileSize) const {
            return tileSize * index;
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

        void TileWorld::tick(const math::vec2 &position, const components::Car &car,
                             entt::DefaultRegistry &foregroundEntities,
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
            const auto chunkIndices = chunkPositionToIndex(position);

            // NOTE: We always create and fill chunks in the current location and 8 adjacent chunks.
            // 1--2--3
            // |--|--|
            // 4--c--5
            // |--|--|
            // 6--7--8
            for (auto i = chunkIndices.x - 1; i <= chunkIndices.x + 1; ++i) {
                for (auto j = chunkIndices.y - 1; j <= chunkIndices.y + 1; ++j) {
                    const auto packedIndices = packIntegers(i, j);
                    if (!existsInMap(packedIndices, mPackedRoadChunkIndicesToEntity)) {
                        //generateTilesForChunk(i, j, backgroundEntities);

                        // NOTE: Just for debugging
                        auto R = (std::rand() % 255) / 255.0f;
                        auto G = (std::rand() % 255) / 255.0f;
                        auto B = (std::rand() % 255) / 255.0f;
                        auto color = math::vec4{R, G, B, 0.3f};
                        auto size = math::vec2{mChunkSizeX, mChunkSizeY};
                        auto currentChunkIndices = components::ChunkIndices{i, j};
                        auto chunkPosition = chunkIndexToPosition(currentChunkIndices);
                        auto positionInWorld = math::vec3{chunkPosition.x, chunkPosition.y, 1.0f};
                        auto chunkID = createSpriteStaticEntity(backgroundEntities, color, size, positionInWorld);

                        auto boarderRoadTiles = generateRoadsForChunk(currentChunkIndices, backgroundEntities);
                        auto chunk = components::Chunk{positionInWorld, packedIndices, boarderRoadTiles};
                        backgroundEntities.assign<components::Chunk>(chunkID, chunk);

                        mPackedRoadChunkIndicesToEntity.emplace(packedIndices, chunkID);
                    }
                }
            }
        }

        components::BoarderRoadTiles TileWorld::generateRoadsForChunk(const components::ChunkIndices &chunkIndices,
                                                                      entt::DefaultRegistry &backgroundEntities) {
            /**
             * 1. Check if there should be a road in this chunk
             * 2. Find the neighbours connected by road to current chunk
             * 3. Find if neighbours are initialized, if yes find the tile position on the boarder of the chunk
             *    that has a road choose the tile next to it in this chunk as the starting chunk, if neighbour is
             *    uninitialized pick a random tile. Repeat the same for the other chunk but this time assign an end
             *    tile.
             * 4. Connect starting tile to the ending tile.
             */

            components::BoarderRoadTiles boarderRoadTiles{};

            if (!chunkWithRoads(chunkIndices)) {
                return boarderRoadTiles;
            }
            auto northChunkIndices = components::ChunkIndices{chunkIndices.x, chunkIndices.y + 1};
            auto northChunkPacked = packIntegers(chunkIndices.x, chunkIndices.y);

            auto southChunkIndices = components::ChunkIndices{chunkIndices.x, chunkIndices.y - 1};
            auto southChunkPacked = packIntegers(southChunkIndices.x, southChunkIndices.y);

            auto westChunkIndices = components::ChunkIndices{chunkIndices.x - 1, chunkIndices.y};
            auto westChunkPacked = packIntegers(westChunkIndices.x, westChunkIndices.y);

            auto eastChunkIndices = components::ChunkIndices{chunkIndices.x + 1, chunkIndices.y};
            auto eastChunkPacked = packIntegers(eastChunkIndices.x, eastChunkIndices.y);

            auto northChunkHasRoads = chunkWithRoads(northChunkIndices);
            auto southChunkHasRoads = chunkWithRoads(southChunkIndices);
            auto westChunkHasRoads = chunkWithRoads(westChunkIndices);
            auto eastChunkHasRoads = chunkWithRoads(eastChunkIndices);

            auto neighboursRoadStatus = {northChunkHasRoads, southChunkHasRoads, westChunkHasRoads, eastChunkHasRoads};
            auto neighboursWithRoad = std::count_if(neighboursRoadStatus.begin(), neighboursRoadStatus.end(),
                                                    [](bool status) { return status; });
            ONI_DEBUG_ASSERT(neighboursWithRoad == 2);

            components::RoadTileIndices startingRoadTileIndices{0};

            components::RoadTileIndices endingRoadTileIndices{0};

            components::RoadTileIndices northBoarderRoadTileIndices{std::rand() % mTilesPerChunkX, mTilesPerChunkY - 1};

            components::RoadTileIndices southBoarderRoadTileIndices{std::rand() % mTilesPerChunkX, 0};

            components::RoadTileIndices westBoarderRoadTileIndices{0, std::rand() % mTilesPerChunkY};

            components::RoadTileIndices eastBoarderRoadTileIndices{mTilesPerChunkX - 1, std::rand() % mTilesPerChunkY};

            if (northChunkHasRoads && southChunkHasRoads) {
                boarderRoadTiles.southBoarder = components::RoadTileIndices{};
                boarderRoadTiles.northBoarder = components::RoadTileIndices{};

                if (existsInMap(southChunkPacked, mPackedRoadChunkIndicesToEntity)) {
                    auto southChunkID = mPackedRoadChunkIndicesToEntity.at(southChunkPacked);
                    const auto &southChunk = backgroundEntities.get<components::Chunk>(southChunkID);

                    boarderRoadTiles.southBoarder.x = southChunk.boarderRoadTiles.northBoarder.x;
                    boarderRoadTiles.southBoarder.y = 0;
                } else {
                    boarderRoadTiles.southBoarder = southBoarderRoadTileIndices;
                }
                if (existsInMap(northChunkPacked, mPackedRoadChunkIndicesToEntity)) {
                    auto northChunkID = mPackedRoadChunkIndicesToEntity.at(northChunkPacked);
                    const auto &northChunk = backgroundEntities.get<components::Chunk>(northChunkID);

                    boarderRoadTiles.northBoarder = northChunk.boarderRoadTiles.southBoarder;

                } else {
                    boarderRoadTiles.northBoarder = northBoarderRoadTileIndices;
                }

                startingRoadTileIndices = boarderRoadTiles.southBoarder;
                endingRoadTileIndices = boarderRoadTiles.northBoarder;

            } else if (northChunkHasRoads && eastChunkHasRoads) {

            } else if (northChunkHasRoads && westChunkHasRoads) {

            } else if (southChunkHasRoads && westChunkHasRoads) {

            } else if (southChunkHasRoads && eastChunkHasRoads) {

            } else if (westChunkHasRoads && eastChunkHasRoads) {
                boarderRoadTiles.westBoarder = components::RoadTileIndices{};
                boarderRoadTiles.eastBoarder = components::RoadTileIndices{};

                if (existsInMap(eastChunkPacked, mPackedRoadChunkIndicesToEntity)) {
                    auto eastChunkID = mPackedRoadChunkIndicesToEntity.at(eastChunkPacked);
                    const auto &eastChunk = backgroundEntities.get<components::Chunk>(eastChunkID);

                    boarderRoadTiles.eastBoarder.x = mTilesPerChunkX - 1;
                    boarderRoadTiles.eastBoarder.y = eastChunk.boarderRoadTiles.westBoarder.y;

                } else {
                    boarderRoadTiles.eastBoarder = eastBoarderRoadTileIndices;
                }

                if (existsInMap(westChunkPacked, mPackedRoadChunkIndicesToEntity)) {
                    auto westChunkID = mPackedRoadChunkIndicesToEntity.at(westChunkPacked);
                    const auto &westChunk = backgroundEntities.get<components::Chunk>(westChunkID);

                    boarderRoadTiles.westBoarder.x = 0;
                    boarderRoadTiles.westBoarder.y = westChunk.boarderRoadTiles.eastBoarder.y;
                } else {
                    boarderRoadTiles.westBoarder = westBoarderRoadTileIndices;
                }

                startingRoadTileIndices = boarderRoadTiles.westBoarder;
                endingRoadTileIndices = boarderRoadTiles.eastBoarder;
            } else {
                ONI_DEBUG_ASSERT(false);
            }

            generateRoadTileBetween(chunkIndices, startingRoadTileIndices, endingRoadTileIndices,
                                    backgroundEntities);

            return boarderRoadTiles;
        }

        void TileWorld::generateRoadTile(const components::ChunkIndices &chunkIndices,
                                         const components::RoadTileIndices &roadTileIndices,
                                         entt::DefaultRegistry &backgroundEntities) {
            const auto color = math::vec4{0.1f, 0.1f, 0.1f, 0.5f};
            const auto roadTileSize = math::vec2{mTileSizeX, mTileSizeY};

            const auto positionInWorld = roadTileInexToPosition(chunkIndices, roadTileIndices);
            const auto roadID = createSpriteStaticEntity(backgroundEntities, color, roadTileSize,
                                                         math::vec3{positionInWorld.x, positionInWorld.y,
                                                                    1.0f});

            const auto packedIndices = packIntegers(roadTileIndices.x, roadTileIndices.y);
            mPackedRoadTileToEntity.emplace(packedIndices, roadID);
        }

        void TileWorld::generateRoadTileBetween(const components::ChunkIndices &chunkIndices,
                                                components::RoadTileIndices startingRoadTileIndices,
                                                components::RoadTileIndices endingRoadTileIndices,
                                                entt::DefaultRegistry &backgroundEntities) {
            // Fill between tiles as if we are sweeping the Manhattan distance between them.
            while (startingRoadTileIndices.x < endingRoadTileIndices.x) {
                generateRoadTile(chunkIndices, startingRoadTileIndices, backgroundEntities);
                ++startingRoadTileIndices.x;
            }

            while (startingRoadTileIndices.x > endingRoadTileIndices.x) {
                generateRoadTile(chunkIndices, startingRoadTileIndices, backgroundEntities);
                --startingRoadTileIndices.x;
            }

/*            if (startTilePosX == endTilePosX) {
                if (startTilePosY > endTilePosY) {
                    startTilePosY -= mTileSizeY;
                }
                if (startTilePosY < endTilePosY) {
                    startTilePosY += mTileSizeY;
                }
            }
            startTilePosX -= mTileSizeX;*/

            while (startingRoadTileIndices.y < endingRoadTileIndices.y) {
                generateRoadTile(chunkIndices, startingRoadTileIndices, backgroundEntities);
                ++startingRoadTileIndices.y;
            }

            while (startingRoadTileIndices.y > endingRoadTileIndices.y) {
                generateRoadTile(chunkIndices, startingRoadTileIndices, backgroundEntities);
                --startingRoadTileIndices.y;
            }
        }

        void TileWorld::generateTilesForChunk(const common::int64 xIndex, const common::int64 yIndex,
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
                const auto skidTilePosX = indexToPosition(skidIndexX, mSkidTileSizeX);
                const auto skidTilePosY = indexToPosition(skidIndexY, mSkidTileSizeY);
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

        bool TileWorld::chunkWithRoads(const components::ChunkIndices &chunkIndices) const {
            return chunkIndices.y == 0;
        }

        math::vec2 TileWorld::chunkIndexToPosition(const components::ChunkIndices &chunkIndices) const {
            return math::vec2{chunkIndices.x * mChunkSizeX, chunkIndices.y * mChunkSizeY};
        }

        components::ChunkIndices TileWorld::chunkPositionToIndex(const math::vec2 &position) const {
            auto x = floor(position.x / mChunkSizeX);
            auto xIndex = static_cast<common::int64>(x);
            auto y = floor(position.y / mChunkSizeY);
            auto yIndex = static_cast<common::int64>(y);
            return components::ChunkIndices{xIndex, yIndex};
        }

        math::vec2 TileWorld::roadTileInexToPosition(const components::ChunkIndices &chunkIndices,
                                                     const components::RoadTileIndices roadTileIndices) const {

            const auto chunkPos = chunkIndexToPosition(chunkIndices);
            const auto tilePos = math::vec2{roadTileIndices.x * mTileSizeX, roadTileIndices.y * mTileSizeY};

            return chunkPos + tilePos;
        }

    }
}
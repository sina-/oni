#include <oni-core/entities/tile-world.h>

#include <ctime>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/utils/oni-assert.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/common/consts.h>


namespace oni {

    namespace entities {

        TileWorld::TileWorld() :
                mTileSizeX{10}, mTileSizeY{10},
                //mHalfTileSizeX{mTileSizeX / 2.0f},
                //mHalfTileSizeY{mTileSizeY / 2.0f},
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

            mNorthToEast = "resources/images/road/1/north-to-east.png";
            mNorthToSouth = "resources/images/road/1/north-to-south.png";
            mSouthToEast = "resources/images/road/1/south-to-east.png";
            mSouthToNorth = "resources/images/road/1/south-to-north.png";
            mWestToEast = "resources/images/road/1/west-to-east.png";
            mWestToNorth = "resources/images/road/1/west-to-north.png";
            mWestToSouth = "resources/images/road/1/west-to-south.png";
        }

        TileWorld::~TileWorld() = default;


        bool TileWorld::existsInMap(const common::uint64 packedIndices, const PackedIndiciesToEntity &map) const {
            return map.find(packedIndices) != map.end();
        }

        math::vec2 TileWorld::unpackCoordinates(common::uint64 coord) const {
            // TODO: This function is incorrect. Need to match it to packIntegers function if I ever use it
            ONI_DEBUG_ASSERT(false);
            //auto x = static_cast<int>(coord >> 32) * mTileSizeX;
            //auto y = static_cast<int>(coord & (0xFFFFFFFF)) * mTileSizeX;

            //return math::vec2{x, y};
            return math::vec2{};
        }

        void TileWorld::tick(entities::EntityManager &manager, const components::Car &car, const math::vec2 &position) {
            tickChunk(manager, position);
        }

        void TileWorld::tickChunk(entities::EntityManager &manager, const math::vec2 &position) {
            const auto chunkIndices = chunkPositionToIndex(position);

            // NOTE: We always create and fill chunks in the current location and 8 adjacent chunks.
            // 1--2--3
            // |--|--|
            // 4--c--5
            // |--|--|
            // 6--7--8
            for (auto i = chunkIndices.x - 1; i <= chunkIndices.x + 1; ++i) {
                for (auto j = chunkIndices.y - 1; j <= chunkIndices.y + 1; ++j) {
                    const auto packedIndices = math::packIntegers(i, j);
                    if (!existsInMap(packedIndices, mPackedRoadChunkIndicesToEntity)) {
                        //generateTilesForChunk(i, j, entities);

                        // NOTE: Just for debugging
                        auto R = (std::rand() % 255) / 255.0f;
                        auto G = (std::rand() % 255) / 255.0f;
                        auto B = (std::rand() % 255) / 255.0f;
                        auto color = math::vec4{R, G, B, 0.0f};
                        auto size = math::vec2{mChunkSizeX, mChunkSizeY};
                        auto currentChunkIndices = components::ChunkIndices{i, j};
                        auto chunkPosition = chunkIndexToPosition(currentChunkIndices);
                        auto positionInWorld = math::vec3{chunkPosition.x, chunkPosition.y, 1.0f};
                        auto chunkID = createSpriteStaticEntity(manager, color, size, positionInWorld);

                        auto boarderRoadTiles = generateRoadsForChunk(manager, currentChunkIndices);
                        auto chunk = components::Chunk{positionInWorld, packedIndices, boarderRoadTiles};
                        manager.assign<components::Chunk>(chunkID, chunk);

                        mPackedRoadChunkIndicesToEntity.emplace(packedIndices, chunkID);
                    }
                }
            }
        }

        components::BoarderRoadTiles TileWorld::generateRoadsForChunk(entities::EntityManager &manager,
                                                                      const components::ChunkIndices &chunkIndices) {
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
            auto northChunkPacked = math::packIntegers(chunkIndices.x, chunkIndices.y);

            auto southChunkIndices = components::ChunkIndices{chunkIndices.x, chunkIndices.y - 1};
            auto southChunkPacked = math::packIntegers(southChunkIndices.x, southChunkIndices.y);

            auto westChunkIndices = components::ChunkIndices{chunkIndices.x - 1, chunkIndices.y};
            auto westChunkPacked = math::packIntegers(westChunkIndices.x, westChunkIndices.y);

            auto eastChunkIndices = components::ChunkIndices{chunkIndices.x + 1, chunkIndices.y};
            auto eastChunkPacked = math::packIntegers(eastChunkIndices.x, eastChunkIndices.y);

            auto northChunkHasRoads = chunkWithRoads(northChunkIndices);
            auto southChunkHasRoads = chunkWithRoads(southChunkIndices);
            auto westChunkHasRoads = chunkWithRoads(westChunkIndices);
            auto eastChunkHasRoads = chunkWithRoads(eastChunkIndices);

            auto neighboursRoadStatus = {northChunkHasRoads, southChunkHasRoads, westChunkHasRoads, eastChunkHasRoads};
            auto neighboursWithRoad = std::count_if(neighboursRoadStatus.begin(), neighboursRoadStatus.end(),
                                                    [](bool status) { return status; });
            ONI_DEBUG_ASSERT(neighboursWithRoad == 2);

            components::RoadTileIndices startingRoadTileIndices{0, 0};

            components::RoadTileIndices endingRoadTileIndices{0, 0};

            components::RoadTileIndices northBoarderRoadTileIndices{std::rand() % mTilesPerChunkX, mTilesPerChunkY - 1};

            components::RoadTileIndices southBoarderRoadTileIndices{std::rand() % mTilesPerChunkX, 0};

            components::RoadTileIndices westBoarderRoadTileIndices{0, std::rand() % mTilesPerChunkY};

            components::RoadTileIndices eastBoarderRoadTileIndices{mTilesPerChunkX - 1, std::rand() % mTilesPerChunkY};

            if (northChunkHasRoads && southChunkHasRoads) {
                boarderRoadTiles.southBoarder = components::RoadTileIndices{};
                boarderRoadTiles.northBoarder = components::RoadTileIndices{};

                if (existsInMap(southChunkPacked, mPackedRoadChunkIndicesToEntity)) {
                    auto southChunkID = mPackedRoadChunkIndicesToEntity.at(southChunkPacked);
                    const auto &southChunk = manager.get<components::Chunk>(southChunkID);

                    boarderRoadTiles.southBoarder.x = southChunk.boarderRoadTiles.northBoarder.x;
                    boarderRoadTiles.southBoarder.y = 0;
                } else {
                    boarderRoadTiles.southBoarder = southBoarderRoadTileIndices;
                }
                if (existsInMap(northChunkPacked, mPackedRoadChunkIndicesToEntity)) {
                    auto northChunkID = mPackedRoadChunkIndicesToEntity.at(northChunkPacked);
                    const auto &northChunk = manager.get<components::Chunk>(northChunkID);

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
                    const auto &eastChunk = manager.get<components::Chunk>(eastChunkID);

                    boarderRoadTiles.eastBoarder.x = mTilesPerChunkX - 1;
                    boarderRoadTiles.eastBoarder.y = eastChunk.boarderRoadTiles.westBoarder.y;

                } else {
                    boarderRoadTiles.eastBoarder = eastBoarderRoadTileIndices;
                }

                if (existsInMap(westChunkPacked, mPackedRoadChunkIndicesToEntity)) {
                    auto westChunkID = mPackedRoadChunkIndicesToEntity.at(westChunkPacked);
                    const auto &westChunk = manager.get<components::Chunk>(westChunkID);

                    boarderRoadTiles.westBoarder.x = 0;
                    boarderRoadTiles.westBoarder.y = westChunk.boarderRoadTiles.eastBoarder.y;
                } else {
                    boarderRoadTiles.westBoarder = westBoarderRoadTileIndices;
                }

                startingRoadTileIndices = boarderRoadTiles.westBoarder;
                endingRoadTileIndices = boarderRoadTiles.eastBoarder;

                common::uint16 currentTileX = startingRoadTileIndices.x;
                common::uint16 currentTileY = startingRoadTileIndices.y;

                auto previousRoadTexture = mWestToEast;

                while (currentTileX < (endingRoadTileIndices.x + 1)) {
                    if (currentTileX == endingRoadTileIndices.x) {
                        // Make sure we connect to endingRoadTile
                        if (currentTileY == endingRoadTileIndices.y) {
                            if (previousRoadTexture == mWestToEast) {
                                generateTexturedRoadTile(manager, chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mWestToEast);
                            } else if (previousRoadTexture == mSouthToNorth) {
                                generateTexturedRoadTile(manager, chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mSouthToEast);
                            } else if (previousRoadTexture == mNorthToSouth) {
                                generateTexturedRoadTile(manager, chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mNorthToEast);
                            } else if (previousRoadTexture == mWestToSouth) {
                                generateTexturedRoadTile(manager, chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mNorthToEast);
                            } else if (previousRoadTexture == mWestToNorth) {
                                generateTexturedRoadTile(manager, chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mSouthToEast);
                            } else {
                                ONI_DEBUG_ASSERT(false);
                            }
                            break;
                            // We are done
                        } else if (currentTileY > endingRoadTileIndices.y) {
                            if (previousRoadTexture == mWestToEast) {
                                generateTexturedRoadTile(manager, chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mWestToSouth);
                                previousRoadTexture = mWestToSouth;
                            } else if (previousRoadTexture == mNorthToSouth) {
                                generateTexturedRoadTile(manager, chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mNorthToSouth);
                                previousRoadTexture = mNorthToSouth;
                            } else if (previousRoadTexture == mWestToSouth) {
                                generateTexturedRoadTile(manager, chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mNorthToSouth);
                                previousRoadTexture = mNorthToSouth;
                            } else {
                                ONI_DEBUG_ASSERT(false);
                            }
                            --currentTileY;
                            // go down
                        } else {
                            if (previousRoadTexture == mWestToEast) {
                                generateTexturedRoadTile(manager, chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mWestToNorth);
                                previousRoadTexture = mWestToNorth;
                            } else if (previousRoadTexture == mSouthToNorth) {
                                generateTexturedRoadTile(manager, chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mSouthToNorth);
                                previousRoadTexture = mSouthToNorth;
                            } else if (previousRoadTexture == mWestToNorth) {
                                generateTexturedRoadTile(manager, chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mSouthToNorth);
                                previousRoadTexture = mSouthToNorth;
                            } else {
                                ONI_DEBUG_ASSERT(false);
                            }
                            ++currentTileY;
                            // go up
                        }
                    } else {
                        // TODO: Randomly generate road instead of straight line
                        generateTexturedRoadTile(manager, chunkIndices,
                                                 components::RoadTileIndices{currentTileX, currentTileY},
                                                 mWestToEast);
                        ++currentTileX;
                    }
                }

            } else {
                ONI_DEBUG_ASSERT(false);
            }

/*            generateRoadTileBetween(chunkIndices, startingRoadTileIndices, endingRoadTileIndices,
                                    entities);*/

            return boarderRoadTiles;
        }

        void TileWorld::generateTexturedRoadTile(entities::EntityManager &manager,
                                                 const components::ChunkIndices &chunkIndices,
                                                 const components::RoadTileIndices &roadTileIndices,
                                                 const std::string &texturePath) {
            const auto roadTileSize = math::vec2{mTileSizeX, mTileSizeY};

            const auto positionInWorld = roadTileIndexToPosition(chunkIndices, roadTileIndices);
            const auto roadID = createStaticEntity(manager, roadTileSize,
                                                   math::vec3{positionInWorld.x, positionInWorld.y, 1.0f});

            auto texture = components::Texture{};
            texture.filePath = texturePath;
            texture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            manager.assign<components::Texture>(roadID, texture);

            const auto packedIndices = math::packIntegers(roadTileIndices.x, roadTileIndices.y);
            mPackedRoadTileToEntity.emplace(packedIndices, roadID);
        }

        void TileWorld::generateRoadTile(entities::EntityManager &manager, const components::ChunkIndices &chunkIndices,
                                         const components::RoadTileIndices &roadTileIndices) {
            const auto color = math::vec4{0.1f, 0.1f, 0.1f, 0.5f};
            const auto roadTileSize = math::vec2{mTileSizeX, mTileSizeY};

            const auto positionInWorld = roadTileIndexToPosition(chunkIndices, roadTileIndices);
            const auto roadID = createSpriteStaticEntity(manager, color, roadTileSize,
                                                         math::vec3{positionInWorld.x, positionInWorld.y, 1.0f});

            const auto packedIndices = math::packIntegers(roadTileIndices.x, roadTileIndices.y);
            mPackedRoadTileToEntity.emplace(packedIndices, roadID);
        }

        void TileWorld::generateRoadTileBetween(entities::EntityManager &manager,
                                                const components::ChunkIndices &chunkIndices,
                                                components::RoadTileIndices startingRoadTileIndices,
                                                components::RoadTileIndices endingRoadTileIndices) {
            // Fill between tiles as if we are sweeping the Manhattan distance between them.
            while (startingRoadTileIndices.x < endingRoadTileIndices.x) {
                generateRoadTile(manager, chunkIndices, startingRoadTileIndices);
                ++startingRoadTileIndices.x;
            }

            while (startingRoadTileIndices.x > endingRoadTileIndices.x) {
                generateRoadTile(manager, chunkIndices, startingRoadTileIndices);
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
                generateRoadTile(manager, chunkIndices, startingRoadTileIndices);
                ++startingRoadTileIndices.y;
            }

            while (startingRoadTileIndices.y > endingRoadTileIndices.y) {
                generateRoadTile(manager, chunkIndices, startingRoadTileIndices);
                --startingRoadTileIndices.y;
            }
        }

        void TileWorld::generateTilesForChunk(entities::EntityManager &manager, const common::int64 xIndex,
                                              const common::int64 yIndex) {

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

                    const auto packedIndices = math::packIntegers(i, j);
                    const auto tileID = createSpriteStaticEntity(manager, color, tileSize,
                                                                 positionInWorld);

                    mPackedTileIndicesToEntity.emplace(packedIndices, tileID);
                }
            }
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

        math::vec2 TileWorld::roadTileIndexToPosition(const components::ChunkIndices &chunkIndices,
                                                      const components::RoadTileIndices roadTileIndices) const {

            const auto chunkPos = chunkIndexToPosition(chunkIndices);
            const auto tilePos = math::vec2{roadTileIndices.x * mTileSizeX, roadTileIndices.y * mTileSizeY};

            return chunkPos + tilePos;
        }

    }
}
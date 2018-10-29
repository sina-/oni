#include <oni-core/entities/tile-world.h>

#include <ctime>

#include <Box2D/Box2D.h>

#include <oni-core/components/visual.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/entities/create-entity.h>
#include <oni-core/utils/oni-assert.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/io/output.h>


namespace oni {

    namespace entities {

        TileWorld::TileWorld(entities::EntityManager &manager, b2World &physicsWorld) :
                mEntityManager{manager},
                mPhysicsWorld{physicsWorld},
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

            mRaceTrack1 = "resources/images/race-track/2/1.png";
            mRaceTrack2 = "resources/images/race-track/2/2.png";
            mRaceTrack3 = "resources/images/race-track/2/3.png";
            mRaceTrack4 = "resources/images/race-track/2/4.png";

            // createWall(components::WallTilePosition::TOP, 1, 1);

            std::vector<components::WallTilePosition> wallPosInTile{};
            std::vector<components::TileIndices> wallTiles{};

            wallPosInTile.emplace_back(components::WallTilePosition::BOTTOM);
            wallTiles.emplace_back(components::TileIndices{-1, -1});

            wallPosInTile.emplace_back(components::WallTilePosition::BOTTOM);
            wallTiles.emplace_back(components::TileIndices{0, -1});

            wallPosInTile.emplace_back(components::WallTilePosition::BOTTOM);
            wallTiles.emplace_back(components::TileIndices{1, -1});

            wallPosInTile.emplace_back(components::WallTilePosition::RIGHT);
            wallTiles.emplace_back(components::TileIndices{1, -1});

            wallPosInTile.emplace_back(components::WallTilePosition::RIGHT);
            wallTiles.emplace_back(components::TileIndices{1, 0});

            wallPosInTile.emplace_back(components::WallTilePosition::RIGHT);
            wallTiles.emplace_back(components::TileIndices{1, 1});

            wallPosInTile.emplace_back(components::WallTilePosition::TOP);
            wallTiles.emplace_back(components::TileIndices{1, 1});

            wallPosInTile.emplace_back(components::WallTilePosition::TOP);
            wallTiles.emplace_back(components::TileIndices{0, 1});

            wallPosInTile.emplace_back(components::WallTilePosition::TOP);
            wallTiles.emplace_back(components::TileIndices{-1, 1});

            wallPosInTile.emplace_back(components::WallTilePosition::LEFT);
            wallTiles.emplace_back(components::TileIndices{-1, 1});

            wallPosInTile.emplace_back(components::WallTilePosition::LEFT);
            wallTiles.emplace_back(components::TileIndices{-1, 0});

            wallPosInTile.emplace_back(components::WallTilePosition::LEFT);
            wallTiles.emplace_back(components::TileIndices{-1, -1});

            createWall(wallPosInTile, wallTiles);
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

        void TileWorld::tick(const math::vec2 &position) {
            tickChunk(position);
        }

        void TileWorld::tickChunk(const math::vec2 &position) {
            const auto chunkIndices = chunkPositionToIndex(position);

            // NOTE: We always create and fill chunks in the current location and 8 adjacent chunks.
            // 1--2--3
            // |--|--|
            // 4--c--5
            // |--|--|
            // 6--7--8
            for (auto i = chunkIndices.x - 1; i <= chunkIndices.x + 1; ++i) {
                for (auto j = chunkIndices.y - 1; j <= chunkIndices.y + 1; ++j) {
                    auto packedIndices = math::packIntegers(i, j);
                    if (!existsInMap(packedIndices, mPackedRoadChunkIndicesToEntity)) {

                        //generateBackgroundForChunk(i, j);
                        generateTilesForChunk(i, j);

                        // NOTE: Just for debugging
/*                        auto R = 100;//(std::rand() % 255) / 255.0f;
                        auto G = 100;//(std::rand() % 255) / 255.0f;
                        auto B = 100;//(std::rand() % 255) / 255.0f;
                        auto color = math::vec4{R, G, B, 0.5f};
                        auto size = math::vec2{mChunkSizeX, mChunkSizeY};
                        auto currentChunkIndices = components::ChunkIndices{i, j};
                        auto chunkPosition = chunkIndexToPosition(currentChunkIndices);
                        auto positionInWorld = math::vec3{chunkPosition.x, chunkPosition.y, 1.0f};
                        auto chunkID = entities::createSpriteStaticEntity(manager, color, size, positionInWorld);

                        auto boarderRoadTiles = generateRoadsForChunk(manager, currentChunkIndices);
                        auto chunk = components::Chunk{positionInWorld, packedIndices, boarderRoadTiles};
                        manager.assign<components::Chunk>(chunkID, chunk);

                        mPackedRoadChunkIndicesToEntity.emplace(packedIndices, chunkID);
                        */
                    }
                }
            }
        }

        components::BoarderRoadTiles TileWorld::generateRoadsForChunk(const components::ChunkIndices &chunkIndices) {
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
                    const auto &southChunk = mEntityManager.get<components::Chunk>(southChunkID);

                    boarderRoadTiles.southBoarder.x = southChunk.boarderRoadTiles.northBoarder.x;
                    boarderRoadTiles.southBoarder.y = 0;
                } else {
                    boarderRoadTiles.southBoarder = southBoarderRoadTileIndices;
                }
                if (existsInMap(northChunkPacked, mPackedRoadChunkIndicesToEntity)) {
                    auto northChunkID = mPackedRoadChunkIndicesToEntity.at(northChunkPacked);
                    const auto &northChunk = mEntityManager.get<components::Chunk>(northChunkID);

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
                    const auto &eastChunk = mEntityManager.get<components::Chunk>(eastChunkID);

                    boarderRoadTiles.eastBoarder.x = mTilesPerChunkX - 1;
                    boarderRoadTiles.eastBoarder.y = eastChunk.boarderRoadTiles.westBoarder.y;

                } else {
                    boarderRoadTiles.eastBoarder = eastBoarderRoadTileIndices;
                }

                if (existsInMap(westChunkPacked, mPackedRoadChunkIndicesToEntity)) {
                    auto westChunkID = mPackedRoadChunkIndicesToEntity.at(westChunkPacked);
                    const auto &westChunk = mEntityManager.get<components::Chunk>(westChunkID);

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
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mWestToEast);
                            } else if (previousRoadTexture == mSouthToNorth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mSouthToEast);
                            } else if (previousRoadTexture == mNorthToSouth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mNorthToEast);
                            } else if (previousRoadTexture == mWestToSouth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mNorthToEast);
                            } else if (previousRoadTexture == mWestToNorth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mSouthToEast);
                            } else {
                                ONI_DEBUG_ASSERT(false);
                            }
                            break;
                            // We are done
                        } else if (currentTileY > endingRoadTileIndices.y) {
                            if (previousRoadTexture == mWestToEast) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mWestToSouth);
                                previousRoadTexture = mWestToSouth;
                            } else if (previousRoadTexture == mNorthToSouth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mNorthToSouth);
                                previousRoadTexture = mNorthToSouth;
                            } else if (previousRoadTexture == mWestToSouth) {
                                generateTexturedRoadTile(chunkIndices,
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
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mWestToNorth);
                                previousRoadTexture = mWestToNorth;
                            } else if (previousRoadTexture == mSouthToNorth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndices{currentTileX, currentTileY},
                                                         mSouthToNorth);
                                previousRoadTexture = mSouthToNorth;
                            } else if (previousRoadTexture == mWestToNorth) {
                                generateTexturedRoadTile(chunkIndices,
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
                        generateTexturedRoadTile(chunkIndices,
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

        void TileWorld::generateTexturedRoadTile(const components::ChunkIndices &chunkIndices,
                                                 const components::RoadTileIndices &roadTileIndices,
                                                 const std::string &texturePath) {
            const auto roadTileSize = math::vec2{mTileSizeX, mTileSizeY};

            const auto positionInWorld = roadTileIndexToPosition(chunkIndices, roadTileIndices);
            const auto roadID = createStaticEntity(mEntityManager, roadTileSize,
                                                   math::vec3{positionInWorld.x, positionInWorld.y, 1.0f});

            auto texture = components::Texture{};
            texture.filePath = texturePath;
            texture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            mEntityManager.assign<components::Texture>(roadID, texture);

            const auto packedIndices = math::packIntegers(roadTileIndices.x, roadTileIndices.y);
            mPackedRoadTileToEntity.emplace(packedIndices, roadID);
        }

        void TileWorld::generateRoadTile(const components::ChunkIndices &chunkIndices,
                                         const components::RoadTileIndices &roadTileIndices) {
            const auto color = math::vec4{0.1f, 0.1f, 0.1f, 0.5f};
            const auto roadTileSize = math::vec2{mTileSizeX, mTileSizeY};

            const auto positionInWorld = roadTileIndexToPosition(chunkIndices, roadTileIndices);
            const auto roadID = createSpriteStaticEntity(mEntityManager, color, roadTileSize,
                                                         math::vec3{positionInWorld.x, positionInWorld.y, 1.0f});

            const auto packedIndices = math::packIntegers(roadTileIndices.x, roadTileIndices.y);
            mPackedRoadTileToEntity.emplace(packedIndices, roadID);
        }

        void TileWorld::generateRoadTileBetween(const components::ChunkIndices &chunkIndices,
                                                components::RoadTileIndices startingRoadTileIndices,
                                                components::RoadTileIndices endingRoadTileIndices) {
            // Fill between tiles as if we are sweeping the Manhattan distance between them.
            while (startingRoadTileIndices.x < endingRoadTileIndices.x) {
                generateRoadTile(chunkIndices, startingRoadTileIndices);
                ++startingRoadTileIndices.x;
            }

            while (startingRoadTileIndices.x > endingRoadTileIndices.x) {
                generateRoadTile(chunkIndices, startingRoadTileIndices);
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
                generateRoadTile(chunkIndices, startingRoadTileIndices);
                ++startingRoadTileIndices.y;
            }

            while (startingRoadTileIndices.y > endingRoadTileIndices.y) {
                generateRoadTile(chunkIndices, startingRoadTileIndices);
                --startingRoadTileIndices.y;
            }
        }

        void TileWorld::generateTilesForChunk(common::int64 xChunkIndex,
                                              common::int64 yChunkIndex) {

            auto firstTileX = xChunkIndex * mChunkSizeX;
            auto lastTileX = xChunkIndex * mChunkSizeX + mChunkSizeX;
            auto firstTileY = yChunkIndex * mChunkSizeY;
            auto lastTileY = yChunkIndex * mChunkSizeY + mChunkSizeY;

            auto tileSize = math::vec2{mTileSizeX, mTileSizeY};

            for (auto i = firstTileX; i < lastTileX; i += mTileSizeX) {
                for (auto j = firstTileY; j < lastTileY; j += mTileSizeY) {
                    auto packedIndices = math::packIntegers(i, j);
                    // Chunks are created in batch, if single tile is created so are others.
                    if (existsInMap(packedIndices, mPackedTileIndicesToEntity)) {
                        return;
                    }
                    auto R = (std::rand() % 255) / 255.0f;
                    auto G = (std::rand() % 255) / 255.0f;
                    auto B = (std::rand() % 255) / 255.0f;
                    auto color = math::vec4{R, G, B, 1.0f};

                    auto positionInWorld = math::vec3{i, j, 1.0f};

                    auto tileID = createSpriteStaticEntity(mEntityManager, color, tileSize,
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

        void TileWorld::createWall(components::WallTilePosition position, common::int64 xTileIndex,
                                   common::int64 yTileIndex) {
            b2Vec2 vs[4];

            math::vec2 wallTextureSize{};
            math::vec3 wallPositionInWorld{};
            float wallWidth = 0.5f;

            switch (position) {
                case components::WallTilePosition::TOP: {
                    vs[0].Set(xTileIndex * mTileSizeX, yTileIndex * mTileSizeY + mTileSizeY);
                    vs[1].Set(xTileIndex * mTileSizeX + mTileSizeX, yTileIndex * mTileSizeY + mTileSizeY);
                    wallTextureSize.x = mTileSizeX;
                    wallTextureSize.y = wallWidth;
                    break;
                }
                case components::WallTilePosition::RIGHT: {
                    vs[0].Set(xTileIndex * mTileSizeX + mTileSizeX, yTileIndex * mTileSizeY);
                    vs[1].Set(xTileIndex * mTileSizeX + mTileSizeX, yTileIndex * mTileSizeY + mTileSizeY);
                    wallTextureSize.x = wallWidth;
                    wallTextureSize.y = mTileSizeY;
                    break;
                }
                case components::WallTilePosition::BOTTOM: {
                    vs[0].Set(xTileIndex * mTileSizeX, yTileIndex * mTileSizeY);
                    vs[1].Set(xTileIndex * mTileSizeX + mTileSizeX, yTileIndex * mTileSizeY);
                    wallTextureSize.x = mTileSizeX;
                    wallTextureSize.y = wallWidth;
                    break;
                }
                case components::WallTilePosition::LEFT: {
                    vs[0].Set(xTileIndex * mTileSizeX, yTileIndex * mTileSizeY);
                    vs[1].Set(xTileIndex * mTileSizeX, yTileIndex * mTileSizeY + mTileSizeY);
                    wallTextureSize.x = wallWidth;
                    wallTextureSize.y = mTileSizeY;
                    break;
                }
            }

            wallPositionInWorld.x = vs[0].x;
            wallPositionInWorld.y = vs[0].y;

            auto entityShapeWorld = components::Shape::fromSizeAndRotation(wallTextureSize, 0);
            physics::Transformation::localToWorldTranslation(wallPositionInWorld, entityShapeWorld);

            std::string wallTexturePath = "resources/images/wall/1/1.png";
            auto wallTexture = components::Texture{};
            wallTexture.filePath = wallTexturePath;
            wallTexture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;

            b2ChainShape chainShape;
            chainShape.CreateChain(vs, 2);

            b2BodyDef bd;
            auto chainBox = mPhysicsWorld.CreateBody(&bd);
            chainBox->CreateFixture(&chainShape, 0.0f);

            // TODO: Move this into create-entity.cpp
            common::EntityID entity{};
            {
                auto entityPhysicalProps = components::PhysicalProperties{chainBox};
                auto lock = mEntityManager.scopedLock();
                entity = mEntityManager.create();
                mEntityManager.assign<components::PhysicalProperties>(entity, entityPhysicalProps);
                mEntityManager.assign<components::Shape>(entity, entityShapeWorld);
                mEntityManager.assign<components::TagTextureShaded>(entity);
                mEntityManager.assign<components::TagStatic>(entity);
                mEntityManager.assign<components::TagAddNewEntities>(entity);
            }

            entities::assignTexture(mEntityManager, entity, wallTexture);
        }

        void TileWorld::createWall(const std::vector<components::WallTilePosition> &position,
                                   const std::vector<components::TileIndices> &indices) {
            assert(position.size() == indices.size());

            size_t wallCount = indices.size();
            size_t wallCornerCount = wallCount * 2;

            std::vector<common::EntityID> wallEntities{};
            wallEntities.reserve(wallCount);

            std::vector<b2Vec2> wallCorners;
            wallCorners.reserve(wallCornerCount);
            common::real32 wallWidth = 0.5f;

            // TODO: I should create the corners in two pass, first one sets them up as requested by the tiles,
            // second pass runs through each adjacent node and merges them if they are closer than 0.005 * 0.005 meters.
            for (size_t i = 0; i < wallCount; ++i) {
                const auto &wallPos = position[i];
                const auto &xTileIndex = indices[i].x;
                const auto &yTileIndex = indices[i].y;

                math::vec2 wallTextureSize{};
                components::Shape entityShapeWorld{};
                math::vec3 wallPositionInWorld{};
                std::string wallTexturePath{};

                auto bottomRight = math::vec2{xTileIndex * mTileSizeX + mTileSizeX - wallWidth,
                                              yTileIndex * mTileSizeY + wallWidth};

                auto topRight = math::vec2{xTileIndex * mTileSizeX + mTileSizeX - wallWidth,
                                           yTileIndex * mTileSizeY + mTileSizeY - wallWidth};

                auto topLeft = math::vec2{xTileIndex * mTileSizeX + wallWidth,
                                          yTileIndex * mTileSizeY + mTileSizeY - wallWidth};

                auto bottomLeft = math::vec2{xTileIndex * mTileSizeX + wallWidth,
                                             yTileIndex * mTileSizeY + wallWidth};

                // Note the counter clockwise winding: bottom -> right -> top -> left
                // Meaning right most node in bottom wall should be adjacent to bottom most node in right wall.
                switch (wallPos) {
                    case components::WallTilePosition::RIGHT: {
                        wallCorners[i * 2].Set(bottomRight.x, bottomRight.y);
                        wallCorners[i * 2 + 1].Set(topRight.x, topRight.y);

                        wallTextureSize.x = wallWidth;
                        wallTextureSize.y = mTileSizeY;
                        wallTexturePath = "resources/images/wall/1/vertical.png";
                        entityShapeWorld = components::Shape::fromSizeAndRotation(wallTextureSize, 0.0f);

                        wallPositionInWorld.x = bottomRight.x;
                        wallPositionInWorld.y = bottomLeft.y - wallWidth;
                        wallPositionInWorld.z = 1.0f;
                        break;
                    }
                    case components::WallTilePosition::TOP: {
                        wallCorners[i * 2].Set(topRight.x, topRight.y);
                        wallCorners[i * 2 + 1].Set(topLeft.x, topLeft.y);

                        wallTextureSize.x = mTileSizeX;
                        wallTextureSize.y = wallWidth;
                        wallTexturePath = "resources/images/wall/1/horizontal.png";
                        entityShapeWorld = components::Shape::fromSizeAndRotation(wallTextureSize, 0.0f);

                        wallPositionInWorld.x = topLeft.x - wallWidth;
                        wallPositionInWorld.y = topLeft.y;
                        wallPositionInWorld.z = 1.0f;
                        break;
                    }
                    case components::WallTilePosition::LEFT: {
                        wallCorners[i * 2].Set(topLeft.x,
                                               topLeft.y);
                        wallCorners[i * 2 + 1].Set(bottomLeft.x,
                                                   bottomLeft.y);

                        wallTextureSize.x = wallWidth;
                        wallTextureSize.y = mTileSizeY;
                        wallTexturePath = "resources/images/wall/1/vertical.png";
                        entityShapeWorld = components::Shape::fromSizeAndRotation(wallTextureSize, 0.0f);

                        wallPositionInWorld.x = bottomLeft.x - wallWidth;
                        wallPositionInWorld.y = bottomLeft.y - wallWidth;
                        wallPositionInWorld.z = 1.0f;
                        break;
                    }
                    case components::WallTilePosition::BOTTOM: {
                        wallCorners[i * 2].Set(bottomLeft.x, bottomLeft.y);
                        wallCorners[i * 2 + 1].Set(bottomRight.x, bottomRight.y);

                        wallTextureSize.x = mTileSizeX;
                        wallTextureSize.y = wallWidth;
                        wallTexturePath = "resources/images/wall/1/horizontal.png";
                        entityShapeWorld = components::Shape::fromSizeAndRotation(wallTextureSize, 0.0f);

                        wallPositionInWorld.x = bottomLeft.x - wallWidth;
                        wallPositionInWorld.y = bottomLeft.y - wallWidth;
                        wallPositionInWorld.z = 1.0f;
                        break;
                    }
                }

                physics::Transformation::localToWorldTranslation(wallPositionInWorld, entityShapeWorld);

                auto wallTexture = components::Texture{};
                wallTexture.filePath = wallTexturePath;
                wallTexture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;

                // TODO: Move this into create-entity.cpp
                common::EntityID entity{};
                {
                    auto lock = mEntityManager.scopedLock();
                    entity = mEntityManager.create();
                    mEntityManager.assign<components::Shape>(entity, entityShapeWorld);
                    mEntityManager.assign<components::TagTextureShaded>(entity);
                    mEntityManager.assign<components::TagStatic>(entity);
                    mEntityManager.assign<components::TagAddNewEntities>(entity);

                    wallEntities.push_back(entity);
                }

                entities::assignTexture(mEntityManager, entity, wallTexture);
            }

            std::vector<b2Vec2> mergedWallCorners;
            common::real32 minimumAllowedCornerDistance = b2_linearSlop * b2_linearSlop;

            for (size_t i = 1; i < wallCornerCount; ++i) {
                const auto &previousCorner = wallCorners[i - 1];
                const auto &currentCorner = wallCorners[i];
                auto distance = b2DistanceSquared(previousCorner, currentCorner);

                if (distance <= minimumAllowedCornerDistance) {
                    continue;
                } else {
                    mergedWallCorners.push_back(currentCorner);
                }
            }

            b2ChainShape chainShape;
            chainShape.CreateLoop(mergedWallCorners.data(), static_cast<common::int32>(mergedWallCorners.size()));

            b2BodyDef bd;
            auto chainBox = mPhysicsWorld.CreateBody(&bd);
            chainBox->CreateFixture(&chainShape, 0.0f);

            for (auto &&entity: wallEntities) {
                auto entityPhysicalProps = components::PhysicalProperties{chainBox};
                mEntityManager.assign<components::PhysicalProperties>(entity, entityPhysicalProps);
            }
        }

        void TileWorld::generateBackgroundForChunk(common::int64 chunkX, common::int64 chunkY) {
            auto chunkIndex = components::ChunkIndices{chunkX, chunkY};
            auto positionInWorld = chunkIndexToPosition(chunkIndex);
            auto roadID = createStaticEntity(mEntityManager, math::vec2{mChunkSizeX, mChunkSizeY},
                                             math::vec3{positionInWorld.x, positionInWorld.y, 1.0f});

            auto packedIndices = math::packIntegers(chunkX, chunkY);
            mPackedRoadChunkIndicesToEntity.emplace(packedIndices, roadID);

            auto texture = components::Texture{};
            texture.filePath = mRaceTrack1;
            texture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            mEntityManager.assign<components::Texture>(roadID, texture);


        }
    }
}
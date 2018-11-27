#include <oni-core/entities/tile-world.h>

#include <ctime>

#include <Box2D/Box2D.h>

#include <oni-core/components/visual.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/entities/create-entity.h>
#include <oni-core/physics/transformation.h>


namespace oni {

    namespace entities {

        TileWorld::TileWorld(entities::EntityManager &manager, b2World &physicsWorld) :
                mEntityManager{manager},
                mPhysicsWorld{physicsWorld},
                mTileSizeX{10}, mTileSizeY{10},
                //mHalfTileSizeX{mTileSizeX / 2.0f},
                //mHalfTileSizeY{mTileSizeY / 2.0f},
                mTilesPerChunkX{11}, mTilesPerChunkY{11},
                mChunkSizeX{static_cast<common::uint16 >(mTileSizeX * mTilesPerChunkX)},
                mChunkSizeY{static_cast<common::uint16 >(mTileSizeY * mTilesPerChunkY)},
                mHalfChunkSizeX{static_cast<common::uint16 >(mChunkSizeX / 2)},
                mHalfChunkSizeY{static_cast<common::uint16>(mChunkSizeY / 2)} {
            std::srand(std::time(nullptr));

            // NOTE: A road chunk is filled with road tiles, therefore road chunk size should be,
            // dividable by road tile size.
            assert(mTileSizeX <= mChunkSizeX);
            assert(mTileSizeY <= mChunkSizeY);
            assert(mChunkSizeX % mTileSizeX == 0);
            assert(mChunkSizeY % mTileSizeY == 0);

            // NOTE: If number of road tiles in a chunk is not an odd number it means there is no middle
            // tile. For convenience its good to have a middle tile.
            assert((mChunkSizeX / mTileSizeX) % 2 == 1);
            assert((mChunkSizeY / mTileSizeY) % 2 == 1);

            assert(mChunkSizeX % 2 == 0);
            assert(mChunkSizeY % 2 == 0);

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

            for (int i = -2; i <= 2; ++i) {
                for (int j = -2; j <= 2; ++j) {
                    generateChunkBackground(i, j);
                }
            }

            std::vector<components::WallTilePosition> wallPosInTile{};
            std::vector<components::TileIndex> wallTiles{};

            common::int8 outerTrackWidth = 8;
            common::int8 outerTrackHeight = 4;

            for (auto i = -outerTrackWidth; i <= outerTrackWidth; ++i) {
                wallPosInTile.emplace_back(components::WallTilePosition::BOTTOM);
                wallTiles.emplace_back(components::TileIndex{i, -outerTrackHeight});
            }

            for (auto i = -outerTrackHeight; i <= outerTrackHeight; ++i) {
                wallPosInTile.emplace_back(components::WallTilePosition::RIGHT);
                wallTiles.emplace_back(components::TileIndex{outerTrackWidth, i});
            }

            for (auto i = -outerTrackWidth; i <= outerTrackWidth; ++i) {
                wallPosInTile.emplace_back(components::WallTilePosition::TOP);
                wallTiles.emplace_back(components::TileIndex{i, outerTrackHeight});
            }

            for (auto i = -outerTrackHeight; i <= outerTrackHeight; ++i) {
                wallPosInTile.emplace_back(components::WallTilePosition::LEFT);
                wallTiles.emplace_back(components::TileIndex{-outerTrackWidth, i});
            }

            common::int8 innerTrackWidth = 6;
            common::int8 innerTrackHeight = 2;

            for (auto i = -innerTrackWidth; i <= innerTrackWidth; ++i) {
                wallPosInTile.emplace_back(components::WallTilePosition::BOTTOM);
                wallTiles.emplace_back(components::TileIndex{i, -innerTrackHeight});
            }

            for (auto i = -innerTrackHeight; i <= innerTrackHeight; ++i) {
                wallPosInTile.emplace_back(components::WallTilePosition::RIGHT);
                wallTiles.emplace_back(components::TileIndex{innerTrackWidth, i});
            }

            for (auto i = -innerTrackWidth; i <= innerTrackWidth; ++i) {
                wallPosInTile.emplace_back(components::WallTilePosition::TOP);
                wallTiles.emplace_back(components::TileIndex{i, innerTrackHeight});
            }

            for (auto i = -innerTrackHeight; i <= innerTrackHeight; ++i) {
                wallPosInTile.emplace_back(components::WallTilePosition::LEFT);
                wallTiles.emplace_back(components::TileIndex{-innerTrackWidth, i});
            }

            createWall(wallPosInTile, wallTiles);
        }

        TileWorld::~TileWorld() = default;


        bool TileWorld::existsInMap(const common::uint64 packedIndex, const PackedIndexToEntityID &map) const {
            return map.find(packedIndex) != map.end();
        }

        math::vec2 TileWorld::unpackCoordinates(common::uint64 coord) const {
            // TODO: This function is incorrect. Need to match it to packIntegers function if I ever use it
            assert(false);
            //auto x = static_cast<int>(coord >> 32) * mTileSizeX;
            //auto y = static_cast<int>(coord & (0xFFFFFFFF)) * mTileSizeX;

            //return math::vec2{x, y};
            return math::vec2{};
        }

        void TileWorld::tick(const math::vec2 &position) {
            tickChunk(position);
        }

        void TileWorld::tickChunk(const math::vec2 &position) {
            const auto chunkIndices = chunkPosToIndex(position);

            // NOTE: We always create and fill chunks in the current location and 8 adjacent chunks.
            // 1--2--3
            // |--|--|
            // 4--c--5
            // |--|--|
            // 6--7--8
            for (auto i = chunkIndices.x - 1; i <= chunkIndices.x + 1; ++i) {
                for (auto j = chunkIndices.y - 1; j <= chunkIndices.y + 1; ++j) {
                    auto chunkIndex = math::packIntegers(i, j);
                    if (!existsInMap(chunkIndex, mChunkLookup)) {

                        // generateChunkBackground(i, j);
                        //generateTilesForChunk(i, j);
/*
                        // NOTE: Just for debugging
                        auto R = 100;//(std::rand() % 255) / 255.0f;
                        auto G = 100;//(std::rand() % 255) / 255.0f;
                        auto B = 100;//(std::rand() % 255) / 255.0f;
                        auto color = math::vec4{R, G, B, 0.5f};
                        auto size = math::vec2{mChunkSizeX, mChunkSizeY};
                        auto currentChunkIndices = components::ChunkIndex{i, j};
                        auto chunkPosition = chunkIndexToPosition(currentChunkIndices);
                        auto worldPos = math::vec3{chunkPosition.x, chunkPosition.y, 1.0f};
                        auto chunkID = entities::createSpriteStaticEntity(manager, color, size, worldPos);

                        auto boarderRoadTiles = generateRoadsForChunk(manager, currentChunkIndices);
                        auto chunk = components::Chunk{worldPos, packedIndex, boarderRoadTiles};
                        manager.assign<components::Chunk>(chunkID, chunk);

                        mChunkLookup.emplace(packedIndex, chunkID);*/
                    }
                }
            }
        }

        components::BoarderRoadTiles TileWorld::generateRoadsForChunk(const components::ChunkIndex &chunkIndices) {
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

            if (!shouldGenerateRoad(chunkIndices)) {
                return boarderRoadTiles;
            }

            auto northChunkIndices = components::ChunkIndex{chunkIndices.x, chunkIndices.y + 1};
            auto northChunkPacked = math::packIntegers(chunkIndices.x, chunkIndices.y);

            auto southChunkIndices = components::ChunkIndex{chunkIndices.x, chunkIndices.y - 1};
            auto southChunkPacked = math::packIntegers(southChunkIndices.x, southChunkIndices.y);

            auto westChunkIndices = components::ChunkIndex{chunkIndices.x - 1, chunkIndices.y};
            auto westChunkPacked = math::packIntegers(westChunkIndices.x, westChunkIndices.y);

            auto eastChunkIndices = components::ChunkIndex{chunkIndices.x + 1, chunkIndices.y};
            auto eastChunkPacked = math::packIntegers(eastChunkIndices.x, eastChunkIndices.y);

            auto northChunkHasRoads = shouldGenerateRoad(northChunkIndices);
            auto southChunkHasRoads = shouldGenerateRoad(southChunkIndices);
            auto westChunkHasRoads = shouldGenerateRoad(westChunkIndices);
            auto eastChunkHasRoads = shouldGenerateRoad(eastChunkIndices);

            auto neighboursRoadStatus = {northChunkHasRoads, southChunkHasRoads, westChunkHasRoads, eastChunkHasRoads};
            auto neighboursWithRoad = std::count_if(neighboursRoadStatus.begin(), neighboursRoadStatus.end(),
                                                    [](bool status) { return status; });
            assert(neighboursWithRoad == 2);

            components::RoadTileIndex startingRoadTileIndices{0, 0};

            components::RoadTileIndex endingRoadTileIndices{0, 0};

            components::RoadTileIndex northBoarderRoadTileIndices{static_cast<uint16>(std::rand() % mTilesPerChunkX),
                                                                  static_cast<uint16>(mTilesPerChunkY - 1)};

            components::RoadTileIndex southBoarderRoadTileIndices{static_cast<uint16>(std::rand() % mTilesPerChunkX),
                                                                  0};

            components::RoadTileIndex westBoarderRoadTileIndices{0,
                                                                 static_cast<uint16>(std::rand() % mTilesPerChunkY)};

            components::RoadTileIndex eastBoarderRoadTileIndices{static_cast<uint16>(mTilesPerChunkX - 1),
                                                                 static_cast<uint16>(std::rand() % mTilesPerChunkY)};

            if (northChunkHasRoads && southChunkHasRoads) {
                boarderRoadTiles.southBoarder = components::RoadTileIndex{};
                boarderRoadTiles.northBoarder = components::RoadTileIndex{};

                if (existsInMap(southChunkPacked, mChunkLookup)) {
                    auto southChunkID = mChunkLookup.at(southChunkPacked);
                    const auto &southChunk = mEntityManager.get<components::Chunk>(southChunkID);

                    boarderRoadTiles.southBoarder.x = southChunk.boarderRoadTiles.northBoarder.x;
                    boarderRoadTiles.southBoarder.y = 0;
                } else {
                    boarderRoadTiles.southBoarder = southBoarderRoadTileIndices;
                }
                if (existsInMap(northChunkPacked, mChunkLookup)) {
                    auto northChunkID = mChunkLookup.at(northChunkPacked);
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
                boarderRoadTiles.westBoarder = components::RoadTileIndex{};
                boarderRoadTiles.eastBoarder = components::RoadTileIndex{};

                if (existsInMap(eastChunkPacked, mChunkLookup)) {
                    auto eastChunkID = mChunkLookup.at(eastChunkPacked);
                    const auto &eastChunk = mEntityManager.get<components::Chunk>(eastChunkID);

                    boarderRoadTiles.eastBoarder.x = mTilesPerChunkX - 1;
                    boarderRoadTiles.eastBoarder.y = eastChunk.boarderRoadTiles.westBoarder.y;

                } else {
                    boarderRoadTiles.eastBoarder = eastBoarderRoadTileIndices;
                }

                if (existsInMap(westChunkPacked, mChunkLookup)) {
                    auto westChunkID = mChunkLookup.at(westChunkPacked);
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
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mWestToEast);
                            } else if (previousRoadTexture == mSouthToNorth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mSouthToEast);
                            } else if (previousRoadTexture == mNorthToSouth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mNorthToEast);
                            } else if (previousRoadTexture == mWestToSouth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mNorthToEast);
                            } else if (previousRoadTexture == mWestToNorth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mSouthToEast);
                            } else {
                                assert(false);
                            }
                            break;
                            // We are done
                        } else if (currentTileY > endingRoadTileIndices.y) {
                            if (previousRoadTexture == mWestToEast) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mWestToSouth);
                                previousRoadTexture = mWestToSouth;
                            } else if (previousRoadTexture == mNorthToSouth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mNorthToSouth);
                                previousRoadTexture = mNorthToSouth;
                            } else if (previousRoadTexture == mWestToSouth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mNorthToSouth);
                                previousRoadTexture = mNorthToSouth;
                            } else {
                                assert(false);
                            }
                            --currentTileY;
                            // go down
                        } else {
                            if (previousRoadTexture == mWestToEast) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mWestToNorth);
                                previousRoadTexture = mWestToNorth;
                            } else if (previousRoadTexture == mSouthToNorth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mSouthToNorth);
                                previousRoadTexture = mSouthToNorth;
                            } else if (previousRoadTexture == mWestToNorth) {
                                generateTexturedRoadTile(chunkIndices,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mSouthToNorth);
                                previousRoadTexture = mSouthToNorth;
                            } else {
                                assert(false);
                            }
                            ++currentTileY;
                            // go up
                        }
                    } else {
                        // TODO: Randomly generate road instead of straight line
                        generateTexturedRoadTile(chunkIndices,
                                                 components::RoadTileIndex{currentTileX, currentTileY},
                                                 mWestToEast);
                        ++currentTileX;
                    }
                }

            } else {
                assert(false);
            }

/*            generateRoadTileBetween(chunkIndices, startingRoadTileIndices, endingRoadTileIndices,
                                    entities);*/

            return boarderRoadTiles;
        }

        void TileWorld::generateTexturedRoadTile(const components::ChunkIndex &chunkIndices,
                                                 const components::RoadTileIndex &roadTileIndices,
                                                 const std::string &texturePath) {
            auto tileSize = getTileSize();

            auto worldPos = roadTileIndexToPos(chunkIndices, roadTileIndices);
            auto lock = mEntityManager.scopedLock();
            auto entityID = entities::createEntity(mEntityManager);
            entities::assignShapeWold(mEntityManager, entityID, tileSize, worldPos);
            entities::assignTagStatic(mEntityManager, entityID);

            auto texture = components::Texture{};
            texture.filePath = texturePath;
            texture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            mEntityManager.assign<components::Texture>(entityID, texture);

            const auto packedIndex = math::packIntegers(roadTileIndices.x, roadTileIndices.y);
            mRoadLookup.emplace(packedIndex, entityID);
        }

        void TileWorld::generateRoadTile(const components::ChunkIndex &chunkIndices,
                                         const components::RoadTileIndex &roadTileIndices) {
            const auto color = math::vec4{0.1f, 0.1f, 0.1f, 0.5f};
            const auto roadTileSize = getTileSize();

            const auto worldPos = roadTileIndexToPos(chunkIndices, roadTileIndices);
            const auto roadID = createSpriteStaticEntity(mEntityManager, color, roadTileSize,
                                                         worldPos);

            const auto packedIndex = math::packIntegers(roadTileIndices.x, roadTileIndices.y);
            mRoadLookup.emplace(packedIndex, roadID);
        }

        void TileWorld::generateRoadTileBetween(const components::ChunkIndex &chunkIndices,
                                                components::RoadTileIndex startingRoadTileIndices,
                                                components::RoadTileIndex endingRoadTileIndices) {
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

            auto tileSize = getTileSize();

            for (auto i = firstTileX; i < lastTileX; i += mTileSizeX) {
                for (auto j = firstTileY; j < lastTileY; j += mTileSizeY) {
                    auto packedIndex = math::packIntegers(i, j);
                    // Chunks are created in batch, if single tile is created so are others.
                    if (existsInMap(packedIndex, mTileLookup)) {
                        return;
                    }
                    auto R = (std::rand() % 255) / 255.0f;
                    auto G = (std::rand() % 255) / 255.0f;
                    auto B = (std::rand() % 255) / 255.0f;
                    auto color = math::vec4{R, G, B, 1.0f};

                    auto worldPos = math::vec3{static_cast<common::real32>(i), static_cast<common::real32>(j),
                                               1.0f};

                    auto tileID = createSpriteStaticEntity(mEntityManager, color, tileSize,
                                                           worldPos);

                    mTileLookup.emplace(packedIndex, tileID);
                }
            }
        }

        bool TileWorld::shouldGenerateRoad(const components::ChunkIndex &chunkIndices) const {
            return chunkIndices.y == 0;
        }

        math::vec2 TileWorld::chunkIndexToPos(const components::ChunkIndex &chunkIndices) const {
            return math::vec2{static_cast<common::real32>(chunkIndices.x * mChunkSizeX),
                              static_cast<common::real32>(chunkIndices.y * mChunkSizeY)};
        }

        components::ChunkIndex TileWorld::chunkPosToIndex(const math::vec2 &position) const {
            auto x = floor(position.x / mChunkSizeX);
            auto xIndex = static_cast<common::int64>(x);
            auto y = floor(position.y / mChunkSizeY);
            auto yIndex = static_cast<common::int64>(y);
            return components::ChunkIndex{xIndex, yIndex};
        }

        math::vec3 TileWorld::roadTileIndexToPos(const components::ChunkIndex &chunkIndices,
                                                 components::RoadTileIndex roadTileIndices) const {

            auto chunkPos = chunkIndexToPos(chunkIndices);
            auto tilePos = math::vec2{static_cast<common::real32>(roadTileIndices.x * mTileSizeX),
                                      static_cast<common::real32>(roadTileIndices.y * mTileSizeY)};
            math::vec3 pos{chunkPos.x + tilePos.x, chunkPos.y + tilePos.y, 1.f};
            return pos;
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
                mEntityManager.assign<components::Tag_TextureShaded>(entity);
                mEntityManager.assign<components::Tag_Static>(entity);
                mEntityManager.assign<components::Tag_NewEntity>(entity);
            }

            entities::assignTexture(mEntityManager, entity, wallTexture);
        }

        void TileWorld::createWall(const std::vector<components::WallTilePosition> &position,
                                   const std::vector<components::TileIndex> &indices) {
            assert(position.size() == indices.size());

            size_t wallCount = indices.size();

            std::vector<common::EntityID> wallEntities{};
            wallEntities.reserve(wallCount);

            common::real32 wallWidth = 0.5f;

            for (size_t i = 0; i < wallCount; ++i) {
                const auto &wallPos = position[i];
                const auto &xTileIndex = indices[i].x;
                const auto &yTileIndex = indices[i].y;

                math::vec3 wallPositionInWorld;
                math::vec2 wallSize;
                std::string wallTexturePath;

                common::real32 currentTileX = xTileIndex * mTileSizeX;
                common::real32 currentTileY = yTileIndex * mTileSizeY;

                switch (wallPos) {
                    case components::WallTilePosition::RIGHT: {
                        wallSize.x = wallWidth;
                        wallSize.y = mTileSizeY - 2 * wallWidth;
                        wallTexturePath = "resources/images/wall/1/vertical.png";

                        wallPositionInWorld.x = currentTileX + mTileSizeX - wallWidth;
                        wallPositionInWorld.y = currentTileY + wallWidth;
                        wallPositionInWorld.z = 1.0f;
                        break;
                    }
                    case components::WallTilePosition::TOP: {
                        wallSize.x = mTileSizeX - 2 * wallWidth;
                        wallSize.y = wallWidth;
                        wallTexturePath = "resources/images/wall/1/horizontal.png";

                        wallPositionInWorld.x = currentTileX + wallWidth;
                        wallPositionInWorld.y = currentTileY + mTileSizeY - wallWidth;
                        wallPositionInWorld.z = 1.0f;
                        break;
                    }
                    case components::WallTilePosition::LEFT: {
                        wallSize.x = wallWidth;
                        wallSize.y = mTileSizeY - 2 * wallWidth;
                        wallTexturePath = "resources/images/wall/1/vertical.png";

                        wallPositionInWorld.x = currentTileX;
                        wallPositionInWorld.y = currentTileY + wallWidth;
                        wallPositionInWorld.z = 1.0f;
                        break;
                    }
                    case components::WallTilePosition::BOTTOM: {
                        wallSize.x = mTileSizeX - 2 * wallWidth;
                        wallSize.y = wallWidth;
                        wallTexturePath = "resources/images/wall/1/horizontal.png";

                        wallPositionInWorld.x = currentTileX + wallWidth;
                        wallPositionInWorld.y = currentTileY;
                        wallPositionInWorld.z = 1.0f;
                        break;
                    }
                }

                // TODO: Maybe you want to keep these somewhere
                auto wallEntity = entities::createStaticPhysicsEntity(mEntityManager,
                                                                      mPhysicsWorld,
                                                                      wallSize,
                                                                      wallPositionInWorld);

                auto wallTexture = components::Texture{};
                wallTexture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
                wallTexture.filePath = wallTexturePath;
                entities::assignTexture(mEntityManager, wallEntity, wallTexture);
            }
        }

        void TileWorld::generateChunkBackground(common::int64 chunkX, common::int64 chunkY) {
            auto chunkIndex = components::ChunkIndex{chunkX, chunkY};
            auto worldPos = chunkIndexToPos(chunkIndex);
            auto lock = mEntityManager.scopedLock();
            auto entityID = entities::createEntity(mEntityManager);
            entities::assignShapeWold(mEntityManager, entityID, getChunkSize(),
                                      math::vec3{worldPos.x, worldPos.y, 1.f});
            entities::assignTagStatic(mEntityManager, entityID);

            auto packed = math::packIntegers(chunkX, chunkY);
            mChunkLookup.emplace(packed, entityID);

            auto texture = components::Texture{};
            texture.filePath = mRaceTrack1;
            texture.status = components::TextureStatus::NEEDS_LOADING_USING_PATH;
            entities::assignTexture(mEntityManager, entityID, texture);
        }

        math::vec2 TileWorld::getTileSize() const {
            return math::vec2{static_cast<common::real32>(mTileSizeX), static_cast<common::real32 >(mTileSizeY)};
        }

        math::vec2 TileWorld::getChunkSize() const {
            return math::vec2{static_cast<common::real32>(mChunkSizeX), static_cast<common::real32 >(mChunkSizeY)};
        }
    }
}
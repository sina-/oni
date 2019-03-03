#include <oni-server/entities/tile-world.h>

#include <ctime>

#include <Box2D/Box2D.h>

#include <oni-core/entities/entity-manager.h>
#include <oni-core/entities/entity-factory.h>


namespace oni {
    namespace server {
        namespace entities {
            TileWorld::TileWorld(oni::entities::EntityFactory &entityFactory,
                                 b2World &physicsWorld,
                                 const math::ZLayerManager &zLevel) :
                    mEntityFactory{entityFactory},
                    mPhysicsWorld{physicsWorld},
                    mZLayerManager{zLevel},
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

                mGroundZ = mZLayerManager.getZForEntity(component::EntityType::BACKGROUND);
                mRoadZ = mZLayerManager.getZForEntity(component::EntityType::ROAD);
                mWallZ = mZLayerManager.getZForEntity(component::EntityType::WALL);
            }

            TileWorld::~TileWorld() = default;


            bool TileWorld::isInMap(common::uint64 packedIndex,
                                    const std::map<common::uint64, common::EntityID> &map) const {
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
                auto chunkIndex = groundChunkPosToIndex(position);

                // NOTE: We always create and fill chunks in the current location and 8 adjacent chunks.
                // 1--2--3
                // |--|--|
                // 4--c--5
                // |--|--|
                // 6--7--8
                for (auto i = chunkIndex.x - 1; i <= chunkIndex.x + 1; ++i) {
                    for (auto j = chunkIndex.y - 1; j <= chunkIndex.y + 1; ++j) {
                        auto chunkID = math::packIntegers(i, j);
                        if (!isInMap(chunkID, mChunkLookup)) {
                            // genChunkTexture(i, j);
                            // genChunkTiles(i, j);
                            // genChunkRoads(i, j);
                        }
                    }
                }
            }

            void TileWorld::genChunkRoads(common::int64 chunkX, common::int64 chunkY) {
                /**
                 * 1. Check if there should be a road in this chunk
                 * 2. Find the neighbours connected by road to current chunk
                 * 3. Find if neighbours are initialized, if yes find the tile position on the boarder of the chunk
                 *    that has a road choose the tile next to it in this chunk as the starting chunk, if neighbour is
                 *    uninitialized pick a random tile. Repeat the same for the other chunk but this time assign an end
                 *    tile.
                 * 4. Connect starting tile to the ending tile.
                 */

                genChunkGroundSprite(chunkX, chunkY);

                component::ChunkIndex chunkIndex{chunkX, chunkY};
                component::EdgeRoadTile edgeRoads{};

                if (!shouldGenerateRoad(chunkIndex)) {
                    return;
                }

                auto northChunkIndex = component::ChunkIndex{chunkIndex.x, chunkIndex.y + 1};
                auto northChunkID = math::packIntegers(chunkIndex.x, chunkIndex.y);

                auto southChunkIndex = component::ChunkIndex{chunkIndex.x, chunkIndex.y - 1};
                auto southChunkID = math::packIntegers(southChunkIndex.x, southChunkIndex.y);

                auto westChunkIndex = component::ChunkIndex{chunkIndex.x - 1, chunkIndex.y};
                auto westChunkID = math::packIntegers(westChunkIndex.x, westChunkIndex.y);

                auto eastChunkIndex = component::ChunkIndex{chunkIndex.x + 1, chunkIndex.y};
                auto eastChunkID = math::packIntegers(eastChunkIndex.x, eastChunkIndex.y);

                auto northChunkHasRoads = shouldGenerateRoad(northChunkIndex);
                auto southChunkHasRoads = shouldGenerateRoad(southChunkIndex);
                auto westChunkHasRoads = shouldGenerateRoad(westChunkIndex);
                auto eastChunkHasRoads = shouldGenerateRoad(eastChunkIndex);

                auto neighboursRoadStatus = {northChunkHasRoads, southChunkHasRoads, westChunkHasRoads,
                                             eastChunkHasRoads};
                auto neighboursWithRoad = std::count_if(neighboursRoadStatus.begin(), neighboursRoadStatus.end(),
                                                        [](bool status) { return status; });
                assert(neighboursWithRoad == 2);

                component::RoadTileIndex startingRoadTileIndex{0, 0};

                component::RoadTileIndex endingRoadTileIndex{0, 0};

                component::RoadTileIndex northBoarderRoadTileIndex{
                        static_cast<uint16>(std::rand() % mTilesPerChunkX),
                        static_cast<uint16>(mTilesPerChunkY - 1)};

                component::RoadTileIndex southBoarderRoadTileIndex{
                        static_cast<uint16>(std::rand() % mTilesPerChunkX),
                        0};

                component::RoadTileIndex westBoarderRoadTileIndex{0,
                                                                  static_cast<uint16>(std::rand() %
                                                                                      mTilesPerChunkY)};

                component::RoadTileIndex eastBoarderRoadTileIndex{static_cast<uint16>(mTilesPerChunkX - 1),
                                                                  static_cast<uint16>(std::rand() %
                                                                                      mTilesPerChunkY)};

                if (northChunkHasRoads && southChunkHasRoads) {
                    edgeRoads.southBoarder = component::RoadTileIndex{};
                    edgeRoads.northBoarder = component::RoadTileIndex{};

                    if (isInMap(southChunkID, mChunkLookup)) {
                        auto southChunkEntityID = mChunkLookup.at(southChunkID);
                        const auto &southChunk = mEntityFactory.getEntityManager().get<component::Chunk>(
                                southChunkEntityID);

                        edgeRoads.southBoarder.x = southChunk.edgeRoad.northBoarder.x;
                        edgeRoads.southBoarder.y = 0;
                    } else {
                        edgeRoads.southBoarder = southBoarderRoadTileIndex;
                    }
                    if (isInMap(northChunkID, mChunkLookup)) {
                        auto northChunkEntityID = mChunkLookup.at(northChunkID);
                        const auto &northChunk = mEntityFactory.getEntityManager().get<component::Chunk>(
                                northChunkEntityID);

                        edgeRoads.northBoarder = northChunk.edgeRoad.southBoarder;

                    } else {
                        edgeRoads.northBoarder = northBoarderRoadTileIndex;
                    }

                    startingRoadTileIndex = edgeRoads.southBoarder;
                    endingRoadTileIndex = edgeRoads.northBoarder;

                } else if (northChunkHasRoads && eastChunkHasRoads) {

                } else if (northChunkHasRoads && westChunkHasRoads) {

                } else if (southChunkHasRoads && westChunkHasRoads) {

                } else if (southChunkHasRoads && eastChunkHasRoads) {

                } else if (westChunkHasRoads && eastChunkHasRoads) {
                    edgeRoads.westBoarder = component::RoadTileIndex{};
                    edgeRoads.eastBoarder = component::RoadTileIndex{};

                    if (isInMap(eastChunkID, mChunkLookup)) {
                        auto eastChunkEntityID = mChunkLookup.at(eastChunkID);
                        const auto &eastChunk = mEntityFactory.getEntityManager().get<component::Chunk>(
                                eastChunkEntityID);

                        edgeRoads.eastBoarder.x = mTilesPerChunkX - 1;
                        edgeRoads.eastBoarder.y = eastChunk.edgeRoad.westBoarder.y;

                    } else {
                        edgeRoads.eastBoarder = eastBoarderRoadTileIndex;
                    }

                    if (isInMap(westChunkID, mChunkLookup)) {
                        auto westChunkEntityID = mChunkLookup.at(westChunkID);
                        const auto &westChunk = mEntityFactory.getEntityManager().get<component::Chunk>(
                                westChunkEntityID);

                        edgeRoads.westBoarder.x = 0;
                        edgeRoads.westBoarder.y = westChunk.edgeRoad.eastBoarder.y;
                    } else {
                        edgeRoads.westBoarder = westBoarderRoadTileIndex;
                    }

                    startingRoadTileIndex = edgeRoads.westBoarder;
                    endingRoadTileIndex = edgeRoads.eastBoarder;

                    common::uint16 currentTileX = startingRoadTileIndex.x;
                    common::uint16 currentTileY = startingRoadTileIndex.y;

                    auto previousRoadTexture = mWestToEast;

                    while (currentTileX < (endingRoadTileIndex.x + 1)) {
                        if (currentTileX == endingRoadTileIndex.x) {
                            // Make sure we connect to endingRoadTile
                            if (currentTileY == endingRoadTileIndex.y) {
                                if (previousRoadTexture == mWestToEast) {
                                    genTileRoad(chunkIndex,
                                                component::RoadTileIndex{currentTileX, currentTileY},
                                                mWestToEast);
                                } else if (previousRoadTexture == mSouthToNorth) {
                                    genTileRoad(chunkIndex,
                                                component::RoadTileIndex{currentTileX, currentTileY},
                                                mSouthToEast);
                                } else if (previousRoadTexture == mNorthToSouth) {
                                    genTileRoad(chunkIndex,
                                                component::RoadTileIndex{currentTileX, currentTileY},
                                                mNorthToEast);
                                } else if (previousRoadTexture == mWestToSouth) {
                                    genTileRoad(chunkIndex,
                                                component::RoadTileIndex{currentTileX, currentTileY},
                                                mNorthToEast);
                                } else if (previousRoadTexture == mWestToNorth) {
                                    genTileRoad(chunkIndex,
                                                component::RoadTileIndex{currentTileX, currentTileY},
                                                mSouthToEast);
                                } else {
                                    assert(false);
                                }
                                break;
                                // We are done
                            } else if (currentTileY > endingRoadTileIndex.y) {
                                if (previousRoadTexture == mWestToEast) {
                                    genTileRoad(chunkIndex,
                                                component::RoadTileIndex{currentTileX, currentTileY},
                                                mWestToSouth);
                                    previousRoadTexture = mWestToSouth;
                                } else if (previousRoadTexture == mNorthToSouth) {
                                    genTileRoad(chunkIndex,
                                                component::RoadTileIndex{currentTileX, currentTileY},
                                                mNorthToSouth);
                                    previousRoadTexture = mNorthToSouth;
                                } else if (previousRoadTexture == mWestToSouth) {
                                    genTileRoad(chunkIndex,
                                                component::RoadTileIndex{currentTileX, currentTileY},
                                                mNorthToSouth);
                                    previousRoadTexture = mNorthToSouth;
                                } else {
                                    assert(false);
                                }
                                --currentTileY;
                                // go down
                            } else {
                                if (previousRoadTexture == mWestToEast) {
                                    genTileRoad(chunkIndex,
                                                component::RoadTileIndex{currentTileX, currentTileY},
                                                mWestToNorth);
                                    previousRoadTexture = mWestToNorth;
                                } else if (previousRoadTexture == mSouthToNorth) {
                                    genTileRoad(chunkIndex,
                                                component::RoadTileIndex{currentTileX, currentTileY},
                                                mSouthToNorth);
                                    previousRoadTexture = mSouthToNorth;
                                } else if (previousRoadTexture == mWestToNorth) {
                                    genTileRoad(chunkIndex,
                                                component::RoadTileIndex{currentTileX, currentTileY},
                                                mSouthToNorth);
                                    previousRoadTexture = mSouthToNorth;
                                } else {
                                    assert(false);
                                }
                                ++currentTileY;
                                // go up
                            }
                        } else {
                            // TODO: Randomly gen road instead of straight line
                            genTileRoad(chunkIndex,
                                        component::RoadTileIndex{currentTileX, currentTileY},
                                        mWestToEast);
                            ++currentTileX;
                        }
                    }

                } else {
                    assert(false);
                }

/*            genTileRoad(chunkIndex, startingRoadTileIndex, endingRoadTileIndex,
                                    entities);*/


                auto chunkID = math::packIntegers(chunkIndex.x, chunkIndex.y);
                auto worldPos = groundChunkIndexToPos(chunkIndex);
                auto chunkEntityID = mChunkLookup[chunkID];
                auto &chunk = mEntityFactory.getEntityManager().get<component::Chunk>(chunkEntityID);
                chunk.position = worldPos;
                chunk.index = chunkID;
                chunk.edgeRoad = edgeRoads;
            }

            void TileWorld::genTileRoad(const component::ChunkIndex &chunkIndex,
                                        const component::RoadTileIndex &roadTileIndex,
                                        const std::string &texturePath) {
                auto worldPos = roadTileIndexToPos(chunkIndex, roadTileIndex);
                auto roadEntityID = genTexture(getTileSize(), worldPos, texturePath);
                auto roadID = math::packIntegers(roadTileIndex.x, roadTileIndex.y);
                mRoadLookup.emplace(roadID, roadEntityID);
            }

            void TileWorld::genTileRoad(const component::ChunkIndex &chunkIndex,
                                        const component::RoadTileIndex &roadTileIndex) {
                math::vec4 color{0.1f, 0.1f, 0.1f, 0.5f};
                auto roadTileSize = getTileSize();

                auto worldPos = roadTileIndexToPos(chunkIndex, roadTileIndex);
                auto roadID = genSprite(color, roadTileSize, worldPos);

                auto packedIndex = math::packIntegers(roadTileIndex.x, roadTileIndex.y);
                mRoadLookup.emplace(packedIndex, roadID);
            }

            void TileWorld::genTileRoad(const component::ChunkIndex &chunkIndex,
                                        component::RoadTileIndex startingRoadTileIndex,
                                        component::RoadTileIndex endingRoadTileIndex) {
                // Fill between tiles as if we are sweeping the Manhattan distance between them.
                while (startingRoadTileIndex.x < endingRoadTileIndex.x) {
                    genTileRoad(chunkIndex, startingRoadTileIndex);
                    ++startingRoadTileIndex.x;
                }

                while (startingRoadTileIndex.x > endingRoadTileIndex.x) {
                    genTileRoad(chunkIndex, startingRoadTileIndex);
                    --startingRoadTileIndex.x;
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

                while (startingRoadTileIndex.y < endingRoadTileIndex.y) {
                    genTileRoad(chunkIndex, startingRoadTileIndex);
                    ++startingRoadTileIndex.y;
                }

                while (startingRoadTileIndex.y > endingRoadTileIndex.y) {
                    genTileRoad(chunkIndex, startingRoadTileIndex);
                    --startingRoadTileIndex.y;
                }
            }

            void TileWorld::genChunkTiles(common::int64 xChunkIndex,
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
                        if (isInMap(packedIndex, mTileLookup)) {
                            return;
                        }
                        auto R = (std::rand() % 255) / 255.0f;
                        auto G = (std::rand() % 255) / 255.0f;
                        auto B = (std::rand() % 255) / 255.0f;
                        auto color = math::vec4{R, G, B, 1.0f};

                        auto worldPos = math::vec3{static_cast<common::real32>(i),
                                                   static_cast<common::real32>(j),
                                                   mGroundZ};

                        auto tileID = genSprite(color, tileSize, worldPos);

                        mTileLookup.emplace(packedIndex, tileID);
                    }
                }
            }

            bool TileWorld::shouldGenerateRoad(const component::ChunkIndex &chunkIndex) const {
                return chunkIndex.y == 0;
            }

            math::vec3 TileWorld::groundChunkIndexToPos(const component::ChunkIndex &chunkIndex) const {
                return math::vec3{static_cast<common::real32>(chunkIndex.x * mChunkSizeX),
                                  static_cast<common::real32>(chunkIndex.y * mChunkSizeY),
                        // TODO: Should I keep Z as part of ChunkIndex maybe?
                                  mGroundZ};
            }

            component::ChunkIndex TileWorld::groundChunkPosToIndex(const math::vec2 &position) const {
                auto x = floor(position.x / mChunkSizeX);
                auto xIndex = static_cast<common::int64>(x);
                auto y = floor(position.y / mChunkSizeY);
                auto yIndex = static_cast<common::int64>(y);
                return component::ChunkIndex{xIndex, yIndex};
            }

            math::vec3 TileWorld::roadTileIndexToPos(const component::ChunkIndex &chunkIndex,
                                                     component::RoadTileIndex roadTileIndex) const {

                auto chunkPos = groundChunkIndexToPos(chunkIndex);
                auto tilePos = math::vec2{static_cast<common::real32>(roadTileIndex.x * mTileSizeX),
                                          static_cast<common::real32>(roadTileIndex.y * mTileSizeY)};
                math::vec3 pos{chunkPos.x + tilePos.x, chunkPos.y + tilePos.y, chunkPos.z};
                return pos;
            }

            void TileWorld::createWall(const std::vector<component::WallTilePosition> &position,
                                       const std::vector<component::TileIndex> &indices) {
                assert(position.size() == indices.size());

                size_t wallCount = indices.size();

                std::vector<common::EntityID> wallEntities{};
                wallEntities.reserve(wallCount);

                common::real32 wallWidth = 0.5f;
                common::real32 heading = 0.f; // For static objects facing angle does not matter.

                auto lock = mEntityFactory.getEntityManager().scopedLock();

                for (size_t i = 0; i < wallCount; ++i) {
                    auto &wallPos = position[i];
                    auto &xTileIndex = indices[i].x;
                    auto &yTileIndex = indices[i].y;

                    math::vec3 wallPositionInWorld;
                    math::vec2 wallSize;
                    std::string wallTexturePath;

                    common::real32 currentTileX = xTileIndex * mTileSizeX;
                    common::real32 currentTileY = yTileIndex * mTileSizeY;

                    switch (wallPos) {
                        case component::WallTilePosition::RIGHT: {
                            wallSize.x = wallWidth;
                            wallSize.y = mTileSizeY - 2 * wallWidth;
                            wallTexturePath = "resources/images/wall/1/vertical.png";

                            wallPositionInWorld.x = currentTileX + mTileSizeX - wallWidth;
                            wallPositionInWorld.y = currentTileY + wallWidth;
                            wallPositionInWorld.z = mWallZ;
                            break;
                        }
                        case component::WallTilePosition::TOP: {
                            wallSize.x = mTileSizeX - 2 * wallWidth;
                            wallSize.y = wallWidth;
                            wallTexturePath = "resources/images/wall/1/horizontal.png";

                            wallPositionInWorld.x = currentTileX + wallWidth;
                            wallPositionInWorld.y = currentTileY + mTileSizeY - wallWidth;
                            wallPositionInWorld.z = mWallZ;
                            break;
                        }
                        case component::WallTilePosition::LEFT: {
                            wallSize.x = wallWidth;
                            wallSize.y = mTileSizeY - 2 * wallWidth;
                            wallTexturePath = "resources/images/wall/1/vertical.png";

                            wallPositionInWorld.x = currentTileX;
                            wallPositionInWorld.y = currentTileY + wallWidth;
                            wallPositionInWorld.z = mWallZ;
                            break;
                        }
                        case component::WallTilePosition::BOTTOM: {
                            wallSize.x = mTileSizeX - 2 * wallWidth;
                            wallSize.y = wallWidth;
                            wallTexturePath = "resources/images/wall/1/horizontal.png";

                            wallPositionInWorld.x = currentTileX + wallWidth;
                            wallPositionInWorld.y = currentTileY;
                            wallPositionInWorld.z = mWallZ;
                            break;
                        }
                    }

                    mEntityFactory.createEntity<component::EntityType::WALL>(
                            wallPositionInWorld,
                            wallSize,
                            heading,
                            wallTexturePath);
                }
            }

            void TileWorld::genChunkGroundTexture(common::int64 chunkX, common::int64 chunkY) {
                auto chunkIndex = component::ChunkIndex{chunkX, chunkY};
                auto worldPos = groundChunkIndexToPos(chunkIndex);

                auto chunkEntityID = genTexture(getChunkSize(), worldPos, mRaceTrack1);
                auto packed = math::packIntegers(chunkX, chunkY);
                mChunkLookup.emplace(packed, chunkEntityID);
            }

            void TileWorld::genChunkGroundSprite(common::int64 chunkX, common::int64 chunkY) {
                auto chunkID = math::packIntegers(chunkX, chunkY);
                auto R = (std::rand() % 255) / 255.0f;
                auto G = (std::rand() % 255) / 255.0f;
                auto B = (std::rand() % 255) / 255.0f;
                math::vec4 color{R, G, B, 0.1f};
                math::vec2 size{static_cast<common::real32>(mChunkSizeX),
                                static_cast<common::real32 >(mChunkSizeY)};
                component::ChunkIndex currentChunkIndex{chunkX, chunkY};
                auto worldPos = groundChunkIndexToPos(currentChunkIndex);
                auto chunkEntityID = genSprite(color, size, worldPos);

                mChunkLookup.emplace(chunkID, chunkEntityID);

            }

            math::vec2 TileWorld::getTileSize() const {
                return math::vec2{static_cast<common::real32>(mTileSizeX),
                                  static_cast<common::real32 >(mTileSizeY)};
            }

            math::vec2 TileWorld::getChunkSize() const {
                return math::vec2{static_cast<common::real32>(mChunkSizeX),
                                  static_cast<common::real32 >(mChunkSizeY)};
            }

            common::EntityID
            TileWorld::genSprite(math::vec4 &color, math::vec2 &tileSize, math::vec3 &worldPos) {
                auto lock = mEntityFactory.getEntityManager().scopedLock();
                common::real32 heading = 0.f;
                return mEntityFactory.createEntity<component::EntityType::WORLD_CHUNK>(worldPos, tileSize, heading,
                                                                                       color);
            }

            common::EntityID TileWorld::genTexture(const math::vec2 &size,
                                                   const math::vec3 &worldPos,
                                                   const std::string &path) {
                auto lock = mEntityFactory.getEntityManager().scopedLock();
                common::real32 heading = 0.f;
                return mEntityFactory.createEntity<component::EntityType::WORLD_CHUNK>(worldPos, size, heading, path);
            }

            void TileWorld::genDemoRaceCourse() {
                for (int i = -2; i <= 2; ++i) {
                    for (int j = -2; j <= 2; ++j) {
                        genChunkGroundTexture(i, j);
                    }
                }

                std::vector<component::WallTilePosition> wallPosInTile{};
                std::vector<component::TileIndex> wallTiles{};

                common::int8 outerTrackWidth = 8;
                common::int8 outerTrackHeight = 4;

                for (auto i = -outerTrackWidth; i <= outerTrackWidth; ++i) {
                    wallPosInTile.emplace_back(component::WallTilePosition::BOTTOM);
                    wallTiles.emplace_back(component::TileIndex{i, -outerTrackHeight});
                }

                for (auto i = -outerTrackHeight; i <= outerTrackHeight; ++i) {
                    wallPosInTile.emplace_back(component::WallTilePosition::RIGHT);
                    wallTiles.emplace_back(component::TileIndex{outerTrackWidth, i});
                }

                for (auto i = -outerTrackWidth; i <= outerTrackWidth; ++i) {
                    wallPosInTile.emplace_back(component::WallTilePosition::TOP);
                    wallTiles.emplace_back(component::TileIndex{i, outerTrackHeight});
                }

                for (auto i = -outerTrackHeight; i <= outerTrackHeight; ++i) {
                    wallPosInTile.emplace_back(component::WallTilePosition::LEFT);
                    wallTiles.emplace_back(component::TileIndex{-outerTrackWidth, i});
                }

                common::int8 innerTrackWidth = 6;
                common::int8 innerTrackHeight = 2;

                for (auto i = -innerTrackWidth; i <= innerTrackWidth; ++i) {
                    wallPosInTile.emplace_back(component::WallTilePosition::BOTTOM);
                    wallTiles.emplace_back(component::TileIndex{i, -innerTrackHeight});
                }

                for (auto i = -innerTrackHeight; i <= innerTrackHeight; ++i) {
                    wallPosInTile.emplace_back(component::WallTilePosition::RIGHT);
                    wallTiles.emplace_back(component::TileIndex{innerTrackWidth, i});
                }

                for (auto i = -innerTrackWidth; i <= innerTrackWidth; ++i) {
                    wallPosInTile.emplace_back(component::WallTilePosition::TOP);
                    wallTiles.emplace_back(component::TileIndex{i, innerTrackHeight});
                }

                for (auto i = -innerTrackHeight; i <= innerTrackHeight; ++i) {
                    wallPosInTile.emplace_back(component::WallTilePosition::LEFT);
                    wallTiles.emplace_back(component::TileIndex{-innerTrackWidth, i});
                }

                createWall(wallPosInTile, wallTiles);
            }
        }
    }
}
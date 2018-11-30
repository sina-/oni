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
                    generateChunkTexture(i, j);
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


        bool TileWorld::existsInMap(common::uint64 packedIndex,
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
            auto chunkIndex = chunkPosToIndex(position);

            // NOTE: We always create and fill chunks in the current location and 8 adjacent chunks.
            // 1--2--3
            // |--|--|
            // 4--c--5
            // |--|--|
            // 6--7--8
            for (auto i = chunkIndex.x - 1; i <= chunkIndex.x + 1; ++i) {
                for (auto j = chunkIndex.y - 1; j <= chunkIndex.y + 1; ++j) {
                    auto chunkID = math::packIntegers(i, j);
                    if (!existsInMap(chunkID, mChunkLookup)) {
                        // generateChunkTexture(i, j);
                        // generateTilesForChunk(i, j);
                        // generateRoadsForChunk(i, j);
                    }
                }
            }
        }

        void TileWorld::generateRoadsForChunk(common::int64 chunkX, common::int64 chunkY) {
            /**
             * 1. Check if there should be a road in this chunk
             * 2. Find the neighbours connected by road to current chunk
             * 3. Find if neighbours are initialized, if yes find the tile position on the boarder of the chunk
             *    that has a road choose the tile next to it in this chunk as the starting chunk, if neighbour is
             *    uninitialized pick a random tile. Repeat the same for the other chunk but this time assign an end
             *    tile.
             * 4. Connect starting tile to the ending tile.
             */

            generateChunkSprite(chunkX, chunkY);

            components::ChunkIndex chunkIndex{chunkX, chunkY};
            components::EdgeRoadTile edgeRoads{};

            if (!shouldGenerateRoad(chunkIndex)) {
                return;
            }

            auto northChunkIndex = components::ChunkIndex{chunkIndex.x, chunkIndex.y + 1};
            auto northChunkID = math::packIntegers(chunkIndex.x, chunkIndex.y);

            auto southChunkIndex = components::ChunkIndex{chunkIndex.x, chunkIndex.y - 1};
            auto southChunkID = math::packIntegers(southChunkIndex.x, southChunkIndex.y);

            auto westChunkIndex = components::ChunkIndex{chunkIndex.x - 1, chunkIndex.y};
            auto westChunkID = math::packIntegers(westChunkIndex.x, westChunkIndex.y);

            auto eastChunkIndex = components::ChunkIndex{chunkIndex.x + 1, chunkIndex.y};
            auto eastChunkID = math::packIntegers(eastChunkIndex.x, eastChunkIndex.y);

            auto northChunkHasRoads = shouldGenerateRoad(northChunkIndex);
            auto southChunkHasRoads = shouldGenerateRoad(southChunkIndex);
            auto westChunkHasRoads = shouldGenerateRoad(westChunkIndex);
            auto eastChunkHasRoads = shouldGenerateRoad(eastChunkIndex);

            auto neighboursRoadStatus = {northChunkHasRoads, southChunkHasRoads, westChunkHasRoads, eastChunkHasRoads};
            auto neighboursWithRoad = std::count_if(neighboursRoadStatus.begin(), neighboursRoadStatus.end(),
                                                    [](bool status) { return status; });
            assert(neighboursWithRoad == 2);

            components::RoadTileIndex startingRoadTileIndex{0, 0};

            components::RoadTileIndex endingRoadTileIndex{0, 0};

            components::RoadTileIndex northBoarderRoadTileIndex{static_cast<uint16>(std::rand() % mTilesPerChunkX),
                                                                static_cast<uint16>(mTilesPerChunkY - 1)};

            components::RoadTileIndex southBoarderRoadTileIndex{static_cast<uint16>(std::rand() % mTilesPerChunkX),
                                                                0};

            components::RoadTileIndex westBoarderRoadTileIndex{0,
                                                               static_cast<uint16>(std::rand() % mTilesPerChunkY)};

            components::RoadTileIndex eastBoarderRoadTileIndex{static_cast<uint16>(mTilesPerChunkX - 1),
                                                               static_cast<uint16>(std::rand() % mTilesPerChunkY)};

            if (northChunkHasRoads && southChunkHasRoads) {
                edgeRoads.southBoarder = components::RoadTileIndex{};
                edgeRoads.northBoarder = components::RoadTileIndex{};

                if (existsInMap(southChunkID, mChunkLookup)) {
                    auto southChunkEntityID = mChunkLookup.at(southChunkID);
                    const auto &southChunk = mEntityManager.get<components::Chunk>(southChunkEntityID);

                    edgeRoads.southBoarder.x = southChunk.edgeRoad.northBoarder.x;
                    edgeRoads.southBoarder.y = 0;
                } else {
                    edgeRoads.southBoarder = southBoarderRoadTileIndex;
                }
                if (existsInMap(northChunkID, mChunkLookup)) {
                    auto northChunkEntityID = mChunkLookup.at(northChunkID);
                    const auto &northChunk = mEntityManager.get<components::Chunk>(northChunkEntityID);

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
                edgeRoads.westBoarder = components::RoadTileIndex{};
                edgeRoads.eastBoarder = components::RoadTileIndex{};

                if (existsInMap(eastChunkID, mChunkLookup)) {
                    auto eastChunkEntityID = mChunkLookup.at(eastChunkID);
                    const auto &eastChunk = mEntityManager.get<components::Chunk>(eastChunkEntityID);

                    edgeRoads.eastBoarder.x = mTilesPerChunkX - 1;
                    edgeRoads.eastBoarder.y = eastChunk.edgeRoad.westBoarder.y;

                } else {
                    edgeRoads.eastBoarder = eastBoarderRoadTileIndex;
                }

                if (existsInMap(westChunkID, mChunkLookup)) {
                    auto westChunkEntityID = mChunkLookup.at(westChunkID);
                    const auto &westChunk = mEntityManager.get<components::Chunk>(westChunkEntityID);

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
                                generateTexturedRoadTile(chunkIndex,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mWestToEast);
                            } else if (previousRoadTexture == mSouthToNorth) {
                                generateTexturedRoadTile(chunkIndex,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mSouthToEast);
                            } else if (previousRoadTexture == mNorthToSouth) {
                                generateTexturedRoadTile(chunkIndex,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mNorthToEast);
                            } else if (previousRoadTexture == mWestToSouth) {
                                generateTexturedRoadTile(chunkIndex,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mNorthToEast);
                            } else if (previousRoadTexture == mWestToNorth) {
                                generateTexturedRoadTile(chunkIndex,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mSouthToEast);
                            } else {
                                assert(false);
                            }
                            break;
                            // We are done
                        } else if (currentTileY > endingRoadTileIndex.y) {
                            if (previousRoadTexture == mWestToEast) {
                                generateTexturedRoadTile(chunkIndex,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mWestToSouth);
                                previousRoadTexture = mWestToSouth;
                            } else if (previousRoadTexture == mNorthToSouth) {
                                generateTexturedRoadTile(chunkIndex,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mNorthToSouth);
                                previousRoadTexture = mNorthToSouth;
                            } else if (previousRoadTexture == mWestToSouth) {
                                generateTexturedRoadTile(chunkIndex,
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
                                generateTexturedRoadTile(chunkIndex,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mWestToNorth);
                                previousRoadTexture = mWestToNorth;
                            } else if (previousRoadTexture == mSouthToNorth) {
                                generateTexturedRoadTile(chunkIndex,
                                                         components::RoadTileIndex{currentTileX, currentTileY},
                                                         mSouthToNorth);
                                previousRoadTexture = mSouthToNorth;
                            } else if (previousRoadTexture == mWestToNorth) {
                                generateTexturedRoadTile(chunkIndex,
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
                        generateTexturedRoadTile(chunkIndex,
                                                 components::RoadTileIndex{currentTileX, currentTileY},
                                                 mWestToEast);
                        ++currentTileX;
                    }
                }

            } else {
                assert(false);
            }

/*            generateRoadTileBetween(chunkIndex, startingRoadTileIndex, endingRoadTileIndex,
                                    entities);*/


            auto chunkID = math::packIntegers(chunkIndex.x, chunkIndex.y);
            auto worldPos = chunkIndexToPos(chunkIndex);
            auto chunkEntityID = mChunkLookup[chunkID];
            auto chunk = components::Chunk{worldPos, chunkID, edgeRoads};
            mEntityManager.assign<components::Chunk>(chunkEntityID, chunk);
        }

        void TileWorld::generateTexturedRoadTile(const components::ChunkIndex &chunkIndex,
                                                 const components::RoadTileIndex &roadTileIndex,
                                                 const std::string &texturePath) {
            auto worldPos = roadTileIndexToPos(chunkIndex, roadTileIndex);
            auto roadEntityID = generateTexture(getTileSize(), worldPos, texturePath);
            auto roadID = math::packIntegers(roadTileIndex.x, roadTileIndex.y);
            mRoadLookup.emplace(roadID, roadEntityID);
        }

        void TileWorld::generateRoadTile(const components::ChunkIndex &chunkIndex,
                                         const components::RoadTileIndex &roadTileIndex) {
            auto color = math::vec4{0.1f, 0.1f, 0.1f, 0.5f};
            auto roadTileSize = getTileSize();

            auto worldPos = roadTileIndexToPos(chunkIndex, roadTileIndex);
            auto roadID = generateSprite(color, roadTileSize, worldPos);

            auto packedIndex = math::packIntegers(roadTileIndex.x, roadTileIndex.y);
            mRoadLookup.emplace(packedIndex, roadID);
        }

        void TileWorld::generateRoadTileBetween(const components::ChunkIndex &chunkIndex,
                                                components::RoadTileIndex startingRoadTileIndex,
                                                components::RoadTileIndex endingRoadTileIndex) {
            // Fill between tiles as if we are sweeping the Manhattan distance between them.
            while (startingRoadTileIndex.x < endingRoadTileIndex.x) {
                generateRoadTile(chunkIndex, startingRoadTileIndex);
                ++startingRoadTileIndex.x;
            }

            while (startingRoadTileIndex.x > endingRoadTileIndex.x) {
                generateRoadTile(chunkIndex, startingRoadTileIndex);
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
                generateRoadTile(chunkIndex, startingRoadTileIndex);
                ++startingRoadTileIndex.y;
            }

            while (startingRoadTileIndex.y > endingRoadTileIndex.y) {
                generateRoadTile(chunkIndex, startingRoadTileIndex);
                --startingRoadTileIndex.y;
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

                    auto tileID = generateSprite(color, tileSize, worldPos);

                    mTileLookup.emplace(packedIndex, tileID);
                }
            }
        }

        bool TileWorld::shouldGenerateRoad(const components::ChunkIndex &chunkIndex) const {
            return chunkIndex.y == 0;
        }

        math::vec3 TileWorld::chunkIndexToPos(const components::ChunkIndex &chunkIndex) const {
            return math::vec3{static_cast<common::real32>(chunkIndex.x * mChunkSizeX),
                              static_cast<common::real32>(chunkIndex.y * mChunkSizeY), 1.f};
        }

        components::ChunkIndex TileWorld::chunkPosToIndex(const math::vec2 &position) const {
            auto x = floor(position.x / mChunkSizeX);
            auto xIndex = static_cast<common::int64>(x);
            auto y = floor(position.y / mChunkSizeY);
            auto yIndex = static_cast<common::int64>(y);
            return components::ChunkIndex{xIndex, yIndex};
        }

        math::vec3 TileWorld::roadTileIndexToPos(const components::ChunkIndex &chunkIndex,
                                                 components::RoadTileIndex roadTileIndex) const {

            auto chunkPos = chunkIndexToPos(chunkIndex);
            auto tilePos = math::vec2{static_cast<common::real32>(roadTileIndex.x * mTileSizeX),
                                      static_cast<common::real32>(roadTileIndex.y * mTileSizeY)};
            math::vec3 pos{chunkPos.x + tilePos.x, chunkPos.y + tilePos.y, chunkPos.z};
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

            entities::assignTextureToLoad(mEntityManager, entity, "resources/images/wall/1/1.png");
        }

        void TileWorld::createWall(const std::vector<components::WallTilePosition> &position,
                                   const std::vector<components::TileIndex> &indices) {
            assert(position.size() == indices.size());

            size_t wallCount = indices.size();

            std::vector<common::EntityID> wallEntities{};
            wallEntities.reserve(wallCount);

            common::real32 wallWidth = 0.5f;

            auto lock = mEntityManager.scopedLock();

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

                auto entityID = entities::createEntity(mEntityManager);
                entities::assignShapeWorld(mEntityManager, entityID, wallSize, wallPositionInWorld);
                entities::assignTextureToLoad(mEntityManager, entityID, wallTexturePath);
                entities::assignPhysicalProperties(mEntityManager, mPhysicsWorld,
                                                   entityID, wallPositionInWorld, wallSize,
                                                   components::BodyType::STATIC, false);
                entities::assignTag<components::Tag_Static>(mEntityManager, entityID);
            }
        }

        void TileWorld::generateChunkTexture(common::int64 chunkX, common::int64 chunkY) {
            auto chunkIndex = components::ChunkIndex{chunkX, chunkY};
            auto worldPos = chunkIndexToPos(chunkIndex);

            auto chunkEntityID = generateTexture(getChunkSize(), worldPos, mRaceTrack1);
            auto packed = math::packIntegers(chunkX, chunkY);
            mChunkLookup.emplace(packed, chunkEntityID);
        }

        void TileWorld::generateChunkSprite(common::int64 chunkX, common::int64 chunkY) {
            auto chunkID = math::packIntegers(chunkX, chunkY);
            auto R = (std::rand() % 255) / 255.0f;
            auto G = (std::rand() % 255) / 255.0f;
            auto B = (std::rand() % 255) / 255.0f;
            math::vec4 color{R, G, B, 0.1f};
            math::vec2 size{static_cast<common::real32>(mChunkSizeX),
                            static_cast<common::real32 >(mChunkSizeY)};
            components::ChunkIndex currentChunkIndex{chunkX, chunkY};
            auto worldPos = chunkIndexToPos(currentChunkIndex);
            auto chunkEntityID = generateSprite(color, size, worldPos);

            mChunkLookup.emplace(chunkID, chunkEntityID);

        }

        math::vec2 TileWorld::getTileSize() const {
            return math::vec2{static_cast<common::real32>(mTileSizeX), static_cast<common::real32 >(mTileSizeY)};
        }

        math::vec2 TileWorld::getChunkSize() const {
            return math::vec2{static_cast<common::real32>(mChunkSizeX), static_cast<common::real32 >(mChunkSizeY)};
        }

        common::EntityID TileWorld::generateSprite(math::vec4 color, math::vec2 tileSize, math::vec3 worldPos) {
            auto lock = mEntityManager.scopedLock();
            auto entity = entities::createEntity(mEntityManager);
            entities::assignShapeWorld(mEntityManager, entity, tileSize, worldPos);
            entities::assignAppearance(mEntityManager, entity, color);
            entities::assignTag<components::Tag_Static>(mEntityManager, entity);

            return entity;
        }

        common::EntityID TileWorld::generateTexture(const math::vec2 &size,
                                                    const math::vec3 &worldPos,
                                                    const std::string &path) {
            auto lock = mEntityManager.scopedLock();
            auto entityID = entities::createEntity(mEntityManager);
            entities::assignShapeWorld(mEntityManager, entityID, size, worldPos);
            entities::assignTextureToLoad(mEntityManager, entityID, path);
            entities::assignTag<components::Tag_Static>(mEntityManager, entityID);

            return entityID;
        }
    }
}
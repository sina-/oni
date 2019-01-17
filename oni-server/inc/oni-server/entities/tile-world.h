#pragma once

#include <map>

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec4.h>
#include <oni-core/components/geometry.h>
#include <oni-core/components/physics.h>
#include <oni-core/components/visual.h>

class b2World;

namespace oni {
    namespace entities {
        class EntityManager;
    }

    namespace server {
        namespace entities {

            class TileWorld {
            public:

                TileWorld(oni::entities::EntityManager &, b2World &, const oni::components::ZLevel &);

                ~TileWorld();

                void tick(const oni::math::vec2 &position);

                void generateDemoRaceCourse();

            private:

                void tickChunk(const oni::math::vec2 &position);

                oni::components::ChunkIndex backgroundChunkPosToIndex(const oni::math::vec2 &position) const;

                oni::math::vec3 backgroundChunkIndexToPos(const oni::components::ChunkIndex &chunkIndex) const;

                oni::math::vec3 roadTileIndexToPos(const oni::components::ChunkIndex &chunkIndex,
                                                   oni::components::RoadTileIndex roadTileIndices) const;

                oni::math::vec2 unpackCoordinates(oni::common::uint64 coord) const;

                void generateTilesForChunk(oni::common::int64 xChunkIndex,
                                           oni::common::int64 yChunkIndex);

                oni::common::EntityID
                generateSprite(oni::math::vec4 color, oni::math::vec2 tileSize, oni::math::vec3 worldPos);

                oni::common::EntityID generateTexture(const oni::math::vec2 &size,
                                                      const oni::math::vec3 &worldPos,
                                                      const std::string &path);

                void generateRoadsForChunk(oni::common::int64 chunkX, oni::common::int64 chunkY);

                void generateChunkBackgroundTexture(oni::common::int64 chunkX, oni::common::int64 chunkY);

                void generateChunkBackgroundSprite(oni::common::int64 chunkX, oni::common::int64 chunkY);

                void generateRoadTile(const oni::components::ChunkIndex &chunkIndex,
                                      const oni::components::RoadTileIndex &roadTileIndex);

                void generateTexturedRoadTile(const oni::components::ChunkIndex &chunkIndex,
                                              const oni::components::RoadTileIndex &roadTileIndex,
                                              const std::string &texturePath);

                void generateRoadTileBetween(const oni::components::ChunkIndex &chunkIndex,
                                             oni::components::RoadTileIndex startingRoadTileIndex,
                                             oni::components::RoadTileIndex endingRoadTileIndex);

                bool
                existsInMap(oni::common::uint64 packedIndex,
                            const std::map<oni::common::uint64, oni::common::EntityID> &map) const;

                bool shouldGenerateRoad(const oni::components::ChunkIndex &chunkIndex) const;

                void
                createWall(oni::components::WallTilePosition position, oni::common::int64 xTileIndex,
                           oni::common::int64 yTileIndex);

                void createWall(const std::vector<oni::components::WallTilePosition> &position,
                                const std::vector<oni::components::TileIndex> &indices);

                oni::math::vec2 getTileSize() const;

                oni::math::vec2 getChunkSize() const;

            private:
                oni::entities::EntityManager &mEntityManager;
                b2World &mPhysicsWorld;
                /**
                 * A tile is determined by its lower left coordinate in the world. This coordinate is
                 * packed into a uint64 and the lookup table mCoordToTileLookup returns the entity ID
                 * corresponding to the tile.
                 */
                std::map<oni::common::uint64, oni::common::EntityID> mTileLookup{};
                std::map<oni::common::uint64, oni::common::EntityID> mRoadLookup{};
                std::map<oni::common::uint64, oni::common::EntityID> mChunkLookup{};

                const oni::common::uint16 mTileSizeX{0};
                const oni::common::uint16 mTileSizeY{0};

                //const oni::common::real32 mHalfTileSizeX{0.0f};
                //const oni::common::real32 mHalfTileSizeY{0.0f};

                const oni::common::uint16 mTilesPerChunkX{0};
                const oni::common::uint16 mTilesPerChunkY{0};

                const oni::common::uint16 mChunkSizeX{0};
                const oni::common::uint16 mChunkSizeY{0};

                const oni::common::uint16 mHalfChunkSizeX{0};
                const oni::common::uint16 mHalfChunkSizeY{0};

                std::string mNorthToEast{};
                std::string mSouthToEast{};
                std::string mSouthToNorth{};
                std::string mNorthToSouth{};
                std::string mWestToEast{};
                std::string mWestToNorth{};
                std::string mWestToSouth{};

                std::string mRaceTrack1{};
                std::string mRaceTrack2{};
                std::string mRaceTrack3{};
                std::string mRaceTrack4{};

                oni::components::ZLevel mZLevel{};
                oni::common::real32 mBackgroundZ{};
                oni::common::real32 mRoadZ{};
                oni::common::real32 mWallZ{};
            };
        }
    }
}
#pragma once

#include <map>

#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-physic.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/level/oni-level-wall.h>
#include <oni-core/math/oni-math-z-layer-manager.h>
#include <oni-core/math/oni-math-vec2.h>
#include <oni-core/math/oni-math-vec4.h>

class b2World;

namespace oni {
    namespace entities {
        class EntityManager;

        class EntityFactory;
    }

    namespace math {
        class ZLayerManager;
    }

    namespace level {
        struct ChunkIndex;
        struct RoadTileIndex;
        struct TileIndex;
    }

    namespace server {
        namespace level {

            class TileWorld {
            public:
                TileWorld(oni::entities::EntityFactory &,
                          b2World &,
                          const oni::math::ZLayerManager &);

                ~TileWorld();

                void
                tick(const component::WorldP2D &position);

                void
                genDemoRaceCourse();

            private:

                void
                tickChunk(const component::WorldP2D &position);

                oni::level::ChunkIndex
                groundChunkPosToIndex(const component::WorldP2D &position) const;

                component::WorldP3D
                groundChunkIndexToPos(const oni::level::ChunkIndex &chunkIndex) const;

                component::WorldP3D
                roadTileIndexToPos(const oni::level::ChunkIndex &chunkIndex,
                                   oni::level::RoadTileIndex roadTileIndex) const;

                oni::math::vec2
                unpackCoordinates(oni::common::u64 coord) const;

                void
                genChunkTiles(oni::common::i64 xChunkIndex,
                              oni::common::i64 yChunkIndex);

                oni::common::EntityID
                genSprite(oni::math::vec4 &color,
                          oni::math::vec2 &tileSize,
                          component::WorldP3D &worldPos);

                oni::common::EntityID
                genTexture(const oni::math::vec2 &size,
                           const component::WorldP3D &worldPos,
                           const std::string &path);

                void
                genChunkRoads(oni::common::i64 chunkX,
                              oni::common::i64 chunkY);

                void
                genChunkGroundTexture(oni::common::i64 chunkX,
                                      oni::common::i64 chunkY);

                void
                genChunkGroundSprite(oni::common::i64 chunkX,
                                     oni::common::i64 chunkY);

                void
                genTileRoad(const oni::level::ChunkIndex &chunkIndex,
                            const oni::level::RoadTileIndex &roadTileIndex);

                void
                genTileRoad(const oni::level::ChunkIndex &chunkIndex,
                            const oni::level::RoadTileIndex &roadTileIndex,
                            const std::string &texturePath);

                void
                genTileRoad(const oni::level::ChunkIndex &chunkIndex,
                            oni::level::RoadTileIndex startingRoadTileIndex,
                            oni::level::RoadTileIndex endingRoadTileIndex);

                bool
                isInMap(oni::common::u64 packedIndex,
                        const std::map<oni::common::u64, oni::common::EntityID> &map) const;

                bool
                shouldGenerateRoad(const oni::level::ChunkIndex &chunkIndex) const;

                void
                createWall(const std::vector<oni::level::WallTilePosition> &position,
                           const std::vector<oni::level::TileIndex> &indices);

                oni::math::vec2
                getTileSize() const;

                oni::math::vec2
                getChunkSize() const;

            private:
                oni::entities::EntityFactory &mEntityFactory;
                b2World &mPhysicsWorld;
                /**
                 * A tile is determined by its lower left coordinate in the world. This coordinate is
                 * packed into a uint64 and the lookup table mCoordToTileLookup returns the entity ID
                 * corresponding to the tile.
                 */
                std::map<oni::common::u64, oni::common::EntityID> mTileLookup{};
                std::map<oni::common::u64, oni::common::EntityID> mRoadLookup{};
                std::map<oni::common::u64, oni::common::EntityID> mChunkLookup{};

                const oni::common::u16 mTileSizeX{0};
                const oni::common::u16 mTileSizeY{0};

                //const oni::common::r32 mHalfTileSizeX{0.0f};
                //const oni::common::r32 mHalfTileSizeY{0.0f};

                const oni::common::u16 mTilesPerChunkX{0};
                const oni::common::u16 mTilesPerChunkY{0};

                const oni::common::u16 mChunkSizeX{0};
                const oni::common::u16 mChunkSizeY{0};

                const oni::common::u16 mHalfChunkSizeX{0};
                const oni::common::u16 mHalfChunkSizeY{0};

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

                const oni::math::ZLayerManager &mZLayerManager;
                oni::common::r32 mGroundZ{};
                oni::common::r32 mRoadZ{};
                oni::common::r32 mWallZ{};
            };
        }
    }
}
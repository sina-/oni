#pragma once

#include <map>

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec4.h>
#include <oni-core/components/geometry.h>
#include <oni-core/components/physics.h>

class b2World;

namespace oni {
    namespace entities {
        class EntityManager;

        class TileWorld {
            using PackedIndexToEntityID = typename std::map<common::uint64, common::EntityID>;
        public:

            TileWorld(entities::EntityManager &manager, b2World &physicsWorld);

            ~TileWorld();

            void tick(const math::vec2 &position);

        private:

            void tickChunk(const math::vec2 &position);

            components::ChunkIndex chunkPosToIndex(const math::vec2 &position) const;

            math::vec2 chunkIndexToPos(const components::ChunkIndex &chunkIndices) const;

            math::vec2 roadTileIndexToPos(const components::ChunkIndex &chunkIndices,
                                          components::RoadTileIndex roadTileIndices) const;

            math::vec2 unpackCoordinates(common::uint64 coord) const;

            void generateTilesForChunk(common::int64 xChunkIndex,
                                       common::int64 yChunkIndex);

            components::BoarderRoadTiles generateRoadsForChunk(const components::ChunkIndex &chunkIndices);

            void generateChunkBackground(common::int64 chunkX, common::int64 chunkY);

            void generateRoadTile(const components::ChunkIndex &chunkIndices,
                                  const components::RoadTileIndex &roadTileIndices);

            void generateTexturedRoadTile(const components::ChunkIndex &chunkIndices,
                                          const components::RoadTileIndex &roadTileIndices,
                                          const std::string &texturePath);

            void generateRoadTileBetween(const components::ChunkIndex &chunkIndices,
                                         components::RoadTileIndex startingRoadTileIndices,
                                         components::RoadTileIndex endingRoadTileIndices);

            bool existsInMap(common::uint64 packedIndex, const PackedIndexToEntityID &map) const;

            bool shouldGenerateRoad(const components::ChunkIndex &chunkIndices) const;

            void createWall(components::WallTilePosition position, common::int64 xTileIndex, common::int64 yTileIndex);

            void createWall(const std::vector<components::WallTilePosition> &position,
                            const std::vector<components::TileIndex> &indices);

            math::vec2 getTileSize() const;
            math::vec2 getChunkSize() const;

        private:
            entities::EntityManager &mEntityManager;
            b2World &mPhysicsWorld;
            /**
             * A tile is determined by its lower left coordinate in the world. This coordinate is
             * packed into a uint64 and the lookup table mCoordToTileLookup returns the entity ID
             * corresponding to the tile.
             */
            PackedIndexToEntityID mTileLookup{};
            PackedIndexToEntityID mRoadLookup{};
            PackedIndexToEntityID mRoadInChunkLookup{};

            const common::uint16 mTileSizeX{0};
            const common::uint16 mTileSizeY{0};

            //const common::real32 mHalfTileSizeX{0.0f};
            //const common::real32 mHalfTileSizeY{0.0f};

            const common::uint16 mTilesPerChunkX{0};
            const common::uint16 mTilesPerChunkY{0};

            const common::uint16 mChunkSizeX{0};
            const common::uint16 mChunkSizeY{0};

            const common::uint16 mHalfChunkSizeX{0};
            const common::uint16 mHalfChunkSizeY{0};

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
        };
    }
}
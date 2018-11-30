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
        public:

            TileWorld(entities::EntityManager &manager, b2World &physicsWorld);

            ~TileWorld();

            void tick(const math::vec2 &position);

        private:

            void tickChunk(const math::vec2 &position);

            components::ChunkIndex chunkPosToIndex(const math::vec2 &position) const;

            math::vec3 chunkIndexToPos(const components::ChunkIndex &chunkIndex) const;

            math::vec3 roadTileIndexToPos(const components::ChunkIndex &chunkIndex,
                                          components::RoadTileIndex roadTileIndices) const;

            math::vec2 unpackCoordinates(common::uint64 coord) const;

            void generateTilesForChunk(common::int64 xChunkIndex,
                                       common::int64 yChunkIndex);

            common::EntityID generateSprite(math::vec4 color, math::vec2 tileSize, math::vec3 worldPos);

            common::EntityID generateTexture(const math::vec2 &size,
                                             const math::vec3 &worldPos,
                                             const std::string &path);

            void generateRoadsForChunk(common::int64 chunkX, common::int64 chunkY);

            void generateChunkTexture(common::int64 chunkX, common::int64 chunkY);

            void generateChunkSprite(common::int64 chunkX, common::int64 chunkY);

            void generateRoadTile(const components::ChunkIndex &chunkIndex,
                                  const components::RoadTileIndex &roadTileIndex);

            void generateTexturedRoadTile(const components::ChunkIndex &chunkIndex,
                                          const components::RoadTileIndex &roadTileIndex,
                                          const std::string &texturePath);

            void generateRoadTileBetween(const components::ChunkIndex &chunkIndex,
                                         components::RoadTileIndex startingRoadTileIndex,
                                         components::RoadTileIndex endingRoadTileIndex);

            bool existsInMap(common::uint64 packedIndex, const std::map<common::uint64, common::EntityID> &map) const;

            bool shouldGenerateRoad(const components::ChunkIndex &chunkIndex) const;

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
            std::map<common::uint64, common::EntityID> mTileLookup{};
            std::map<common::uint64, common::EntityID> mRoadLookup{};
            std::map<common::uint64, common::EntityID> mChunkLookup{};

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
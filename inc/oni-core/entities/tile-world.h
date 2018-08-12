#pragma once

#include <map>

#include <entt/entity/registry.hpp>

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec4.h>
#include <oni-core/entities/create-entity.h>
#include <oni-core/components/geometry.h>

namespace oni {
    namespace entities {
        class TileWorld {
            using PackedIndiciesToEntity = typename std::map<common::uint64, entities::EntityID>;
        public:

            TileWorld();

            ~TileWorld();

            /**
             * Prepares the chunk in the given position and its neighbours
             *
             * @param position location in the world coordinate to tick
             */
            // TODO: Instead of a car the function should go through all the car entities in the world and
            // update as needed.
            void tick(const math::vec2 &position, const components::Car &car, entt::DefaultRegistry &entities);

        private:

            void tickChunk(const math::vec2 &position, entt::DefaultRegistry &entities);

            components::ChunkIndices chunkPositionToIndex(const math::vec2 &position) const;

            math::vec2 chunkIndexToPosition(const components::ChunkIndices &chunkIndices) const;

            math::vec2 roadTileIndexToPosition(const components::ChunkIndices &chunkIndices,
                                               components::RoadTileIndices roadTileIndices) const;

            math::vec2 unpackCoordinates(common::uint64 coord) const;

            void generateTilesForChunk(common::int64 xIndex, common::int64 yIndex,
                                       entt::DefaultRegistry &entities);

            components::BoarderRoadTiles
            generateRoadsForChunk(const components::ChunkIndices &chunkIndices,
                                  entt::DefaultRegistry &entities);


            void generateRoadTile(const components::ChunkIndices &chunkIndices,
                                  const components::RoadTileIndices &roadTileIndices,
                                  entt::DefaultRegistry &entities);

            void generateTexturedRoadTile(const components::ChunkIndices &chunkIndices,
                                          const components::RoadTileIndices &roadTileIndices,
                                          const std::string &texturePath,
                                          entt::DefaultRegistry &entities);

            void generateRoadTileBetween(const components::ChunkIndices &chunkIndices,
                                         components::RoadTileIndices startingRoadTileIndices,
                                         components::RoadTileIndices endingRoadTileIndices,
                                         entt::DefaultRegistry &entities);

            bool existsInMap(common::uint64 packedIndices, const PackedIndiciesToEntity &map) const;

            bool chunkWithRoads(const components::ChunkIndices &chunkIndices) const;

        private:
            /**
             * A tile is determined by its lower left coordinate in the world. This coordinate is
             * packed into a uint64 and the lookup table mCoordToTileLookup returns the entity ID
             * corresponding to the tile.
             */
            PackedIndiciesToEntity mPackedTileIndicesToEntity{};
            PackedIndiciesToEntity mPackedRoadTileToEntity{};
            PackedIndiciesToEntity mPackedRoadChunkIndicesToEntity{};

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
        };
    }
}
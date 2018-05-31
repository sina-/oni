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
            using PackedIndiciesToEntity = typename std::map<common::uint64, entities::entityID>;
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
            void tick(const math::vec2 &position, const components::Car &car, entt::DefaultRegistry &foregroundEntities,
                      entt::DefaultRegistry &backgroundEntities);

        private:

            void tickCars(const components::Car &car, entt::DefaultRegistry &foregroundEntities);

            void tickChunk(const math::vec2 &position, entt::DefaultRegistry &backgroundEntities);

            common::int64 positionToIndex(const common::real64 position, const common::uint16 tileSize) const;

            common::real32 indexToPosition(const common::int64 index, const common::uint16 tileSize) const;

            components::ChunkIndices chunkPositionToIndex(const math::vec2 &position) const;

            math::vec2 chunkIndexToPosition(const components::ChunkIndices &chunkIndices) const;

            math::vec2 roadTileInexToPosition(const components::ChunkIndices &chunkIndices,
                                               const components::RoadTileIndices roadTileIndices) const;

            /**
             * Pack two unique int32 values into unique uint64.
             * @param x
             * @return pack uint64 value
             */
            common::packedInt32 packIntegers(const common::int64 x, const common::int64 y) const;

            math::vec2 unpackCoordinates(common::uint64 coord) const;

            void generateTilesForChunk(const common::int64 xIndex, const common::int64 yIndex,
                                       entt::DefaultRegistry &backgroundEntities);

            entities::entityID createSkidTileIfMissing(const math::vec2 &position,
                                                       entt::DefaultRegistry &foregroundEntities);

            components::BoarderRoadTiles
            generateRoadsForChunk(const components::ChunkIndices &chunkIndices,
                                  entt::DefaultRegistry &backgroundEntities);

            void updateSkidTexture(const math::vec3 &position, entities::entityID skidTextureEntity,
                                   entt::DefaultRegistry &foregroundEntities, common::uint8 alpha);

            void generateRoadTile(const components::ChunkIndices &chunkIndices,
                                  const components::RoadTileIndices &roadTileIndices,
                                  entt::DefaultRegistry &backgroundEntities);

            void
            generateRoadTileBetween(const components::ChunkIndices &chunkIndices,
                                    components::RoadTileIndices startingRoadTileIndices,
                                    components::RoadTileIndices endingRoadTileIndices,
                                    entt::DefaultRegistry &backgroundEntities);

            bool existsInMap(const common::uint64 packedIndices, const PackedIndiciesToEntity &map) const;

            bool chunkWithRoads(const components::ChunkIndices &chunkIndices) const;

        private:
            /**
             * A tile is determined by its lower left coordinate in the world. This coordinate is
             * packed into a uint64 and the lookup table mCoordToTileLookup returns the entity ID
             * corresponding to the tile.
             */
            PackedIndiciesToEntity mPackedTileIndicesToEntity{};
            PackedIndiciesToEntity mPackedSkidIndicesToEntity{};
            PackedIndiciesToEntity mPackedRoadTileToEntity{};
            PackedIndiciesToEntity mPackedRoadChunkIndicesToEntity{};

            const common::uint16 mTileSizeX{0};
            const common::uint16 mTileSizeY{0};

            //const common::real32 mHalfTileSizeX{0.0f};
            //const common::real32 mHalfTileSizeY{0.0f};

            const common::uint16 mSkidTileSizeX{0};
            const common::uint16 mSkidTileSizeY{0};

            const common::real32 mHalfSkidTileSizeX{0.0f};
            const common::real32 mHalfSkidTileSizeY{0.0f};

            const common::uint16 mTilesPerChunkX{0};
            const common::uint16 mTilesPerChunkY{0};

            const common::uint16 mChunkSizeX{0};
            const common::uint16 mChunkSizeY{0};

            const common::uint16 mHalfChunkSizeX{0};
            const common::uint16 mHalfChunkSizeY{0};
        };
    }
}
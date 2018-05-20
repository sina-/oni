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
        public:
            TileWorld();

            ~TileWorld();

            /**
             * Prepares the tiles at given position for the tickRadius so that information will be available
             * to the renderer or other systems.
             *
             * @param position location in the world coordinate to tick
             * @param viewWidth in game units
             * @param viewHeight in game units
             */
            // TODO: Instead of a car the function should go through all the car entities in the world and
            // update as needed.
            void tick(const math::vec2 &position, common::uint16 viewWidth, common::uint16 viewHeight,
                      const components::Car &car, entt::DefaultRegistry &foregroundEntities,
                      entt::DefaultRegistry &backgroundEntities);

            common::uint16 getTileSizeX() const;

            common::uint16 getTileSizeY() const;

            /**
             * Find corresponding tile index for a given x.
             * @param position in world coordinates
             * @return index
             */
            common::int64 positionToIndex(const common::real64 position, const common::uint16 tileSize) const;

            /**
             * Do the inverse of getTileIndexX()
             * @param index
             * @return minimum x in the tile corresponding to the given index.
             */
            common::real32 indexToPosition(const common::int64 index, const common::uint16 tileSize,
                                           const common::real32 halfTileSize) const;

        private:
            /**
             * Pack real32 values in the range (x +-mTileSizeX, y +-mTileSizeY) into a uint64.
             * For example: any x and y from 0 to 15.99999 is saved into the same long.
             * @param x
             * @return pack uint64 value
             */
            common::packedInt32 packIntegers(const common::int64 x, const common::int64 y) const;

            math::vec2 unpackCoordinates(common::uint64 coord) const;

            bool tileExists(common::uint64 tileCoordinates) const;

            bool skidTileExists(common::uint64 tileCoordinates) const;

            void createTileIfMissing(const math::vec2 &tileForPosition, entt::DefaultRegistry &backgroundEntities);

            entities::entityID createSkidTileIfMissing(const math::vec2 &position,
                                                       entt::DefaultRegistry &foregroundEntities);

            void updateSkidTexture(const math::vec3 &position, entities::entityID skidTextureEntity,
                                   entt::DefaultRegistry &foregroundEntities, common::uint8 alpha);

        public:
            /**
             * A tile is determined by its lower left coordinate in the world. This coordinate is
             * packed into a uint64 and the lookup table mCoordToTileLookup returns the entity ID
             * corresponding to the tile.
             */
            std::map<common::uint64, entities::entityID> mPackedTileIndicesToEntity{};
            std::map<common::uint64, entities::entityID> mSkidPackedIndicesToEntity{};

            const common::uint16 mTileSizeX{0};
            const common::uint16 mTileSizeY{0};

            const common::real32 mHalfTileSizeX{0.0f};
            const common::real32 mHalfTileSizeY{0.0f};

            const common::uint16 mSkidTileSizeX{0};
            const common::uint16 mSkidTileSizeY{0};

            const common::real32 mHalfSkidTileSizeX{0.0f};
            const common::real32 mHalfSkidTileSizeY{0.0f};

            math::vec2 mSkidTileSize{0.0f, 0.0f};
        };
    }
}
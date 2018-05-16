#pragma once

#include <map>

#include <entt/entity/registry.hpp>

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec4.h>
#include <oni-core/entities/create-entity.h>
#include <oni-core/components/geometry.h>

// TODO: Need to use int32_t and its siblings instead of int and long
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
             * @param x in world coordinates
             * @return index
             */
            common::int64 getTileXIndex(common::real64 x) const;

            /**
             * Find corresponding tile index for a given y.
             * @param y in world coordinates
             * @return index
             */
            common::int64 getTileYIndex(common::real64 y) const;

            /**
             * Do the inverse of getTileIndexX()
             * @param xIndex
             * @return minimum x in the tile corresponding to the given index.
             */
            common::real32 getTilePosForXIndex(common::int64 xIndex) const;

            common::real32 getTilePosForYIndex(common::int64 yIndex) const;

        private:
            /**
             * Pack common::real32 values in the range (x +-16, y +-16) into a uint64.
             * For example: any x and y from 0 to 15.99999 is saved into the same long.
             * @param position
             * @return pack uint64 value
             */
            common::packedTileCoordinates packCoordinates(const math::vec2 &position) const;

            math::vec2 unpackCoordinates(common::uint64 coord) const;

            bool tileExists(common::uint64 tileCoordinates) const;

            bool skidTileExists(common::uint64 tileCoordinates) const;

            void createTileIfMissing(const math::vec2 &position, entt::DefaultRegistry &backgroundEntities);

            entities::entityID createSkidTileIfMissing(const math::vec2 &position,
                                                       entt::DefaultRegistry &foregroundEntities);

            void updateSkidTexture(const math::vec3 &position, entities::entityID skidTextureEntity,
                                   entt::DefaultRegistry &foregroundEntities, common::uint8 alpha);

        public:
            std::map<common::uint64, entities::entityID> mCoordToTileLookup{};
            std::map<common::uint64, entities::entityID> mCoordToSkidLineLookup{};

            const common::uint16 mTileSizeX{0};
            const common::uint16 mTileSizeY{0};

            const common::real32 mHalfTileSizeX{};
            const common::real32 mHalfTileSizeY{};

            math::vec2 mSkidSize{0.0f, 0.0f};
        };
    }
}
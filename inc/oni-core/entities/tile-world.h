#pragma once

#include <map>

#include <entt/entity/registry.hpp>

#include <oni-core/math/vec2.h>
#include <oni-core/math/vec4.h>
#include <oni-core/entities/create-entity.h>

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
             * @param tickRadius in game units that is meters
             */
            void tick(const math::vec2 &position, common::uint16 tickRadius);

            common::uint16 getTileSizeX() const;

            common::uint16 getTileSizeY() const;

            common::int64 getTileIndexX(common::real64 x) const;

            common::int64 getTileIndexY(common::real64 y) const;

            entt::DefaultRegistry &getRegistry();

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

            void createTileIfMissing(const math::vec2 &position);

        public:
            std::map<common::uint64, entities::entityID> mCoordToTileLookup{};
            std::unique_ptr<entt::DefaultRegistry> mTileRegistry{};

            // Square tiles of 16mx16m
            const common::uint16 mTileSizeX{16};
            const common::uint16 mTileSizeY{16};
        };
    }
}
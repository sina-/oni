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
            void tick(const math::vec2 &position, unsigned int tickRadius);

            // TODO: Template the function and call it getTileData
            math::vec4 getTileColor(const math::vec2 &position) const;

            int getTileSizeX() const;

            int getTileSizeY() const;

            int getTileIndexX(float x) const;

            int getTileIndexY(float y) const;

        private:
            /**
             * Pack float values in the range (x +-16, y +-16) into a long.
             * For example: any x and y from 0 to 15.99999 is saved into the same long.
             * @param position
             * @return pack long value
             */
            long packCoordinates(const math::vec2 &position) const;

            math::vec2 unpackCoordinates(long coord) const;

            bool tileExists(long tileCoordinates) const;

        public:
            std::map<long, entities::entityID> mCoordToTileLookup{};
            std::unique_ptr<entt::DefaultRegistry> mTileRegistry{};

            // Square tiles of 16mx16m
            int mTileSizeX{16};
            int mTileSizeY{16};
        };
    }
}
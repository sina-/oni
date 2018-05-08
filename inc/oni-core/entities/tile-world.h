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

            void tick(const math::vec2 &position, unsigned int distanceToRightSideOfScreen);

            math::vec4 getTileColor(const math::vec2 &position);

        private:
            /**
             * Pack float values in the range (x +-16, y +-16) into a long.
             * For example: any x and y from 0 to 15.99999 is saved into the same long.
             * @param position
             * @return
             */
            long packCoords(const math::vec2 &position);

            math::vec2 unpackCoords(long coord);

            int getTileSize() const;

            /**
             * Given a packed coordinate, using packCoords(), return the tile ID that (x, y) are within.
             * @param tileCoords
             * @return
             */
            entities::entityID getTileID(double tileCoords) const;

            bool tileExsists(long tileCoords) const;

        public:
            std::map<long, entities::entityID> mCoordToTileLookup{};
            std::unique_ptr<entt::DefaultRegistry> mTileRegistry{};

            // Square tiles of 16mx16m
            int mTileSize{16};
        };
    }
}
#pragma once

#include <graphics/tilelayer.h>
#include <utils/oni_assert.h>

namespace oni {
    namespace graphics {
        /**
         * Same as TileLayer but every DynamicSprite can be updated.
         */
        class DynamicTileLayer : public TileLayer {
            // Hide it, to make sure only DynamicSprite objects are
            // added to this tile.
            void add(std::unique_ptr<Renderable2D>) override { ONI_DEBUG_ASSERT(false); };

        public:
            DynamicTileLayer(std::unique_ptr<Shader> shader, unsigned long maxSprintCount) : TileLayer(
                    std::move(shader), maxSprintCount) {}

            void add(std::unique_ptr<DynamicSprite> renderable);

            void update(int key);

        };
    }

}

#pragma once

#include <graphics/shader.h>
#include <graphics/layer.h>

namespace oni {
    namespace graphics {

        /**
         * A Layer with top-down projection matrix.
         */
        class TileLayer : public Layer {
        public:
            TileLayer(std::unique_ptr<Shader> shader, unsigned long maxSpriteCount);

            ~TileLayer() override = default;
        };

    }
}

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

/*            void add(std::unique_ptr<Renderable2D> renderable) override {
                m_Renderables.push_back(std::move(renderable));
            }*/

        };

    }
}

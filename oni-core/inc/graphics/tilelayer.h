#pragma once

#include <graphics/renderable2d.h>
#include <graphics/shader.h>
#include <graphics/layer.h>

namespace oni {
    namespace graphics {

        class TileLayer : public Layer {
        public:
            TileLayer(std::unique_ptr<Shader> shader, unsigned long maxSpriteCount);

            ~TileLayer() = default;

            void add(std::unique_ptr<Renderable2D> renderable) override {
                m_Renderables.push_back(std::move(renderable));
            }

            void render() override;

        };

    }
}

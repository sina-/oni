#pragma once

#include <graphics/tilelayer.h>

namespace oni {
    namespace graphics {
        class DynamicTileLayer : public TileLayer {
        public:
            DynamicTileLayer(std::unique_ptr<Shader> shader, unsigned long maxSprintCount) : TileLayer(
                    std::move(shader), maxSprintCount) {}

            void update(int key){
                for(const auto & renderable: m_Renderables){
                    renderable->update(key, m_Shader);
                }
            }

        };
    }

}

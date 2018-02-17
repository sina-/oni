
#include <graphics/dynamic-tile-layer.h>

namespace oni {
    namespace graphics {

        void DynamicTileLayer::add(std::unique_ptr<components::Renderable2D> renderable) {
            m_Renderables.push_back(std::move(renderable));
        }

        void DynamicTileLayer::update(int key) {
            for (auto &renderable: m_Renderables) {
                // TODO: Maybe there is a better way to model DynamicTile and DynamicSprite
                // to avoid the need for down-casting.
                const auto d = dynamic_cast<components::Renderable2D *>(renderable.get());
                // TODO: move update function from car here
                //d->update(key, *m_Shader);
            }
        }
    }
}

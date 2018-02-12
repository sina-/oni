
#include <graphics/dynamictilelayer.h>

namespace oni {
    namespace graphics {

        void DynamicTileLayer::add(std::unique_ptr<DynamicSprite> renderable) {
            m_Renderables.push_back(std::move(renderable));
        }

        void DynamicTileLayer::update(int key) {
            for (auto &renderable: m_Renderables) {
                // TODO: Maybe there is a better way to model DynamicTile and DynamicSprite
                // to avoid the need for down-casting.
                const auto d = dynamic_cast<DynamicSprite *>(renderable.get());
                d->update(key, *m_Shader);
            }
        }
    }
}

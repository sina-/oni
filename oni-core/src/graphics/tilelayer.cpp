#include <graphics/tilelayer.h>
#include <graphics/batch-renderer2d.h>

namespace oni {
    namespace graphics {

        TileLayer::TileLayer(std::unique_ptr<Shader> shader, unsigned long maxSpriteCount) : Layer(
                std::make_unique<BatchRenderer2D>(maxSpriteCount), std::move(shader),
                math::mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f)) {
        }
    }
}
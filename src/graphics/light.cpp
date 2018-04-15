#include <oni-core/graphics/light.h>
#include <oni-core/graphics/shader.h>

namespace oni {
    namespace graphics {

        void Light::tick(Layer &layer, double mouseX, double mouseY, int windowWidth, int windowHeight) {
            auto &shader = layer.getShader();
            shader.enable();
            // TODO: Hardcoded value
            shader.setUniform2f("light_pos", math::vec2{static_cast<float>(mouseX * 16.0f / windowWidth - 8.0f),
                                                        static_cast<float>(9.0f - mouseY * 9.0f / windowHeight -
                                                                           4.5f)});
            shader.disable();
        }
    }
}
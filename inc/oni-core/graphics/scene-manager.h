#pragma once

#include <map>
#include <memory>

#include <entt/entt.hpp>

#include <oni-core/components/visual.h>
#include <oni-core/components/buffer.h>
#include <oni-core/math/mat4.h>

namespace oni {
    namespace graphics {
        class Shader;

        class BatchRenderer2D;

        class Renderer2D;

        class SceneManager {
        public:
            explicit SceneManager(const components::ScreenBounds &screenBounds);

            ~SceneManager();

            void render(entt::DefaultRegistry &registry);

            // TODO: What about a camera class?
            void lookAt(float x, float y);

            void lookAt(float x, float y, float distance);

        private:
            void begin(const Shader &shader, Renderer2D &renderer2D);

            void end(const Shader &shader, Renderer2D &renderer2D);

            void initializeTextureRenderer(const Shader &shader);

            void initializeColorRenderer(const Shader &shader);

        private:
            std::unique_ptr<Shader> mColorShader;
            std::unique_ptr<Shader> mTextureShader;
            std::unique_ptr<BatchRenderer2D> mColorRenderer;
            std::unique_ptr<BatchRenderer2D> mTextureRenderer;

            math::mat4 mModelMatrix{};
            math::mat4 mViewMatrix{};
            math::mat4 mProjectionMatrix{};

            const unsigned int mMaxSpriteCount{0};

        };
    }
}

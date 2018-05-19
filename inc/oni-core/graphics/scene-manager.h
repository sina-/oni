#pragma once

#include <map>
#include <memory>

#include <entt/entity/registry.hpp>

#include <oni-core/components/visual.h>
#include <oni-core/components/buffer.h>
#include <oni-core/math/mat4.h>
#include <oni-core/entities/tile-world.h>

namespace oni {
    namespace math {
        class vec2;
    }

    namespace entities {
        class TileWorld;
    }

    namespace graphics {
        class Shader;

        class BatchRenderer2D;

        class Renderer2D;

        class SceneManager {
        public:
            explicit SceneManager(const components::ScreenBounds &screenBounds);

            ~SceneManager();

            void render(entt::DefaultRegistry &registry);

            void lookAt(common::real32 x, common::real32 y);

            void lookAt(common::real32 x, common::real32 y, common::real32 distance);

            const math::mat4 &getProjectionMatrix() const;

            const math::mat4 &getViewMatrix() const;

            common::real32 getViewWidth() const;

            common::real32 getViewHeight() const;

            common::uint16 getSpritesPerFrame() const;

            common::uint16 getTexturesPerFrame() const;

            void resetCounters();

        private:
            void begin(const Shader &shader, Renderer2D &renderer2D);

            void end(const Shader &shader, Renderer2D &renderer2D);

            void initializeTextureRenderer(const Shader &shader);

            void initializeColorRenderer(const Shader &shader);

            bool visibleToCamera(const components::Shape &shape) const;

        private:
            std::unique_ptr<Shader> mColorShader;
            std::unique_ptr<Shader> mTextureShader;
            std::unique_ptr<BatchRenderer2D> mColorRenderer;
            std::unique_ptr<BatchRenderer2D> mTextureRenderer;

            math::mat4 mModelMatrix{};
            math::mat4 mViewMatrix{};
            math::mat4 mProjectionMatrix{};

            components::ScreenBounds mScreenBounds{};
            components::Camera mCamera{0.0f, 0.0f, 1.0f};

            const common::uint16 mMaxSpriteCount{0};

            common::uint16 mRenderedSpritesPerFrame{0};
            common::uint16 mRenderedTexturesPerFrame{0};
        };
    }
}

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

        class TextureManager;

        class SceneManager {
        public:
            explicit SceneManager(const components::ScreenBounds &screenBounds);

            ~SceneManager();

            void render(entt::DefaultRegistry &registry);

            void tick(entt::DefaultRegistry &registry);

            void renderRaw(const components::Shape &shape, const components::Appearance &appearance);

            void lookAt(common::real32 x, common::real32 y);

            void lookAt(common::real32 x, common::real32 y, common::real32 distance);

            const math::mat4 &getProjectionMatrix() const;

            const math::mat4 &getViewMatrix() const;

            common::real32 getViewWidth() const;

            common::real32 getViewHeight() const;

            common::uint16 getSpritesPerFrame() const;

            common::uint16 getTexturesPerFrame() const;

            void resetCounters();

            // TODO: This is awful and inconsistent with the API of this class where I should only expose render().
            // Instead of exposing the internals, I can batch the render objects in DebugDrawBox2D and then
            // pass them to a new render() function that receives a vector of sprites
            // .
            void beginColorRendering();

            void endColorRendering();

        private:
            void begin(const Shader &shader, Renderer2D &renderer2D);

            void end(const Shader &shader, Renderer2D &renderer2D);

            void initializeTextureRenderer(const Shader &shader);

            void initializeColorRenderer(const Shader &shader);

            bool visibleToCamera(const components::Shape &shape, const common::real32 halfViewWidth,
                                 const common::real32 halfViewHeight) const;

            void prepareTexture(components::Texture &texture);

            entities::entityID createSkidTileIfMissing(const math::vec2 &position,
                                                       entt::DefaultRegistry &foregroundEntities);

            void updateSkidTexture(const math::vec3 &position, entities::entityID skidTextureEntity,
                                   entt::DefaultRegistry &foregroundEntities, common::uint8 alpha);

        private:
            std::unique_ptr<Shader> mColorShader{};
            std::unique_ptr<Shader> mTextureShader{};
            std::unique_ptr<BatchRenderer2D> mColorRenderer{};
            std::unique_ptr<BatchRenderer2D> mTextureRenderer{};
            std::unique_ptr<TextureManager> mTextureManager{};

            math::mat4 mModelMatrix{};
            math::mat4 mViewMatrix{};
            math::mat4 mProjectionMatrix{};

            std::map<common::uint64, entities::entityID> mPackedSkidIndicesToEntity{};

            const common::uint16 mSkidTileSizeX{0};
            const common::uint16 mSkidTileSizeY{0};

            const common::real32 mHalfSkidTileSizeX{0.0f};
            const common::real32 mHalfSkidTileSizeY{0.0f};


            components::ScreenBounds mScreenBounds{};
            components::Camera mCamera{0.0f, 0.0f, 1.0f};

            const common::uint16 mMaxSpriteCount{0};

            common::uint16 mRenderedSpritesPerFrame{0};
            common::uint16 mRenderedTexturesPerFrame{0};
        };
    }
}

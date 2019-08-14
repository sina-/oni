#pragma once

#include <map>
#include <queue>
#include <memory>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/math/oni-math-mat4.h>
#include <oni-core/gameplay/oni-gameplay-lap-tracker.h>
#include <oni-core/graphic/oni-graphic-camera.h>
#include <oni-core/graphic/oni-graphic-renderer.h>

class b2World;

namespace oni {
    namespace math {
        struct vec2;

        class Rand;

        class ZLayerManager;
    }
    namespace asset {
        class AssetManager;
    }

    namespace entities {
        class EntityManager;
    }

    namespace component {
        struct Shape;
    }

    namespace graphic {
        class Renderer_OpenGL_Tessellation;

        class Renderer_OpenGL_Strip;

        class Renderer_OpenGL_Quad;

        class Renderer;

        class TextureManager;

        class DebugDrawBox2D;

        struct Brush;

        class SceneManager {
        public:
            // TODO: Getting quite big, I can split it into, SceneRenderer, SceneUpdater, and SceneManager
            SceneManager(const graphic::ScreenBounds &,
                         asset::AssetManager &,
                         math::ZLayerManager &,
                         graphic::TextureManager &,
                         b2World &);

            ~SceneManager();

            void
            submit(entities::EntityManager &);

            void
            render();

            void
            _render(entities::EntityManager &);

            void
            renderInternal();

            static void
            updateSmokeEmitter(entities::EntityManager &,
                               common::r64 tickTime);

            void
            updateAfterMark(entities::EntityManager &,
                            common::r64 tickTime);

            static void
            updateTextureAnimated(entities::EntityManager &,
                                  common::r64 tickTime);

            void
            renderRaw(const component::WorldP3D pos,
                      const component::Color &color);

            void
            blend(const component::Texture &front,
                  component::Texture &back);

            void
            renderToTexture(const component::Quad &quad,
                            const component::Texture &src,
                            const graphic::ScreenBounds &destBounds,
                            component::Texture &dest,
                            const math::mat4 *model);

            void
            renderToTexture(const component::Quad &quad,
                            const component::Color &src,
                            const graphic::ScreenBounds &destBounds,
                            component::Texture &dest,
                            const math::mat4 *model);

            void
            lookAt(common::r32 x,
                   common::r32 y);

            void
            lookAt(common::r32 x,
                   common::r32 y,
                   common::r32 distance);

            void
            zoom(common::r32 distance);

            bool
            isVisible(const component::WorldP3D &);

            bool
            isVisible(const component::WorldP3D &,
                      const component::Scale &);

            void
            prepareTexture(entities::EntityManager &manager,
                           common::EntityID id,
                           component::EntityPreset tag);

            common::u16
            getSpritesPerFrame() const;

            common::u16
            getParticlesPerFrame() const;

            common::u16
            getTexturesPerFrame() const;

            const graphic::ScreenBounds &
            getScreenBounds() const;

            void
            resetCounters();

            void
            renderPhysicsDebugData();

            // TODO: This is awful and inconsistent with the API of this class where I should only expose render().
            // Instead of exposing the internals, I can batch the render objects in DebugDrawBox2D and then
            // pass them to a new render() function that receives a vector of sprites
            // .
            void
            beginColorRendering();

            void
            endColorRendering();

        public:
            struct WorldP3DAndHeading {
                component::WorldP3D pos;
                component::Heading heading;
            };

            // TODO: Move somewhere else
            static SceneManager::WorldP3DAndHeading
            applyParentTransforms(const entities::EntityManager &manager,
                                  common::EntityID child,
                                  const component::WorldP3D &pos,
                                  const component::Heading &heading);


        private:
            void
            renderStaticText(entities::EntityManager &,
                             common::r32 viewWidth,
                             common::r32 viewHeight);

            void
            renderStrip(entities::EntityManager &,
                        common::r32 viewWidth,
                        common::r32 viewHeight);

            void
            renderTessellationColor(entities::EntityManager &,
                                    common::r32 viewWidth,
                                    common::r32 viewHeight);

            void
            renderTessellationTexture(entities::EntityManager &,
                                      common::r32 viewWidth,
                                      common::r32 viewHeight);

        private:
            static void
            begin(Renderer &,
                  const RenderSpec &);

            math::mat4
            getCameraScale();

            math::mat4
            getCameraTranslation();

            static void
            end(Renderer &renderer2D);

            void
            setMVP(RenderSpec &,
                   bool translate,
                   bool scale,
                   bool project);

            static void
            setMVP(RenderSpec &,
                   const ScreenBounds &,
                   const math::mat4 *model);

            math::vec2
            getScreenSize();

            common::r32
            getViewWidth() const;

            common::r32
            getViewHeight() const;

        private:
            std::unique_ptr<Renderer_OpenGL_Tessellation> mRendererTessellation{};
            std::unique_ptr<Renderer_OpenGL_Strip> mRendererStrip{};
            std::unique_ptr<Renderer_OpenGL_Quad> mRendererQuad{};

            TextureManager &mTextureManager;
            std::unique_ptr<DebugDrawBox2D> mDebugDrawBox2D{};
            b2World &mPhysicsWorld;

            std::unique_ptr<math::Rand> mRand{};

            math::mat4 mModelMatrix{};
            math::mat4 mViewMatrix{};
            math::mat4 mProjectionMatrix{};

            graphic::ScreenBounds mScreenBounds{};
            graphic::Camera mCamera{0.0f, 0.0f, 1.0f};

            const common::u16 mMaxSpriteCount{0};

            common::u16 mRenderedSpritesPerFrame{0};
            common::u16 mRenderedTexturesPerFrame{0};
            common::u16 mRenderedParticlesPerFrame{0};

            math::ZLayerManager &mZLayerManager;

            using RenderableQueue = std::priority_queue<Renderable>;
            std::array<RenderableQueue, math::enumCast(component::MaterialFinish_Type::LAST)> mRenderables{};
        };
    }
}

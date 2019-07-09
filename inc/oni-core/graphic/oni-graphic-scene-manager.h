#pragma once

#include <map>
#include <memory>

#include <oni-core/component/oni-component-visual.h>
#include <oni-core/math/oni-math-mat4.h>
#include <oni-core/gameplay/oni-gameplay-lap-tracker.h>
#include <oni-core/graphic/oni-graphic-camera.h>

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
                         b2World &,
                         common::r32 gameUnitToPixel
            );

            ~SceneManager();

            void
            render(entities::EntityManager &serverManager,
                   entities::EntityManager &clientManager);

            static void
            updateSmokeEmitter(entities::EntityManager &,
                               common::r64 tickTime);

            void
            updateAfterMark(entities::EntityManager &serverManager,
                            entities::EntityManager &clientManager,
                            common::r64 tickTime);

            void
            renderRaw(const component::WorldP3D pos,
                      const component::Color &color);

            void
            splat(entities::EntityManager &,
                  const component::WorldP3D &,
                  const component::Scale &,
                  const graphic::Brush &);

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
            prepareTexture(entities::EntityManager &,
                           common::EntityID,
                           component::TextureTag);

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

        private:
            void
            renderStaticText(entities::EntityManager &,
                             common::r32 viewWidth,
                             common::r32 viewHeight);

            void
            renderTessellation(entities::EntityManager &serverManager,
                               entities::EntityManager &clientManager,
                               common::r32 viewWidth,
                               common::r32 viewHeight);

            void
            renderStrip(entities::EntityManager &serverManager,
                        entities::EntityManager &clientManager,
                        common::r32 viewWidth,
                        common::r32 viewHeight);

            void
            renderQuad(entities::EntityManager &serverManager,
                       entities::EntityManager &clientManager,
                       common::r32 viewWidth,
                       common::r32 viewHeight);

            void
            renderTessellationColor(entities::EntityManager &,
                                    common::r32 viewWidth,
                                    common::r32 viewHeight);

        private:
            struct WorldP3DAndHeading {
                component::WorldP3D pos;
                component::Heading heading;
            };

        private:
            void
            begin(Renderer &renderer2D,
                  bool translate,
                  bool scale,
                  bool project);

            static void
            end(Renderer &renderer2D);

            void
            updateCanvasTile(entities::EntityManager &entityManager,
                             common::EntityID entityID,
                             const graphic::Brush &,
                             const component::WorldP3D &,
                             const component::Scale &);


            common::EntityID
            getOrCreateCanvasTile(entities::EntityManager &,
                                  const component::WorldP3D &pos);

            static SceneManager::WorldP3DAndHeading
            applyParentTransforms(const entities::EntityManager &manager,
                                  common::EntityID child,
                                  const component::WorldP3D &pos,
                                  const component::Heading &heading);

            common::r32
            getViewWidth() const;

            common::r32
            getViewHeight() const;

        private:
            std::unique_ptr<Renderer_OpenGL_Tessellation> mRendererTessellation{};
            std::unique_ptr<Renderer_OpenGL_Strip> mRendererStrip{};
            std::unique_ptr<Renderer_OpenGL_Quad> mRendererQuad{};

            std::unique_ptr<TextureManager> mTextureManager{};
            std::unique_ptr<DebugDrawBox2D> mDebugDrawBox2D{};
            asset::AssetManager &mAssetManager;
            b2World &mPhysicsWorld;

            std::unique_ptr<math::Rand> mRand{};

            math::mat4 mModelMatrix{};
            math::mat4 mViewMatrix{};
            math::mat4 mProjectionMatrix{};

            std::map<common::u64, common::EntityID> mCanvasTileLookup{};

            const common::u16 mCanvasTileSizeX{0};
            const common::u16 mCanvasTileSizeY{0};
            const common::r32 mHalfCanvasTileSizeX{0.0f};
            const common::r32 mHalfCanvasTileSizeY{0.0f};

            graphic::ScreenBounds mScreenBounds{};
            graphic::Camera mCamera{0.0f, 0.0f, 1.0f};

            const common::u16 mMaxSpriteCount{0};
            const common::r32 mGameUnitToPixels{0};

            common::u16 mRenderedSpritesPerFrame{0};
            common::u16 mRenderedTexturesPerFrame{0};
            common::u16 mRenderedParticlesPerFrame{0};

            math::ZLayerManager &mZLayerManager;
        };
    }
}

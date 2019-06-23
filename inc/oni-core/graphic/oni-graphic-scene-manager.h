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

        class Renderer2D;

        class TextureManager;

        class FontManager;

        class DebugDrawBox2D;

        struct Brush;

        class SceneManager {
        public:
            // TODO: Getting quite big, I can split it into, SceneRenderer, SceneUpdater, and SceneManager
            SceneManager(const graphic::ScreenBounds &,
                         asset::AssetManager &,
                         FontManager &,
                         math::ZLayerManager &,
                         b2World &,
                         common::r32
            );

            ~SceneManager();

            void
            render(entities::EntityManager &serverManager,
                   entities::EntityManager &clientManager);

            void
            tick(const entities::EntityManager &server,
                 entities::EntityManager &client,
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
            renderTessellationColor(entities::EntityManager &,
                                    common::r32 viewWidth,
                                    common::r32 viewHeight);

        private:
            struct RaceInfoEntities {
                common::EntityID lapEntity{0};
                common::EntityID lapTimeEntity{0};
                common::EntityID lapBestTimeEntity{0};
            };

            struct WorldP3DAndHeading {
                component::WorldP3D pos;
                component::Heading heading;
            };

        private:
            void
            begin(Renderer2D &renderer2D,
                  bool translate,
                  bool scale,
                  bool project);

            static void
            end(Renderer2D &renderer2D);

            void
            initRenderer();

            common::EntityID
            createText(entities::EntityManager &,
                       const component::WorldP3D &worldPos,
                       const std::string &text);

            void
            updateRaceInfo(entities::EntityManager &,
                           const gameplay::CarLapInfo &carLap,
                           const RaceInfoEntities &carLapTextEntities);

            void
            updateCanvasTile(entities::EntityManager &entityManager,
                             common::EntityID entityID,
                             const graphic::Brush &,
                             const component::WorldP3D &,
                             const component::Scale &);


            const RaceInfoEntities &
            getOrCreateLapText(entities::EntityManager &,
                               common::EntityID carEntityID,
                               const gameplay::CarLapInfo &carLap);

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

            std::unique_ptr<TextureManager> mTextureManager{};
            std::unique_ptr<DebugDrawBox2D> mDebugDrawBox2D{};
            asset::AssetManager &mAssetManager;
            graphic::FontManager &mFontManager;
            b2World &mPhysicsWorld;

            std::unique_ptr<math::Rand> mRand{};

            math::mat4 mModelMatrix{};
            math::mat4 mViewMatrix{};
            math::mat4 mProjectionMatrix{};

            std::map<common::u64, common::EntityID> mCanvasTileLookup{};
            std::map<common::EntityID, RaceInfoEntities> mLapInfoLookup{};

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

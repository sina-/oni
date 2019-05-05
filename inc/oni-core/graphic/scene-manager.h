#pragma once

#include <map>
#include <memory>

#include <oni-core/component/visual.h>
#include <oni-core/math/mat4.h>
#include <oni-core/gameplay/lap-tracker.h>
#include <oni-core/graphic/camera.h>

class b2World;

namespace oni {
    namespace math {
        struct vec2;

        class Rand;

        class ZLayerManager;
    }

    namespace entities {
        class EntityManager;

        class EntityFactory;
    }

    namespace component {
        struct Shape;
    }

    namespace graphic {
        class Shader;

        class BatchRenderer2D;

        class Renderer2D;

        class TextureManager;

        class FontManager;

        class DebugDrawBox2D;

        class SceneManager {
        public:
            SceneManager(const graphic::ScreenBounds &,
                         FontManager &,
                         math::ZLayerManager &,
                         b2World &,
                         common::real32
            );

            ~SceneManager();

            void
            render(entities::EntityFactory &server,
                   entities::EntityFactory &client,
                   common::EntityID lookAtEntity);

            void
            renderPhysicsDebugData();

            void
            tick(entities::EntityFactory &server,
                 entities::EntityFactory &client,
                 common::real64 tickTime);

            void
            renderRaw(const component::Shape &,
                      const component::Appearance &);

            void
            splat(entities::EntityFactory &entityFactory,
                  component::Brush brush,
                  const component::WorldP3D &worldPos,
                  const component::Size &size);

            void
            lookAt(common::real32 x,
                   common::real32 y);

            void
            lookAt(common::real32 x,
                   common::real32 y,
                   common::real32 distance);

            void
            zoom(common::real32 distance);

            const graphic::Camera &
            getCamera() const;

            const math::mat4 &
            getProjectionMatrix() const;

            const math::mat4 &
            getViewMatrix() const;

            common::real32
            getViewWidth() const;

            common::real32
            getViewHeight() const;

            common::uint16
            getSpritesPerFrame() const;

            common::uint16
            getParticlesPerFrame() const;

            common::uint16
            getTexturesPerFrame() const;

            void
            resetCounters();

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
            render(entities::EntityFactory &);

            void
            renderStaticTextures(entities::EntityManager &,
                                 common::real32 viewWidth,
                                 common::real32 viewHeight);

            void
            renderStaticText(entities::EntityManager &,
                             common::real32 viewWidth,
                             common::real32 viewHeight);

            void
            renderDynamicTextures(entities::EntityManager &,
                                  common::real32 viewWidth,
                                  common::real32 viewHeight);

            void
            renderColorSprites(entities::EntityManager &,
                               common::real32 viewWidth,
                               common::real32 viewHeight);

            void
            renderParticles(entities::EntityManager &,
                            common::real32 viewWidth,
                            common::real32 viewHeight);

            void
            renderAndUpdateAnimation(entities::EntityFactory &);

        private:
            struct RaceInfoEntities {
                common::EntityID lapEntity{0};
                common::EntityID lapTimeEntity{0};
                common::EntityID lapBestTimeEntity{0};
            };

        private:
            void
            begin(const Shader &shader,
                  Renderer2D &renderer2D,
                  bool translate,
                  bool scale,
                  bool setMVP);

            void
            end(const Shader &shader,
                Renderer2D &renderer2D);

            void
            initializeTextureRenderer();

            void
            initializeColorRenderer();

            void
            initializeParticleRenderer();

            void
            prepareTexture(component::Texture &texture);

            const RaceInfoEntities &
            getOrCreateLapText(entities::EntityFactory &,
                               common::EntityID carEntityID,
                               const gameplay::CarLapInfo &carLap);

            common::EntityID
            createText(entities::EntityFactory &entityFactory,
                       const component::WorldP3D &worldPos,
                       const std::string &text);

            void
            updateParticles(entities::EntityFactory &entityFactory,
                            common::real64 tickTime);

            void
            updateRaceInfo(entities::EntityManager &,
                           const gameplay::CarLapInfo&carLap,
                           const RaceInfoEntities &carLapTextEntities);

            common::EntityID
            getOrCreateCanvasTile(entities::EntityFactory &,
                                  const component::WorldP3D &pos);

            void
            updateCanvasTile(entities::EntityManager &entityManager,
                             common::EntityID entityID,
                             const component::Brush &brush,
                             const component::WorldP3D &worldPos,
                             const component::Size &size);

        private:
            std::unique_ptr<Shader> mColorShader{};
            std::unique_ptr<Shader> mTextureShader{};
            std::unique_ptr<Shader> mParticleShader{};
            std::unique_ptr<BatchRenderer2D> mColorRenderer{};
            std::unique_ptr<BatchRenderer2D> mTextureRenderer{};
            std::unique_ptr<BatchRenderer2D> mParticleRenderer{};
            std::unique_ptr<TextureManager> mTextureManager{};
            std::unique_ptr<DebugDrawBox2D> mDebugDrawBox2D{};
            FontManager &mFontManager;
            b2World &mPhysicsWorld;

            std::unique_ptr<math::Rand> mRand{};

            math::mat4 mModelMatrix{};
            math::mat4 mViewMatrix{};
            math::mat4 mProjectionMatrix{};

            std::map<common::uint64, common::EntityID> mCanvasTileLookup{};
            std::map<common::EntityID, RaceInfoEntities> mLapInfoLookup{};

            const common::uint16 mCanvasTileSizeX{0};
            const common::uint16 mCanvasTileSizeY{0};
            const common::real32 mHalfCanvasTileSizeX{0.0f};
            const common::real32 mHalfCanvasTileSizeY{0.0f};

            graphic::ScreenBounds mScreenBounds{};
            graphic::Camera mCamera{0.0f, 0.0f, 1.0f};

            const common::uint16 mMaxSpriteCount{0};
            const common::real32 mGameUnitToPixels{0};

            common::uint16 mRenderedSpritesPerFrame{0};
            common::uint16 mRenderedTexturesPerFrame{0};
            common::uint16 mRenderedParticlesPerFrame{0};

            math::ZLayerManager &mZLayerManager;
        };
    }
}

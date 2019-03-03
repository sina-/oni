#pragma once

#include <map>
#include <memory>

#include <oni-core/component/visual.h>
#include <oni-core/math/mat4.h>

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
        struct CarLapInfo;
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
            SceneManager(const component::ScreenBounds &, FontManager &,
                         math::ZLayerManager &,
                         b2World &,
                         common::real32
            );

            ~SceneManager();

            void render(entities::EntityManager &, common::EntityID lookAtEntity);

            void renderInternal();

            void renderPhysicsDebugData();

            void tick(entities::EntityFactory &, common::real64 tickTime);

            void tickInternal(common::real64 tickTime);

            void renderRaw(const component::Shape &, const component::Appearance &);

            void lookAt(common::real32 x, common::real32 y);

            void lookAt(common::real32 x, common::real32 y, common::real32 distance);

            void zoom(common::real32 distance);

            const component::Camera &getCamera() const;

            const math::mat4 &getProjectionMatrix() const;

            const math::mat4 &getViewMatrix() const;

            common::real32 getViewWidth() const;

            common::real32 getViewHeight() const;

            common::uint16 getSpritesPerFrame() const;

            common::uint16 getParticlesPerFrame() const;

            common::uint16 getTexturesPerFrame() const;

            void resetCounters();

            // TODO: This is awful and inconsistent with the API of this class where I should only expose render().
            // Instead of exposing the internals, I can batch the render objects in DebugDrawBox2D and then
            // pass them to a new render() function that receives a vector of sprites
            // .
            void beginColorRendering();

            void endColorRendering();

        private:
            void renderStaticTextures(entities::EntityManager &, common::real32 viewWidth,
                                      common::real32 viewHeight);

            void renderStaticText(entities::EntityManager &, common::real32 viewWidth,
                                  common::real32 viewHeight);

            void renderDynamicTextures(entities::EntityManager &, common::real32 viewWidth,
                                       common::real32 viewHeight);

            void renderColorSprites(entities::EntityManager &, common::real32 viewWidth,
                                    common::real32 viewHeight);

            void renderParticles(entities::EntityManager &, common::real32 viewWidth,
                                 common::real32 viewHeight);


        private:
            struct RaceInfoEntities {
                common::EntityID lapEntity{0};
                common::EntityID lapTimeEntity{0};
                common::EntityID lapBestTimeEntity{0};
            };

        private:
            void begin(const Shader &shader, Renderer2D &renderer2D, bool translate, bool scale, bool setMVP);

            void end(const Shader &shader, Renderer2D &renderer2D);

            void initializeTextureRenderer();

            void initializeColorRenderer();

            void initializeParticleRenderer();

            void prepareTexture(component::Texture &texture);

            common::EntityID getOrCreateSkidTile(const math::vec2 &position);

            const RaceInfoEntities &getOrCreateLapText(common::EntityID carEntityID,
                                                       const component::CarLapInfo &carLap);

            common::EntityID createText(const math::vec3 &worldPos, const std::string &text);

            void updateSkidlines(const math::vec3 &position,
                                 common::EntityID skidTextureEntity,
                                 common::uint8 alpha);

            void updateParticles(entities::EntityFactory &entityFactory, common::real64 tickTime);

            void updateRaceInfo(const component::CarLapInfo &carLap,
                                const RaceInfoEntities &carLapTextEntities);

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

            std::map<common::uint64, common::EntityID> mSkidlineLookup{};
            std::map<common::EntityID, RaceInfoEntities> mLapInfoLookup{};

            const common::uint16 mSkidTileSizeX{0};
            const common::uint16 mSkidTileSizeY{0};
            const common::real32 mHalfSkidTileSizeX{0.0f};
            const common::real32 mHalfSkidTileSizeY{0.0f};

            component::ScreenBounds mScreenBounds{};
            component::Camera mCamera{0.0f, 0.0f, 1.0f};

            const common::uint16 mMaxSpriteCount{0};
            const common::real32 mGameUnitToPixels{0};

            common::uint16 mRenderedSpritesPerFrame{0};
            common::uint16 mRenderedTexturesPerFrame{0};
            common::uint16 mRenderedParticlesPerFrame{0};

            std::unique_ptr<entities::EntityFactory> mInternalEntityFactory{};
            math::ZLayerManager &mZLayerManager;

        };
    }
}

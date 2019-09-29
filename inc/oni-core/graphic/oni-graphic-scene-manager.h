#pragma once

#include <map>
#include <queue>
#include <memory>

#include <oni-core/asset/oni-asset-fwd.h>
#include <oni-core/component/oni-component-fwd.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-fwd.h>
#include <oni-core/graphic/oni-graphic-camera.h>
#include <oni-core/graphic/oni-graphic-renderer.h>
#include <oni-core/graphic/oni-graphic-fwd.h>
#include <oni-core/math/oni-math-mat4.h>
#include <oni-core/math/oni-math-fwd.h>
#include <oni-core/math/oni-math-function.h>

class b2World;

namespace oni {
    class SceneManager {
    public:
        // TODO: Getting quite big, I can split it into, SceneRenderer, SceneUpdater, and SceneManager
        SceneManager(const ScreenBounds &,
                     oni::AssetManager &,
                     ZLayerManager &,
                     TextureManager &,
                     b2World &);

        ~SceneManager();

        void
        submit(EntityManager &);

        void
        render();

        // TODO: Maybe this should move to rac-client-scene-manager
        static void
        updateAfterMark(EntityTickContext &,
                        AfterMark &,
                        Orientation &,
                        WorldP3D &);

        static void
        updateGrowthInTime(EntityTickContext &,
                           GrowOverTime &,
                           Scale &);

        void
        renderRaw(const WorldP3D pos,
                  const Color &color);

        void
        blend(const Texture &front,
              Texture &back);

        void
        renderToTexture(const Quad &quad,
                        const Texture &src,
                        const ScreenBounds &destBounds,
                        Texture &dest,
                        const mat4 *model);

        void
        renderToTexture(const Quad &quad,
                        const Color &src,
                        const ScreenBounds &destBounds,
                        Texture &dest,
                        const mat4 *model);

        void
        lookAt(r32 x,
               r32 y);

        void
        lookAt(r32 x,
               r32 y,
               r32 distance);

        void
        zoom(r32 distance);

        bool
        isVisible(const WorldP3D &);

        bool
        isVisible(const WorldP3D &,
                  const Scale &);

        void
        prepareTexture(EntityManager &manager,
                       EntityID id,
                       EntityAssetsPack tag);

        u16
        getSpritesPerFrame() const;

        u16
        getParticlesPerFrame() const;

        u16
        getTexturesPerFrame() const;

        const ScreenBounds &
        getScreenBounds() const;

        ZLayerManager&
        getZLayerManager();

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
        struct WorldP3DAndOrientation {
            WorldP3D pos;
            Orientation ornt;
        };

        // TODO: Move somewhere else
        static SceneManager::WorldP3DAndOrientation
        applyParentTransforms(const EntityManager &manager,
                              EntityID child,
                              const WorldP3D &pos,
                              const Orientation &ornt);


    private:
        void
        renderStrip(EntityManager &,
                    r32 viewWidth,
                    r32 viewHeight);

    private:
        static void
        begin(Renderer &,
              const RenderSpec &);

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
               const mat4 *model);

        vec2
        getScreenSize();

        r32
        getViewWidth() const;

        r32
        getViewHeight() const;

    private:
        std::unique_ptr<Renderer_OpenGL_Tessellation> mRendererTessellation{};
        std::unique_ptr<Renderer_OpenGL_Strip> mRendererStrip{};
        std::unique_ptr<Renderer_OpenGL_Quad> mRendererQuad{};

        TextureManager &mTextureManager;
        std::unique_ptr<DebugDrawBox2D> mDebugDrawBox2D{};
        b2World &mPhysicsWorld;

        std::unique_ptr<Rand> mRand{};

        mat4 mModelMatrix{};
        mat4 mViewMatrix{};
        mat4 mProjectionMatrix{};

        ScreenBounds mScreenBounds{};
        Camera mCamera{0.0f, 0.0f, 1.0f};

        const u16 mMaxSpriteCount{0};

        u16 mRenderedSpritesPerFrame{0};
        u16 mRenderedTexturesPerFrame{0};
        u16 mRenderedParticlesPerFrame{0};

        ZLayerManager &mZLayerManager;

        using RenderableQueue = std::priority_queue<Renderable>;
        std::array<RenderableQueue, enumCast(MaterialFinish_Type::LAST)> mRenderables{};
    };
}

#include <oni-core/graphic/oni-graphic-scene-manager.h>

#include <set>

#include <oni-core/asset/oni-asset-manager.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/common/oni-common-const.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/graphic/oni-graphic-brush.h>
#include <oni-core/graphic/oni-graphic-renderer-ogl-tessellation.h>
#include <oni-core/graphic/oni-graphic-renderer-ogl-strip.h>
#include <oni-core/graphic/oni-graphic-renderer-ogl-quad.h>
#include <oni-core/graphic/oni-graphic-debug-draw-box2d.h>
#include <oni-core/graphic/oni-graphic-shader.h>
#include <oni-core/graphic/oni-graphic-texture-manager.h>
#include <oni-core/math/oni-math-transformation.h>
#include <oni-core/math/oni-math-intersects.h>
#include <oni-core/math/oni-math-rand.h>
#include <oni-core/math/oni-math-z-layer-manager.h>
#include <oni-core/math/oni-math-mat4.h>
#include <oni-core/math/oni-math-vec2.h>


namespace oni {
    SceneManager::SceneManager(const ScreenBounds &screenBounds,
                               AssetManager &assetManager,
                               ZLayerManager &zLayerManager,
                               TextureManager &tm) :
    // 64k vertices
            mMaxSpriteCount(64 * 1000),
            mScreenBounds(screenBounds),
            mTextureManager(tm),
            mZLayerManager(zLayerManager) {

        mProjectionMatrix = mat4::orthographic(screenBounds.xMin, screenBounds.xMax, screenBounds.yMin,
                                               screenBounds.yMax, -1.0f, 1.0f);
        mViewMatrix = mat4::identity();

        mModelMatrix = mat4::identity();

        mRendererTessellation = std::make_unique<Renderer_OpenGL_Tessellation>(mMaxSpriteCount, tm);
        mRendererStrip = std::make_unique<Renderer_OpenGL_Strip>(mMaxSpriteCount, tm);
        mRendererQuad = std::make_unique<Renderer_OpenGL_Quad>(mMaxSpriteCount, tm);

        mRand = std::make_unique<Rand>(0, 0);

        mDebugDrawBox2D = std::make_unique<DebugDrawBox2D>(this);
        mDebugDrawBox2D->AppendFlags(b2Draw::e_shapeBit);
        //mDebugDrawBox2D->AppendFlags(b2Draw::e_aabbBit);
        mDebugDrawBox2D->AppendFlags(b2Draw::e_pairBit);
        mDebugDrawBox2D->AppendFlags(b2Draw::e_centerOfMassBit);
    }

    SceneManager::~SceneManager() = default;

    void
    SceneManager::renderRaw(const WorldP3D pos,
                            const Color &color) {
        //mColorRenderer->submit(pos, color);
        ++mRenderedSpritesPerFrame;
    }

    void
    SceneManager::beginColorRendering() {
        RenderSpec spec;
        spec.renderTarget = nullptr;
        spec.screenSize = getScreenSize();
        spec.zoom = mCamera.z;
        spec.finishType = Material_Finish_Enum::SOLID;
        setMVP(spec, true, false, true);
        begin(*mRendererTessellation, spec);
    }

    void
    SceneManager::endColorRendering() {
        end(*mRendererTessellation);
    }

    void
    SceneManager::setMVP(RenderSpec &spec,
                         bool translate,
                         bool scale,
                         bool project) {
        spec.view = mat4::identity();
        spec.proj = mat4::identity();
        if (project) {
            spec.proj = mProjectionMatrix;
        }
        if (scale) {
            spec.view *= mat4::scale({mCamera.z, mCamera.z, 1.0f});
        }
        if (translate) {
            spec.view *= mat4::translation(-mCamera.x, -mCamera.y, 0.0f);
        }
    }

    void
    SceneManager::setMVP(RenderSpec &spec,
                         const ScreenBounds &destBounds,
                         const mat4 *model) {
        spec.proj = mat4::orthographic(destBounds.xMin, destBounds.xMax,
                                       destBounds.yMin, destBounds.yMax,
                                       -1.0f, 1.0f);
        spec.view = mat4::identity();
        spec.model = mat4::identity();
        if (model) {
            spec.model = *model;
        }
    }

    vec2
    SceneManager::getScreenSize() {
        return {getViewWidth(), getViewHeight()};
    }

    void
    SceneManager::begin(Renderer &renderer,
                        const RenderSpec &spec) {
        renderer.begin(spec);
    }

    void
    SceneManager::prepareTexture(EntityManager &manager,
                                 EntityID id,
                                 EntityAssetsPack tag) {
        auto &ms = manager.get<Material_Skin>(id);
        mTextureManager.initTexture(tag, ms.texture);
    }

    void
    SceneManager::end(Renderer &renderer2D) {
        renderer2D.end();
    }

    void
    SceneManager::renderPhysicsDebugData() {
        mDebugDrawBox2D->Begin();
//        mPhysicsWorld.DrawDebugData();
        mDebugDrawBox2D->End();
    }

    void
    SceneManager::submit(EntityManager &manager) {
        {
            // TODO: The following code includes everything, even the particles will be sorted, which might be over-kill
            auto view = manager.createView<
                    EntityType,
                    WorldP3D,
                    Orientation,
                    Scale,
                    Material_Skin,
                    Material_Finish_Enum>();
            for (auto &&id: view) {
                const auto &pos = view.get<WorldP3D>(id);
                const auto &ornt = view.get<Orientation>(id);
                const auto &scale = view.get<Scale>(id);
                const auto &finish = view.get<Material_Finish_Enum>(id);
                const auto &skin = view.get<Material_Skin>(id);

                auto renderable = Renderable{};
                renderable.id = id;
                renderable.type = view.get<EntityType>(id); // NOTE: Just for debug
                renderable.manager = &manager;
                renderable.pos = &pos;
                renderable.ornt = &ornt;
                renderable.scale = &scale;
                renderable.skin = &skin;
                // TODO: Read this value from the entity
                renderable.pt = PrimitiveTransforms::DYNAMIC;

                if (manager.has<MaterialTransition_List>(id)) {
                    const auto &transList = manager.get<MaterialTransition_List>(id);
                    const auto &trans = transList.transitions[transList.activeTransIdx];
                    renderable.trans = &trans;
                }
                // TODO: Now that I distinguish between opaque and translucent objects I should draw the
                // opaque objects front to back to avoid over draw
                mRenderables[enumCast(finish)].push(renderable);
            }
        }

        {
            auto view = manager.createView<
                    EntityType,
                    WorldP3D,
                    Orientation,
                    Scale,
                    Material_Text>();
            for (auto &&id: view) {
                const auto &pos = view.get<WorldP3D>(id);
                const auto &ornt = view.get<Orientation>(id);
                const auto &scale = view.get<Scale>(id);
                const auto &text = view.get<Material_Text>(id);

                auto renderable = Renderable{};
                renderable.id = id;
                renderable.type = view.get<EntityType>(id); // NOTE: Just for debug
                renderable.manager = &manager;
                renderable.pos = &pos;
                renderable.ornt = &ornt;
                renderable.scale = &scale;
                renderable.text = &text;
                renderable.skin = &text.skin;
                // TODO: Read this value from the entity
                renderable.pt = PrimitiveTransforms::UI;

                mRenderables[enumCast(Material_Finish_Enum::TRANSLUCENT)].push(renderable);
            }
        }
    }

    void
    SceneManager::render() {
        for (auto i = 0; i < enumCast(Material_Finish_Enum::LAST); ++i) {
            RenderSpec spec;
            spec.renderTarget = nullptr;
            spec.screenSize = getScreenSize();
            spec.zoom = mCamera.z;
            spec.finishType = static_cast<Material_Finish_Enum>(i);
            setMVP(spec, true, true, true);

            begin(*mRendererTessellation, spec);
            while (!mRenderables[i].empty()) {
                auto &r = const_cast<Renderable &> (mRenderables[i].top());
                auto ePos = applyParentTransforms(*r.manager, r.id, *r.pos, *r.ornt);

                if (r.pt == PrimitiveTransforms::DYNAMIC && !isVisible(ePos.pos, *r.scale)) {
                    mRenderables[i].pop();
                    continue;
                }

                // This will just point r.pos to a new location, which is temporary to this scope, but submit()
                // will create a copy so it is safe.
                r.pos = &ePos.pos;
                r.ornt = &ePos.ornt;

                mRendererTessellation->submit(r);

                mRenderables[i].pop();

                ++mRenderedSpritesPerFrame;
            }
            end(*mRendererTessellation);
        }
    }

    void
    SceneManager::renderStrip(EntityManager &manager,
                              r32 viewWidth,
                              r32 viewHeight) {
        RenderSpec spec;
        spec.renderTarget = nullptr;
        spec.screenSize = getScreenSize();
        spec.zoom = mCamera.z;
        setMVP(spec, true, true, true);

        auto view = manager.createView<
                WorldP3D_History>();
        for (auto &&id: view) {
            begin(*mRendererStrip, spec);

            const auto &ph = view.get<WorldP3D_History>(id).pos;
            auto count = 0;
            for (auto &&p: ph) {
                auto alpha = r32(count) / ph.size();
                auto color = Color{};
                color.set_rgba(1, 1, 1, alpha);
                mRendererStrip->submit(p, 1, color, {});
                mRendererStrip->submit(p, -1, color, {});
                ++count;
            }

            end(*mRendererStrip);
        }
    }

    void
    SceneManager::renderToTexture(const Quad &quad,
                                  const Color &src,
                                  const ScreenBounds &destBounds,
                                  Texture &dest,
                                  const mat4 *model) {
        RenderSpec spec;
        spec.renderTarget = &dest;
        spec.screenSize = getScreenSize();
        spec.zoom = mCamera.z;
        // TODO: I should probably take this as an argument
        spec.finishType = Material_Finish_Enum::SOLID;
        setMVP(spec, destBounds, model);

        begin(*mRendererQuad, spec);
        mRendererQuad->submit(quad, src, nullptr);
        end(*mRendererQuad);
    }

    void
    SceneManager::renderToTexture(const Quad &quad,
                                  const Texture &src,
                                  const ScreenBounds &destBounds,
                                  Texture &dest,
                                  const mat4 *model) {
        RenderSpec spec;
        // TODO: I should probably take this as an argument
        spec.finishType = Material_Finish_Enum::SOLID;
        spec.renderTarget = &dest;
        spec.screenSize = getScreenSize();
        spec.zoom = mCamera.z;
        setMVP(spec, destBounds, model);

        begin(*mRendererQuad, spec);
        mRendererQuad->submit(quad, {}, &src);
        end(*mRendererQuad);
    }

    void
    SceneManager::blend(const Texture &front,
                        Texture &back) {
        RenderSpec spec;
        // TODO: I should probably take this as an argument
        spec.finishType = Material_Finish_Enum::SOLID;
        spec.renderTarget = &back;
        spec.screenSize = getScreenSize();
        spec.zoom = mCamera.z;
        spec.view = mat4::identity();
        spec.model = mat4::identity();
        spec.proj = mat4::orthographic(-1, +1,
                                       -1, +1,
                                       -1, +1);

        auto quad = Quad{};
        mRendererQuad->begin(spec);
        mRendererQuad->submit(quad, {}, front, back);
        end(*mRendererQuad);
    }

    void
    SceneManager::zoom(r32 distance) {
        mCamera.z = 1 / distance;
    }

    bool
    SceneManager::isVisible(const WorldP3D &pos) {
        return intersects(pos, mCamera.x, mCamera.y, getViewWidth(), getViewHeight());
    }

    bool
    SceneManager::isVisible(const WorldP3D &pos,
                            const Scale &scale) {
        return intersects(pos, scale, mCamera.x, mCamera.y, getViewWidth(), getViewHeight());
    }

    void
    SceneManager::lookAt(r32 x,
                         r32 y) {
        mCamera.x = x;
        mCamera.y = y;
    }

    void
    SceneManager::lookAt(r32 x,
                         r32 y,
                         r32 distance) {
        mCamera.x = x;
        mCamera.y = y;
        mCamera.z = 1 / distance;
    }

    WorldP3D
    SceneManager::unProject(const Screen2D &point) {
        auto proj = mProjectionMatrix;
        proj *= mat4::scale({mCamera.z, mCamera.z, 1.0f});
        proj *= mat4::translation(-mCamera.x, -mCamera.y, 0.0f);
        auto invProj = proj.inverse();

        auto pos = vec4{};
        // TODO: Have to get these values from Window system
        static const r32 WIDTH = 1600.f;
        static const r32 HEIGHT = 900.f;
        pos.x = 2 * point.x / WIDTH - 1;
        pos.y = 2 * point.y / HEIGHT - 1;
        pos.z = 1.f;
        pos.w = 1.f;

        pos = invProj * pos;

        auto result = WorldP3D{};
        result.x = pos.x;
        result.y = pos.y;
        return result;
    }

    u16
    SceneManager::getSpritesPerFrame() const {
        return mRenderedSpritesPerFrame;
    }

    u16
    SceneManager::getParticlesPerFrame() const {
        return mRenderedParticlesPerFrame;
    }

    u16
    SceneManager::getTexturesPerFrame() const {
        return mRenderedTexturesPerFrame;
    }

    r32
    SceneManager::getViewWidth() const {
        return (mScreenBounds.xMax - mScreenBounds.xMin) * (1.0f / mCamera.z);
    }

    r32
    SceneManager::getViewHeight() const {
        return (mScreenBounds.yMax - mScreenBounds.yMin) * (1.0f / mCamera.z);
    }

    const ScreenBounds &
    SceneManager::getScreenBounds() const {
        return mScreenBounds;
    }

    ZLayerManager &
    SceneManager::getZLayerManager() {
        return mZLayerManager;
    }

    void
    SceneManager::resetCounters() {
        mRenderedSpritesPerFrame = 0;
        mRenderedParticlesPerFrame = 0;
        mRenderedTexturesPerFrame = 0;
    }

    SceneManager::WorldP3DAndOrientation
    SceneManager::applyParentTransforms(const EntityManager &manager,
                                        EntityID child,
                                        const WorldP3D &childPos,
                                        const Orientation &childOrientation) {
        auto transformation = mat4::identity();
        auto result = WorldP3DAndOrientation{childPos, childOrientation};
        auto numParents = size{};

        while (manager.has<EntityAttachee>(child)) {
            const auto &parent = manager.get<EntityAttachee>(child);
            const auto &parentPos = manager.get<WorldP3D>(parent.entityID);
            const auto &parentOrientation = manager.get<Orientation>(parent.entityID);

            // TODO: Scaling is ignored because it is used to store the size of the object, so even non-scaled
            // objects have scaling matrix larger than identity matrix, so if I use the parent scaling
            // that will just "scale" the child by the size of parent, which is not what I want.
            // Perhaps removing Size component was the wrong decision, and I should have a distinction
            // between size and Scale. Renderer passes scale down to shader as a size anyway, it does NOT
            // use it as a multiplier.
            transformation *= createTransformation(parentPos, parentOrientation, {});
            result.pos.value = transformation * result.pos.value;
            result.ornt.value += parentOrientation.value;

            child = parent.entityID;
            ++numParents;

            assert(numParents < 20);
        }
        return result;
    }

    void
    SceneManager::updateAfterMark(EntityTickContext &etc,
                                  AfterMark &mark,
                                  Orientation &ornt,
                                  WorldP3D &pos) {
        assert(etc.mng.getSimMode() == SimMode::CLIENT ||
               etc.mng.getSimMode() == SimMode::CLIENT_SIDE_SERVER);
        auto transformed = applyParentTransforms(etc.mng, etc.id, pos, ornt);
        Brush brush;
        auto quad = Quad{};
        auto model = createTransformation(transformed.pos, transformed.ornt, mark.scale);
        switch (mark.type) {
            case BrushType::COLOR: {
                brush.color = &mark.color;
                break;
            }
            case BrushType::TEXTURE: {
                // TODO: Implement
                assert(false);
                break;
            }
            case BrushType::TEXTURE_TAG: {
                brush.tag = mark.tag;
                break;
            }
            default: {
                assert(false);
                break;
            }
        }
        brush.shape_Quad = &quad;
        brush.type = BrushType::COLOR;
        brush.model = &model;

        // TODO: This function needs to move to game scene manager, it is not part of the engine.
        assert(false);
        // splat(brush);
    }
}

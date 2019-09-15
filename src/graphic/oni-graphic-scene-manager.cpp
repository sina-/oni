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
#include <GL/glew.h>


#define DEBUG_Z 0

namespace oni {
    SceneManager::SceneManager(const ScreenBounds &screenBounds,
                               AssetManager &assetManager,
                               ZLayerManager &zLayerManager,
                               TextureManager &textureManager,
                               b2World &physicsWorld) :
    // 64k vertices
            mMaxSpriteCount(64 * 1000),
            mScreenBounds(screenBounds),
            mPhysicsWorld(physicsWorld),
            mTextureManager(textureManager),
            mZLayerManager(zLayerManager) {

        mProjectionMatrix = mat4::orthographic(screenBounds.xMin, screenBounds.xMax, screenBounds.yMin,
                                               screenBounds.yMax, -1.0f, 1.0f);
        mViewMatrix = mat4::identity();

        mModelMatrix = mat4::identity();

        mRendererTessellation = std::make_unique<Renderer_OpenGL_Tessellation>(mMaxSpriteCount);
        mRendererStrip = std::make_unique<Renderer_OpenGL_Strip>(mMaxSpriteCount);
        mRendererQuad = std::make_unique<Renderer_OpenGL_Quad>(mMaxSpriteCount);


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
        spec.finishType = MaterialFinish_Type::SOLID;
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
        auto &ms = manager.get<MaterialSkin>(id);
        mTextureManager.initTexture(tag, ms.texture);
    }

    void
    SceneManager::end(Renderer &renderer2D) {
        renderer2D.end();
    }

    void
    SceneManager::renderPhysicsDebugData() {
        mDebugDrawBox2D->Begin();
        mPhysicsWorld.DrawDebugData();
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
                    MaterialDefinition>();
            for (auto &&id: view) {
                const auto &pos = view.get<WorldP3D>(id);
                const auto &ornt = view.get<Orientation>(id);
                const auto &scale = view.get<Scale>(id);
                const auto &def = view.get<MaterialDefinition>(id);

                auto renderable = Renderable{};
                renderable.id = id;
                renderable.type = view.get<EntityType>(id); // NOTE: Just for debug
                renderable.manager = &manager;
                renderable.pos = &pos;
                renderable.ornt = &ornt;
                renderable.scale = &scale;
                renderable.def = def;

                switch (def.transition) {
                    case MaterialTransition_Type::NONE:
                        renderable.skin = &manager.get<MaterialSkin>(id);
                        break;
                    case MaterialTransition_Type::FADE: {
                        renderable.skin = &manager.get<MaterialSkin>(id);
                        renderable.transitionFade = &manager.get<MaterialTransition_Fade>(id);
                        break;
                    }
                    case MaterialTransition_Type::TINT: {
                        renderable.skin = &manager.get<MaterialSkin>(id);
                        renderable.transitionTint = &manager.get<MaterialTransition_Color>(id);
                        renderable.age = &manager.get<Age>(id);
                        break;
                    }
                    case MaterialTransition_Type::TEXTURE: {
                        renderable.transitionAnimation = &manager.get<MaterialTransition_Texture>(id);
                        break;
                    }
                    default: {
                        assert(false);
                        break;
                    }
                }

                mRenderables[enumCast(def.finish)].push(renderable);
            }
        }

        {
            auto view = manager.createView<
                    EntityType,
                    WorldP3D,
                    Orientation,
                    Scale,
                    MaterialText>();
            for (auto &&id: view) {
                const auto &pos = view.get<WorldP3D>(id);
                const auto &ornt = view.get<Orientation>(id);
                const auto &scale = view.get<Scale>(id);
                const auto &text = view.get<MaterialText>(id);

                auto renderable = Renderable{};
                renderable.id = id;
                renderable.type = view.get<EntityType>(id); // NOTE: Just for debug
                renderable.manager = &manager;
                renderable.pos = &pos;
                renderable.ornt = &ornt;
                renderable.scale = &scale;
                renderable.text = &text;
                renderable.skin = &text.skin;

                mRenderables[enumCast(MaterialFinish_Type::TRANSLUCENT)].push(renderable);
            }
        }
    }

    void
    SceneManager::render() {
        for (auto i = 0; i < enumCast(MaterialFinish_Type::LAST); ++i) {
            RenderSpec spec;
            spec.renderTarget = nullptr;
            spec.screenSize = getScreenSize();
            spec.zoom = mCamera.z;
            spec.finishType = static_cast<MaterialFinish_Type>(i);
            setMVP(spec, true, true, true);

            begin(*mRendererTessellation, spec);
            while (!mRenderables[i].empty()) {
                auto &r = const_cast<Renderable &> (mRenderables[i].top());
                auto ePos = applyParentTransforms(*r.manager, r.id, *r.pos, *r.ornt);

                if (!isVisible(ePos.pos, *r.scale)) {
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
#if DEBUG_Z
                serverManager.printEntityType(id);
                printf("%f\n", p.z);
#endif
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
        spec.finishType = MaterialFinish_Type::SOLID;
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
        spec.finishType = MaterialFinish_Type::SOLID;
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
        spec.finishType = MaterialFinish_Type::SOLID;
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
    SceneManager::updateSmokeEmitter(EntityManager &manager,
                                     r64 tickTime) {
        assert(manager.getSimMode() == SimMode::CLIENT ||
               manager.getSimMode() == SimMode::CLIENT_SIDE_SERVER);

        auto view = manager.createView<CoolDown>();
        for (auto &&id: view) {
            auto &emitter = view.get<CoolDown>(id);
            subAndZeroClip(emitter.current, tickTime);
        }
    }

    void
    SceneManager::updateAfterMark(EntityManager &manager,
                                  r64 tickTime) {
        assert(manager.getSimMode() == SimMode::CLIENT ||
               manager.getSimMode() == SimMode::CLIENT_SIDE_SERVER);

        auto view = manager.createView<
                AfterMark,
                Orientation,
                WorldP3D>();
        for (auto &&id: view) {
            const auto &mark = view.get<AfterMark>(id);
            const auto &pos = view.get<WorldP3D>(id);
            const auto &ornt = view.get<Orientation>(id);

            auto transformed = applyParentTransforms(manager, id, pos, ornt);
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

    void
    SceneManager::updateGrowthInTime(EntityManager &manager,
                                     r64 tickTime) {
        assert(manager.getSimMode() == SimMode::CLIENT ||
               manager.getSimMode() == SimMode::CLIENT_SIDE_SERVER);

        auto doneGrowing = std::vector<EntityID>();
        auto view = manager.createView<
                GrowInTime,
                Scale>();
        for (auto &&id: view) {
            auto &growth = view.get<GrowInTime>(id);
            auto &scale = view.get<Scale>(id);
            bool doneGrowingX = false;
            bool doneGrowingY = false;

            growth.elapsed += tickTime;
            if (almost_Greater(growth.elapsed, growth.period)) {
                if (almost_Less(scale.x, growth.maxSize.x)) {
                    scale.x += growth.factor;
                } else {
                    doneGrowingX = true;
                }

                if (almost_Less(scale.y, growth.maxSize.y)) {
                    scale.y += growth.factor;
                } else {
                    doneGrowingY = true;
                }

                if (doneGrowingX && doneGrowingY) {
                    doneGrowing.emplace_back(id);
                }
                growth.elapsed = 0.f;
            }
        }

        for (auto &&id: doneGrowing) {
            manager.removeComponent<GrowInTime>(id);
        }
    }

    void
    SceneManager::updateTextureAnimated(EntityManager &manager,
                                        r64 tickTime) {
        assert(manager.getSimMode() == SimMode::CLIENT ||
               manager.getSimMode() == SimMode::CLIENT_SIDE_SERVER);

        auto view = manager.createView<MaterialTransition_Texture>();
        for (auto &&id: view) {
            auto &mta = view.get<MaterialTransition_Texture>(id);
            auto &ta = mta.value;
            ta.timeElapsed += tickTime;
            // NOTE: It is assumed that this function is called more often than animation fps!
            assert(ta.frameDuration > tickTime);
            if (ta.playing && almost_Greater(ta.timeElapsed, ta.frameDuration)) {
                ta.nextFrame = (ta.nextFrame + 1) % enumCast(ta.numFrames);
                ta.timeElapsed = 0;
                if (ta.nextFrame == 0) {
                    switch (ta.endingBehaviour) {
                        case AnimationEndingBehavior::LOOP: {
                            break;
                        }
                        case AnimationEndingBehavior::PLAY_AND_STOP: {
                            ta.playing = false;
                            break;
                        }
                        case AnimationEndingBehavior::PLAY_AND_REMOVE_ENTITY: {
                            manager.markForDeletion(id);
                            break;
                        }
                        default: {
                            assert(false);
                            break;
                        }
                    }
                }
            }
        }
        manager.flushDeletions();
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
}

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
                               TextureManager &tm,
                               b2World &physicsWorld) :
    // 64k vertices
            mMaxSpriteCount(64 * 1000),
            mScreenBounds(screenBounds),
            mPhysicsWorld(physicsWorld),
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
                    MaterialSkin,
                    MaterialFinish_Type>();
            for (auto &&id: view) {
                const auto &pos = view.get<WorldP3D>(id);
                const auto &ornt = view.get<Orientation>(id);
                const auto &scale = view.get<Scale>(id);
                const auto &finish = view.get<MaterialFinish_Type>(id);
                const auto &skin = view.get<MaterialSkin>(id);

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
                // TODO: Read this value from the entity
                renderable.pt = PrimitiveTransforms::UI;

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
                GrowOverTime,
                Scale>();
        for (auto &&id: view) {
            auto &growth = view.get<GrowOverTime>(id);
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
            manager.removeComponent<GrowOverTime>(id);
        }
    }

    void
    SceneManager::updateTextureAnimated(MaterialTransition_Texture &mta,
                                        r64 tickTime) {
        mta.ttl.currentAge += tickTime;
        // NOTE: It is assumed that this function is called more often than animation fps!
        assert(mta.ttl.maxAge > tickTime);
        if (mta.playing && almost_Greater(mta.ttl.currentAge, mta.ttl.maxAge)) {
            mta.nextFrame = (mta.nextFrame + 1) % enumCast(mta.numFrames);
            mta.ttl.currentAge = 0;
        }
    }

    void
    SceneManager::updateTint(MaterialSkin &skin,
                             MaterialTransition_Color mtc,
                             const TimeToLive &ttl) {
        // TODO: Very accurate and slow calculations, I don't need the accuracy but it can be faster!
        auto &begin = mtc.begin;
        auto &end = mtc.end;
        auto t = ttl.currentAge / ttl.maxAge;

        auto r = lerp(begin.r_r32(), end.r_r32(), t);
        auto g = lerp(begin.g_r32(), end.g_r32(), t);
        auto b = lerp(begin.b_r32(), end.b_r32(), t);
        auto a = lerp(begin.a_r32(), end.a_r32(), t);

        skin.color.set_r(r);
        skin.color.set_g(g);
        skin.color.set_b(b);
        skin.color.set_a(a);
    }

    void
    SceneManager::updateFade(MaterialSkin &skin,
                             MaterialTransition_Fade &fade,
                             const TimeToLive &ttl) {
        auto targetAlpha = 1.f;
        switch (fade.fadeFunc) {
            case FadeFunc::LINEAR: {
                targetAlpha = 1 - ttl.currentAge / ttl.maxAge;
                break;
            }
            case FadeFunc::TAIL: {
                constexpr auto dropOffT = 0.7f;
                auto ageRatio = ttl.currentAge / ttl.maxAge;
                if (ageRatio > dropOffT) {
                    targetAlpha = 1 - ageRatio;
                }
                break;
            }
            default: {
                assert(false);
                break;
            }
        }

        auto currentAlpha = skin.color.a_r32();
        skin.color.set_a(lerp(currentAlpha, targetAlpha, fade.factor));
    }

    void
    SceneManager::updateTransitions(EntityManager &manager,
                                    r64 tickTime) {
        auto view = manager.createView<
                MaterialSkin,
                MaterialTransition_List>();
        for (auto &&id: view) {
            auto &mtl = view.get<MaterialTransition_List>(id);
            if (mtl.ended) {
                continue;
            }
            auto &current = mtl.transitions[mtl.activeTransIdx];
            current.ttl.currentAge += tickTime;
            if (almost_Greater(current.ttl.currentAge, current.ttl.maxAge)) {
                if (mtl.transitions.size() - 1 > mtl.activeTransIdx) {
                    current.ttl.currentAge = 0.f;
                    ++mtl.activeTransIdx;
                } else {
                    switch (mtl.ending) {
                        case MaterialTransition_EndBehavior::LOOP: {
                            current.ttl.currentAge = 0.f;
                            mtl.activeTransIdx = 0;
                            break;
                        }
                        case MaterialTransition_EndBehavior::PLAY_AND_STOP: {
                            // TODO: Can I not have this special case?
                            if (current.type == MaterialTransition_Type::TEXTURE) {
                                current.texture.playing = false;
                            }
                            mtl.ended = true;
                            break;
                        }
                        case MaterialTransition_EndBehavior::PLAY_AND_REMOVE_ENTITY: {
                            assert(manager.getSimMode() != SimMode::CLIENT_SIDE_SERVER);
                            manager.markForDeletion(id);
                            break;
                        }
                        default: {
                            assert(false);
                        }
                    }
                }
            } else {
                auto &skin = view.get<MaterialSkin>(id);
                switch (current.type) {
                    case MaterialTransition_Type::TEXTURE: {
                        updateTextureAnimated(current.texture, tickTime);
                        break;
                    }
                    case MaterialTransition_Type::FADE: {
                        updateFade(skin, current.fade, current.ttl);
                        break;
                    }
                    case MaterialTransition_Type::TINT: {
                        updateTint(skin, current.color, current.ttl);
                        break;
                    }
                    default: {
                        assert(false);
                    }
                }
            }
        }
        manager.flushDeletions();
    }
}

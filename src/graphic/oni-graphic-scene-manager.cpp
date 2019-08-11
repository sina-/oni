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
    namespace graphic {
        SceneManager::SceneManager(const graphic::ScreenBounds &screenBounds,
                                   asset::AssetManager &assetManager,
                                   math::ZLayerManager &zLayerManager,
                                   graphic::TextureManager &textureManager,
                                   b2World &physicsWorld,
                                   common::r32 gameUnitToPixels) :
                mCanvasTileSizeX{110},
                mCanvasTileSizeY{110},
                mHalfCanvasTileSizeX{mCanvasTileSizeX / 2.f},
                mHalfCanvasTileSizeY{mCanvasTileSizeY / 2.f},
                // 64k vertices
                mMaxSpriteCount(64 * 1000),
                mScreenBounds(screenBounds),
                mPhysicsWorld(physicsWorld),
                mGameUnitToPixels(gameUnitToPixels),
                mTextureManager(textureManager),
                mZLayerManager(zLayerManager) {

            mProjectionMatrix = math::mat4::orthographic(screenBounds.xMin, screenBounds.xMax, screenBounds.yMin,
                                                         screenBounds.yMax, -1.0f, 1.0f);
            mViewMatrix = math::mat4::identity();

            mModelMatrix = math::mat4::identity();

            mRendererTessellation = std::make_unique<Renderer_OpenGL_Tessellation>(mMaxSpriteCount);
            mRendererStrip = std::make_unique<Renderer_OpenGL_Strip>(mMaxSpriteCount);
            mRendererQuad = std::make_unique<Renderer_OpenGL_Quad>(mMaxSpriteCount);


            mInternalReg = std::make_unique<entities::EntityManager>(entities::SimMode::CLIENT, nullptr);

            mRand = std::make_unique<math::Rand>(0, 0);

            mDebugDrawBox2D = std::make_unique<DebugDrawBox2D>(this);
            mDebugDrawBox2D->AppendFlags(b2Draw::e_shapeBit);
            //mDebugDrawBox2D->AppendFlags(b2Draw::e_aabbBit);
            mDebugDrawBox2D->AppendFlags(b2Draw::e_pairBit);
            mDebugDrawBox2D->AppendFlags(b2Draw::e_centerOfMassBit);
        }

        SceneManager::~SceneManager() = default;

        void
        SceneManager::renderRaw(const component::WorldP3D pos,
                                const component::Color &color) {
            //mColorRenderer->submit(pos, color);
            ++mRenderedSpritesPerFrame;
        }

        void
        SceneManager::beginColorRendering() {
            RenderSpec spec;
            spec.renderTarget = nullptr;
            spec.screenSize = getScreenSize();
            spec.zoom = mCamera.z;
            spec.finishType = component::MaterialFinishType::SOLID;
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
            spec.view = math::mat4::identity();
            if (project) {
                spec.proj = mProjectionMatrix;
            }
            if (scale) {
                spec.view *= math::mat4::scale({mCamera.z, mCamera.z, 1.0f});
            }
            if (translate) {
                spec.view *= math::mat4::translation(-mCamera.x, -mCamera.y, 0.0f);
            }
        }

        void
        SceneManager::setMVP(RenderSpec &spec,
                             const ScreenBounds &destBounds,
                             const math::mat4 *model) {
            spec.proj = math::mat4::orthographic(destBounds.xMin, destBounds.xMax,
                                                 destBounds.yMin, destBounds.yMax,
                                                 -1.0f, 1.0f);
            spec.view = math::mat4::identity();
            spec.model = math::mat4::identity();
            if (model) {
                spec.model = *model;
            }
        }

        math::vec2
        SceneManager::getScreenSize() {
            return {getViewWidth(), getViewHeight()};
        }

        math::mat4
        SceneManager::getCameraScale() {
            return math::mat4::scale(mCamera.z, mCamera.z, 1.f);
        }

        math::mat4
        SceneManager::getCameraTranslation() {
            return math::mat4::translation(-mCamera.x, -mCamera.y, 0.f);
        }

        void
        SceneManager::begin(Renderer &renderer,
                            const RenderSpec &spec) {
            renderer.begin(spec);
        }

        void
        SceneManager::prepareTexture(entities::EntityManager &manager,
                                     common::EntityID id,
                                     component::EntityPreset tag) {
            auto &ms = manager.get<component::MaterialSurface>(id);
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
        SceneManager::submit(entities::EntityManager &manager) {
            {
                // TODO: This only renders BrushTrails and only renders them to texture, which doesn't fit nicely
                // to the current structure of submit and render.
                // TODO: Another refactoring I should do is to remove the quads from BrushTrail as the renderer doesn't
                // need to know about them. The visible part, the quad and texture, should be another entity with Texture
                // and QuadList component, the car can have that entity as a reference in the BrushTrail
                renderQuad(manager);
            }
            // TODO: The following code includes everything, even the particles will be sorted, which might be
            // over-kill, I could think about separating particles from the rest and always rendering them at the
            // end on top of everything and accepting the fact that I won't be able to occlude particles with
            // other solid objects. Although... I could occlude them, if I keep the z-buffer around and do z test but
            // don't write to z-buffer when rendering particles...
            {
                auto view = manager.createView<
                        component::WorldP3D,
                        component::Heading,
                        component::Scale,
                        component::MaterialSurface,
                        component::MaterialTransition_Type,
                        component::MaterialFinish>();
                for (auto &&id: view) {
                    const auto &pos = view.get<component::WorldP3D>(id);
                    const auto &heading = view.get<component::Heading>(id);
                    const auto &scale = view.get<component::Scale>(id);
                    const auto &material = view.get<component::MaterialSurface>(id);
                    const auto &transitionType = view.get<component::MaterialTransition_Type>(id);
                    const auto &finish = view.get<component::MaterialFinish>(id);

                    auto renderable = Renderable{};
                    renderable.id = id;
                    renderable.manager = &manager;
                    renderable.material = &material;
                    renderable.pos = &pos;
                    renderable.heading = &heading;
                    renderable.scale = &scale;

                    renderable.material = &material;
                    renderable.finish = &finish;
                    renderable.transitionType = transitionType;
                    switch (transitionType) {
                        case component::MaterialTransition_Type::NONE:
                            break;
                        case component::MaterialTransition_Type::FADE: {
                            renderable.transitionFade = &manager.get<component::MaterialTransition_Fade>(id);
                            break;
                        }
                        case component::MaterialTransition_Type::TINT: {
                            renderable.transitionTint = &manager.get<component::MaterialTransition_Tint>(id);
                            break;
                        }
                        case component::MaterialTransition_Type::ANIMATED: {
                            renderable.transitionAnimated = &manager.get<component::MaterialTransition_Animated>(id);
                            break;
                        }
                        case component::MaterialTransition_Type::LAST:
                        default: {
                            assert(false);
                            break;
                        }
                    }

                    mRenderables[finish.typeID()].push(renderable);
                }
            }
        }

        void
        SceneManager::render() {
            /// Queue internal entities
            {
                submit(*mInternalReg);
            }

            /// Render everything but shinnies
            {
                for (auto i = 0; i < math::enumCast(component::MaterialFinishType::LAST); ++i) {
                    RenderSpec spec;
                    spec.renderTarget = nullptr;
                    spec.screenSize = getScreenSize();
                    spec.zoom = mCamera.z;
                    spec.finishType = static_cast<component::MaterialFinishType>(i);
                    setMVP(spec, true, true, true);

                    begin(*mRendererTessellation, spec);
                    while (!mRenderables[i].empty()) {
                        auto &r = const_cast<graphic::Renderable &> (mRenderables[i].top());
                        auto ePos = applyParentTransforms(*r.manager, r.id, *r.pos, *r.heading);

                        if (!isVisible(ePos.pos, *r.scale)) {
                            mRenderables[i].pop();
                            continue;
                        }

                        // This will just point r.pos to a new location, which is temporary to this scope, but submit()
                        // will create a copy so it is safe.
                        r.pos = &ePos.pos;
                        r.heading = &ePos.heading;

                        mRendererTessellation->submit(r);

                        mRenderables[i].pop();

                        ++mRenderedSpritesPerFrame;
                    }
                    end(*mRendererTessellation);
                }
            }
        }

        void
        SceneManager::_render(entities::EntityManager &manager) {
            auto viewWidth = getViewWidth();
            auto viewHeight = getViewHeight();

            /// Sprites
            {
            }

            // TODO: This is obsolete in favour of Trail rendering with quads, for now!
            /// Trails
            {
                // renderStrip(manager, viewWidth, viewHeight);
            }

            /// UI
            {
                // Render UI text with fixed camera
                //begin(*mTextureShader, *mTextureRenderer, false, false, true);
                // TODO: This should actually be split up from static text and entities part of UI should be tagged so
                // renderStaticText(entityManager, viewWidth, viewHeight);
                //end(*mTextureShader, *mTextureRenderer);
            }

            /// Quads
            {
                renderQuad(manager);
            }
        }

        void
        SceneManager::renderInternal() {
            // render(*mSceneEntityManager);
        }

        void
        SceneManager::renderStaticText(entities::EntityManager &manager,
                                       common::r32 viewWidth,
                                       common::r32 viewHeight) {
            auto view = manager.createView<
                    component::Text,
                    component::Tag_Static,
                    component::WorldP3D>();
            for (const auto &entity: view) {
                auto &text = view.get<component::Text>(entity);
                auto &pos = view.get<component::WorldP3D>(entity);

                ++mRenderedSpritesPerFrame;
                ++mRenderedTexturesPerFrame;

                // mTextureRenderer->submit(text, pos);
            }
        }

        void
        SceneManager::renderStrip(entities::EntityManager &manager,
                                  common::r32 viewWidth,
                                  common::r32 viewHeight) {
            RenderSpec spec;
            spec.renderTarget = nullptr;
            spec.screenSize = getScreenSize();
            spec.zoom = mCamera.z;
            setMVP(spec, true, true, true);

            auto view = manager.createView<
                    component::WorldP3D_History>();
            for (auto &&id: view) {
                begin(*mRendererStrip, spec);

                const auto &ph = view.get<component::WorldP3D_History>(id).pos;
                auto count = 0;
                for (auto &&p: ph) {
#if DEBUG_Z
                    serverManager.printEntityType(id);
                    printf("%f\n", p.z);
#endif
                    auto alpha = common::r32(count) / ph.size();
                    auto color = component::Color{};
                    color.set_rgba(1, 1, 1, alpha);
                    mRendererStrip->submit(p, 1, color, {});
                    mRendererStrip->submit(p, -1, color, {});
                    ++count;
                }

                end(*mRendererStrip);
            }
        }

        void
        SceneManager::renderQuad(entities::EntityManager &manager) {
            {
                auto view = manager.createView<
                        component::BrushTrail,
                        component::WorldP3D>();
                const auto &rocketTrailTexture = mTextureManager.getTexture(
                        component::EntityPreset::ROCKET_TRAIL_DEFAULT);

                auto brush = Brush{};
                brush.type = component::BrushType::TEXTURE;
                // TODO: After setting the texture on BrushTrail, use that instead.
                brush.texture = &rocketTrailTexture;
                for (auto &&id: view) {
                    auto &trail = view.get<component::BrushTrail>(id);
                    for (auto &&quad: trail.quads) {
                        brush.shape_Quad = &quad;
                        splat(brush);
                    }
                    trail.quads.clear();
                }
            }
        }

        void
        SceneManager::renderTessellationColor(entities::EntityManager &manager,
                                              common::r32 viewWidth,
                                              common::r32 viewHeight) {
            auto texture = component::Texture{};
            auto view = manager.createView<
                    component::WorldP3D,
                    component::Heading,
                    component::Scale,
                    component::Color>();

            for (auto &&id: view) {
                const auto &pos = view.get<component::WorldP3D>(id);
                const auto &heading = view.get<component::Heading>(id);
                const auto &scale = view.get<component::Scale>(id);

                auto result = applyParentTransforms(manager, id, pos, heading);

                if (!isVisible(result.pos, scale)) {
                    continue;
                }

                const auto &color = view.get<component::Color>(id);

#if DEBUG_Z
                manager.printEntityType(id);
                printf("%f\n", result.pos.z);
#endif
                // mRendererTessellation->submit(result.pos, result.heading, scale, color, texture);

                ++mRenderedSpritesPerFrame;
            }
        }

        void
        SceneManager::renderTessellationTexture(entities::EntityManager &manager,
                                                common::r32 viewWidth,
                                                common::r32 viewHeight) {
            {
                auto color = component::Color{};
                auto view = manager.createView<
                        component::WorldP3D,
                        component::Heading,
                        component::Scale,
                        component::Texture>();
                for (auto &&id: view) {
                    const auto &pos = view.get<component::WorldP3D>(id);
                    const auto &heading = view.get<component::Heading>(id);
                    const auto &scale = view.get<component::Scale>(id);

                    auto result = applyParentTransforms(manager, id, pos, heading);

                    if (!isVisible(result.pos, scale)) {
                        continue;
                    }

                    const auto &texture = manager.get<component::Texture>(id);
                    assert(!texture.image.path.empty());
#if DEBUG_Z
                    serverManager.printEntityType(id);
                        printf("%f\n", result.pos.z);
#endif
                    // mRendererTessellation->submit(result.pos, result.heading, scale, color, texture);

                    ++mRenderedTexturesPerFrame;
                }
            }
        }

        void
        SceneManager::renderToTexture(const component::Quad &quad,
                                      const component::Color &src,
                                      const graphic::ScreenBounds &destBounds,
                                      component::Texture &dest,
                                      const math::mat4 *model) {
            RenderSpec spec;
            spec.renderTarget = &dest;
            spec.screenSize = getScreenSize();
            spec.zoom = mCamera.z;
            // TODO: I should probably take this as an argument
            spec.finishType = component::MaterialFinishType::SOLID;
            setMVP(spec, destBounds, model);

            begin(*mRendererQuad, spec);
            mRendererQuad->submit(quad, src, nullptr);
            end(*mRendererQuad);
        }

        void
        SceneManager::renderToTexture(const component::Quad &quad,
                                      const component::Texture &src,
                                      const graphic::ScreenBounds &destBounds,
                                      component::Texture &dest,
                                      const math::mat4 *model) {
            RenderSpec spec;
            // TODO: I should probably take this as an argument
            spec.finishType = component::MaterialFinishType::SOLID;
            spec.renderTarget = &dest;
            spec.screenSize = getScreenSize();
            spec.zoom = mCamera.z;
            setMVP(spec, destBounds, model);

            begin(*mRendererQuad, spec);
            mRendererQuad->submit(quad, {}, &src);
            end(*mRendererQuad);
        }

        void
        SceneManager::blend(const component::Texture &front,
                            component::Texture &back) {
            RenderSpec spec;
            // TODO: I should probably take this as an argument
            spec.finishType = component::MaterialFinishType::SOLID;
            spec.renderTarget = &back;
            spec.screenSize = getScreenSize();
            spec.zoom = mCamera.z;
            spec.view = math::mat4::identity();
            spec.model = math::mat4::identity();
            spec.proj = math::mat4::orthographic(-1, +1,
                                                 -1, +1,
                                                 -1, +1);

            auto quad = component::Quad{};
            mRendererQuad->begin(spec);
            mRendererQuad->submit(quad, {}, front, back);
            end(*mRendererQuad);
        }

        void
        SceneManager::splat(Brush &brush) {
            component::AABB aabb;
            math::findAABB(*brush.shape_Quad, aabb);

            auto tileEntities = std::set<common::EntityID>();
            auto tl = getOrCreateCanvasTile(aabb.topLeft());
            tileEntities.insert(tl);

            auto bl = getOrCreateCanvasTile(aabb.bottomLeft());
            tileEntities.insert(bl);

            auto br = getOrCreateCanvasTile(aabb.bottomRight());
            tileEntities.insert(br);

            auto tr = getOrCreateCanvasTile(aabb.topRight());
            tileEntities.insert(tr);

            // TODO: The following code is a good candidate for multi-threaded worker-pool based architecture
            // as it uses at least two draw calls per-canvas and the work does not interfere with there
            // reset of the code since usage of mSceneEntityManager is limited to this class.

            /// Draw
            for (auto &&canvasEntity: tileEntities) {
                auto &canvasPos = mInternalReg->get<component::WorldP3D>(canvasEntity);
                graphic::ScreenBounds screenBounds;
                screenBounds.xMin = canvasPos.x - mHalfCanvasTileSizeX;
                screenBounds.xMax = canvasPos.x + mHalfCanvasTileSizeX;
                screenBounds.yMin = canvasPos.y - mHalfCanvasTileSizeY;
                screenBounds.yMax = canvasPos.y + mHalfCanvasTileSizeY;
                auto &texture = mInternalReg->get<component::Texture>(canvasEntity);
                switch (brush.type) {
                    case component::BrushType::COLOR: {
                        renderToTexture(*brush.shape_Quad, *brush.color, screenBounds, texture, brush.model);
                        break;
                    }
                    case component::BrushType::TEXTURE: {
                        renderToTexture(*brush.shape_Quad, *brush.texture, screenBounds, texture, brush.model);
                        break;
                    }
                    case component::BrushType::TEXTURE_TAG: {
                        auto &brushTexture = mTextureManager.getTexture(brush.tag);
                        renderToTexture(*brush.shape_Quad, brushTexture, screenBounds, texture, brush.model);
                        break;
                    }
                    case component::BrushType::LAST: {
                        assert(false);
                        break;
                    }
                }
            }
        }

        common::EntityID
        SceneManager::getOrCreateCanvasTile(const math::vec2 &pos) {
            return getOrCreateCanvasTile(component::WorldP3D{pos.x, pos.y, 0.f});
        }

        common::EntityID
        SceneManager::getOrCreateCanvasTile(const component::WorldP3D &pos) {
            auto x = math::findBin(pos.x, mCanvasTileSizeX);
            auto y = math::findBin(pos.y, mCanvasTileSizeY);
            auto xy = math::pack_i32(x, y);

            auto missing = mCanvasTileLookup.find(xy) == mCanvasTileLookup.end();
            if (missing) {
                // TODO: I need a factory to create entities like this and then initialize them based on
                // preset.
                auto id = mInternalReg->createEntity_CanvasTile();

                auto tilePosX = math::binPos(x, mCanvasTileSizeX) + mCanvasTileSizeX / 2.f;
                auto tilePosY = math::binPos(y, mCanvasTileSizeY) + mCanvasTileSizeY / 2.f;
                auto tilePosZ = mZLayerManager.getZForEntity(entities::EntityType::CANVAS);

                auto heading = component::Heading{0.f};

                mInternalReg->setWorldP3D(id, tilePosX, tilePosY, tilePosZ);
                mInternalReg->setScale(id,
                                       static_cast<common::r32>(mCanvasTileSizeX),
                                       static_cast<common::r32>(mCanvasTileSizeY));
                mInternalReg->setHeading(id, heading.value);

                auto &ms = mInternalReg->get<component::MaterialSurface>(id);

                auto widthInPixels = static_cast<common::u16>(mCanvasTileSizeX * mGameUnitToPixels +
                                                              common::EP32);
                auto heightInPixels = static_cast<common::u16>(mCanvasTileSizeY * mGameUnitToPixels +
                                                               common::EP32);

                ms.texture.image.width = widthInPixels;
                ms.texture.image.height = heightInPixels;
                ms.texture.image.path = "generated_by_getOrCreateCanvasTile::canvasFront";
                ms.texture.clear = false;

                mTextureManager.createTexture(ms.texture, false);

                mCanvasTileLookup.emplace(xy, id);
            }

            auto entity = mCanvasTileLookup[xy];
            assert(mInternalReg->valid(entity));
            return entity;
        }

        void
        SceneManager::updateSmokeEmitter(entities::EntityManager &manager,
                                         common::r64 tickTime) {
            assert(manager.getSimMode() == entities::SimMode::CLIENT ||
                   manager.getSimMode() == entities::SimMode::CLIENT_SIDE_SERVER);

            auto view = manager.createView<component::CoolDown>();
            for (auto &&id: view) {
                auto &emitter = view.get<component::CoolDown>(id);
                math::subAndZeroClip(emitter.current, tickTime);
            }
        }

        void
        SceneManager::updateAfterMark(entities::EntityManager &manager,
                                      common::r64 tickTime) {
            auto view = manager.createView<
                    component::AfterMark,
                    component::Heading,
                    component::WorldP3D>();
            for (auto &&id: view) {
                const auto &mark = view.get<component::AfterMark>(id);
                const auto &pos = view.get<component::WorldP3D>(id);
                const auto &heading = view.get<component::Heading>(id);

                auto transformed = applyParentTransforms(manager, id, pos, heading);
                Brush brush;
                auto quad = component::Quad{};
                auto model = math::createTransformation(transformed.pos, transformed.heading, mark.scale);
                switch (mark.type) {
                    case component::BrushType::COLOR: {
                        brush.color = &mark.color;
                        break;
                    }
                    case component::BrushType::TEXTURE: {
                        // TODO: Implement
                        assert(false);
                        break;
                    }
                    case component::BrushType::TEXTURE_TAG: {
                        brush.tag = mark.tag;
                        break;
                    }
                    case component::BrushType::LAST:
                    default: {
                        assert(false);
                        break;
                    }
                }
                brush.shape_Quad = &quad;
                brush.type = component::BrushType::COLOR;
                brush.model = &model;

                splat(brush);
            }
        }

        void
        SceneManager::updateTextureAnimated(entities::EntityManager &manager,
                                            common::r64 tickTime) {
            assert(manager.getSimMode() == entities::SimMode::CLIENT ||
                   manager.getSimMode() == entities::SimMode::CLIENT_SIDE_SERVER);

            auto view = manager.createView<component::TextureAnimated>();
            for (auto &&id: view) {
                auto &ta = view.get<component::TextureAnimated>(id);
                ta.timeElapsed += tickTime;
                // NOTE: It is assumed that this function is called more often than animation fps!
                assert(ta.frameDuration > tickTime);
                if (ta.playing && math::almost_Greater(ta.timeElapsed, ta.frameDuration)) {
                    ta.nextFrame = (ta.nextFrame + 1) % math::enumCast(ta.numFrames);
                    ta.timeElapsed = 0;
                    if (ta.nextFrame == 0) {
                        switch (ta.endingBehaviour) {
                            case component::AnimationEndingBehavior::LOOP: {
                                break;
                            }
                            case component::AnimationEndingBehavior::PLAY_AND_STOP: {
                                ta.playing = false;
                                break;
                            }
                            case component::AnimationEndingBehavior::PLAY_AND_REMOVE_ENTITY: {
                                manager.markForDeletion(id);
                                break;
                            }
                            case component::AnimationEndingBehavior::LAST:
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
        SceneManager::zoom(common::r32 distance) {
            mCamera.z = 1 / distance;
        }

        bool
        SceneManager::isVisible(const component::WorldP3D &pos) {
            return math::intersects(pos, mCamera.x, mCamera.y, getViewWidth(), getViewHeight());
        }

        bool
        SceneManager::isVisible(const component::WorldP3D &pos,
                                const component::Scale &scale) {
            return math::intersects(pos, scale, mCamera.x, mCamera.y, getViewWidth(), getViewHeight());
        }

        void
        SceneManager::lookAt(common::r32 x,
                             common::r32 y) {
            mCamera.x = x;
            mCamera.y = y;
        }

        void
        SceneManager::lookAt(common::r32 x,
                             common::r32 y,
                             common::r32 distance) {
            mCamera.x = x;
            mCamera.y = y;
            mCamera.z = 1 / distance;
        }

        common::u16
        SceneManager::getSpritesPerFrame() const {
            return mRenderedSpritesPerFrame;
        }

        common::u16
        SceneManager::getParticlesPerFrame() const {
            return mRenderedParticlesPerFrame;
        }

        common::u16
        SceneManager::getTexturesPerFrame() const {
            return mRenderedTexturesPerFrame;
        }

        common::r32
        SceneManager::getViewWidth() const {
            return (mScreenBounds.xMax - mScreenBounds.xMin) * (1.0f / mCamera.z);
        }

        common::r32
        SceneManager::getViewHeight() const {
            return (mScreenBounds.yMax - mScreenBounds.yMin) * (1.0f / mCamera.z);
        }

        const graphic::ScreenBounds &
        SceneManager::getScreenBounds() const {
            return mScreenBounds;
        }

        void
        SceneManager::resetCounters() {
            mRenderedSpritesPerFrame = 0;
            mRenderedParticlesPerFrame = 0;
            mRenderedTexturesPerFrame = 0;
        }

        SceneManager::WorldP3DAndHeading
        SceneManager::applyParentTransforms(const entities::EntityManager &manager,
                                            common::EntityID child,
                                            const component::WorldP3D &childPos,
                                            const component::Heading &childHeading) {
            auto transformation = math::mat4::identity();
            auto result = WorldP3DAndHeading{childPos, childHeading};
            auto numParents = common::size{};

            while (manager.has<component::EntityAttachee>(child)) {
                const auto &parent = manager.get<component::EntityAttachee>(child);
                const auto &parentPos = manager.get<component::WorldP3D>(parent.entityID);
                const auto &parentHeading = manager.get<component::Heading>(parent.entityID);

                // TODO: Scaling is ignored because it is used to store the size of the object, so even non-scaled
                // objects have scaling matrix larger than identity matrix, so if I use the parent scaling
                // that will just "scale" the child by the size of parent, which is not what I want.
                // Perhaps removing Size component was the wrong decision, and I should have a distinction
                // between size and Scale. Renderer passes scale down to shader as a size anyway, it does NOT
                // use it as a multiplier.
                transformation *= math::createTransformation(parentPos, parentHeading, {});
                result.pos.value = transformation * result.pos.value;
                result.heading.value += parentHeading.value;

                child = parent.entityID;
                ++numParents;

                assert(numParents < 20);
            }
            return result;
        }
    }
}

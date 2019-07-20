#include <oni-core/graphic/oni-graphic-scene-manager.h>

#include <set>

#include <GL/glew.h>

#include <oni-core/asset/oni-asset-manager.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-gameplay.h>
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
#include <oni-core/physics/oni-physics-dynamics.h>

#define DEBUG_Z 0

namespace oni {
    namespace graphic {
        SceneManager::SceneManager(const graphic::ScreenBounds &screenBounds,
                                   asset::AssetManager &assetManager,
                                   math::ZLayerManager &zLayerManager,
                                   b2World &physicsWorld,
                                   common::r32 gameUnitToPixels) :
                mCanvasTileSizeX{110},
                mCanvasTileSizeY{110},
                mHalfCanvasTileSizeX{mCanvasTileSizeX / 2.f},
                mHalfCanvasTileSizeY{mCanvasTileSizeY / 2.f},
                // 64k vertices
                mMaxSpriteCount(64 * 1000),
                mScreenBounds(screenBounds),
                mAssetManager(assetManager),
                mPhysicsWorld(physicsWorld),
                mGameUnitToPixels(gameUnitToPixels),
                mZLayerManager(zLayerManager) {

            mProjectionMatrix = math::mat4::orthographic(screenBounds.xMin, screenBounds.xMax, screenBounds.yMin,
                                                         screenBounds.yMax, -1.0f, 1.0f);
            mViewMatrix = math::mat4::identity();

            mModelMatrix = math::mat4::identity();

            mRendererTessellation = std::make_unique<Renderer_OpenGL_Tessellation>(mMaxSpriteCount);
            mRendererStrip = std::make_unique<Renderer_OpenGL_Strip>(mMaxSpriteCount);
            mRendererQuad = std::make_unique<Renderer_OpenGL_Quad>(mMaxSpriteCount);

            mTextureManager = std::make_unique<TextureManager>(mAssetManager);

            mSceneEntityManager = std::make_unique<entities::EntityManager>(entities::SimMode::CLIENT, zLayerManager,
                                                                            nullptr);

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
            begin(*mRendererTessellation, true, false, true);
        }

        void
        SceneManager::endColorRendering() {
            end(*mRendererTessellation);
        }

        void
        SceneManager::begin(Renderer &renderer2D,
                            bool translate,
                            bool scale,
                            bool project,
                            component::Texture *renderTarget) {
            auto model = math::mat4::identity();
            auto view = math::mat4::identity();
            auto proj = math::mat4::identity();
            if (scale) {
                view = view * math::mat4::scale({mCamera.z, mCamera.z, 1.0f});
            }
            if (renderTarget and false) {
                auto multiplier = 1.f;
                if (mCamera.z > 1.f and false) {
                    view = view * math::mat4::scale({multiplier * mCamera.z, multiplier * mCamera.z, 1});
                } else {
                    view = view * math::mat4::scale({multiplier, multiplier, 1});
                }
            }
            if (translate) {
                view = view * math::mat4::translation(-mCamera.x, -mCamera.y, 0.0f);
            }
            if (project) {
                proj = mProjectionMatrix;
            }
            renderer2D.begin(model, view, proj, {getViewWidth(), getViewHeight()}, mCamera.z, renderTarget);
        }

        void
        SceneManager::begin(Renderer &renderer2D,
                            bool translate,
                            bool scale,
                            bool project) {
            begin(renderer2D, translate, scale, project, nullptr);
        }

        void
        SceneManager::begin(Renderer &renderer2D,
                            component::Texture *renderTarget) {
            begin(renderer2D, false, false, false, renderTarget);
        }

        void
        SceneManager::prepareTexture(entities::EntityManager &manager,
                                     common::EntityID id,
                                     component::TextureTag tag) {
            auto &texture = manager.get<component::Texture>(id);
            texture = mTextureManager->loadOrGetTexture(tag, false);
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
        SceneManager::render(entities::EntityManager &manager) {
            auto viewWidth = getViewWidth();
            auto viewHeight = getViewHeight();

            /// Sprites
            {
                renderTessellation(manager, viewWidth, viewHeight);
            }

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
                renderQuad(manager, viewWidth, viewHeight);
            }
        }

        void
        SceneManager::renderInternal() {
            render(*mSceneEntityManager);
        }

        void
        SceneManager::renderStaticText(entities::EntityManager &manager,
                                       common::r32 viewWidth,
                                       common::r32 viewHeight) {
            auto view = manager.createView<component::Text, component::Tag_Static, component::WorldP3D>();
            for (const auto &entity: view) {
                auto &text = view.get<component::Text>(entity);
                auto &pos = view.get<component::WorldP3D>(entity);

                ++mRenderedSpritesPerFrame;
                ++mRenderedTexturesPerFrame;

                // mTextureRenderer->submit(text, pos);
            }
        }

        void
        SceneManager::renderTessellation(entities::EntityManager &manager,
                                         common::r32 viewWidth,
                                         common::r32 viewHeight) {
            manager.sort<component::WorldP3D>([](const auto &lhs,
                                                 const auto &rhs) {
                return lhs.z < rhs.z;
            });

            begin(*mRendererTessellation, true, true, true);

            renderTessellationColor(manager, viewWidth, viewHeight);
            renderTessellationTexture(manager, viewWidth, viewHeight);

            end(*mRendererTessellation);
        }

        void
        SceneManager::renderStrip(entities::EntityManager &manager,
                                  common::r32 viewWidth,
                                  common::r32 viewHeight) {
            auto view = manager.createView<
                    component::WorldP3D_History>();
            for (auto &&id: view) {
                begin(*mRendererStrip, true, true, true);

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
        SceneManager::renderQuad(entities::EntityManager &manager,
                                 common::r32 viewWidth,
                                 common::r32 viewHeight) {
#if 0
            {
                static auto texture = component::Texture{};
                texture.image.path = "WHAT";
                auto trailTexture = mTextureManager->loadOrGetTexture(component::TextureTag::TEST_TEXTURE, false);
                auto m = math::vec3{};
                {
                    auto C = 2.f;
                    auto a = component::WorldP3D{-1 * C, -1 * C, 1};
                    auto b = component::WorldP3D{-1 * C, +1 * C, 1};
                    auto c = component::WorldP3D{+1 * C, +1 * C, 1};
                    auto d = component::WorldP3D{+1 * C, -1 * C, 1};
                    m = a.value + b.value + c.value + d.value;
                    m = m / 4.f;
                    a.value -= m;
                    b.value -= m;
                    c.value -= m;
                    d.value -= m;
                    auto v = std::vector<component::WorldP3D>();
                    v.push_back(a);
                    v.push_back(b);
                    v.push_back(c);
                    v.push_back(d);

                    auto width = std::ceil((d.x - a.x) * mGameUnitToPixels);
                    auto height = std::ceil((b.y - a.y) * mGameUnitToPixels);
                    if (!texture.textureID || texture.image.width != width || texture.image.height != height) {
                        texture.image.width = width;
                        texture.image.height = height;
                        mTextureManager->createTexture(texture, false);
                    }
#if 1
                    auto model = math::mat4::identity();
                    auto view = math::mat4::identity();
                    auto proj = math::mat4::orthographic(-C, C, -C, C, -1.0f, 1.0f);
                    mRendererQuad->begin(model, view, proj, {getViewWidth(), getViewHeight()}, mCamera.z, &texture);
#else
                    begin(*mRendererQuad, &texture);
#endif

                    mRendererQuad->submit(v.data(), {}, trailTexture);
                    end(*mRendererQuad);
                }
                {
                    auto brush = Brush{};
                    brush.type = component::BrushType::TEXTURE;
                    brush.texture = &texture;
                    // TODO: Correct the scale
                    auto scale = component::Scale{5, 5, 1};
                    splat({m.x, m.y, m.z}, scale, brush);
                }
                {
                    begin(*mRendererTessellation, false, false, true);
                    auto pos = component::WorldP3D{8 - 8 / 4.f, 4 - 4 / 4.f, 0.4f};
                    auto heading = component::Heading{};
                    auto scale = component::Scale{texture.image.width / 100.f, texture.image.height / 100.f, 1.f};
                    auto color = component::Color::WHITE();

                    mRendererTessellation->submit(pos, heading, scale, color, texture);
                    end(*mRendererTessellation);
                }

                {
                    begin(*mRendererTessellation, false, false, true);
                    auto pos = component::WorldP3D{8 - 8 / 4.f, -4 + 4 / 4.f, 0.4f};
                    auto heading = component::Heading{};
                    auto scale = component::Scale{texture.image.width / 100.f, texture.image.height / 100.f, 1.f};
                    auto color = component::Color::WHITE();

                    mRendererTessellation->submit(pos, heading, scale, color, trailTexture);
                    end(*mRendererTessellation);
                }
            }

#elif 0
            {
                auto view = manager.createView<
                        component::BrushTrail,
                        component::WorldP3D>();
                if (view.empty()) {
                    return;
                }

                auto rocketTrailTexture = mTextureManager->loadOrGetTexture(component::TextureTag::ROCKET_TRAIL, false);
                for (auto &&id: view) {
                    auto &trail = view.get<component::BrushTrail>(id);
                    if (trail.vertices.empty()) {
                        continue;
                    }
                    auto &texture = trail.texture;
#if 1
                    auto mid = math::vec3{};
                    auto aabb = component::AABB{};
                    for (common::size i = 0; i + 3 < trail.vertices.size();) {
                        {
                            /// 1) Find the AABB
                            /// 2) Set texture.width to gameUnitToPixel * width of AABB
                            /// 3) Set texture.height to gameUnitToPixel * height of AABB
                            /// 4) Create a new texture that fits the sizes
                            /// 5) Set the projection matrix size to match AABB
                            /// 6) Render to the texture
                            /// 7) Splat
                            // TODO: This is one draw call per quad!
                            // TODO: This calculation could be replaced by a flag in the shader, such as centerAlign, which
                            // will move back quads so that center of quad is at (0, 0, z);
                            auto &a = trail.vertices[i];
                            auto &b = trail.vertices[i + 1];
                            auto &c = trail.vertices[i + 2];
                            auto &d = trail.vertices[i + 3];

                            mid = a.value + b.value + c.value + d.value;
                            mid = mid / 4.f;
                            a.value -= mid;
                            b.value -= mid;
                            c.value -= mid;
                            d.value -= mid;

                            aabb.min = {math::min(math::min(a.x, b.x), math::min(c.x, d.x)),
                                        math::min(math::min(a.y, b.y), math::min(c.y, d.y))};
                            aabb.max = {math::max(math::max(a.x, b.x), math::max(c.x, d.x)),
                                        math::max(math::max(a.y, b.y), math::max(c.y, d.y))};

                            auto width = common::i32(mGameUnitToPixels * aabb.width());
                            auto height = common::i32(mGameUnitToPixels * aabb.height());
                            if (width == 0 || height == 0) {
                                i += 4;
                                continue;
                            }

                            assert(aabb.min.y < aabb.max.y);
                            assert(aabb.min.x < aabb.max.x);

                            if (!texture.textureID || texture.image.width != width || texture.image.height != height) {
                                texture.image.width = width;
                                texture.image.height = height;
                                texture.image.path = "WHAT";
                                mTextureManager->createTexture(texture, false);
                            }

                            auto modelM = math::mat4::identity();
                            auto viewM = math::mat4::identity();
                            auto projM = math::mat4::orthographic(aabb.min.x, aabb.max.x, aabb.min.y, aabb.max.y, -1.0f,
                                                                  1.0f);
                            mRendererQuad->begin(modelM, viewM, projM, {getViewWidth(), getViewHeight()}, mCamera.z,
                                                 &texture);
                            mRendererQuad->submit(&trail.vertices[i], {}, rocketTrailTexture);
                            std::cout <<
                                      trail.vertices[i].value + mid << ", " << trail.vertices[i + 1].value  + mid<< ", " <<
                                      trail.vertices[i + 2].value  + mid<< ", " << trail.vertices[i + 3].value  + mid<< "\n";
                            i += 4;
                            end(*mRendererQuad);

                        }
                        {
                            auto brush = Brush{};
                            brush.type = component::BrushType::TEXTURE;
                            brush.texture = &texture;
                            // TODO: Correct the scale
                            auto scale = component::Scale{aabb.width(), aabb.height(), 1};
                            splat({mid.x, mid.y, mid.z}, scale, brush);
                        }
                        {
                            begin(*mRendererTessellation, false, false, true);
                            auto pos = component::WorldP3D{8 - 8 / 4.f, 4 - 4 / 4.f, 0.4f};
                            auto heading = component::Heading{};
                            auto scale = component::Scale{8 / 4.f, 4 / 4.f, 1};
                            auto color = component::Color::WHITE();

                            mRendererTessellation->submit(pos, heading, scale, color, texture);
                            end(*mRendererTessellation);
                        }
                    }

                    trail.vertices.clear();
#else
                    {
                        begin(*mRendererQuad, true, true, true);
                        for (common::size i = 0; i + 3 < trail.vertices.size();) {
                            mRendererQuad->submit(&trail.vertices[i], {}, rocketTrailTexture);
                            std::cout <<
                                      trail.vertices[i].value << ", " << trail.vertices[i + 1].value << ", " <<
                                      trail.vertices[i + 2].value << ", " << trail.vertices[i + 3].value << "\n";
                            i += 4;
                        }
                        std::cout << "-----\n";
                        end(*mRendererQuad);
                    }
#endif
                }
            }
#else
            {
                auto view = manager.createView<
                        component::BrushTrail,
                        component::WorldP3D>();
                if (view.empty()) {
                    return;
                }

                auto rocketTrailTexture = mTextureManager->loadOrGetTexture(component::TextureTag::ROCKET_TRAIL, false);
                for (auto &&id: view) {
                    auto &trail = view.get<component::BrushTrail>(id);
                    if (trail.vertices.empty()) {
                        continue;
                    }

                    for (common::size i = 0; i + 3 < trail.vertices.size();) {
                        auto aabb = component::AABB{};
                        auto &a = trail.vertices[i];
                        auto &b = trail.vertices[i + 1];
                        auto &c = trail.vertices[i + 2];
                        auto &d = trail.vertices[i + 3];

                        aabb.min = {math::min(math::min(a.x, b.x), math::min(c.x, d.x)),
                                    math::min(math::min(a.y, b.y), math::min(c.y, d.y))};
                        aabb.max = {math::max(math::max(a.x, b.x), math::max(c.x, d.x)),
                                    math::max(math::max(a.y, b.y), math::max(c.y, d.y))};

                        std::set<common::EntityID> tileEntities;

                        auto tl = getOrCreateCanvasTile(aabb.topLeft());
                        tileEntities.insert(tl);

                        auto bl = getOrCreateCanvasTile(aabb.bottomLeft());
                        tileEntities.insert(bl);

                        auto br = getOrCreateCanvasTile(aabb.bottomRight());
                        tileEntities.insert(br);

                        auto tr = getOrCreateCanvasTile(aabb.topRight());
                        tileEntities.insert(tr);

                        /// Draw
                        for (auto &&canvasEntity: tileEntities) {
                            auto &canvasPos = mSceneEntityManager->get<component::WorldP3D>(canvasEntity);
                            auto modelM = math::mat4::identity();
                            auto viewM = math::mat4::identity();
                            auto projM = math::mat4::orthographic(canvasPos.x - mHalfCanvasTileSizeX,
                                                                  canvasPos.x + mHalfCanvasTileSizeX,
                                                                  canvasPos.y - mHalfCanvasTileSizeY,
                                                                  canvasPos.y + mHalfCanvasTileSizeY,
                                                                  -1.0f,
                                                                  1.0f);
                            auto &canvasTexture = mSceneEntityManager->get<component::CanvasTexture>(canvasEntity);
                            mRendererQuad->begin(modelM, viewM, projM, {getViewWidth(), getViewHeight()}, mCamera.z,
                                                 &canvasTexture.canvasFront);
                            mRendererQuad->submit(&trail.vertices[i], {}, rocketTrailTexture);
                            end(*mRendererQuad);
                        }

                        for (auto &&canvasEntity: tileEntities) {
                            auto &canvasTexture = mSceneEntityManager->get<component::CanvasTexture>(canvasEntity);
                            blend(canvasTexture.canvasFront, canvasTexture.canvasBack);
                        }
                        i += 4;
                    }

                    trail.vertices.clear();
                }

            }
#endif
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
                    component::Color,
                    component::Tag_ColorShaded>();

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
                mRendererTessellation->submit(result.pos, result.heading, scale, color, texture);

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
                        component::Texture,
                        component::Tag_TextureShaded>();
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
                    mRendererTessellation->submit(result.pos, result.heading, scale, color, texture);

                    ++mRenderedTexturesPerFrame;
                }
            }
            {
                auto color = component::Color{};
                auto view = manager.createView<
                        component::WorldP3D,
                        component::Heading,
                        component::Scale,
                        component::CanvasTexture,
                        component::Tag_TextureShaded>();
                for (auto &&id: view) {
                    const auto &pos = view.get<component::WorldP3D>(id);
                    const auto &heading = view.get<component::Heading>(id);
                    const auto &scale = view.get<component::Scale>(id);

                    auto result = applyParentTransforms(manager, id, pos, heading);

                    if (!isVisible(result.pos, scale)) {
                        continue;
                    }

                    const auto &texture = manager.get<component::CanvasTexture>(id);
                    assert(!texture.canvasBack.image.path.empty());
#if DEBUG_Z
                    serverManager.printEntityType(id);
                        printf("%f\n", result.pos.z);
#endif
                    mRendererTessellation->submit(result.pos, result.heading, scale, color, texture.canvasBack);

                    ++mRenderedTexturesPerFrame;

                }
            }
        }

        void
        SceneManager::blend(const component::Texture &front,
                            component::Texture &back) {
            auto quad = component::Quad{};
            auto modelM = math::mat4::identity();
            auto viewM = math::mat4::identity();
            auto projM = math::mat4::orthographic(-1, +1,
                                                  -1, +1,
                                                  -1, +1);
            mRendererQuad->begin(modelM, viewM, projM, {getViewWidth(), getViewHeight()}, mCamera.z, &back);
            mRendererQuad->submit(quad, {}, front, back);
            end(*mRendererQuad);
        }

        void
        SceneManager::splat(const component::WorldP3D &worldPos,
                            const component::Scale &scale,
                            Brush &brush) {
            std::set<common::EntityID> tileEntities;

            auto entityID = getOrCreateCanvasTile(worldPos);
            tileEntities.insert(entityID);

            auto lowerLeft = worldPos;
            lowerLeft.x -= scale.x / 2.f;
            lowerLeft.y -= scale.y / 2.f;
            auto lowerLeftEntityID = getOrCreateCanvasTile(lowerLeft);
            tileEntities.insert(lowerLeftEntityID);

            auto topRight = worldPos;
            topRight.x += scale.x / 2.f;
            topRight.y += scale.y / 2.f;

            auto topRightEntityID = getOrCreateCanvasTile(topRight);
            tileEntities.insert(topRightEntityID);

            auto topLeft = worldPos;
            topLeft.x -= scale.x / 2.f;
            topLeft.y += scale.y / 2.f;
            auto topLeftEntityID = getOrCreateCanvasTile(topLeft);
            tileEntities.insert(topLeftEntityID);

            auto lowerRight = worldPos;
            lowerRight.x += scale.x / 2.f;
            lowerRight.y -= scale.y / 2.f;
            auto lowerRightEntityID = getOrCreateCanvasTile(lowerRight);
            tileEntities.insert(lowerRightEntityID);

            for (auto &&tileEntity: tileEntities) {
                updateCanvasTile(tileEntity, brush, worldPos, scale);
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
            auto xy = math::pack_i64(x, y);

            auto missing = mCanvasTileLookup.find(xy) == mCanvasTileLookup.end();
            if (missing) {
                auto id = mSceneEntityManager->createEntity_CanvasTile();

                auto tilePosX = math::binPos(x, mCanvasTileSizeX) + mCanvasTileSizeX / 2.f;
                auto tilePosY = math::binPos(y, mCanvasTileSizeY) + mCanvasTileSizeY / 2.f;
                auto tilePosZ = mZLayerManager.getZForEntity(entities::EntityType::CANVAS);

                auto heading = component::Heading{0.f};

                mSceneEntityManager->setWorldP3D(id, tilePosX, tilePosY, tilePosZ);
                mSceneEntityManager->setScale(id,
                                              static_cast<common::r32>(mCanvasTileSizeX),
                                              static_cast<common::r32>(mCanvasTileSizeY));
                mSceneEntityManager->setHeading(id, heading.value);

                auto &canvasTexture = mSceneEntityManager->get<component::CanvasTexture>(id);

                auto widthInPixels = static_cast<common::u16>(mCanvasTileSizeX * mGameUnitToPixels +
                                                              common::EP32);
                auto heightInPixels = static_cast<common::u16>(mCanvasTileSizeY * mGameUnitToPixels +
                                                               common::EP32);

                canvasTexture.canvasFront.image.width = widthInPixels;
                canvasTexture.canvasFront.image.height = heightInPixels;
                canvasTexture.canvasFront.image.path = "generated_by_getOrCreateCanvasTile::canvasFront";
                canvasTexture.canvasFront.clear = true;

                canvasTexture.canvasBack.image.width = widthInPixels;
                canvasTexture.canvasBack.image.height = heightInPixels;
                canvasTexture.canvasBack.image.path = "generated_by_getOrCreateCanvasTile::canvasBack";

                mTextureManager->fill(canvasTexture.canvasFront.image, {});
                mTextureManager->fill(canvasTexture.canvasBack.image, {});

                mTextureManager->createTexture(canvasTexture.canvasFront, false);
                mTextureManager->createTexture(canvasTexture.canvasBack, false);

                mCanvasTileLookup.emplace(xy, id);
            }

            auto entity = mCanvasTileLookup[xy];
            assert(mSceneEntityManager->valid(entity));
            return entity;
        }

        void
        SceneManager::updateSmokeEmitter(entities::EntityManager &manager,
                                         common::r64 tickTime) {
            assert(manager.getSimMode() == entities::SimMode::CLIENT ||
                   manager.getSimMode() == entities::SimMode::CLIENT_SIDE_SERVER);

            auto view = manager.createView<component::SmokeEmitterCD>();
            for (auto &&id: view) {
                auto &emitter = view.get<component::SmokeEmitterCD>(id);
                math::subAndZeroClip(emitter.currentCD, tickTime);
            }
        }

        void
        SceneManager::updateAfterMark(entities::EntityManager &manager,
                                      common::r64 tickTime) {
            auto view = manager.createView<
                    component::AfterMark,
                    component::WorldP3D,
                    component::Scale>();
            for (auto &&id: view) {
                const auto &scale = view.get<component::Scale>(id);
                const auto &mark = view.get<component::AfterMark>(id);
                auto brush = graphic::Brush{};
                brush.tag = mark.textureTag;
                brush.type = component::BrushType::TEXTURE_TAG;

                const auto &pos = view.get<component::WorldP3D>(id);
                splat(pos, scale, brush);
            }
        }

        void
        SceneManager::updateCanvasTile(common::EntityID entityID,
                                       Brush &brush,
                                       const component::WorldP3D &worldPos,
                                       const component::Scale &scale) {
            auto &canvasTexture = mSceneEntityManager->get<component::Texture>(entityID);
            const auto &canvasTilePos = mSceneEntityManager->get<component::WorldP3D>(entityID);
            const auto &canvasSize = mSceneEntityManager->get<component::Scale>(entityID);

            // TODO: Probably better to not use texture coordinates until the very last moment and keep everything
            // in game units at this point.
            auto canvasLowerLeftPos = component::WorldP3D{canvasTilePos.x - canvasSize.x / 2.f,
                                                          canvasTilePos.y - canvasSize.y / 2.f,
                                                          canvasTilePos.z};
            auto brushTexturePos = worldPos;
            math::worldToTextureCoordinate(canvasLowerLeftPos, mGameUnitToPixels, brushTexturePos);

            switch (brush.type) {
                case component::BrushType::SPRITE: {
                    auto image = component::Image{};
                    image.width = static_cast<uint16>(scale.x * mGameUnitToPixels);
                    image.height = static_cast<uint16>(scale.y * mGameUnitToPixels);
                    mTextureManager->fill(image, brush.color);
                    mTextureManager->blendAndUpdateTexture(canvasTexture, image, brushTexturePos.value);
                    break;
                }
                case component::BrushType::TEXTURE_TAG: {
                    auto image = component::Image{};
                    // TODO: This will ignore brushSize and it will only depend on the image pixel size which is not
                    // at all what I intended
                    mTextureManager->loadOrGetImage(brush.tag, image);
                    mTextureManager->blendAndUpdateTexture(canvasTexture, image, brushTexturePos.value);
                    break;
                }
                case component::BrushType::TEXTURE: {
                    mTextureManager->blendAndUpdateTexture(canvasTexture, brush.texture->image,
                                                           brushTexturePos.value);
                    break;
                }
                default: {
                    assert(false);
                    return;
                }
            }
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

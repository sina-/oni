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
                            bool project) {
            auto model = math::mat4::identity();
            auto view = math::mat4::identity();
            auto proj = math::mat4::identity();
            if (scale) {
                view = view * math::mat4::scale(math::vec3{mCamera.z, mCamera.z, 1.0f});
            }

            if (translate) {
                view = view * math::mat4::translation(-mCamera.x, -mCamera.y, 0.0f);
            }
            if (project) {
                proj = mProjectionMatrix;
            }
            renderer2D.begin(model, view, proj, math::vec2{getViewWidth(), getViewHeight()}, mCamera.z);
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
                renderStrip(manager, viewWidth, viewHeight);
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
            /// Test draw brush trail
            {
                begin(*mRendererQuad, true, true, true);
                auto view = manager.createView<component::BrushTrail>();
                auto scale = component::Scale{};
                auto texture = component::Texture{};
                auto color = component::Color::WHITE();
                color.set_a(0.1f);
                for (auto &&id: view) {
                    const auto &trail = view.get<component::BrushTrail>(id);
                    for (common::size i = 0; i + 4 < trail.vertices.size();) {
                        mRendererQuad->submit(&trail.vertices[i], scale, color, texture);
                        i += 4;
                    }
                }
                end(*mRendererQuad);
            }
        }

        void
        SceneManager::renderTessellationColor(entities::EntityManager &manager,
                                              common::r32 viewWidth,
                                              common::r32 viewHeight) {
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
                mRendererTessellation->submit(result.pos, result.heading, scale, color, component::Texture{});

                ++mRenderedSpritesPerFrame;
            }
        }

        void
        SceneManager::renderTessellationTexture(entities::EntityManager &manager,
                                                common::r32 viewWidth,
                                                common::r32 viewHeight) {
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
                mRendererTessellation->submit(result.pos, result.heading, scale, component::Color{},
                                              texture);

                ++mRenderedTexturesPerFrame;
            }
        }

        void
        SceneManager::splat(entities::EntityManager &manager,
                            const component::WorldP3D &worldPos,
                            const component::Scale &scale,
                            const graphic::Brush &brush) {
            std::set<common::EntityID> tileEntities;

            auto entityID = getOrCreateCanvasTile(manager, worldPos);
            tileEntities.insert(entityID);

            auto lowerLeft = worldPos;
            lowerLeft.x -= scale.x / 2.f;
            lowerLeft.y -= scale.y / 2.f;
            auto lowerLeftEntityID = getOrCreateCanvasTile(manager, lowerLeft);
            tileEntities.insert(lowerLeftEntityID);

            auto topRight = worldPos;
            topRight.x += scale.x / 2.f;
            topRight.y += scale.y / 2.f;

            auto topRightEntityID = getOrCreateCanvasTile(manager, topRight);
            tileEntities.insert(topRightEntityID);

            auto topLeft = worldPos;
            topLeft.x -= scale.x / 2.f;
            topLeft.y += scale.y / 2.f;
            auto topLeftEntityID = getOrCreateCanvasTile(manager, topLeft);
            tileEntities.insert(topLeftEntityID);

            auto lowerRight = worldPos;
            lowerRight.x += scale.x / 2.f;
            lowerRight.y -= scale.y / 2.f;
            auto lowerRightEntityID = getOrCreateCanvasTile(manager, lowerRight);
            tileEntities.insert(lowerRightEntityID);

            for (auto &&tileEntity: tileEntities) {
                updateCanvasTile(manager, tileEntity, brush, worldPos, scale);
            }
        }

        common::EntityID
        SceneManager::getOrCreateCanvasTile(entities::EntityManager &manager,
                                            const component::WorldP3D &pos) {
            auto x = math::findBin(pos.x, mCanvasTileSizeX);
            auto y = math::findBin(pos.y, mCanvasTileSizeY);
            auto xy = math::pack_i64(x, y);

            auto missing = mCanvasTileLookup.find(xy) == mCanvasTileLookup.end();
            if (missing) {
                auto tilePosX = math::binPos(x, mCanvasTileSizeX) + mCanvasTileSizeX / 2.f;
                auto tilePosY = math::binPos(y, mCanvasTileSizeY) + mCanvasTileSizeY / 2.f;
                auto tilePosZ = mZLayerManager.getZForEntity(entities::EntityType::CANVAS);

                auto heading = component::Heading{0.f};

                auto id = manager.createEntity_CanvasTile();
                manager.setWorldP3D(id, tilePosX, tilePosY, tilePosZ);
                manager.setScale(id,
                                 static_cast<common::r32>(mCanvasTileSizeX),
                                 static_cast<common::r32>(mCanvasTileSizeY));
                manager.setHeading(id, heading.value);

                auto &texture = manager.get<component::Texture>(id);

                auto widthInPixels = static_cast<common::u16>(mCanvasTileSizeX * mGameUnitToPixels +
                                                              common::EP32);
                auto heightInPixels = static_cast<common::u16>(mCanvasTileSizeY * mGameUnitToPixels +
                                                               common::EP32);

                texture.image.width = widthInPixels;
                texture.image.height = heightInPixels;
                texture.image.path = "generated_by_getOrCreateCanvasTile";

                auto defaultColor = component::Color{};
                mTextureManager->fill(texture.image, defaultColor);
                mTextureManager->loadFromImage(texture);

                mCanvasTileLookup.emplace(xy, id);
            }

            auto entity = mCanvasTileLookup[xy];
            assert(entity);
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
                                      entities::EntityManager &clientManager,
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
                brush.type = component::BrushType::TEXTURE;

                const auto &pos = view.get<component::WorldP3D>(id);
                splat(clientManager, pos, scale, brush);
            }
        }

        void
        SceneManager::updateCanvasTile(entities::EntityManager &entityManager,
                                       common::EntityID entityID,
                                       const graphic::Brush &brush,
                                       const component::WorldP3D &worldPos,
                                       const component::Scale &scale) {
            auto &canvasTexture = entityManager.get<component::Texture>(entityID);
            auto canvasTilePos = entityManager.get<component::WorldP3D>(entityID);
            auto canvasSize = entityManager.get<component::Scale>(entityID);
            auto pos = component::WorldP3D{canvasTilePos.x - canvasSize.x / 2.f,
                                           canvasTilePos.y - canvasSize.y / 2.f,
                                           canvasTilePos.z};

            auto brushTexturePos = worldPos;
            math::worldToTextureCoordinate(pos, mGameUnitToPixels, brushTexturePos);

            component::Image image{};
            switch (brush.type) {
                case component::BrushType::SPRITE: {
                    image.width = static_cast<uint16>(scale.x * mGameUnitToPixels);
                    image.height = static_cast<uint16>(scale.y * mGameUnitToPixels);
                    mTextureManager->fill(image, brush.color);
                    break;
                }
                case component::BrushType::TEXTURE: {
                    // TODO: This will create a copy every time! I don't need a copy a const ref should do the work
                    // as long as down the line I can call sub texture update with the texture data only,
                    // something along the lines of take the updated texture data and point to where the offsets point
                    // TODO: This will ignore brushSize and it will only depend on the image pixel size which is not
                    // at all what I intended
                    image = mTextureManager->loadOrGetImage(brush.tag);
                    break;
                }
                default: {
                    assert(false);
                    return;
                }
            }

            assert(image.width);
            assert(image.height);

            // TODO: Probably better to not use texture coordinates until the very last moment and keep everything
            // in game units at this point.
            auto textureOffsetX = static_cast<common::oniGLint>(brushTexturePos.x - (image.width / 2.f));
            auto textureOffsetY = static_cast<common::oniGLint>(brushTexturePos.y - (image.height / 2.f));

            mTextureManager->blendAndUpdateTexture(canvasTexture,
                                                   textureOffsetX,
                                                   textureOffsetY,
                                                   image);
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

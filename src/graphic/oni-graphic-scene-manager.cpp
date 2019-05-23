#include <oni-core/graphic/oni-graphic-scene-manager.h>

#include <set>

#include <GL/glew.h>

#include <oni-core/graphic/oni-graphic-shader.h>
#include <oni-core/graphic/oni-graphic-batch-renderer-2d.h>
#include <oni-core/graphic/oni-graphic-texture-manager.h>
#include <oni-core/graphic/oni-graphic-font-manager.h>
#include <oni-core/graphic/oni-graphic-debug-draw-box2d.h>
#include <oni-core/physics/oni-physics-dynamics.h>
#include <oni-core/math/oni-math-transformation.h>
#include <oni-core/entities/oni-entities-manager.h>
#include <oni-core/common/oni-common-const.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-gameplay.h>
#include <oni-core/math/oni-math-intersects.h>
#include <oni-core/math/oni-math-rand.h>
#include <oni-core/math/oni-math-z-layer-manager.h>
#include <oni-core/graphic/oni-graphic-brush.h>


namespace oni {
    namespace graphic {
        SceneManager::SceneManager(const graphic::ScreenBounds &screenBounds,
                                   FontManager &fontManager,
                                   math::ZLayerManager &zLayerManager,
                                   b2World &physicsWorld,
                                   common::r32 gameUnitToPixels) :
                mCanvasTileSizeX{110},
                mCanvasTileSizeY{110},
                mHalfCanvasTileSizeX{mCanvasTileSizeX / 2.f},
                mHalfCanvasTileSizeY{mCanvasTileSizeY / 2.f},
                // 64k vertices
                mMaxSpriteCount(64 * 1000), mScreenBounds(screenBounds),
                mFontManager(fontManager),
                mPhysicsWorld(physicsWorld),
                mGameUnitToPixels(gameUnitToPixels),
                mZLayerManager(zLayerManager) {

            mProjectionMatrix = math::mat4::orthographic(screenBounds.xMin, screenBounds.xMax, screenBounds.yMin,
                                                         screenBounds.yMax, -1.0f, 1.0f);
            mViewMatrix = math::mat4::identity();

            mModelMatrix = math::mat4::identity();

            // TODO: Resources are not part of oni-core library! This structure as is not flexible, meaning
            // I am forcing the users to only depend on built-in shaders. I should think of a better way
            // to provide flexibility in type of shaders users can define and expect to just work by having buffer
            // structure and what not set up automatically. Asset system should take care of this.
            // TODO: As it is now, BatchRenderer is coupled with the Shader. It requires the user to setup the
            // shader prior to render series of calls. Maybe shader should be built into the renderer.
            mShader = std::make_unique<graphic::Shader>("resources/shaders/particle.vert",
                                                        "resources/shaders/particle.geom",
                                                        "resources/shaders/particle.frag");
            initRenderer();

            mTextureManager = std::make_unique<TextureManager>();

            mRand = std::make_unique<math::Rand>(0);

            mDebugDrawBox2D = std::make_unique<DebugDrawBox2D>(this);
            mDebugDrawBox2D->AppendFlags(b2Draw::e_shapeBit);
            //mDebugDrawBox2D->AppendFlags(b2Draw::e_aabbBit);
            mDebugDrawBox2D->AppendFlags(b2Draw::e_pairBit);
            mDebugDrawBox2D->AppendFlags(b2Draw::e_centerOfMassBit);
        }

        SceneManager::~SceneManager() = default;

        void
        SceneManager::initRenderer() {
            auto program = mShader->getProgram();

            auto positionIndex = glGetAttribLocation(program, "position");
            auto headingIndex = glGetAttribLocation(program, "heading");
            auto halfSizeIndex = glGetAttribLocation(program, "halfSize");
            auto colorIndex = glGetAttribLocation(program, "color");
            auto uvIndex = glGetAttribLocation(program, "uv");
            auto samplerIDIndex = glGetAttribLocation(program, "samplerID");

            if (positionIndex == -1 || headingIndex == -1 || uvIndex == -1 || colorIndex == -1 ||
                samplerIDIndex == -1 || halfSizeIndex == -1) {
                assert(false);
            }

            common::oniGLsizei stride = sizeof(graphic::Vertex);

            graphic::BufferStructure sampler;
            sampler.index = static_cast<common::oniGLuint>(samplerIDIndex);
            sampler.componentCount = 1;
            sampler.componentType = GL_FLOAT;
            sampler.normalized = GL_FALSE;
            sampler.stride = stride;
            sampler.offset = static_cast<const common::oniGLvoid *>(nullptr);

            graphic::BufferStructure halfSize;
            halfSize.index = static_cast<common::oniGLuint>(halfSizeIndex);
            halfSize.componentCount = 2;
            halfSize.componentType = GL_FLOAT;
            halfSize.normalized = GL_FALSE;
            halfSize.stride = stride;
            halfSize.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::Vertex, halfSize));

            graphic::BufferStructure heading;
            heading.index = static_cast<common::oniGLuint>(headingIndex);
            heading.componentCount = 1;
            heading.componentType = GL_FLOAT;
            heading.normalized = GL_FALSE;
            heading.stride = stride;
            heading.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::Vertex, heading));

            graphic::BufferStructure position;
            position.index = static_cast<common::oniGLuint>(positionIndex);
            position.componentCount = 3;
            position.componentType = GL_FLOAT;
            position.normalized = GL_FALSE;
            position.stride = stride;
            position.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::Vertex, position));

            graphic::BufferStructure color;
            color.index = static_cast<common::oniGLuint>(colorIndex);
            color.componentCount = 4;
            color.componentType = GL_FLOAT;
            color.normalized = GL_TRUE;
            color.stride = stride;
            color.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::Vertex, color));

            graphic::BufferStructure uv;
            uv.index = static_cast<common::oniGLuint>(uvIndex);
            uv.componentCount = 2;
            uv.componentType = GL_FLOAT;
            uv.normalized = GL_TRUE;
            uv.stride = stride;
            uv.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(graphic::Vertex, uv));

            std::vector<graphic::BufferStructure> bufferStructures;
            bufferStructures.push_back(sampler);
            bufferStructures.push_back(halfSize);
            bufferStructures.push_back(heading);
            bufferStructures.push_back(position);
            bufferStructures.push_back(color);
            bufferStructures.push_back(uv);

            mRenderer = std::make_unique<BatchRenderer2D>(
                    mMaxSpriteCount,
                    // TODO: If there are more than this number of textures to render in a draw call, it will fail
                    common::maxNumTextureSamplers,
                    sizeof(graphic::Vertex),
                    bufferStructures,
                    PrimitiveType::POINT
            );

            mShader->enable();
            mShader->setUniformiv("samplers", mRenderer->generateSamplerIDs());
            mShader->disable();
        }

        void
        SceneManager::begin(const Shader &shader,
                            Renderer2D &renderer2D,
                            bool translate,
                            bool scale,
                            bool setMVP) {
            shader.enable();

            auto view = math::mat4::identity();
            if (scale) {
                view = view * math::mat4::scale(math::vec3{mCamera.z, mCamera.z, 1.0f});
            }

            if (translate) {
                view = view * math::mat4::translation(-mCamera.x, -mCamera.y, 0.0f);
            }
            if (setMVP) {
                auto mv = mProjectionMatrix * view;
                shader.setUniformMat4("mv", mv);
            }
            renderer2D.begin();
        }

        void
        SceneManager::prepareTexture(component::Texture &texture) {
            // TODO: With current network registry sync code every time an entity gets component sync this texture
            // status is reverted back to what it was on server, which might be okay, but something to keep in mind
            // if it becomes a perf bottle-neck.
            switch (texture.status) {
                case component::TextureStatus::INVALID : {
                    assert(false);
                }
                case component::TextureStatus::READY: {
                    break;
                }
                case component::TextureStatus::NEEDS_LOADING_USING_PATH: {
                    const auto &loadedTexture = mTextureManager->loadOrGetTexture(texture.path.c_str());
                    texture = loadedTexture;
                    break;
                }
                case component::TextureStatus::NEEDS_LOADING_USING_DATA: {
                    assert(texture.image.width);
                    assert(texture.image.height);

                    mTextureManager->loadFromImage(texture);
                    break;
                }
                case component::TextureStatus::NEEDS_RELOADING_USING_PATH: {
                    break;
                }
                case component::TextureStatus::NEEDS_RELOADING_USING_DATA: {
                    break;
                }
                default: {
                    assert(false);
                    break;
                }
            }
        }

        void
        SceneManager::end(const Shader &shader,
                          Renderer2D &renderer2D) {
            renderer2D.end();
            renderer2D.flush();
            shader.disable();
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

            {
                begin(*mShader, *mRenderer, true, true, true);
                _render(manager, viewWidth, viewHeight);
                end(*mShader, *mRenderer);
            }

            /// UI
            {
                // Render UI text with fixed camera
                //begin(*mTextureShader, *mTextureRenderer, false, false, true);
                // TODO: This should actually be split up from static text and entities part of UI should be tagged so
                // renderStaticText(entityManager, viewWidth, viewHeight);
                //end(*mTextureShader, *mTextureRenderer);
            }
        }

        void
        SceneManager::renderRaw(const component::WorldP3D pos,
                                const component::Appearance &appearance) {
            //mColorRenderer->submit(pos, appearance);
            ++mRenderedSpritesPerFrame;
        }

        void
        SceneManager::beginColorRendering() {
            begin(*mShader, *mRenderer, true, false, true);
        }

        void
        SceneManager::endColorRendering() {
            end(*mShader, *mRenderer);
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
        SceneManager::_render(entities::EntityManager &manager,
                              common::r32 viewWidth,
                              common::r32 viewHeight) {
            /// Color shading
            {
                auto view = manager.createView<
                        component::WorldP3D,
                        component::Heading,
                        component::Scale,
                        component::Appearance,
                        component::Tag_ColorShaded
                                              >();

                for (auto &&id: view) {
                    const auto &pos = view.get<component::WorldP3D>(id);
                    const auto &heading = view.get<component::Heading>(id);
                    const auto &scale = view.get<component::Scale>(id);

                    auto result = applyParentTransforms(manager, id, pos, heading);

                    if (!math::intersects(result.pos, scale, mCamera.x, mCamera.y, viewWidth, viewHeight)) {
                        continue;
                    }

                    const auto &appearance = view.get<component::Appearance>(id);

                    mRenderer->submit(result.pos, result.heading, scale, appearance, component::Texture{});

                    ++mRenderedSpritesPerFrame;
                }
            }

            /// Texture shading
            {
                auto view = manager.createView<
                        component::WorldP3D,
                        component::Heading,
                        component::Scale,
                        component::Texture,
                        component::Tag_TextureShaded
                                              >();
                for (auto &&id: view) {
                    const auto &pos = view.get<component::WorldP3D>(id);
                    const auto &heading = view.get<component::Heading>(id);
                    const auto &scale = view.get<component::Scale>(id);

                    auto result = applyParentTransforms(manager, id, pos, heading);

                    if (!math::intersects(result.pos, scale, mCamera.x, mCamera.y, viewWidth, viewHeight)) {
                        continue;
                    }

                    auto &texture = view.get<component::Texture>(id);
                    prepareTexture(texture);

                    mRenderer->submit(result.pos, result.heading, scale, component::Appearance{}, texture);

                    ++mRenderedTexturesPerFrame;
                }
            }
        }

        void
        SceneManager::tick(const entities::EntityManager &serverManager,
                           entities::EntityManager &clientManager,
                           common::r64 tickTime) {
            auto viewWidth = getViewWidth();
            auto viewHeight = getViewHeight();

/*
            math::vec4 red{1, 0, 0, 0.25};
            math::vec4 green{0, 1, 0, 0.25};
            math::vec4 blue{0, 0, 1, 1};

            math::vec3 redPos{0, 0, 1};
            math::vec3 greenPos{5, 5, 0.1f};
            math::vec3 bluePos{0, 5, 0.0f};

            static bool add = true;
            if (add) {
                math::vec2 size{10, 10};
                common::r32 heading = 0;
                clientEntityFactory.createEntity<entities::EntityType::SIMPLE_SPRITE>(redPos, size, heading, red);

                clientEntityFactory.createEntity<entities::EntityType::SIMPLE_SPRITE>(greenPos, size, heading, green);

                clientEntityFactory.createEntity<entities::EntityType::SIMPLE_SPRITE>(bluePos, size, heading, blue);
                add = false;
            }
*/

            /// Update Emitters
            {
                auto view = clientManager.createView<component::SmokeEmitterCD>();
                for (auto &&id: view) {
                    auto &emitter = view.get<component::SmokeEmitterCD>(id);
                    math::subAndZeroClip(emitter.currentCD, tickTime);
                }
            }

            /// Particle trails
            {
#if 1
                auto texturePath = std::string("resources/images/smoke/1.png");
                common::r32 particleHalfSize = 0.35f;
                common::r32 particleSize = particleHalfSize * 2;
                common::r32 halfConeAngle = static_cast<common::r32>(math::toRadians(45)) / 2.f;

                auto view = serverManager.createView<
                        component::Trail,
                        component::WorldP3D,
                        component::Heading>();
                for (auto &&entity: view) {
                    const auto &worldPos = view.get<component::WorldP3D>(entity);
                    const auto &heading = view.get<component::Heading>(entity);
                    const auto &currentPos = worldPos;
                    const auto &trail = view.get<component::Trail>(entity);
                    common::r32 projectileHeading = heading.value;
                    common::r32 spawnMinAngle = projectileHeading + common::PI - halfConeAngle;
                    common::r32 spawnMaxAngle = projectileHeading + common::PI + halfConeAngle;

                    assert(trail.previousPos.size() == trail.velocity.size());

                    //math::vec4 color{1.f, 1.f, 1.f, 1.f};

                    if (trail.previousPos.empty()) {
                        auto trailEntity = clientManager.createEntity_SimpleParticle();
                        clientManager.setWorldP3D(trailEntity, currentPos.x, currentPos.y, currentPos.z);
                        clientManager.setTexture(trailEntity, texturePath);
                        clientManager.setScale(trailEntity, particleSize, particleSize);
                        continue;
                    }

                    const auto &previousPos = trail.previousPos[0];

                    if (!math::intersects(currentPos, mCamera.x, mCamera.y, viewWidth, viewHeight) &&
                        !math::intersects(previousPos, mCamera.x, mCamera.y, viewWidth, viewHeight)) {
                        continue;
                    }

                    common::r32 dX = currentPos.x - previousPos.x;
                    common::r32 dY = currentPos.y - previousPos.y;

                    common::r32 distance = std::sqrt(dX * dX + dY * dY);

                    auto x = previousPos.x;
                    auto y = previousPos.y;

                    auto alpha = std::atan2(dX, dY); // Between line crossing previousPos and currentPos and X-axis

                    common::EntityID trailEntity;
                    for (auto numParticles = 0; numParticles < mRand->next_u8(1, 2); ++numParticles) {
                        auto pos = component::WorldP3D{x, y, currentPos.z};
                        for (common::r32 i = 0.f; i <= distance; i += particleSize) {
/*                        if(i == 0){
                            trailEntity = mInternalEntityFactory->createEntity<entities::EntityType::SIMPLE_PARTICLE>(
                                    pos, math::vec4{0.f, 1.f, 0.f, 1.f}, false);
                        }
                        else if (i >= distance ){
                            trailEntity = mInternalEntityFactory->createEntity<entities::EntityType::SIMPLE_PARTICLE>(
                                    pos, math::vec4{1.f, 0.f, 0.f, 1.f}, false);
                        }
                        else{
                            trailEntity = mInternalEntityFactory->createEntity<entities::EntityType::SIMPLE_PARTICLE>(
                                    pos, color, false);
                        }
                        */
                            auto trailEntity = clientManager.createEntity_SimpleParticle();
                            clientManager.setWorldP3D(trailEntity, currentPos.x, currentPos.y, currentPos.z);
                            clientManager.setTexture(trailEntity, texturePath);
                            clientManager.setScale(trailEntity, particleSize, particleSize);

                            auto &age = clientManager.get<component::Age>(
                                    trailEntity);
                            // TODO: I don't use any other velocity than the first one, should I just not store the rest?
                            // or should I update this code to use all by iterating over trail.previousPos?
                            age.maxAge = 1.f - (distance - i) / trail.velocity[0];

                            auto &velocity = clientManager.get<component::Velocity>(
                                    trailEntity);
                            velocity.currentVelocity = mRand->next_r32(2.f, 10.f);

                            auto &particleHeading = clientManager.get<component::Heading>(
                                    trailEntity);
                            particleHeading.value = mRand->next_r32(spawnMinAngle, spawnMaxAngle);

                            pos.x += particleSize * std::sin(alpha);
                            pos.y += particleSize * std::cos(alpha);
                        }
                    }

//                    math::vec4 colorBlue{0.f, 0.f, 1.f, 1.f};
//                    math::vec4 colorRed{1.f, 0.f, 0.f, 1.f};
                }
#endif
            }

            /// Entities that leave mark
            {
                auto view = clientManager.createView<
                        component::Tag_LeavesMark,
                        component::Texture,
                        component::WorldP3D,
                        component::Scale>();
                for (auto &&entity: view) {
                    const auto &texture = view.get<component::Texture>(entity);
                    const auto &scale = view.get<component::Scale>(entity);
                    auto brush = graphic::Brush{};
                    // TODO: This is messy distinction between texture path and textureID!
                    brush.textureID = texture.path.c_str();
                    brush.type = component::BrushType::TEXTURE;

                    const auto &pos = view.get<component::WorldP3D>(entity);
                    splat(clientManager, pos, scale, brush);
                }
            }

            /// Update Skid lines.
            {
                std::vector<component::WorldP3D> skidPosList{};
                std::vector<common::u8> skidOpacity{};
                {
                    auto view = serverManager.createView<
                            component::Car,
                            component::WorldP3D,
                            component::Heading,
                            component::Scale,
                            component::CarConfig>();
                    for (auto &&carEntity: view) {

                        const auto car = view.get<component::Car>(carEntity);
                        const auto &pos = view.get<component::WorldP3D>(carEntity);
                        // NOTE: Technically I should use slippingRear, but this gives better effect
                        if (car.slippingFront || true) {
                            const auto &carConfig = view.get<component::CarConfig>(carEntity);
                            const auto &heading = view.get<component::Heading>(carEntity);
                            const auto scale = component::Scale{}; // TODO: This is unused, think about how the parent scale
                            // affects children

                            // TODO: This is game logic, maybe tire placement should be saved as part of CarConfig?
                            // same logic is hard-coded when spawningCar server side.
                            math::vec3 skidPosRL{static_cast<common::r32>(-carConfig.cgToRearAxle),
                                                 static_cast<common::r32>(carConfig.wheelWidth +
                                                                          carConfig.halfWidth / 2),
                                    // NOTE: This z-value is unused.
                                                 0.f};
                            math::vec3 skidPosRR{static_cast<common::r32>(-carConfig.cgToRearAxle),
                                                 static_cast<common::r32>(-carConfig.wheelWidth -
                                                                          carConfig.halfWidth / 2),
                                                 0.f};
                            auto transform = math::createTransformation(pos, heading, scale);
                            auto posRL = transform * skidPosRL;
                            auto posRR = transform * skidPosRR;
                            skidPosList.push_back(component::WorldP3D{posRL.x, posRL.y, posRL.z});
                            skidPosList.push_back(component::WorldP3D{posRR.x, posRR.y, posRR.z});

//                            auto alpha = static_cast<common::u8>((car.velocityAbsolute / car.maxVelocityAbsolute) *
//                                                                    255);
                            // TODO: arbitrary number based on number of frames, think about better way of determining this
                            skidOpacity.push_back(10);
                        }
                        if (car.slippingFront && math::abs(car.slipAngleFront) > 1.f or true) {
                            auto z = mZLayerManager.getZForEntity(entities::EntityType::SMOKE);
                            auto &emitter = getOrCreateEmitterCD(clientManager, carEntity);
                            if (math::safeZero(emitter.currentCD)) {
                                for (common::i32 i = 0; i < mRand->next_i32(2, 3); ++i) {
                                    auto id = clientManager.createEntity_SmokeCloud();
                                    clientManager.setWorldP3D(id, pos.x, pos.y, z);
                                    clientManager.setTexture(id, "resources/images/cloud/1.png");
                                    clientManager.setRandAge(id, 1, 3);
                                    clientManager.setRandHeading(id);
                                    clientManager.setRandVelocity(id, 1, 2);
                                    clientManager.setScale(id, 10, 10);
                                }
                                emitter.currentCD = emitter.initialCD;
                            }
                        }
                    }
                }

                {
                    auto brush = graphic::Brush{};
                    brush.type = component::BrushType::SPRITE;
                    auto scale = component::Scale{0.5f, 0.5f};

                    for (size_t i = 0; i < skidPosList.size(); ++i) {
                        brush.color = component::PixelRGBA{0, 0, 0, skidOpacity[i / 2]};
                        splat(clientManager, skidPosList[i], scale, brush);
                    }
                }
            }

            /// Update Laps
            {
                auto carLapView = serverManager.createView<component::Car, gameplay::CarLapInfo>();
                for (auto &&carEntity: carLapView) {
                    // TODO: This will render all player laps on top of each other. I should render the data in rows
                    // instead. Something like:
                    /**
                     * Player Name: lap, lap time, best time
                     *
                     * Player 1: 4, 1:12, :1:50
                     * Player 2: 5, 0:02, :1:50
                     */
                    const auto &carLap = carLapView.get<gameplay::CarLapInfo>(carEntity);

                    const auto &carLapText = getOrCreateLapText(clientManager, carEntity, carLap);
                    updateRaceInfo(clientManager, carLap, carLapText);
                }
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

                // TODO: Should this be a canvas type?
                auto id = manager.createEntity_SimpleSpriteTextured();
                manager.setWorldP3D(id, tilePosX, tilePosY, tilePosZ);
                manager.setScale(id,
                                       static_cast<common::r32>(mCanvasTileSizeX),
                                       static_cast<common::r32>(mCanvasTileSizeY));
                manager.setHeading(id, heading.value);

                auto &texture = manager.get<component::Texture>(id);

                auto widthInPixels = static_cast<common::u16>(mCanvasTileSizeX * mGameUnitToPixels +
                                                              common::EP);
                auto heightInPixels = static_cast<common::u16>(mCanvasTileSizeY * mGameUnitToPixels +
                                                               common::EP);

                texture.image.width = widthInPixels;
                texture.image.height = heightInPixels;

                auto defaultColor = component::PixelRGBA{};
                mTextureManager->fill(texture.image, defaultColor);
                mTextureManager->loadFromImage(texture);

                mCanvasTileLookup.emplace(xy, id);
            }

            auto entity = mCanvasTileLookup.at(xy);
            assert(entity);
            return entity;
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
            auto pos = component::WorldP3D{canvasTilePos.x - canvasSize.x / 2.f, canvasTilePos.y - canvasSize.y / 2.f,
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
                    image = mTextureManager->loadOrGetImage(brush.textureID);
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

        const SceneManager::RaceInfoEntities &
        SceneManager::getOrCreateLapText(entities::EntityManager &manager,
                                         common::EntityID carEntityID,
                                         const gameplay::CarLapInfo &carLap) {
            auto exists = mLapInfoLookup.find(carEntityID) != mLapInfoLookup.end();
            if (!exists) {
                auto zLevel = mZLayerManager.getZForEntity(entities::EntityType::UI);
                RaceInfoEntities carLapText{0};
                auto lapRenderPos = component::WorldP3D{mScreenBounds.xMax - 3.5f, mScreenBounds.yMax - 0.5f, zLevel};
                auto lapTimeRenderPos = component::WorldP3D{mScreenBounds.xMax - 3.5f, mScreenBounds.yMax - 1.0f,
                                                            zLevel};
                auto bestTimeRenderPos = component::WorldP3D{mScreenBounds.xMax - 3.5f, mScreenBounds.yMax - 1.5f,
                                                             zLevel};

                carLapText.lapEntity = createText(manager, lapRenderPos, "Lap: " + std::to_string(carLap.lap));
                carLapText.lapTimeEntity = createText(manager, lapTimeRenderPos,
                                                      "Lap time: " + std::to_string(carLap.lapTimeS));
                carLapText.lapBestTimeEntity = createText(manager, bestTimeRenderPos,
                                                          "Best time: " + std::to_string(carLap.bestLapTimeS));
                mLapInfoLookup[carEntityID] = carLapText;
            }
            return mLapInfoLookup.at(carEntityID);
        }

        component::SmokeEmitterCD &
        SceneManager::getOrCreateEmitterCD(entities::EntityManager &manager,
                                           common::EntityID id) {
            if (manager.hasComplement(id)) {
                auto complementID = manager.getComplementOf(id);
                auto &emitter = manager.get<component::SmokeEmitterCD>(complementID);
                return emitter;
            } else {
                // TODO: This is a divergance from the way entities are created, not sure if I want to expose createComponent
                auto complementID = manager.createComplementTo(id);
                auto &emitter = manager.createComponent<component::SmokeEmitterCD>(complementID);
                return emitter;
            }
        }

        void
        SceneManager::updateRaceInfo(entities::EntityManager &entityManager,
                                     const gameplay::CarLapInfo &carLap,
                                     const SceneManager::RaceInfoEntities &carLapTextEntities) {
            // TODO: This is updated every tick, which is unnecessary. Lap information is rarely updated.
            auto &lapText = entityManager.get<component::Text>(carLapTextEntities.lapEntity);
            mFontManager.updateText("Lap: " + std::to_string(carLap.lap), lapText);

            auto &lapTimeText = entityManager.get<component::Text>(carLapTextEntities.lapTimeEntity);
            mFontManager.updateText("Lap time: " + std::to_string(carLap.lapTimeS) + "s", lapTimeText);

            auto &bestTimeText = entityManager.get<component::Text>(carLapTextEntities.lapBestTimeEntity);
            mFontManager.updateText("Best time: " + std::to_string(carLap.bestLapTimeS) + "s", bestTimeText);
        }

        const graphic::Camera &
        SceneManager::getCamera() const {
            return mCamera;
        }

        void
        SceneManager::zoom(common::r32 distance) {
            mCamera.z = 1 / distance;
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

        const math::mat4 &
        SceneManager::getProjectionMatrix() const {
            return mProjectionMatrix;
        }

        const math::mat4 &
        SceneManager::getViewMatrix() const {
            return mViewMatrix;
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

        void
        SceneManager::resetCounters() {
            mRenderedSpritesPerFrame = 0;
            mRenderedParticlesPerFrame = 0;
            mRenderedTexturesPerFrame = 0;
        }

        common::EntityID
        SceneManager::createText(entities::EntityManager &manager,
                                 const component::WorldP3D &worldPos,
                                 const std::string &text) {
            auto entityID = mFontManager.createTextFromString(manager, text, worldPos);
            return entityID;
        }

        SceneManager::WorldP3DAndHeading
        SceneManager::applyParentTransforms(entities::EntityManager &manager,
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

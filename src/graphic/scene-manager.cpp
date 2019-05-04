#include <oni-core/graphic/scene-manager.h>

#include <set>

#include <GL/glew.h>

#include <oni-core/graphic/shader.h>
#include <oni-core/graphic/batch-renderer-2d.h>
#include <oni-core/graphic/texture-manager.h>
#include <oni-core/graphic/font-manager.h>
#include <oni-core/graphic/debug-draw-box2d.h>
#include <oni-core/physics/dynamics.h>
#include <oni-core/math/transformation.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/entities/entity-factory.h>
#include <oni-core/common/consts.h>
#include <oni-core/component/geometry.h>
#include <oni-core/component/hierarchy.h>
#include <oni-core/component/gameplay.h>
#include <oni-core/math/intesects.h>
#include <oni-core/math/rand.h>
#include <oni-core/math/z-layer-manager.h>


namespace oni {
    namespace graphic {
        SceneManager::SceneManager(const component::ScreenBounds &screenBounds,
                                   FontManager &fontManager,
                                   math::ZLayerManager &zLayerManager,
                                   b2World &physicsWorld,
                                   common::real32 gameUnitToPixels) :
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
            mTextureShader = std::make_unique<graphic::Shader>("resources/shaders/texture.vert",
                                                               "",
                                                               "resources/shaders/texture.frag");
            initializeTextureRenderer();
            // TODO: As it is now, BatchRenderer is coupled with the Shader. It requires the user to setup the
            // shader prior to render series of calls. Maybe shader should be built into the renderer.

            mColorShader = std::make_unique<graphic::Shader>("resources/shaders/basic.vert",
                                                             "",
                                                             "resources/shaders/basic.frag");
            initializeColorRenderer();

            mParticleShader = std::make_unique<graphic::Shader>("resources/shaders/particle.vert",
                                                                "resources/shaders/particle.geom",
                                                                "resources/shaders/particle.frag");
            initializeParticleRenderer();

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
        SceneManager::initializeParticleRenderer() {
            auto program = mParticleShader->getProgram();

            auto positionIndex = glGetAttribLocation(program, "position");
            auto colorIndex = glGetAttribLocation(program, "color");
            auto ageIndex = glGetAttribLocation(program, "age");
            auto velocityIndex = glGetAttribLocation(program, "velocity");
            auto headingIndex = glGetAttribLocation(program, "heading");
            auto samplerIDIndex = glGetAttribLocation(program, "samplerID");
            auto halfSizeIndex = glGetAttribLocation(program, "halfSize");

            if (positionIndex == -1 || colorIndex == -1 || ageIndex == -1 || velocityIndex == -1 ||
                headingIndex == -1 || samplerIDIndex == -1 || halfSizeIndex == -1) {
                assert(false);
            }

            common::oniGLsizei stride = sizeof(component::ParticleVertex);

            component::BufferStructure position;
            position.index = static_cast<common::oniGLuint>(positionIndex);
            position.componentCount = 3;
            position.componentType = GL_FLOAT;
            position.normalized = GL_FALSE;
            position.stride = stride;
            position.offset = static_cast<const common::oniGLvoid *>(nullptr);

            component::BufferStructure color;
            color.index = static_cast<common::oniGLuint>(colorIndex);
            color.componentCount = 4;
            color.componentType = GL_FLOAT;
            color.normalized = GL_TRUE;
            color.stride = stride;
            color.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(
                    component::ParticleVertex,
                    color));

            component::BufferStructure age;
            age.index = static_cast<common::oniGLuint>(ageIndex);
            age.componentCount = 1;
            age.componentType = GL_FLOAT;
            age.normalized = GL_FALSE;
            age.stride = stride;
            age.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(
                    component::ParticleVertex,
                    age));

            component::BufferStructure heading;
            heading.index = static_cast<common::oniGLuint>(headingIndex);
            heading.componentCount = 1;
            heading.componentType = GL_FLOAT;
            heading.normalized = GL_FALSE;
            heading.stride = stride;
            heading.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(
                    component::ParticleVertex,
                    heading));

            component::BufferStructure velocity;
            velocity.index = static_cast<common::oniGLuint>(velocityIndex);
            velocity.componentCount = 1;
            velocity.componentType = GL_FLOAT;
            velocity.normalized = GL_FALSE;
            velocity.stride = stride;
            velocity.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(
                    component::ParticleVertex,
                    velocity));

            component::BufferStructure sampler;
            sampler.index = static_cast<common::oniGLuint>(samplerIDIndex);
            sampler.componentCount = 1;
            sampler.componentType = GL_FLOAT;
            sampler.normalized = GL_FALSE;
            sampler.stride = stride;
            sampler.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(
                    component::ParticleVertex,
                    samplerID));

            component::BufferStructure halfSize;
            halfSize.index = static_cast<common::oniGLuint>(halfSizeIndex);
            halfSize.componentCount = 1;
            halfSize.componentType = GL_FLOAT;
            halfSize.normalized = GL_FALSE;
            halfSize.stride = stride;
            halfSize.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(
                    component::ParticleVertex,
                    halfSize));

            std::vector<component::BufferStructure> bufferStructures;
            bufferStructures.push_back(position);
            bufferStructures.push_back(color);
            bufferStructures.push_back(age);
            bufferStructures.push_back(heading);
            bufferStructures.push_back(velocity);
            bufferStructures.push_back(sampler);
            bufferStructures.push_back(halfSize);

            mParticleRenderer = std::make_unique<BatchRenderer2D>(
                    mMaxSpriteCount,
                    // TODO: If there are more than this number of textures to render in a draw call, it will fail
                    common::maxNumTextureSamplers,
                    sizeof(component::ParticleVertex),
                    bufferStructures,
                    PrimitiveType::POINT
            );
        }

        void
        SceneManager::initializeColorRenderer() {
            auto program = mColorShader->getProgram();

            auto positionIndex = glGetAttribLocation(program, "position");
            auto colorIndex = glGetAttribLocation(program, "color");

            if (positionIndex == -1 || colorIndex == -1) {
                throw std::runtime_error("Invalid attribute name.");
            }

            common::oniGLsizei stride = sizeof(component::ColoredVertex);

            component::BufferStructure position;
            position.index = static_cast<common::oniGLuint>(positionIndex);
            position.componentCount = 3;
            position.componentType = GL_FLOAT;
            position.normalized = GL_FALSE;
            position.stride = stride;
            position.offset = static_cast<const common::oniGLvoid *>(nullptr);

            component::BufferStructure color;
            color.index = static_cast<common::oniGLuint>(colorIndex);
            color.componentCount = 4;
            color.componentType = GL_FLOAT;
            color.normalized = GL_TRUE;
            color.stride = stride;
            color.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(
                    component::ColoredVertex,
                    color));

            std::vector<component::BufferStructure> bufferStructures;
            bufferStructures.push_back(position);
            bufferStructures.push_back(color);

            mColorRenderer = std::make_unique<BatchRenderer2D>(
                    mMaxSpriteCount,
                    common::maxNumTextureSamplers,
                    sizeof(component::ColoredVertex),
                    bufferStructures,
                    PrimitiveType::TRIANGLE
            );
        }

        void
        SceneManager::initializeTextureRenderer() {
            auto program = mTextureShader->getProgram();

            auto positionIndex = glGetAttribLocation(program, "position");
            auto samplerIDIndex = glGetAttribLocation(program, "samplerID");
            auto uvIndex = glGetAttribLocation(program, "uv");

            if (positionIndex == -1 || samplerIDIndex == -1 || uvIndex == -1) {
                throw std::runtime_error("Invalid attribute name.");
            }

            common::oniGLsizei stride = sizeof(component::TexturedVertex);

            component::BufferStructure position;
            position.index = static_cast<common::oniGLuint>(positionIndex);
            position.componentCount = 3;
            position.componentType = GL_FLOAT;
            position.normalized = GL_FALSE;
            position.stride = stride;
            position.offset = static_cast<const common::oniGLvoid *>(nullptr);

            component::BufferStructure sampler;
            sampler.index = static_cast<common::oniGLuint>(samplerIDIndex);
            sampler.componentCount = 1;
            sampler.componentType = GL_FLOAT;
            sampler.normalized = GL_FALSE;
            sampler.stride = stride;
            sampler.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(
                    component::TexturedVertex,
                    samplerID));

            component::BufferStructure uv;
            uv.index = static_cast<common::oniGLuint>(uvIndex);
            uv.componentCount = 2;
            uv.componentType = GL_FLOAT;
            uv.normalized = GL_FALSE;
            uv.stride = stride;
            uv.offset = reinterpret_cast<const common::oniGLvoid *>(offsetof(
                    component::TexturedVertex,
                    uv));

            std::vector<component::BufferStructure> bufferStructure;
            bufferStructure.push_back(position);
            bufferStructure.push_back(sampler);
            bufferStructure.push_back(uv);

            mTextureRenderer = std::make_unique<BatchRenderer2D>(
                    mMaxSpriteCount,
                    // TODO: If there are more than this number of textures to render in a draw call, it will fail
                    common::maxNumTextureSamplers,
                    sizeof(component::TexturedVertex),
                    bufferStructure,
                    PrimitiveType::TRIANGLE);

            mTextureShader->enable();
            mTextureShader->setUniformiv("samplers", mTextureRenderer->generateSamplerIDs());
            mTextureShader->disable();
        }

        void
        SceneManager::tick(entities::EntityFactory &serverEntityFactory,
                           entities::EntityFactory &clientEntityFactory,
                           common::real64 tickTime) {
            updateParticles(clientEntityFactory, tickTime);

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
                common::real32 heading = 0;
                clientEntityFactory.createEntity<component::EntityType::SIMPLE_SPRITE>(redPos, size, heading, red);

                clientEntityFactory.createEntity<component::EntityType::SIMPLE_SPRITE>(greenPos, size, heading, green);

                clientEntityFactory.createEntity<component::EntityType::SIMPLE_SPRITE>(bluePos, size, heading, blue);
                add = false;
            }
*/

            // trails
            {
                std::string textureID = "resources/images/smoke/1.png";
                common::real32 particleHalfSize = 0.35f;
                common::real32 particleSize = particleHalfSize * 2;
                common::real32 halfConeAngle = static_cast<common::real32>(math::toRadians(45)) / 2.f;

                auto view = serverEntityFactory.getEntityManager().createView<component::Trail, component::Placement>();
                for (auto &&entity: view) {
                    const auto &placement = view.get<component::Placement>(entity);
                    const auto &currentPos = placement.position;
                    const auto &trail = view.get<component::Trail>(entity);
                    common::real32 projectileHeading = placement.rotation;
                    common::real32 spawnMinAngle = projectileHeading + common::PI - halfConeAngle;
                    common::real32 spawnMaxAngle = projectileHeading + common::PI + halfConeAngle;

                    assert(trail.previousPos.size() == trail.velocity.size());

                    //math::vec4 color{1.f, 1.f, 1.f, 1.f};

                    if (trail.previousPos.empty()) {
                        auto trailEntity = clientEntityFactory.createEntity<component::EntityType::SIMPLE_PARTICLE>(
                                currentPos, textureID, particleHalfSize, false);
                        continue;
                    }

                    const auto &previousPos = trail.previousPos[0];

                    if (!math::intersects(currentPos, mCamera.x, mCamera.y, viewWidth, viewHeight) &&
                        !math::intersects(previousPos, mCamera.x, mCamera.y, viewWidth, viewHeight)) {
                        continue;
                    }

                    common::real32 dX = currentPos.x - previousPos.x;
                    common::real32 dY = currentPos.y - previousPos.y;

                    common::real32 distance = std::sqrt(dX * dX + dY * dY);

                    auto x = previousPos.x;
                    auto y = previousPos.y;

                    auto alpha = std::atan2(dX, dY); // Between line crossing previousPos and currentPos and X-axis

                    common::EntityID trailEntity;
                    for (auto numParticles = 0; numParticles < mRand->nextUint8(1, 2); ++numParticles) {
                        math::vec3 pos{x, y, currentPos.z};
                        for (common::real32 i = 0.f; i <= distance; i += particleSize) {
/*                        if(i == 0){
                            trailEntity = mInternalEntityFactory->createEntity<component::EntityType::SIMPLE_PARTICLE>(
                                    pos, math::vec4{0.f, 1.f, 0.f, 1.f}, false);
                        }
                        else if (i >= distance ){
                            trailEntity = mInternalEntityFactory->createEntity<component::EntityType::SIMPLE_PARTICLE>(
                                    pos, math::vec4{1.f, 0.f, 0.f, 1.f}, false);
                        }
                        else{
                            trailEntity = mInternalEntityFactory->createEntity<component::EntityType::SIMPLE_PARTICLE>(
                                    pos, color, false);
                        }
                        */
                            trailEntity = clientEntityFactory.createEntity<component::EntityType::SIMPLE_PARTICLE>(
                                    pos, textureID, particleHalfSize, false);
                            auto &age = clientEntityFactory.getEntityManager().get<component::Age>(
                                    trailEntity);
                            // TODO: I don't use any other velocity than the first one, should I just not store the rest?
                            // or should I update this code to use all by iterating over trail.previousPos?
                            age.maxAge = 1.f - (distance - i) / trail.velocity[0];

                            auto &velocity = clientEntityFactory.getEntityManager().get<component::Velocity>(
                                    trailEntity);
                            velocity.currentVelocity = mRand->nextReal32(2.f, 10.f);

                            auto &particlePlacement = clientEntityFactory.getEntityManager().get<component::Placement>(
                                    trailEntity);
                            particlePlacement.rotation = mRand->nextReal32(spawnMinAngle, spawnMaxAngle);

                            pos.x += particleSize * std::sin(alpha);
                            pos.y += particleSize * std::cos(alpha);
                        }
                    }

//                    math::vec4 colorBlue{0.f, 0.f, 1.f, 1.f};
//                    math::vec4 colorRed{1.f, 0.f, 0.f, 1.f};
                }
            }

            // Entities that leave mark
            {
                auto view = clientEntityFactory.getEntityManager().createView<component::Texture, component::Tag_LeavesMark, component::Placement>();
                for (auto &&entity: view) {
                    const auto &texture = view.get<component::Texture>(entity);
                    auto brush = component::Brush{};
                    // TODO: This is messy distinction between texture path and textureID!
                    brush.textureID = texture.filePath.c_str();
                    brush.type = component::BrushType::CUSTOM_TEXTURE;
                    brush.size = math::vec2{1, 1};

                    const auto &pos = view.get<component::Placement>(entity).position.getXY();
                    splat(clientEntityFactory, brush, pos);
                }
            }

            // Update Skid lines.
            {
                std::vector<math::vec2> skidPosList{};
                std::vector<common::uint8> skidOpacity{};
                {
                    auto carView = serverEntityFactory.getEntityManager().createView<component::Car, component::Placement, component::CarConfig>();
                    for (auto &&carEntity: carView) {

                        const auto car = carView.get<component::Car>(carEntity);
                        // NOTE: Technically I should use slippingRear, but this gives better effect
                        if (car.slippingFront || true) {
                            const auto &carConfig = carView.get<component::CarConfig>(carEntity);
                            const auto &placement = carView.get<component::Placement>(carEntity);

                            // TODO: This is game logic, maybe tire placement should be saved as part of CarConfig?
                            // same logic is hard-coded when spawningCar server side.
                            math::vec3 skidPosRL{static_cast<common::real32>(-carConfig.cgToRearAxle),
                                                 static_cast<common::real32>(carConfig.wheelWidth +
                                                                             carConfig.halfWidth / 2),
                                    // NOTE: This z-value is unused.
                                                 0.f};
                            math::vec3 skidPosRR{static_cast<common::real32>(-carConfig.cgToRearAxle),
                                                 static_cast<common::real32>(-carConfig.wheelWidth -
                                                                             carConfig.halfWidth / 2),
                                                 0.f};
                            auto transform = math::Transformation::createTransformation(placement.position,
                                                                                        placement.rotation,
                                                                                        placement.scale);
                            skidPosList.emplace_back((transform * skidPosRL).getXY());
                            skidPosList.emplace_back((transform * skidPosRR).getXY());

//                            auto alpha = static_cast<common::uint8>((car.velocityAbsolute / car.maxVelocityAbsolute) *
//                                                                    255);
                            // TODO: arbitrary number based on number of frames, think about better way of determining this
                            skidOpacity.push_back(10);
                        }
                    }
                }

                {
                    auto brush = component::Brush{};
                    brush.type = component::BrushType::PLAIN_RECTANGLE;
                    brush.size = math::vec2{0.5f, 0.5f};

                    for (size_t i = 0; i < skidPosList.size(); ++i) {
                        brush.color = component::PixelRGBA{0, 0, 0, skidOpacity[i / 2]};
                        splat(clientEntityFactory, brush, skidPosList[i]);
                    }
                }
            }

            // Update Laps
            {
                auto carLapView = serverEntityFactory.getEntityManager().createView<component::Car, component::CarLapInfo>();
                for (auto &&carEntity: carLapView) {
                    // TODO: This will render all player laps on top of each other. I should render the data in rows
                    // instead. Something like:
                    /**
                     * Player Name: lap, lap time, best time
                     *
                     * Player 1: 4, 1:12, :1:50
                     * Player 2: 5, 0:02, :1:50
                     */
                    const auto &carLap = carLapView.get<component::CarLapInfo>(carEntity);

                    const auto &carLapText = getOrCreateLapText(clientEntityFactory, carEntity, carLap);
                    updateRaceInfo(clientEntityFactory.getEntityManager(), carLap, carLapText);
                }
            }
        }

        void
        SceneManager::splat(entities::EntityFactory &entityFactory,
                            component::Brush brush,
                            const math::vec2 &worldPos) {
            auto &entityManager = entityFactory.getEntityManager();

            std::set<common::EntityID> tileEntities;

            auto entityID = getOrCreateCanvasTile(entityFactory, worldPos);
            tileEntities.insert(entityID);

            auto lowerLeft = worldPos - brush.size / 2.f;
            auto lowerLeftEntityID = getOrCreateCanvasTile(entityFactory, lowerLeft);
            tileEntities.insert(lowerLeftEntityID);

            auto topRight = worldPos + brush.size / 2.f;
            auto topRightEntityID = getOrCreateCanvasTile(entityFactory, topRight);
            tileEntities.insert(topRightEntityID);

            math::vec2 topLeft{worldPos.x - brush.size.x / 2.f, worldPos.y + brush.size.y / 2.f};
            auto topLeftEntityID = getOrCreateCanvasTile(entityFactory, topLeft);
            tileEntities.insert(topLeftEntityID);

            math::vec2 lowerRight{worldPos.x + brush.size.x / 2.f, worldPos.y - brush.size.y / 2.f};
            auto lowerRightEntityID = getOrCreateCanvasTile(entityFactory, lowerRight);
            tileEntities.insert(lowerRightEntityID);

            for (auto &&tileEntity: tileEntities) {
                updateCanvasTile(entityManager, tileEntity, brush, worldPos);
            }
        }

        common::EntityID
        SceneManager::getOrCreateCanvasTile(entities::EntityFactory &entityFactory,
                                            const math::vec2 &pos) {
            auto x = math::findBin(pos.x, mCanvasTileSizeX);
            auto y = math::findBin(pos.y, mCanvasTileSizeY);
            auto xy = math::packInt64(x, y);
            auto &entityRegistry = entityFactory.getEntityManager();

            auto missing = mCanvasTileLookup.find(xy) == mCanvasTileLookup.end();
            if (missing) {
                auto tilePosX = math::binPos(x, mCanvasTileSizeX);
                auto tilePosY = math::binPos(y, mCanvasTileSizeY);

                auto worldPos = math::vec3{tilePosX, tilePosY,
                                           mZLayerManager.getZForEntity(component::EntityType::CANVAS)};
                auto tileSize = math::vec2{static_cast<common::real32>(mCanvasTileSizeX),
                                           static_cast<common::real32>(mCanvasTileSizeY)};

                common::real32 heading = 0.f;
                std::string emptyTextureID;

                // TODO: Should this be a canvas type?
                auto entityID = entityFactory.createEntity<component::EntityType::SIMPLE_SPRITE>(worldPos,
                                                                                                 tileSize,
                                                                                                 heading,
                                                                                                 emptyTextureID);

                auto &texture = entityRegistry.get<component::Texture>(entityID);

                auto widthInPixels = static_cast<common::uint16>(mCanvasTileSizeX * mGameUnitToPixels +
                                                                 common::EP);
                auto heightInPixels = static_cast<common::uint16>(mCanvasTileSizeY * mGameUnitToPixels +
                                                                  common::EP);

                texture.image.width = widthInPixels;
                texture.image.height = heightInPixels;

                auto defaultColor = component::PixelRGBA{};
                mTextureManager->fill(texture.image, defaultColor);
                mTextureManager->loadFromImage(texture);

                mCanvasTileLookup.emplace(xy, entityID);
            }

            auto entity = mCanvasTileLookup.at(xy);
            assert(entity);
            return entity;
        }

        void
        SceneManager::updateCanvasTile(entities::EntityManager &entityManager,
                                       common::EntityID entityID,
                                       const component::Brush &brush,
                                       const math::vec2 &worldPos) {
            auto &canvasTexture = entityManager.get<component::Texture>(entityID);
            // TODO: why the hell from Shape and not Placement?
            auto canvasTilePos = entityManager.get<component::Shape>(entityID).getPosition();

            auto brushTexturePos = math::vec3{worldPos.x, worldPos.y, 0.f};
            math::Transformation::worldToTextureCoordinate(canvasTilePos, mGameUnitToPixels,
                                                           brushTexturePos);

            component::Image image{};
            switch (brush.type) {
                case component::BrushType::PLAIN_RECTANGLE: {
                    image.width = static_cast<uint16>(brush.size.x * mGameUnitToPixels);
                    image.height = static_cast<uint16>(brush.size.y * mGameUnitToPixels);
                    mTextureManager->fill(image, brush.color);
                    break;
                }
                case component::BrushType::CUSTOM_TEXTURE: {
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

        void
        SceneManager::updateParticles(entities::EntityFactory &entityFactory,
                                      common::real64 tickTime) {
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
                auto mvp = mProjectionMatrix * view;
                shader.setUniformMat4("mvp", mvp);
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
                    const auto &loadedTexture = mTextureManager->loadOrGetTexture(texture.filePath.c_str());
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
        SceneManager::render(entities::EntityFactory &serverEntityFactory,
                             entities::EntityFactory &clientEntityFactory,
                             common::EntityID lookAtEntity) {
            if (lookAtEntity && serverEntityFactory.getEntityManager().has<component::Placement>(lookAtEntity)) {
                const auto &pos = serverEntityFactory.getEntityManager().get<component::Placement>(
                        lookAtEntity).position;
                lookAt(pos.x, pos.y);
            }

            render(serverEntityFactory);
            render(clientEntityFactory);
        }

        void
        SceneManager::render(entities::EntityFactory &entityFactory) {
            auto viewWidth = getViewWidth();
            auto viewHeight = getViewHeight();
            auto &entityManager = entityFactory.getEntityManager();

            {
                begin(*mColorShader, *mColorRenderer, true, true, true);
                renderColorSprites(entityManager, viewWidth, viewHeight);
                end(*mColorShader, *mColorRenderer);
            }

            {
                begin(*mTextureShader, *mTextureRenderer, true, true, true);
                renderStaticText(entityManager, viewWidth, viewHeight);
                renderStaticTextures(entityManager, viewWidth, viewHeight);
                renderDynamicTextures(entityManager, viewWidth, viewHeight);
                end(*mTextureShader, *mTextureRenderer);
            }

            {
                // Render UI text with fixed camera
                begin(*mTextureShader, *mTextureRenderer, false, false, true);
                // TODO: This should actually be split up from static text and entities part of UI should be tagged so
                // renderStaticText(entityManager, viewWidth, viewHeight);
                end(*mTextureShader, *mTextureRenderer);
            }

            {
                renderAndUpdateAnimation(entityFactory);
            }
            
            {
                begin(*mParticleShader, *mParticleRenderer, true, true, true);
                renderParticles(entityManager, viewWidth, viewHeight);
                end(*mParticleShader, *mParticleRenderer);
            }

        }

        void
        SceneManager::renderRaw(const component::Shape &shape,
                                const component::Appearance &appearance) {
            mColorRenderer->submit(shape, appearance);
            ++mRenderedSpritesPerFrame;
        }

        void
        SceneManager::beginColorRendering() {
            begin(*mColorShader, *mColorRenderer, true, false, true);
        }

        void
        SceneManager::endColorRendering() {
            end(*mColorShader, *mColorRenderer);
        }

        void
        SceneManager::renderAndUpdateAnimation(entities::EntityFactory &entityFactory) {
            auto policy = component::EntityOperationPolicy{};
            policy.safe = false;
            policy.track = false;

            auto view = entityFactory.getEntityManager().createView<component::AnimatedSplat, component::WorldPos>();
            auto now = std::chrono::steady_clock::now().time_since_epoch().count();
            for (auto &&entity: view) {
                const auto &animatedSplat = view.get<component::AnimatedSplat>(entity);
                const auto &pos = view.get<component::WorldPos>(entity).value.getXY();
                if (animatedSplat.diesAt >= now) {
                    splat(entityFactory, animatedSplat.brush, pos);
                    entityFactory.removeEntity(entity, policy);
                } else {
                    // TODO: Render a the particle using particle shader, not sure if I should do the physics calculation
                    // on the shader or here, probably I can do it on the shader but I have to make sure that I can
                    // consistently find the final position correctly

                }
            }
        }

        void
        SceneManager::renderStaticText(entities::EntityManager &manager,
                                       common::real32 viewWidth,
                                       common::real32 viewHeight) {
            auto staticTextView = manager.createView<component::Text, component::Tag_Static>();
            for (const auto &entity: staticTextView) {
                auto &text = staticTextView.get<component::Text>(entity);

                ++mRenderedSpritesPerFrame;
                ++mRenderedTexturesPerFrame;

                mTextureRenderer->submit(text);
            }
        }

        void
        SceneManager::renderStaticTextures(entities::EntityManager &manager,
                                           common::real32 viewWidth,
                                           common::real32 viewHeight) {
            auto staticTextureView = manager.createView<component::Tag_TextureShaded, component::Shape,
                    component::Texture, component::Tag_Static>();
            for (const auto &entity: staticTextureView) {
                const auto &shape = staticTextureView.get<component::Shape>(entity);
                if (!math::intersects(shape, mCamera.x, mCamera.y, viewWidth, viewHeight)) {
                    continue;
                }
                auto &texture = staticTextureView.get<component::Texture>(entity);
                prepareTexture(texture);

                ++mRenderedSpritesPerFrame;
                ++mRenderedTexturesPerFrame;

                // TODO: submit will fail if we reach maximum number of sprites.
                // I could also check the number of entities using the view and decide before hand at which point I
                // flush the renderer and open up room for new sprites.

                // TODO: For buffer data storage take a look at: https://www.khronos.org/opengl/wiki/Buffer_Object#Immutable_Storage
                // Currently the renderer is limited to 32 samplers, I have to either use the reset method
                // or look to alternatives of how to deal with many textures, one solution is to create a texture atlas
                // by merging many textures to keep below the limit. Other solutions might be looking into other type
                // of texture storage that can hold bigger number of textures.
                mTextureRenderer->submit(shape, texture);
            }
        }

        void
        SceneManager::renderDynamicTextures(entities::EntityManager &manager,
                                            common::real32 viewWidth,
                                            common::real32 viewHeight) {
            auto view = manager.createView<component::Tag_TextureShaded, component::Shape,
                    component::Texture, component::Placement, component::Tag_Dynamic>();
            for (const auto &entity: view) {
                const auto &shape = view.get<component::Shape>(entity);
                const auto &placement = view.get<component::Placement>(entity);

                auto transformation = math::Transformation::createTransformation(placement.position,
                                                                                 placement.rotation,
                                                                                 placement.scale);

                // TODO: I need to do this for physics anyway! Maybe I can store PlacementLocal and PlacementWorld
                // separately for each entity and each time a physics system updates an entity it will automatically
                // recalculate PlacementWorld for the entity and all its child entities.
                // TODO: Instead of calling .has(), slow operation, split up dynamic entity rendering into two
                // 1) Create a view with all of them that has TransformParent; 2) Create a view without parent
                if (manager.has<component::TransformParent>(entity)) {
                    const auto &transformParent = manager.get<component::TransformParent>(entity);
                    // NOTE: Order matters. First transform by parent's transformation then child.
                    transformation = transformParent.transform * transformation;
                }

                auto shapeTransformed = math::Transformation::shapeTransformation(transformation, shape);
                if (!math::intersects(shapeTransformed, mCamera.x, mCamera.y, viewWidth, viewHeight)) {
                    continue;
                }

                auto &texture = view.get<component::Texture>(entity);

                prepareTexture(texture);
                mTextureRenderer->submit(shapeTransformed, texture);

                ++mRenderedSpritesPerFrame;
                ++mRenderedTexturesPerFrame;
            }
        }

        void
        SceneManager::renderColorSprites(entities::EntityManager &manager,
                                         common::real32 viewWidth,
                                         common::real32 viewHeight) {
            auto view = manager.createView<component::Tag_ColorShaded, component::Shape,
                    component::Appearance, component::Tag_Static>();
            for (const auto &entity: view) {
                const auto &shape = view.get<component::Shape>(entity);
                if (!math::intersects(shape, mCamera.x, mCamera.y, viewWidth, viewHeight)) {
                    continue;
                }
                const auto &appearance = view.get<component::Appearance>(entity);

                ++mRenderedSpritesPerFrame;

                mColorRenderer->submit(shape, appearance);
            }
        }

        void
        SceneManager::renderParticles(entities::EntityManager &manager,
                                      common::real32 viewWidth,
                                      common::real32 viewHeight) {
            // Particles with color shading
            {
                auto view = manager.createView<component::Tessellation, component::Appearance, component::Placement, component::Age, component::Velocity, component::Tag_Particle,
                        component::Tag_ShaderOnlyParticlePhysics>();

                mParticleShader->setUniform1i("shaderPhysics", true);

                for (const auto &entity: view) {
                    const auto &placement = view.get<component::Placement>(entity);
                    if (!math::intersects(placement.position, mCamera.x, mCamera.y, viewWidth, viewHeight)) {
                        continue;
                    }
                    const auto &tessellation = view.get<component::Tessellation>(entity);
                    const auto &age = view.get<component::Age>(entity);
                    const auto &velocity = view.get<component::Velocity>(entity);
                    const auto &appearance = view.get<component::Appearance>(entity);

                    ++mRenderedParticlesPerFrame;

                    mParticleRenderer->submit(tessellation, placement, age, velocity, appearance);
                }
            }

            // Particles with texture shading
            {
                auto view = manager.createView<component::Tessellation, component::Texture, component::Age, component::Velocity, component::Placement, component::Tag_Particle,
                        component::Tag_ShaderOnlyParticlePhysics>();
                mParticleShader->setUniform1i("shaderPhysics", true);

                for (const auto &entity: view) {
                    const auto &placement = view.get<component::Placement>(entity);
                    if (!math::intersects(placement.position, mCamera.x, mCamera.y, viewWidth, viewHeight)) {
                        continue;
                    }
                    const auto &tessellation = view.get<component::Tessellation>(entity);
                    const auto &age = view.get<component::Age>(entity);
                    const auto &velocity = view.get<component::Velocity>(entity);

                    auto &texture = view.get<component::Texture>(entity);
                    prepareTexture(texture);

                    ++mRenderedParticlesPerFrame;

                    mParticleRenderer->submit(tessellation, placement, age, velocity, texture);
                }
            }

        }


        const SceneManager::RaceInfoEntities &
        SceneManager::getOrCreateLapText(entities::EntityFactory &entityFactory,
                                         common::EntityID carEntityID,
                                         const component::CarLapInfo &carLap) {
            auto exists = mLapInfoLookup.find(carEntityID) != mLapInfoLookup.end();
            if (!exists) {
                auto zLevel = mZLayerManager.getZForEntity(component::EntityType::UI);
                RaceInfoEntities carLapText{0};
                math::vec3 lapRenderPos{mScreenBounds.xMax - 3.5f, mScreenBounds.yMax - 0.5f, zLevel};
                math::vec3 lapTimeRenderPos{mScreenBounds.xMax - 3.5f, mScreenBounds.yMax - 1.0f, zLevel};
                math::vec3 bestTimeRenderPos{mScreenBounds.xMax - 3.5f, mScreenBounds.yMax - 1.5f, zLevel};

                carLapText.lapEntity = createText(entityFactory, lapRenderPos, "Lap: " + std::to_string(carLap.lap));
                carLapText.lapTimeEntity = createText(entityFactory, lapTimeRenderPos,
                                                      "Lap time: " + std::to_string(carLap.lapTimeS));
                carLapText.lapBestTimeEntity = createText(entityFactory, bestTimeRenderPos,
                                                          "Best time: " + std::to_string(carLap.bestLapTimeS));
                mLapInfoLookup[carEntityID] = carLapText;
            }
            return mLapInfoLookup.at(carEntityID);
        }

        void
        SceneManager::updateRaceInfo(entities::EntityManager &entityManager,
                                     const component::CarLapInfo &carLap,
                                     const SceneManager::RaceInfoEntities &carLapTextEntities) {
            // TODO: This is updated every tick, which is unnecessary. Lap information is rarely updated.
            auto &lapText = entityManager.get<component::Text>(carLapTextEntities.lapEntity);
            mFontManager.updateText("Lap: " + std::to_string(carLap.lap), lapText);

            auto &lapTimeText = entityManager.get<component::Text>(carLapTextEntities.lapTimeEntity);
            mFontManager.updateText("Lap time: " + std::to_string(carLap.lapTimeS) + "s", lapTimeText);

            auto &bestTimeText = entityManager.get<component::Text>(carLapTextEntities.lapBestTimeEntity);
            mFontManager.updateText("Best time: " + std::to_string(carLap.bestLapTimeS) + "s", bestTimeText);
        }

        const component::Camera &
        SceneManager::getCamera() const {
            return mCamera;
        }

        void
        SceneManager::zoom(common::real32 distance) {
            mCamera.z = 1 / distance;
        }

        void
        SceneManager::lookAt(common::real32 x,
                             common::real32 y) {
            mCamera.x = x;
            mCamera.y = y;
        }

        void
        SceneManager::lookAt(common::real32 x,
                             common::real32 y,
                             common::real32 distance) {
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

        common::uint16
        SceneManager::getSpritesPerFrame() const {
            return mRenderedSpritesPerFrame;
        }

        common::uint16
        SceneManager::getParticlesPerFrame() const {
            return mRenderedParticlesPerFrame;
        }

        common::uint16
        SceneManager::getTexturesPerFrame() const {
            return mRenderedTexturesPerFrame;
        }

        common::real32
        SceneManager::getViewWidth() const {
            return (mScreenBounds.xMax - mScreenBounds.xMin) * (1.0f / mCamera.z);
        }

        common::real32
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
        SceneManager::createText(entities::EntityFactory &entityFactory,
                                 const math::vec3 &worldPos,
                                 const std::string &text) {
            auto entityID = mFontManager.createTextFromString(entityFactory, text, worldPos);
            return entityID;
        }
    }
}

#include <oni-core/physics/dynamics.h>
#include <oni-core/graphic/scene-manager.h>

#include <GL/glew.h>

#include <oni-core/graphic/shader.h>
#include <oni-core/graphic/batch-renderer-2d.h>
#include <oni-core/graphic/texture-manager.h>
#include <oni-core/graphic/font-manager.h>
#include <oni-core/graphic/debug-draw-box2d.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/entities/entity-manager.h>
#include <oni-core/entities/create-entity.h>
#include <oni-core/common/consts.h>
#include <oni-core/component/geometry.h>
#include <oni-core/component/hierarchy.h>
#include <oni-core/component/gameplay.h>
#include <oni-core/component/tag.h>


namespace oni {
    namespace graphic {
        SceneManager::SceneManager(const component::ScreenBounds &screenBounds, FontManager &fontManager,
                                   b2World &physicsWorld,
                                   common::real32 gameUnitToPixels
        ) :
                mSkidTileSizeX{64}, mSkidTileSizeY{64},
                mHalfSkidTileSizeX{mSkidTileSizeX / 2.0f},
                mHalfSkidTileSizeY{mSkidTileSizeY / 2.0f},
                // 64k vertices
                mMaxSpriteCount(16 * 1000), mScreenBounds(screenBounds),
                mFontManager(fontManager),
                mPhysicsWorld(physicsWorld),
                mGameUnitToPixels{gameUnitToPixels} {

            mProjectionMatrix = math::mat4::orthographic(screenBounds.xMin, screenBounds.xMax, screenBounds.yMin,
                                                         screenBounds.yMax, -1.0f, 1.0f);
            mViewMatrix = math::mat4::identity();

            mModelMatrix = math::mat4::identity();

            mInternalRegistry = std::make_unique<entities::EntityManager>();

            // TODO: Resources are not part of oni-core library! This structure as is not flexible, meaning
            // I am forcing the users to only depend on built-in shaders. I should think of a better way
            // to provide flexibility in type of shaders users can define and expect to just work by having buffer
            // structure and what not set up automatically.
            mTextureShader = std::make_unique<graphic::Shader>("resources/shaders/texture.vert",
                                                               "resources/shaders/texture.frag");
            initializeTextureRenderer(*mTextureShader);
            // TODO: As it is now, BatchRenderer is coupled with the Shader. It requires the user to setup the
            // shader prior to render series of calls. Maybe shader should be built into the renderer.

            mColorShader = std::make_unique<graphic::Shader>("resources/shaders/basic.vert",
                                                             "resources/shaders/basic.frag");

            mTextureManager = std::make_unique<TextureManager>();

            initializeColorRenderer(*mColorShader);

            mDebugDrawBox2D = std::make_unique<DebugDrawBox2D>(this);
            mDebugDrawBox2D->AppendFlags(b2Draw::e_shapeBit);
            //mDebugDrawBox2D->AppendFlags(b2Draw::e_aabbBit);
            mDebugDrawBox2D->AppendFlags(b2Draw::e_pairBit);
            mDebugDrawBox2D->AppendFlags(b2Draw::e_centerOfMassBit);
        }

        SceneManager::~SceneManager() = default;

        void SceneManager::initializeColorRenderer(const Shader &shader) {
            auto program = shader.getProgram();

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
                    bufferStructures);
        }

        void SceneManager::initializeTextureRenderer(const Shader &shader) {
            auto program = shader.getProgram();

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

            auto renderer = std::make_unique<BatchRenderer2D>(
                    mMaxSpriteCount,
                    // TODO: If there are more than this number of textures to render in a draw call, it will fail
                    common::maxNumTextureSamplers,
                    sizeof(component::TexturedVertex),
                    bufferStructure);

            shader.enable();
            shader.setUniformiv("samplers", renderer->generateSamplerIDs());
            shader.disable();

            mTextureRenderer = std::move(renderer);
        }

        void SceneManager::begin(const Shader &shader, Renderer2D &renderer2D, bool translate, bool scale) {
            shader.enable();

            auto view = math::mat4::identity();
            if (scale) {
                view = view * math::mat4::scale(math::vec3{mCamera.z, mCamera.z, 1.0f});
            }

            if (translate) {
                view = view * math::mat4::translation(-mCamera.x, -mCamera.y, 0.0f);
            }
            auto mvp = mProjectionMatrix * view;
            shader.setUniformMat4("mvp", mvp);
            renderer2D.begin();
        }

        void SceneManager::prepareTexture(component::Texture &texture) {
            switch (texture.status) {
                case component::TextureStatus::READY: {
                    break;
                }
                case component::TextureStatus::NEEDS_LOADING_USING_PATH: {
                    auto loadedTexture = mTextureManager->findOrLoad(texture.filePath);
                    texture = *loadedTexture;
                    break;
                }
                case component::TextureStatus::NEEDS_LOADING_USING_DATA: {
                    assert(texture.width);
                    assert(texture.height);

                    auto loadedTexture = mTextureManager->loadFromData(texture.width, texture.height, texture.data);
                    texture = loadedTexture;
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

        void SceneManager::end(const Shader &shader, Renderer2D &renderer2D) {
            renderer2D.end();
            renderer2D.flush();
            shader.disable();
        }

        void SceneManager::renderPhysicsDebugData() {
            mDebugDrawBox2D->Begin();
            mPhysicsWorld.DrawDebugData();
            mDebugDrawBox2D->End();
        }

        void SceneManager::render(entities::EntityManager &manager, common::EntityID lookAtEntity) {
            auto halfViewWidth = getViewWidth() / 2.0f;
            auto halfViewHeight = getViewHeight() / 2.0f;

            {
                auto lock = manager.scopedLock();
                begin(*mColorShader, *mColorRenderer, true, true);
                renderColored(manager, halfViewWidth, halfViewHeight);
            }

            end(*mColorShader, *mColorRenderer);

            {
                auto lock = manager.scopedLock();
                if (lookAtEntity && manager.has<component::Placement>(lookAtEntity)) {
                    const auto &pos = manager.get<component::Placement>(lookAtEntity).position;
                    lookAt(pos.x, pos.y);
                }

                begin(*mTextureShader, *mTextureRenderer, true, true);
                renderStaticTextured(manager, halfViewWidth, halfViewHeight);
                renderDynamicTextured(manager, halfViewWidth, halfViewHeight);
                renderStaticText(manager, halfViewWidth, halfViewHeight);
                // Release the lock as soon as we are done with the registry.
            }

            end(*mTextureShader, *mTextureRenderer);
        }

        void SceneManager::renderInternal() {
            auto halfViewWidth = getViewWidth() / 2.0f;
            auto halfViewHeight = getViewHeight() / 2.0f;

            {
                auto lock = mInternalRegistry->scopedLock();
                begin(*mTextureShader, *mTextureRenderer, true, true);
                renderStaticTextured(*mInternalRegistry, halfViewWidth, halfViewHeight);
            }
            end(*mTextureShader, *mTextureRenderer);

            {
                auto lock = mInternalRegistry->scopedLock();
                // Render UI text with fixed camera
                begin(*mTextureShader, *mTextureRenderer, false, false);
                renderStaticText(*mInternalRegistry, halfViewWidth, halfViewHeight);
            }
            end(*mTextureShader, *mTextureRenderer);

        }

        void SceneManager::renderRaw(const component::Shape &shape, const component::Appearance &appearance) {
            mColorRenderer->submit(shape, appearance);
            ++mRenderedSpritesPerFrame;
        }

        void SceneManager::beginColorRendering() {
            begin(*mColorShader, *mColorRenderer, true, false);
        }

        void SceneManager::endColorRendering() {
            end(*mColorShader, *mColorRenderer);
        }

        void SceneManager::renderStaticText(entities::EntityManager &manager, common::real32 halfViewWidth,
                                            common::real32 halfViewHeight) {
            auto staticTextView = manager.createView<component::Text, component::Tag_Static>();
            for (const auto &entity: staticTextView) {
                auto &text = staticTextView.get<component::Text>(entity);

                ++mRenderedSpritesPerFrame;
                ++mRenderedTexturesPerFrame;

                mTextureRenderer->submit(text);
            }
        }

        void SceneManager::renderStaticTextured(entities::EntityManager &manager, common::real32 halfViewWidth,
                                                common::real32 halfViewHeight) {
            auto staticTextureView = manager.createView<component::Tag_TextureShaded, component::Shape,
                    component::Texture, component::Tag_Static>();
            for (const auto &entity: staticTextureView) {
                const auto &shape = staticTextureView.get<component::Shape>(entity);
                if (!isVisible(shape, halfViewWidth, halfViewHeight)) {
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

        void SceneManager::renderDynamicTextured(entities::EntityManager &manager, common::real32 halfViewWidth,
                                                 common::real32 halfViewHeight) {
            // TODO: Maybe I can switch to none-locking view if I can split the registry so that rendering and
            // other systems don't share any entity component, or the shared section is minimum and I can create
            // copy of that data before starting rendering and only lock the registry at that point
            auto view = manager.createView<component::Tag_TextureShaded, component::Shape,
                    component::Texture, component::Placement, component::Tag_Dynamic>();
            for (const auto &entity: view) {
                const auto &shape = view.get<component::Shape>(entity);
                const auto &placement = view.get<component::Placement>(entity);

                auto transformation = physics::Transformation::createTransformation(placement.position,
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

                auto shapeTransformed = physics::Transformation::shapeTransformation(transformation, shape);
                if (!isVisible(shapeTransformed, halfViewWidth, halfViewHeight)) {
                    continue;
                }

                auto &texture = view.get<component::Texture>(entity);

                prepareTexture(texture);
                mTextureRenderer->submit(shapeTransformed, texture);

                ++mRenderedSpritesPerFrame;
                ++mRenderedTexturesPerFrame;
            }
        }

        void SceneManager::renderColored(entities::EntityManager &manager, common::real32 halfViewWidth,
                                         common::real32 halfViewHeight) {
            auto staticSpriteView = manager.createView<component::Tag_ColorShaded, component::Shape,
                    component::Appearance, component::Tag_Static>();
            for (const auto &entity: staticSpriteView) {
                const auto &shape = staticSpriteView.get<component::Shape>(entity);
                if (!isVisible(shape, halfViewWidth, halfViewHeight)) {
                    continue;
                }
                const auto &appearance = staticSpriteView.get<component::Appearance>(entity);

                ++mRenderedSpritesPerFrame;

                mColorRenderer->submit(shape, appearance);
            }
        }


        void SceneManager::tick(entities::EntityManager &manager) {
            // NOTE: Server needs to send zLevel data prior to creating any visual object on clients.
            // TODO: A better way is no not call tick before we know we have all the information to do actual
            // work. Kinda like a level loading screen.
            if (mZLevel.majorLevelDelta <= common::ep) {
                return;
            }

            std::vector<component::Placement> carTireRRPlacements{};
            std::vector<component::Placement> carTireRLPlacements{};
            std::vector<component::TransformParent> carTireRRTransformParent{};
            std::vector<component::TransformParent> carTireRLTransformParent{};
            std::vector<common::uint8> skidOpacity{};
            {
                auto carView = manager.createViewScopeLock<component::Car, component::Placement>();
                for (auto &&carEntity: carView) {

                    const auto car = carView.get<component::Car>(carEntity);
                    // NOTE: Technically I should use slippingRear, but this gives better effect
                    if (car.slippingFront || true) {
                        // TODO: This is not in the view and it will be very slow as more entities are added to the
                        // registry. Perhaps I can save the tires together with the car
                        const auto &carTireRRPlacement = manager.get<component::Placement>(car.tireRR);
                        carTireRRPlacements.push_back(carTireRRPlacement);
                        const auto &carTireRLPlacement = manager.get<component::Placement>(car.tireRL);
                        carTireRLPlacements.push_back(carTireRLPlacement);

                        const auto &transformParentRR = manager.get<component::TransformParent>(car.tireRR);
                        carTireRRTransformParent.push_back(transformParentRR);
                        const auto &transformParentRL = manager.get<component::TransformParent>(car.tireRL);
                        carTireRLTransformParent.push_back(transformParentRL);

                        auto alpha = static_cast<common::uint8>((car.velocityAbsolute / car.maxVelocityAbsolute) * 255);
                        skidOpacity.push_back(alpha);
                    }
                }
            }

            for (size_t i = 0; i < skidOpacity.size(); ++i) {
                auto skidMarkRRPos = carTireRRTransformParent[i].transform * carTireRRPlacements[i].position;
                auto skidMarkRLPos = carTireRLTransformParent[i].transform * carTireRLPlacements[i].position;

                common::EntityID skidEntityRL{0};
                common::EntityID skidEntityRR{0};

                skidEntityRL = createOrGetSkidTile(skidMarkRLPos.getXY());
                skidEntityRR = createOrGetSkidTile(skidMarkRRPos.getXY());

                updateSkidlines(skidMarkRLPos, skidEntityRL, skidOpacity[i]);
                updateSkidlines(skidMarkRRPos, skidEntityRR, skidOpacity[i]);
            }
            {
                auto carLapView = manager.createViewScopeLock<component::Car, component::CarLapInfo>();
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
                    const auto &carLapText = createLapTextIfMissing(carEntity, carLap);
                    updateRaceInfo(carLap, carLapText);
                }
            }
        }

        const SceneManager::RaceInfoEntities &SceneManager::createLapTextIfMissing(common::EntityID carEntityID,
                                                                                   const component::CarLapInfo &carLap) {
            auto exists = mLapInfoLookup.find(carEntityID) != mLapInfoLookup.end();
            if (!exists) {
                RaceInfoEntities carLapText{0};
                math::vec3 lapRenderPos{mScreenBounds.xMax - 3.5f, mScreenBounds.yMax - 0.5f, mZLevel.level_8};
                math::vec3 lapTimeRenderPos{mScreenBounds.xMax - 3.5f, mScreenBounds.yMax - 1.0f, mZLevel.level_8};
                math::vec3 bestTimeRenderPos{mScreenBounds.xMax - 3.5f, mScreenBounds.yMax - 1.5f, mZLevel.level_8};

                carLapText.lapEntity = createText(lapRenderPos, "Lap: " + std::to_string(carLap.lap));
                carLapText.lapTimeEntity = createText(lapTimeRenderPos, "Lap time: " + std::to_string(carLap.lapTimeS));
                carLapText.lapBestTimeEntity = createText(bestTimeRenderPos,
                                                          "Best time: " + std::to_string(carLap.bestLapTimeS));
                mLapInfoLookup[carEntityID] = carLapText;
            }
            return mLapInfoLookup.at(carEntityID);
        }

        void SceneManager::updateRaceInfo(const component::CarLapInfo &carLap,
                                          const SceneManager::RaceInfoEntities &carLapTextEntities) {
            // TODO: This is updated every tick, which is unnecessary. Lap information is rarely updated.
            auto &lapText = mInternalRegistry->get<component::Text>(carLapTextEntities.lapEntity);
            mFontManager.updateText("Lap: " + std::to_string(carLap.lap), lapText);

            auto &lapTimeText = mInternalRegistry->get<component::Text>(carLapTextEntities.lapTimeEntity);
            mFontManager.updateText("Lap time: " + std::to_string(carLap.lapTimeS) + "s", lapTimeText);

            auto &bestTimeText = mInternalRegistry->get<component::Text>(carLapTextEntities.lapBestTimeEntity);
            mFontManager.updateText("Best time: " + std::to_string(carLap.bestLapTimeS) + "s", bestTimeText);
        }

        common::EntityID SceneManager::createOrGetSkidTile(const math::vec2 &position) {
            auto x = math::positionToIndex(position.x, mSkidTileSizeX);
            auto y = math::positionToIndex(position.y, mSkidTileSizeY);
            auto packedIndices = math::packIntegers(x, y);

            auto exists = mSkidlineLookup.find(packedIndices) != mSkidlineLookup.end();
            if (!exists) {
                auto tilePosX = math::indexToPosition(x, mSkidTileSizeX);
                auto tilePosY = math::indexToPosition(y, mSkidTileSizeY);
                auto worldPos = math::vec3{tilePosX, tilePosY, mZLevel.level_1};
                auto tileSize = math::vec2{static_cast<common::real32>(mSkidTileSizeX),
                                           static_cast<common::real32>(mSkidTileSizeY)};

                auto widthInPixels = static_cast<common::uint16>(mSkidTileSizeX * mGameUnitToPixels +
                                                                 common::ep);
                auto heightInPixels = static_cast<common::uint16>(mSkidTileSizeY * mGameUnitToPixels +
                                                                  common::ep);
                auto defaultColor = component::PixelRGBA{};
                // TODO: I can blend skid textures using this data
                auto data = graphic::TextureManager::generateBits(widthInPixels, heightInPixels,
                                                                  defaultColor);

                auto texture = mTextureManager->loadFromData(widthInPixels, heightInPixels, data);
                auto lock = mInternalRegistry->scopedLock();
                auto entityID = entities::createEntity(*mInternalRegistry);
                entities::assignShapeWorld(*mInternalRegistry, entityID, tileSize, worldPos);
                entities::assignTextureLoaded(*mInternalRegistry, entityID, texture);
                entities::assignTag<component::Tag_Static>(*mInternalRegistry, entityID);
                mSkidlineLookup.emplace(packedIndices, entityID);
            }

            return mSkidlineLookup.at(packedIndices);
        }

        void SceneManager::updateSkidlines(const math::vec3 &position, common::EntityID skidTextureEntity,
                                           common::uint8 alpha) {
            auto skidMarksTexture = mInternalRegistry->get<component::Texture>(skidTextureEntity);
            const auto skidMarksTexturePos = mInternalRegistry->get<component::Shape>(
                    skidTextureEntity).getPosition();

            auto skidPos = position;
            physics::Transformation::worldToTextureCoordinate(skidMarksTexturePos, mGameUnitToPixels,
                                                              skidPos);

            // TODO: I can not generate geometrical shapes that are rotated. Until I have that I will stick to
            // squares.
            //auto width = static_cast<int>(carConfig.wheelRadius * mGameUnitToPixels * 2);
            //auto height = static_cast<int>(carConfig.wheelWidth * mGameUnitToPixels / 2);
            common::uint16 height = 5;
            common::uint16 width = 5;

            auto bits = graphic::TextureManager::generateBits(width, height, component::PixelRGBA{0, 0, 0, alpha});
            // TODO: Need to create skid texture data as it should be and set it.
            mTextureManager->updateSubTexture(skidMarksTexture,
                                              static_cast<common::oniGLint>(skidPos.x - width / 2.0f),
                                              static_cast<common::oniGLint>(skidPos.y - height / 2.0f),
                                              width, height, bits);
        }

        const component::Camera &SceneManager::getCamera() const {
            return mCamera;
        }

        void SceneManager::zoom(common::real32 distance) {
            mCamera.z = 1 / distance;
        }

        void SceneManager::lookAt(common::real32 x, common::real32 y) {
            mCamera.x = x;
            mCamera.y = y;
        }

        void SceneManager::lookAt(common::real32 x, common::real32 y, common::real32 distance) {
            mCamera.x = x;
            mCamera.y = y;
            mCamera.z = 1 / distance;
        }

        const math::mat4 &SceneManager::getProjectionMatrix() const {
            return mProjectionMatrix;
        }

        const math::mat4 &SceneManager::getViewMatrix() const {
            return mViewMatrix;
        }

        common::uint16 SceneManager::getSpritesPerFrame() const {
            return mRenderedSpritesPerFrame;
        }

        common::uint16 SceneManager::getTexturesPerFrame() const {
            return mRenderedTexturesPerFrame;
        }

        common::real32 SceneManager::getViewWidth() const {
            return (mScreenBounds.xMax - mScreenBounds.xMin) * (1.0f / mCamera.z);
        }

        common::real32 SceneManager::getViewHeight() const {
            return (mScreenBounds.yMax - mScreenBounds.yMin) * (1.0f / mCamera.z);
        }

        bool SceneManager::isVisible(const component::Shape &shape, common::real32 halfViewWidth,
                                     common::real32 halfViewHeight) const {
            // TODO: Garbage collision detection, use the same method used in lap tracker for figuring out if a check
            // point is reached.
            auto x = false;

            auto xMin = shape.vertexA.x;
            auto xMax = shape.vertexC.x;

            auto viewXMin = mCamera.x - halfViewWidth;
            auto viewXMax = mCamera.x + halfViewWidth;

            if (xMin >= viewXMin && xMin <= viewXMax) {
                x = true;
            }

            if (xMax >= viewXMin && xMax <= viewXMax) {
                x = true;
            }

            // Object is bigger than the frustum, i.e., view is inside the object
            if (viewXMin >= xMin && viewXMax <= xMax) {
                x = true;
            }

            if (!x) {
                return false;
            }

            auto y = false;

            auto yMin = shape.vertexA.y;
            auto yMax = shape.vertexC.y;

            auto viewYMin = mCamera.y - halfViewHeight;
            auto viewYMax = mCamera.y + halfViewHeight;

            if (yMin >= viewYMin && yMin <= viewYMax) {
                y = true;
            }
            if (yMax >= viewYMin && yMax <= viewYMax) {
                y = true;
            }

            // Object is bigger than the frustum, i.e., view is inside the object
            if (viewYMin >= yMin && viewYMax <= yMax) {
                y = true;
            }

            return y;
        }

        void SceneManager::resetCounters() {
            mRenderedSpritesPerFrame = 0;
            mRenderedTexturesPerFrame = 0;
        }

        common::EntityID SceneManager::createText(const math::vec3 &worldPos, const std::string &text) {
            auto entityID = mFontManager.createTextFromString(*mInternalRegistry, text, worldPos);
            return entityID;
        }

        void SceneManager::setZLevel(const component::ZLevel &zLevel) {
            mZLevel = zLevel;
        }
    }
}
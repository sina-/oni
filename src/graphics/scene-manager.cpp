#include <oni-core/graphics/scene-manager.h>
#include <oni-core/graphics/shader.h>
#include <oni-core/graphics/renderer-2d.h>
#include <oni-core/graphics/batch-renderer-2d.h>
#include <oni-core/common/consts.h>
#include <oni-core/io/output.h>
#include <oni-core/physics/transformation.h>
#include <oni-core/math/vec2.h>
#include <oni-core/entities/tile-world.h>

namespace oni {
    namespace graphics {

        SceneManager::SceneManager(const components::ScreenBounds &screenBounds) :
        // 64k vertices
                mMaxSpriteCount(16 * 1000), mScreenBounds(screenBounds) {

            mProjectionMatrix = math::mat4::orthographic(screenBounds.xMin, screenBounds.xMax, screenBounds.yMin,
                                                         screenBounds.yMax, -1.0f, 1.0f);
            mViewMatrix = math::mat4::identity();

            mModelMatrix = math::mat4::identity();

            // TODO: Resources are not part of oni-core library! This structure as is not flexible, meaning
            // I am forcing the users to only depend on built-in shaders. I should think of a better way
            // to provide flexibility in type of shaders users can define and expect to just work by having buffer
            // structure and what not set up automatically.
            mTextureShader = std::make_unique<graphics::Shader>("resources/shaders/texture.vert",
                                                                "resources/shaders/texture.frag");
            initializeTextureRenderer(*mTextureShader);
            // TODO: As it is now, BatchRenderer is coupled with the Shader. It requires the user to setup the
            // shader prior to render series of calls. Maybe shader should be built into the renderer.

            mColorShader = std::make_unique<graphics::Shader>("resources/shaders/basic.vert",
                                                              "resources/shaders/basic.frag");
            initializeColorRenderer(*mColorShader);
        }

        SceneManager::~SceneManager() = default;

        void SceneManager::initializeColorRenderer(const Shader &shader) {
            auto program = shader.getProgram();

            auto positionIndex = glGetAttribLocation(program, "position");
            auto colorIndex = glGetAttribLocation(program, "color");

            if (positionIndex == -1 || colorIndex == -1) {
                throw std::runtime_error("Invalid attribute name.");
            }

            GLsizei stride = sizeof(components::ColoredVertex);
            auto position = std::make_unique<const components::BufferStructure>
                    (components::BufferStructure{static_cast<GLuint>(positionIndex), 3, GL_FLOAT, GL_FALSE,
                                                 stride,
                                                 static_cast<const GLvoid *>(nullptr)});
            auto color = std::make_unique<const components::BufferStructure>
                    (components::BufferStructure{static_cast<GLuint>(colorIndex), 4, GL_FLOAT, GL_TRUE, stride,
                                                 reinterpret_cast<const GLvoid *>(offsetof(components::ColoredVertex,
                                                                                           components::ColoredVertex::color))});

            auto bufferStructures = common::BufferStructures();
            bufferStructures.push_back(std::move(position));
            bufferStructures.push_back(std::move(color));

            mColorRenderer = std::make_unique<BatchRenderer2D>(
                    mMaxSpriteCount,
                    common::maxNumTextureSamplers,
                    sizeof(components::ColoredVertex),
                    std::move(bufferStructures));
        }

        void SceneManager::initializeTextureRenderer(const Shader &shader) {
            auto program = shader.getProgram();

            auto positionIndex = glGetAttribLocation(program, "position");
            auto samplerIDIndex = glGetAttribLocation(program, "samplerID");
            auto uvIndex = glGetAttribLocation(program, "uv");

            if (positionIndex == -1 || samplerIDIndex == -1 || uvIndex == -1) {
                throw std::runtime_error("Invalid attribute name.");
            }

            GLsizei stride = sizeof(components::TexturedVertex);
            auto position = std::make_unique<const components::BufferStructure>
                    (components::BufferStructure{static_cast<GLuint>(positionIndex), 3, GL_FLOAT, GL_FALSE, stride,
                                                 static_cast<const GLvoid *>(nullptr)});
            auto samplerID = std::make_unique<const components::BufferStructure>
                    (components::BufferStructure{static_cast<GLuint>(samplerIDIndex), 1, GL_FLOAT, GL_FALSE, stride,
                                                 reinterpret_cast<const GLvoid *>(offsetof(components::TexturedVertex,
                                                                                           components::TexturedVertex::samplerID))});
            auto uv = std::make_unique<const components::BufferStructure>
                    (components::BufferStructure{static_cast<GLuint>(uvIndex), 2, GL_FLOAT, GL_FALSE, stride,
                                                 reinterpret_cast<const GLvoid *>(offsetof(components::TexturedVertex,
                                                                                           components::TexturedVertex::uv))});

            auto bufferStructures = common::BufferStructures();
            bufferStructures.push_back(std::move(position));
            bufferStructures.push_back(std::move(samplerID));
            bufferStructures.push_back(std::move(uv));

            auto renderer = std::make_unique<BatchRenderer2D>(
                    mMaxSpriteCount,
                    // TODO: If there are more than this number of textures to render in a draw call, it will fail
                    common::maxNumTextureSamplers,
                    sizeof(components::TexturedVertex),
                    std::move(bufferStructures));

            shader.enable();
            shader.setUniformiv("samplers", renderer->generateSamplerIDs());
            shader.disable();

            mTextureRenderer = std::move(renderer);
        }

        void SceneManager::begin(const Shader &shader, Renderer2D &renderer2D) {
            shader.enable();

            auto view = math::mat4::scale(math::vec3{mCamera.z, mCamera.z, 1.0f}) *
                        math::mat4::translation(-mCamera.x, -mCamera.y, 0.0f);
            auto mvp = mProjectionMatrix * view;
            shader.setUniformMat4("mvp", mvp);
            renderer2D.begin();
        }

        void SceneManager::end(const Shader &shader, Renderer2D &renderer2D) {
            renderer2D.end();
            renderer2D.flush();
            shader.disable();
        }

        void SceneManager::render(entt::DefaultRegistry &registry) {
            begin(*mTextureShader, *mTextureRenderer);

            auto halfViewWidth = getViewWidth() / 2.0f;
            auto halfViewHeight = getViewHeight() / 2.0f;

            auto staticTextureSpriteView = registry.persistent<components::TagTextureShaded, components::Shape,
                    components::Texture, components::TagStatic>();
            for (const auto &entity: staticTextureSpriteView) {
                const auto &shape = staticTextureSpriteView.get<components::Shape>(entity);
                if (!visibleToCamera(shape, halfViewWidth, halfViewHeight)) {
                    continue;
                }
                const auto &texture = staticTextureSpriteView.get<components::Texture>(entity);

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

            auto dynamicTextureSpriteView = registry.persistent<components::TagTextureShaded, components::Shape,
                    components::Texture, components::Placement, components::TagDynamic>();
            for (const auto &entity: dynamicTextureSpriteView) {
                const auto &shape = dynamicTextureSpriteView.get<components::Shape>(entity);
                const auto &placement = dynamicTextureSpriteView.get<components::Placement>(entity);
                const auto &texture = dynamicTextureSpriteView.get<components::Texture>(entity);

                auto transformation = physics::Transformation::createTransformation(placement.position,
                                                                                    placement.rotation,
                                                                                    placement.scale);

                // TODO: I need to do this for physics anyway! Maybe I can store PlacementLocal and PlacementWorld
                // separately for each entity and each time a physics system updates an entity it will automatically
                // recalculate PlacementWorld for the entity and all its child entities.
                // TODO: Instead of calling .has(), slow opertaion, split up dynamic entity rendering into two
                // 1) Create a view with all of them that has TransformParent; 2) Create a view without parent
                if (registry.has<components::TransformParent>(entity)) {
                    const auto &transformParent = registry.get<components::TransformParent>(entity);
                    // NOTE: Order matters. First transform by parent's transformation then child.
                    transformation = transformParent.transform * transformation;
                }

                auto shapeTransformed = physics::Transformation::shapeTransformation(transformation, shape);
                if (!visibleToCamera(shapeTransformed, halfViewWidth, halfViewHeight)) {
                    continue;
                }

                ++mRenderedSpritesPerFrame;
                ++mRenderedTexturesPerFrame;

                mTextureRenderer->submit(shapeTransformed, texture);
            }

            end(*mTextureShader, *mTextureRenderer);

            begin(*mColorShader, *mColorRenderer);
            auto staticSpriteView = registry.persistent<components::TagColorShaded, components::Shape,
                    components::Appearance, components::TagStatic>();
            for (const auto &entity: staticSpriteView) {
                const auto &shape = staticSpriteView.get<components::Shape>(entity);
                if (!visibleToCamera(shape, halfViewWidth, halfViewHeight)) {
                    continue;
                }
                const auto &appearance = staticSpriteView.get<components::Appearance>(entity);

                ++mRenderedSpritesPerFrame;

                mColorRenderer->submit(shape, appearance);
            }

            end(*mColorShader, *mColorRenderer);
        }

        void SceneManager::lookAt(common::real32 x, common::real32 y) {
            mCamera.x = x;
            mCamera.y = y;
        }

        void SceneManager::lookAt(common::real32 x, common::real32 y, common::real32 distance) {
            mCamera.x = x;
            mCamera.y = y;
            mCamera.z = distance;
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

        bool SceneManager::visibleToCamera(const components::Shape &shape, const common::real32 halfViewWidth,
                                           const common::real32 halfViewHeight) const {
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
    }
}
#include <oni-core/graphics/layer.h>

#include <ftgl/texture-atlas.h>
#include <ftgl/texture-font.h>

#include <oni-core/graphics/renderer-2d.h>
#include <oni-core/graphics/batch-renderer-2d.h>
#include <oni-core/graphics/font-manager.h>
#include <oni-core/graphics/shader.h>
#include <oni-core/components/visual.h>
#include <oni-core/entities/basic-entity-repo.h>
#include <oni-core/graphics/texture.h>
#include <oni-core/graphics/window.h>

namespace oni {
    namespace graphics {

        Layer::Layer(std::unique_ptr<BatchRenderer2D> renderer, std::unique_ptr<Shader> shader,
                     const graphics::ScreenBounds &screenBound, const math::mat4 &modalMatrix,
                     const math::mat4 &viewMatrix,
                     const math::mat4 &projectionMatrix)
                : mRenderer2D(std::move(renderer)),
                  mShader(std::move(shader)),
                  mScreenBound(screenBound),
                  mModalMatrix(modalMatrix),
                  mViewMatrix(viewMatrix),
                  mProjectionMatrix(projectionMatrix) {
            lookAt(0.0f, 0.0f);
        }

        Layer::~Layer() = default;

        void Layer::renderSprites(const entities::BasicEntityRepo &basicEntityRepo) {
            begin();

            auto layerID = getLayerID();

            unsigned long entityIndex = 0;

            for (const auto &entity: basicEntityRepo.getEntities()) {
                if ((basicEntityRepo.getEntityLayerID(entityIndex) == layerID &&
                     (entity & entities::Sprite) == entities::Sprite)) {

                    mRenderer2D->submit(basicEntityRepo.getEntityPlacement(entityIndex),
                                        basicEntityRepo.getEntityAppearance(entityIndex));
                }
                ++entityIndex;
            }

            end();
        }

        void Layer::renderTexturedSprites(const entities::BasicEntityRepo &basicEntityRepo) {
            begin();

            auto layerID = getLayerID();

            unsigned long entityIndex = 0;

            for (const auto &entity: basicEntityRepo.getEntities()) {
                if ((basicEntityRepo.getEntityLayerID(entityIndex) == layerID &&
                     (entity & entities::TexturedSprite) == entities::TexturedSprite)) {

                    mRenderer2D->submit(basicEntityRepo.getEntityPlacement(entityIndex),
                                        basicEntityRepo.getEntityTexture(entityIndex));
                }
                ++entityIndex;
            }

            end();

        }

        void Layer::renderText(const entities::BasicEntityRepo &basicEntityRepo) {
            begin();

            auto layerID = getLayerID();

            unsigned long entityIndex = 0;

            for (const auto &entity: basicEntityRepo.getEntities()) {
                if ((basicEntityRepo.getEntityLayerID(entityIndex) == layerID &&
                     (entity & entities::TextSprite) == entities::TextSprite)) {

                    mRenderer2D->submit(basicEntityRepo.getEntityText(entityIndex));
                }
                ++entityIndex;
            }
            end();
        }

        void Layer::begin() const {
            mShader->enable();
            // TODO: Merge all these matrices into one and call it ModelViewProjMatrix and use it in the
            // shader. This is to avoid the same matrix multiplication for each vertex in the game over and over.
            mShader->setUniformMat4("pr_matrix", mProjectionMatrix);
            mShader->setUniformMat4("vw_matrix", mViewMatrix);
            mShader->setUniformMat4("ml_matrix", mModalMatrix);
            mRenderer2D->begin();
        }

        void Layer::end() const {
            mRenderer2D->end();
            mRenderer2D->flush();
            mShader->disable();
        }

        void Layer::lookAt(float x, float y) {
            mViewMatrix = math::mat4::translation(-x, -y, 0.0f);
        }

        void Layer::lookAt(float x, float y, float distance) {
            mViewMatrix = math::mat4::scale(math::vec3{distance, distance, 1.0f}) *
                          math::mat4::translation(-x, -y, 0.0f);
        }

        void Layer::orientTo(float x, float y, float degree) {
            auto rotate = math::mat4::rotation(degree, math::vec3{0.0f, 0.0f, 1.0f});
            auto translation = math::mat4::translation(x, y, 0.0f);
            mModalMatrix = translation * rotate;
        }

        const math::mat4 &Layer::getModelMatrix() const {
            return mModalMatrix;
        }

        void Layer::setModelMatrix(const math::mat4 &modelMatrix) {
            mModalMatrix = modelMatrix;
        }

        const math::mat4 &Layer::getViewMatrix() const {
            return mViewMatrix;
        }

        void Layer::setViewMatrix(const math::mat4 &viewMatrix) {
            mViewMatrix = viewMatrix;
        }

        const math::mat4 &Layer::getProjectionMatrix() const {
            return mProjectionMatrix;
        }

        void Layer::setProjectionMatrix(const math::mat4 &projectionMatrix) {
            mProjectionMatrix = projectionMatrix;
        }

        components::LayerID Layer::getLayerID() {
            return mShader->getProgram();
        }

        Shader &Layer::getShader() {
            return *mShader;
        }

    }

    /**
     * Static functions follow:
    */
    namespace graphics {

        std::unique_ptr<Layer> Layer::createTileLayer(unsigned long maxSpriteCount, std::string &&vertexShader,
                                                      std::string &&fragmentShader,
                                                      const graphics::ScreenBounds &screenBound) {

            auto shader = std::make_unique<graphics::Shader>(std::move(vertexShader), std::move(fragmentShader));
            auto program = shader->getProgram();

            GLsizei stride = sizeof(components::ColoredVertex);
            auto positionIndex = glGetAttribLocation(program, "position");
            auto colorIndex = glGetAttribLocation(program, "color");

            if (positionIndex == -1 || colorIndex == -1) {
                throw std::runtime_error("Invalid attribute name.");
            }

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

            auto renderer = std::make_unique<BatchRenderer2D>(maxSpriteCount,
                                                              mMaxNumTextureSamplers,
                                                              sizeof(components::ColoredVertex),
                                                              std::move(bufferStructures));

            auto projection = math::mat4::orthographic(screenBound.xMin, screenBound.xMax, screenBound.yMin,
                                                       screenBound.yMax, -1.0f, 1.0f);
            auto identity = math::mat4::identity();
            return std::make_unique<Layer>(std::move(renderer), std::move(shader), screenBound, identity,
                                           identity, projection);
        }

        std::unique_ptr<Layer> Layer::createTexturedTileLayer(unsigned long maxSpriteCount,
                                                              std::string &&vertexShader, std::string &&fragmentShader,
                                                              const graphics::ScreenBounds &screenBound) {

            auto shader = std::make_unique<graphics::Shader>(std::move(vertexShader), std::move(fragmentShader));
            auto program = shader->getProgram();

            GLsizei stride = sizeof(components::TexturedVertex);
            auto positionIndex = glGetAttribLocation(program, "position");
            auto samplerIDIndex = glGetAttribLocation(program, "samplerID");
            auto uvIndex = glGetAttribLocation(program, "uv");

            if (positionIndex == -1 || samplerIDIndex == -1 || uvIndex == -1) {
                throw std::runtime_error("Invalid attribute name.");
            }

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

            auto renderer = std::make_unique<BatchRenderer2D>(maxSpriteCount,
                                                              mMaxNumTextureSamplers,
                                                              sizeof(components::TexturedVertex),
                                                              std::move(bufferStructures));

            shader->enable();
            shader->setUniformiv("samplers", renderer->generateSamplerIDs());
            shader->disable();

            auto projection = math::mat4::orthographic(screenBound.xMin, screenBound.xMax, screenBound.yMin,
                                                       screenBound.yMax, -1.0f, 1.0f);
            auto identity = math::mat4::identity();
            return std::make_unique<Layer>(std::move(renderer), std::move(shader), screenBound, identity, identity,
                                           projection);
        }
    }
}

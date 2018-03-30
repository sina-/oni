#include <oni-core/graphics/layer.h>
#include <oni-core/graphics/texture.h>
#include <oni-core/graphics/window.h>

namespace oni {
    namespace graphics {

        Layer::Layer(std::unique_ptr<BatchRenderer2D> renderer, std::unique_ptr<Shader> shader,
                     const graphics::ScreenBounds &screenBound)
                : mRenderer2D(std::move(renderer)),
                  mShader(std::move(shader)),
                  mScreenBound(screenBound) {
            moveCamera(0.0f, 0.0f);
        }

        void Layer::renderSprites(const entities::World &world) {
            begin();

            auto layerID = getLayerID();

            unsigned long entityIndex = 0;

            for (const auto &entity: world.getEntities()) {
                if ((world.getEntityLayerID(entityIndex) == layerID &&
                     (entity & entities::Sprite) == entities::Sprite)) {

                    mRenderer2D->submit(world.getEntityPlacement(entityIndex),
                                        world.getEntityAppearance(entityIndex));
                }
                ++entityIndex;
            }

            end();
        }

        void Layer::renderTexturedSprites(const entities::World &world) {
            begin();

            auto layerID = getLayerID();

            unsigned long entityIndex = 0;

            for (const auto &entity: world.getEntities()) {
                if ((world.getEntityLayerID(entityIndex) == layerID &&
                     (entity & entities::TexturedSprite) == entities::TexturedSprite)) {

                    mRenderer2D->submit(world.getEntityPlacement(entityIndex), world.getEntityTexture(entityIndex));
                }
                ++entityIndex;
            }

            end();

        }

        void Layer::renderText(const entities::World &world) {
            begin();

            auto layerID = getLayerID();

            unsigned long entityIndex = 0;

            for (const auto &entity: world.getEntities()) {
                if ((world.getEntityLayerID(entityIndex) == layerID &&
                     (entity & entities::TextSprite) == entities::TextSprite)) {

                    mRenderer2D->submit(world.getEntityText(entityIndex));
                }
                ++entityIndex;
            }
            end();
        }

        void Layer::begin() const {
            mShader->enable();
            mRenderer2D->begin();
        }

        void Layer::end() const {
            mRenderer2D->end();
            mRenderer2D->flush();
            mShader->disable();
        }

        std::unique_ptr<Layer>
        Layer::createTileLayer(unsigned long maxSpriteCount, std::string &&vertexShader, std::string &&fragmentShader,
                               const graphics::ScreenBounds &screenBound) {
            auto shader = std::make_unique<graphics::Shader>(std::move(vertexShader), std::move(fragmentShader));

            auto program = shader->getProgram();

            auto stride = sizeof(components::ColoredVertex);
            auto positionIndex = glGetAttribLocation(program, "position");
            auto colorIndex = glGetAttribLocation(program, "color");

            if (positionIndex == -1 || colorIndex == -1) {
                throw std::runtime_error("Invalid attribute name.");
            }

            auto position = std::make_unique<const components::BufferStructure>
                    (positionIndex, 3, GL_FLOAT, GL_FALSE, stride, static_cast<const GLvoid *>(nullptr));
            auto color = std::make_unique<const components::BufferStructure>
                    (colorIndex, 4, GL_FLOAT, GL_TRUE, stride,
                     reinterpret_cast<const GLvoid *>(offsetof(components::ColoredVertex,
                                                               components::ColoredVertex::color)));

            auto bufferStructures = components::BufferStructures();
            bufferStructures.push_back(std::move(position));
            bufferStructures.push_back(std::move(color));

            auto renderer = std::make_unique<BatchRenderer2D>(maxSpriteCount,
                                                              mMaxNumTextureSamplers,
                                                              sizeof(components::ColoredVertex),
                                                              std::move(bufferStructures));
            return std::make_unique<Layer>(std::move(renderer), std::move(shader), screenBound);
        }

        std::unique_ptr<Layer>
        Layer::createTexturedTileLayer(unsigned long maxSpriteCount, std::string &&vertexShader,
                                       std::string &&fragmentShader,
                                       const graphics::ScreenBounds &screenBound) {
            auto shader = std::make_unique<graphics::Shader>(std::move(vertexShader), std::move(fragmentShader));

            auto program = shader->getProgram();

            auto stride = sizeof(components::TexturedVertex);
            auto positionIndex = glGetAttribLocation(program, "position");
            auto samplerIDIndex = glGetAttribLocation(program, "samplerID");
            auto uvIndex = glGetAttribLocation(program, "uv");

            if (positionIndex == -1 || samplerIDIndex == -1 || uvIndex == -1) {
                throw std::runtime_error("Invalid attribute name.");
            }

            auto position = std::make_unique<const components::BufferStructure>
                    (positionIndex, 3, GL_FLOAT, GL_FALSE, stride, static_cast<const GLvoid *>(nullptr));
            auto samplerID = std::make_unique<const components::BufferStructure>
                    (samplerIDIndex, 1, GL_FLOAT, GL_FALSE, stride,
                     reinterpret_cast<const GLvoid *>(offsetof(components::TexturedVertex,
                                                               components::TexturedVertex::samplerID)));
            auto uv = std::make_unique<const components::BufferStructure>
                    (uvIndex, 2, GL_FLOAT, GL_FALSE, stride,
                     reinterpret_cast<const GLvoid *>(offsetof(components::TexturedVertex,
                                                               components::TexturedVertex::uv)));

            auto bufferStructures = components::BufferStructures();
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

            return std::make_unique<Layer>(std::move(renderer), std::move(shader), screenBound);
        }

        void Layer::moveCamera(float x, float y) {
            // TODO: Is it really necessary to create a new matrix? Can't we just transalte the old one?
            auto projMatrix = math::mat4::orthographic(mScreenBound.xMin + x, mScreenBound.xMax + x,
                                                       mScreenBound.yMin + y, mScreenBound.yMax + y, -1.0f, 1.0f);

            mShader->enable();
            mShader->setUniformMat4("pr_matrix", projMatrix);
            mShader->disable();
        }
    }
}

#include <graphics/layer.h>
#include <graphics/texture.h>

namespace oni {
    namespace graphics {

        Layer::Layer(std::unique_ptr<BatchRenderer2D> renderer, std::unique_ptr<Shader> shader,
                     const math::mat4 &projMatrix) : mRenderer2D(std::move(renderer)), mShader(std::move(shader)),
                                                     mProjectionMatrix(projMatrix) {
            mShader->enable();
            mShader->setUniformMat4("pr_matrix", mProjectionMatrix);
            mShader->disable();
        }

        void Layer::renderSprites(const entities::World &world) {
            begin();

            auto layerID = getLayerID();

            unsigned long entityIndex = 0;

            for (const auto &entity: world.getEntities()) {
                if ((world.getEntityLayerID(entityIndex).layerID == layerID &&
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
                if ((world.getEntityLayerID(entityIndex).layerID == layerID &&
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
                if ((world.getEntityLayerID(entityIndex).layerID == layerID &&
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

        std::unique_ptr<Layer> Layer::createTileLayer(unsigned long maxSpriteCount, std::string &&vertexShader,
                                                      std::string &&fragmentShader) {
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
            auto projMatrix = math::mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f);
            return std::make_unique<Layer>(std::move(renderer), std::move(shader), projMatrix);
        }

        std::unique_ptr<Layer> Layer::createTexturedTileLayer(unsigned long maxSpriteCount,
                                                              std::string &&vertexShader,
                                                              std::string &&fragmentShader) {
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

            auto projMatrix = math::mat4::orthographic(0.0f, 16.0f, 0.0f, 9.0f, -1.0f, 1.0f);

            shader->enable();
            shader->setUniformiv("samplers", renderer->generateSamplerIDs());
            shader->disable();

            return std::make_unique<Layer>(std::move(renderer), std::move(shader), projMatrix);
        }
    }
}

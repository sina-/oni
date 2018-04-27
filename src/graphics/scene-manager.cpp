#include <oni-core/graphics/scene-manager.h>
#include <oni-core/graphics/shader.h>
#include <oni-core/graphics/renderer-2d.h>
#include <oni-core/graphics/batch-renderer-2d.h>
#include <oni-core/entities/basic-entity-repo.h>
#include <oni-core/common/consts.h>
#include <oni-core/io/output.h>
#include <oni-core/physics/translation.h>
#include <oni-core/entities/vehicle-entity-repo.h>

namespace oni {
    namespace graphics {

        SceneManager::SceneManager(const components::ScreenBounds &screenBounds) :
        // 64k vertices
                mMaxSpriteCount(16 * 1000) {

            mProjectionMatrix = math::mat4::orthographic(screenBounds.xMin, screenBounds.xMax, screenBounds.yMin,
                                                         screenBounds.yMax, -1.0f, 1.0f);
            mViewMatrix = math::mat4::identity();

            mModelMatrix = math::mat4::identity();

            // TODO: Resources are not part of oni-core library! This structure as is not flexible, meaning
            // I am forcing the users to only depend on built-in shaders. I should think of a better way
            // to provide flexibility in type of shaders users can define and expect to just work by having buffer
            // structure and what not set up automatically.
            auto textureShader = std::make_unique<graphics::Shader>("resources/shaders/texture.vert",
                                                                    "resources/shaders/texture.frag");
            initializeTextureRenderer(*textureShader);
            mShaderCache.emplace(components::VertexType::TEXTURE_VERTEX, textureShader->getProgram());
            mShaders.emplace(textureShader->getProgram(), std::move(textureShader));
            // TODO: As it is now, BatchRenderer is coupled with the Shader. It requires the user to setup the
            // shader prior to render series of calls. Maybe shader should be built into the renderer.

            auto colorShader = std::make_unique<graphics::Shader>("resources/shaders/basic.vert",
                                                                  "resources/shaders/basic.frag");
            initializeColorRenderer(*colorShader);
            mShaderCache.emplace(components::VertexType::COLOR_VERTEX, colorShader->getProgram());
            mShaders.emplace(colorShader->getProgram(), std::move(colorShader));
        }

        SceneManager::~SceneManager() = default;

        // TODO: What I really need to send back to the users is a renderer ID, which could be under the hood just
        // program ID, but the users don't really need to know about shader. Rename all the references to renderer ID.
        components::ShaderID SceneManager::requestShaderID(const components::VertexType &vertexType) {
            auto shaderID = mShaderCache.at(vertexType);
            // Just to make sure there is a renderer that can take care of this type of shader.
            mRenderers2D.at(shaderID);
            return shaderID;
        }

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

            auto renderer = std::make_unique<BatchRenderer2D>(
                    mMaxSpriteCount,
                    common::maxNumTextureSamplers,
                    sizeof(components::ColoredVertex),
                    std::move(bufferStructures));

            mRenderers2D.emplace(program, std::move(renderer));
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
                    // TODO: This is a problem
                    common::maxNumTextureSamplers,
                    sizeof(components::TexturedVertex),
                    std::move(bufferStructures));

            shader.enable();
            shader.setUniformiv("samplers", renderer->generateSamplerIDs());
            shader.disable();

            mRenderers2D.emplace(program, std::move(renderer));
        }

        void SceneManager::begin(const Shader &shader, Renderer2D &renderer2D) {
            shader.enable();
            // TODO: Merge all these matrices into one and call it ModelViewProjMatrix and use it in the
            // shader. This is to avoid the same matrix multiplication for each vertex in the game over and over.
            shader.setUniformMat4("pr_matrix", mProjectionMatrix);
            shader.setUniformMat4("vw_matrix", mViewMatrix);
            shader.setUniformMat4("ml_matrix", mModelMatrix);
            renderer2D.begin();
        }

        void SceneManager::end(const Shader &shader, Renderer2D &renderer2D) {
            renderer2D.end();
            renderer2D.flush();
            shader.disable();
        }

        void SceneManager::render(const entities::BasicEntityRepo &entityRepo) {
            // TODO: It makes more sense to iterate over renderers when shader and renderer are merged.
            for (auto const &shaderPair : mShaders) {
                auto shader = shaderPair.second.get();
                auto shaderID = shaderPair.first;
                const auto &renderer = mRenderers2D.at(shaderID);
                begin(*shader, *renderer);

                unsigned long entityIndex = 0;

                for (const auto &entity: entityRepo.getEntities()) {
                    auto entityShaderID = entityRepo.getEntityShaderID(entityIndex);
                    if (entityShaderID == shaderID) {
                        if (entities::isSpriteTexturedStatic(entity)) {
                            const auto &placement = entityRepo.getEntityPlacementWorld(entityIndex);
                            const auto &texture = entityRepo.getEntityTexture(entityIndex);
                            renderer->submit(placement, texture);
                        } else if (entities::isSpriteStatic(entity)) {
                            const auto &placement = entityRepo.getEntityPlacementWorld(entityIndex);
                            const auto &appearance = entityRepo.getEntityAppearance(entityIndex);
                            renderer->submit(placement, appearance);
                        } else if (entities::isVehicleTextured(entity)) {
                            // TODO: We know that vehicle entities are stored in entity repositories of type
                            // VehicleEntityRepo, so the cast is fine. But I rather have the design that does not
                            // require this cast. One way to do that is to define:
                            // void SceneManager::render(const entities::VehicleEntityRepo &entityRepo);
                            // and call it separately.
                            const auto &vehicleRepo = dynamic_cast<const entities::VehicleEntityRepo &>(entityRepo);
                            const auto heading = vehicleRepo.getCar(entityIndex).heading;
                            const auto &placement = vehicleRepo.getEntityPlacementWorld(entityIndex);
                            const auto &texture = vehicleRepo.getEntityTexture(entityIndex);

                            // TODO: All this CPU calculations to avoid another draw call for dynamic entities.
                            // Maybe its faster to just reset the view matrix and make a new
                            // draw call instead of this shit.
                            const auto centerX = (placement.vertexA.x + placement.vertexD.x) / 2.0f;
                            const auto centerY = (placement.vertexA.y + placement.vertexB.y) / 2.0f;

                            const auto Ax = placement.vertexA.x - centerX;
                            const auto Bx = placement.vertexB.x - centerX;
                            const auto Cx = placement.vertexC.x - centerX;
                            const auto Dx = placement.vertexD.x - centerX;

                            const auto Ay = placement.vertexA.y - centerY;
                            const auto By = placement.vertexB.y - centerY;
                            const auto Cy = placement.vertexC.y - centerY;
                            const auto Dy = placement.vertexD.y - centerY;

                            const auto cs = std::cos(heading);
                            const auto sn = std::sin(heading);

                            const auto Ax_ = Ax * cs - Ay * sn + centerX;
                            const auto Bx_ = Bx * cs - By * sn + centerX;
                            const auto Cx_ = Cx * cs - Cy * sn + centerX;
                            const auto Dx_ = Dx * cs - Dy * sn + centerX;

                            const auto Ay_ = Ax * sn + Ay * cs + centerY;
                            const auto By_ = Bx * sn + By * cs + centerY;
                            const auto Cy_ = Cx * sn + Cy * cs + centerY;
                            const auto Dy_ = Dx * sn + Dy * cs + centerY;

                            const auto A = math::vec3{Ax_, Ay_, 0.0f};
                            const auto B = math::vec3{Bx_, By_, 0.0f};
                            const auto C = math::vec3{Cx_, Cy_, 0.0f};
                            const auto D = math::vec3{Dx_, Dy_, 0.0f};

                            auto placementFinal = components::Placement{A, B, C, D};

                            const auto &positionInWorld = vehicleRepo.getCar(entityIndex).position;

                            physics::Translation::localToWorld(math::vec3{positionInWorld.x, positionInWorld.y, 0.0f},
                                                               placementFinal);

                            renderer->submit(placementFinal, texture);
                        }
                        else if(entities::isSpriteTexturedDynamic(entity)){
                            // TODO: Render these bad boys.
                        }
                    }
                    ++entityIndex;

                    // TODO: This is a dumb way to handle BatchRenderer's limit. It could be good enough for a long
                    // time before it becomes and issue. Something to keep in mind for future improvements.
                    if (entityIndex >= mMaxSpriteCount) {
                        io::printl("WARNING: Max number of sprites reached. Flushing the current pending sprites!");
                        end(*shader, *renderer);
                        begin(*shader, *renderer);
                    }
                }
                end(*shader, *renderer);
            }
        }

        void SceneManager::lookAt(float x, float y) {
            mViewMatrix = math::mat4::translation(-x, -y, 0.0f);
        }

        void SceneManager::lookAt(float x, float y, float distance) {
            mViewMatrix = math::mat4::scale(math::vec3{distance, distance, 1.0f}) *
                          math::mat4::translation(-x, -y, 0.0f);
        }
    }
}
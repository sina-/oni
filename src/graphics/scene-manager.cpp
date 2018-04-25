#include <oni-core/graphics/scene-manager.h>
#include <oni-core/graphics/shader.h>
#include <oni-core/graphics/renderer-2d.h>
#include <oni-core/graphics/batch-renderer-2d.h>
#include <oni-core/entities/basic-entity-repo.h>
#include <oni-core/common/consts.h>

namespace oni {
    namespace graphics {

        SceneManager::SceneManager(const components::ScreenBounds &screenBounds) :
                mMaxSpriteCount(16 * 1000) {

            mProjectionMatrix = math::mat4::orthographic(screenBounds.xMin, screenBounds.xMax, screenBounds.yMin,
                                                         screenBounds.yMax, -1.0f, 1.0f);
            mViewMatrix = math::mat4::identity();

            mModelMatrix = math::mat4::identity();
        }

        SceneManager::~SceneManager() = default;

        components::ShaderID SceneManager::requestShader(std::string &&vertShader,
                                                         std::string &&fragShader) {
            auto shaderKey = vertShader + fragShader;
            auto search = mShaderCache.find(shaderKey);
            if (search == mShaderCache.end()) {
                auto shader = std::make_unique<graphics::Shader>(std::move(vertShader), std::move(fragShader));

                // TODO: Either rename this function or move this initialization somewhere else as it is really
                // awkward and unexpected to get a renderer created when asking for a shader.
                // Probably in the future when I have a better grasp of what kind of renderers I need and how they
                // are created to be used by SceneManager in a smarter way I can make a better decision as to where
                // to move this call.
                // TODO: What kind of renderer?
                // TODO: As it is now, BatchRenderer is coupled with the Shader. It requires the user to setup the
                // shader prior to render series of calls. Maybe shader should be built into the renderer.
                initializeRenderer(*shader);

                auto shaderID = shader->getProgram();
                mShaderCache.emplace(shaderKey, shaderID);
                mShaders.emplace(shaderID, std::move(shader));
                return shaderID;
            } else {
                return search->second;
            }
        }

        void SceneManager::initializeRenderer(const Shader &shader) {

            auto program = shader.getProgram();

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

            mRenderer2D = std::make_unique<BatchRenderer2D>(mMaxSpriteCount,
                    // TODO: This is a problem
                                                            common::maxNumTextureSamplers,
                                                            sizeof(components::TexturedVertex),
                                                            std::move(bufferStructures));

            shader.enable();
            shader.setUniformiv("samplers", mRenderer2D->generateSamplerIDs());
            shader.disable();
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
                begin(*shader, *mRenderer2D);

                unsigned long entityIndex = 0;

                for (const auto &entity: entityRepo.getEntities()) {
                    if ((entityRepo.getEntityShaderID(entityIndex) == shaderID &&
                         (entity & entities::TexturedSpriteStatic) == entities::TexturedSpriteStatic)) {

                        auto placement = entityRepo.getEntityPlacementWorld(entityIndex);
                        const auto &texture = entityRepo.getEntityTexture(entityIndex);

                        mRenderer2D->submit(placement, texture);
                    }
                    ++entityIndex;
                }
                end(*shader, *mRenderer2D);
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
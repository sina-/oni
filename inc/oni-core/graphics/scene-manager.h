#pragma once

#include <map>
#include <memory>

#include <oni-core/components/visual.h>
#include <oni-core/math/mat4.h>

namespace oni {
    namespace entities {
        class BasicEntityRepo;
    }

    namespace graphics {
        class Shader;
        class BatchRenderer2D;
        class Renderer2D;

        class SceneManager {
        public:
            explicit SceneManager(const components::ScreenBounds &screenBounds);

            ~SceneManager();

            components::ShaderID requestShader(std::string &&vertShader, std::string &&fragShader);

            void render(const entities::BasicEntityRepo &entityRepo);

            // TODO: What about a camera class?
            void lookAt(float x, float y);

            void lookAt(float x, float y, float distance);

        private:
            void begin(const Shader& shader, Renderer2D& renderer2D);

            void end(const Shader &shader, Renderer2D &renderer2D);

            void initializeRenderer(const Shader& shader);

        private:
            std::map<std::string, components::ShaderID> mShaderCache{};
            std::map<components::ShaderID, std::unique_ptr<Shader>> mShaders{};

            std::unique_ptr<BatchRenderer2D> mRenderer2D;

            math::mat4 mModelMatrix{};
            math::mat4 mViewMatrix{};
            math::mat4 mProjectionMatrix{};

            // 64k vertices
            const unsigned int mMaxSpriteCount{0};

        };
    }
}

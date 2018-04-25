#pragma once

#include <map>
#include <memory>

#include <oni-core/components/visual.h>
#include <oni-core/components/buffer.h>
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

            components::ShaderID requestShaderID(const components::VertexType &shaderType);

            void render(const entities::BasicEntityRepo &entityRepo);

            // TODO: What about a camera class?
            void lookAt(float x, float y);

            void lookAt(float x, float y, float distance);

        private:
            void begin(const Shader &shader, Renderer2D &renderer2D);

            void end(const Shader &shader, Renderer2D &renderer2D);

            void initializeTextureRenderer(const Shader &shader);

            void initializeColorRenderer(const Shader &shader);

        private:
            // Users of this class request a renderer for a given VertexType and we keep a 1-to-1 mapping between
            // VertexType and Shader+Renderer associated with that type.
            std::map<components::VertexType, components::ShaderID> mShaderCache{};
            std::map<components::ShaderID, std::unique_ptr<Shader>> mShaders{};
            std::map<components::ShaderID, std::unique_ptr<BatchRenderer2D>> mRenderers2D{};

            math::mat4 mModelMatrix{};
            math::mat4 mViewMatrix{};
            math::mat4 mProjectionMatrix{};

            const unsigned int mMaxSpriteCount{0};

        };
    }
}

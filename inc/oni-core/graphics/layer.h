#pragma once

#include <ftgl/texture-atlas.h>
#include <ftgl/texture-font.h>

#include <oni-core/graphics/renderer-2d.h>
#include <oni-core/graphics/batch-renderer-2d.h>
#include <oni-core/graphics/font-manager.h>
#include <oni-core/graphics/shader.h>
#include <oni-core/math/mat4.h>
#include <oni-core/components/visual.h>
#include <oni-core/entities/basic-entity-repo.h>

namespace oni {
    namespace graphics {

        static const unsigned long mMaxNumTextureSamplers = 32;

        struct ScreenBounds {
            float xMin;
            float xMax;
            float yMin;
            float yMax;
        };

        /**
         * A Layer is the composition that renders entities in the world. Each entity
         * is assigned a layer. Every rendering property that affects group of entities is defined in
         * this class, e.g., shader, to avoid duplication and unnecessary context switches.
         */
        class Layer {
            // TODO: Using BatchRenderer2D instead of Renderer2D for x0.7 speed up by avoiding
            // polymorphism. Do I really need Renderer2D interface?
            std::unique_ptr<BatchRenderer2D> mRenderer2D;
            std::unique_ptr<Shader> mShader;
            graphics::ScreenBounds mScreenBound;
            math::mat4 mModalMatrix;
            math::mat4 mViewMatrix;
            math::mat4 mProjectionMatrix;


        public:
            Layer(std::unique_ptr<BatchRenderer2D> renderer, std::unique_ptr<Shader> shader,
                  const graphics::ScreenBounds &screenBound, const math::mat4 &modalMatrix,
                  const math::mat4 &viewMatrix,
                  const math::mat4 &projectionMatrix);

            ~Layer() = default;

            void renderSprites(const entities::BasicEntityRepo &basicEntityRepo);

            void renderTexturedSprites(const entities::BasicEntityRepo &basicEntityRepo);

            void renderText(const entities::BasicEntityRepo &basicEntityRepo);

            Shader &getShader() { return *mShader; }

            void begin() const;

            void end() const;

            void lookAt(float x, float y);

            void orientTo(float x, float y, float degree);

            const math::mat4 &getModelMatrix() const;

            void setModelMatrix(const math::mat4 &mModelMatrix);

            const math::mat4 &getViewMatrix() const;

            void setViewMatrix(const math::mat4 &mViewMatrix);

            const math::mat4 &getProjectionMatrix() const;

            void setProjectionMatrix(const math::mat4 &mProjectionMatrix);

            /**
             * This function should always return a unique ID.
             *
             * @return unique ID
             */
            components::LayerID getLayerID() { return mShader->getProgram(); }

        public:
            /**
             * A tile layer is a layer with top down projection matrix.
             * @param maxSpriteCount tile layer uses batch rendering, specify maximum number of sprite that the layer will
             * @param vertexShader path to shader
             * @param fragmentShader path to shader
             * hold.
             * @return
             */
            static std::unique_ptr<Layer>
            createTileLayer(unsigned long maxSpriteCount, std::string &&vertexShader, std::string &&fragmentShader,
                            const graphics::ScreenBounds &screenBound);

            /**
             * Similar to tile layer but rendered with textures instead of solid colors.
             * @param maxSpriteCount
             * @return
             */
            static std::unique_ptr<Layer>
            createTexturedTileLayer(unsigned long maxSpriteCount, std::string &&vertexShader,
                                    std::string &&fragmentShader,
                                    const graphics::ScreenBounds &screenBound);

        };


    }
}
#pragma once

#include <ftgl/texture-atlas.h>
#include <ftgl/texture-font.h>

#include <graphics/renderer-2d.h>
#include <graphics/batch-renderer-2d.h>
#include <graphics/font-manager.h>
#include <graphics/shader.h>
#include <math/mat4.h>
#include <components/visual.h>
#include <entities/world.h>

namespace oni {
    namespace graphics {

        static const unsigned long mMaxNumTextureSamplers = 32;

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
            math::mat4 mProjectionMatrix;

        public:
            Layer(std::unique_ptr<BatchRenderer2D>, std::unique_ptr<Shader>, const math::mat4 &);

            ~Layer() = default;

            void renderSprites(const entities::World &world);

            void renderTexturedSprites(const entities::World &world);

            void renderText(const entities::World &world);

            Shader &getShader() { return *mShader; }

            void begin() const;

            void end() const;

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
            createTileLayer(unsigned long maxSpriteCount, std::string &&vertexShader,
                            std::string &&fragmentShader);

            /**
             * Similar to tile layer but rendered with textures instead of solid colors.
             * @param maxSpriteCount
             * @return
             */
            static std::unique_ptr<Layer>
            createTexturedTileLayer(unsigned long maxSpriteCount, std::string &&vertexShader,
                                    std::string &&fragmentShader);

        };


    }
}
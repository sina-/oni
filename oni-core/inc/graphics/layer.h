#pragma once

#include <graphics/renderer-2d.h>
#include <graphics/shader.h>
#include <math/mat4.h>
#include <components/visual.h>
#include <entities/world.h>

namespace oni {
    namespace graphics {

        static const unsigned long m_MaxNumTextureSamplers = 32;

        /**
         * A Layer is the composition that renders entities in the world. Each entity
         * is assigned a layer. Every rendering property that affects group of entities is defined in
         * this class, e.g., shader, to avoid duplication and unnecessary contex switches.
         */
        class Layer {
            std::unique_ptr<Renderer2D> m_Renderer2D;
            std::unique_ptr<Shader> m_Shader;
            math::mat4 m_ProjectionMatrix;

        public:
            Layer(std::unique_ptr<Renderer2D>, std::unique_ptr<Shader>, const math::mat4 &);

            ~Layer() = default;

            void renderSprites(const entities::World &world);

            void renderTexturedSprites(const entities::World &world);

            Shader &getShader() { return *m_Shader; }

            void begin() const;

            void end() const;

            /**
             * This function should always return a unique ID.
             *
             * @return unique ID
             */
            GLuint getLayerID() { return m_Shader->getShaderID(); }

            /**
             * A tile layer is a layer with top down projection matrix.
             * @param maxSpriteCount tile layer uses batch rendering, specify maximum number of sprite that the layer will
             * hold.
             * @return
             */
            static std::unique_ptr<Layer> createTileLayer(unsigned long maxSpriteCount);

            /**
             * A lit tile layer.
             * @param maxSpriteCount
             * hold.
             * @return
             */
            static std::unique_ptr<Layer> createLitTileLayer(unsigned long maxSpriteCount);

            /**
             * Similar to tile layer but rendered with textures instead of solid colors.
             * @param maxSpriteCount
             * @return
             */
            static std::unique_ptr<Layer> createTexturedTileLayer(unsigned long maxSpriteCount);

        };


    }
}
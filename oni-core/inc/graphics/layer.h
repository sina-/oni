#pragma once

#include <graphics/renderer2d.h>
#include <graphics/renderable2d.h>
#include <graphics/shader.h>
#include <math/mat4.h>

namespace oni {
    namespace graphics {

        /**
         * Encapsulation of renderer, shader, and list of renderables, depicting layers such as
         * UI, game objects, text, etc.
         */
        class Layer {
        protected:
            std::unique_ptr<Renderer2D> m_Renderer2D;
            Renderables m_Renderables;
            std::unique_ptr<Shader> m_Shader;
            math::mat4 m_ProjectionMatrix;

        public:
            /**
             * Initialize shader's pr_matrix with projectionMatrix.
             */
            Layer(std::unique_ptr<Renderer2D> renderer, std::unique_ptr<Shader> shader, math::mat4 projectionMatrix);

            virtual ~Layer() = default;

            virtual void add(std::unique_ptr<Renderable2D> renderable) {
                m_Renderables.push_back(std::move(renderable));
            }

            virtual void render();

            virtual const std::unique_ptr<Shader> &getShader() { return m_Shader; }

        };

    }
}
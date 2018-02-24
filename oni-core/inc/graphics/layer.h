#pragma once

#include <graphics/renderer-2d.h>
#include <graphics/shader.h>
#include <math/mat4.h>
#include <components/visual.h>
#include <entities/world.h>

namespace oni {
    namespace graphics {

        class Layer {
        protected:
            std::unique_ptr<Renderer2D> m_Renderer2D;
            std::unique_ptr<Shader> m_Shader;
            math::mat4 m_ProjectionMatrix;

        public:
            /**
             * Initialize shader's pr_matrix with projectionMatrix.
             */
            Layer(std::unique_ptr<Renderer2D> renderer, std::unique_ptr<Shader> shader, math::mat4 projectionMatrix);

            virtual ~Layer() = default;

            virtual void render(const entities::World &world, const components::Mask &mask);

            virtual const std::unique_ptr<Shader> &getShader() { return m_Shader; }

        };

    }
}
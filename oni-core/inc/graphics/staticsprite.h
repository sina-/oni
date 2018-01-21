#pragma once

#include "renderable2d.h"
#include <math/vec3.h>
#include <math/vec2.h>
#include <math/vec4.h>
#include <graphics/shader.h>
#include <buffers/indexbuffer.h>
#include <buffers/vertexarray.h>

namespace oni {
    namespace graphics {
        class StaticSprite : public Renderable2D {

            std::unique_ptr<buffers::VertexArray> m_VertexArray;
            std::unique_ptr<buffers::IndexBuffer> m_IndexBuffer;

            Shader &m_Shader;
        public:
            StaticSprite(const math::vec2 &size, const math::vec3 &pos, const math::vec4 &color, Shader &shader);

            ~StaticSprite() = default;

            // https://stackoverflow.com/questions/15648844/using-smart-pointers-for-class-members
            inline const buffers::VertexArray *getVAO() const { return m_VertexArray.get(); }

            inline const buffers::IndexBuffer *getIBO() const { return m_IndexBuffer.get(); }

            inline const Shader &getShader() const { return m_Shader; }

        };
    }
}
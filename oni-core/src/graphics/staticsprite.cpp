#include <graphics/staticsprite.h>

namespace oni {
    namespace graphics {
        StaticSprite::StaticSprite(const math::vec2 &size, const math::vec3 &pos, const math::vec4 &color,
                                   Shader &shader) : Renderable2D(size, pos, color), m_Shader(shader) {
                auto vertices = std::vector<GLfloat>{
                        0, 0, 0,
                        0, size.y, 0,
                        size.x, size.y, 0,
                        size.x, 0, 0
                };

                auto colors = std::vector<GLfloat>{
                        color.x, color.y, color.z, color.w,
                        color.x, color.y, color.z, color.w,
                        color.x, color.y, color.z, color.w,
                        color.x, color.y, color.z, color.w
                };

                m_VertexArray = std::make_unique<buffers::VertexArray>();

                m_VertexArray->addBuffer(std::make_shared<buffers::Buffer>(vertices, 3), 0);
                m_VertexArray->addBuffer(std::make_shared<buffers::Buffer>(colors, 4), 1);

                auto indices = std::vector<GLushort>{0, 1, 2, 2, 3, 0};
                m_IndexBuffer = std::make_unique<buffers::IndexBuffer>(indices, indices.size());

        }
    }

}

#pragma once

#include <memory>
#include "math/vec2.h"
#include "math/vec3.h"
#include "math/vec4.h"
#include "math/mat4.h"
#include "buffers/indexbuffer.h"
#include "buffers/vertexarray.h"
#include "graphics/shader.h"

namespace granite {
	namespace graphics {
		/*
		 * Its really just a sprite.
		 */
		class Renderable2D {
			math::vec2 m_Size;
			math::vec3 m_Position;
			math::vec4 m_Color;

			math::mat4 m_ModelMatrix;

			std::unique_ptr<VertexArray> m_VertexArray;
			std::unique_ptr<IndexBuffer> m_IndexBuffer;

			Shader & m_Shader;

		public:
			Renderable2D(const math::vec2& size, const math::vec3& pos, const math::vec4& color, Shader& shader)
				: m_Size(size), m_Position(pos), m_Color(color), m_Shader(shader)
			{
				m_ModelMatrix = math::mat4::translation(m_Position);

				auto vertices = std::vector<GLfloat>{
					0, 0, 0,
					0, size.y, 0,
					size.x, size.y, 0,
					size.x, 0, 0
				};
				auto vertexBuffer = std::make_shared<Buffer>(vertices, 3);

				auto colors = std::vector<GLfloat>{
					color.x, color.y, color.z, color.w,
					color.x, color.y, color.z, color.w,
					color.x, color.y, color.z, color.w,
					color.x, color.y, color.z, color.w
				};
				auto colorBuffer = std::make_shared<Buffer>(colors, 4);

				m_VertexArray = std::make_unique<VertexArray>();

				m_VertexArray->addBuffer(vertexBuffer, 0);
				m_VertexArray->addBuffer(colorBuffer, 1);

				auto indices = std::vector<GLushort>{ 0, 1, 2, 2, 3, 0};
				m_IndexBuffer = std::make_unique<IndexBuffer>(indices, indices.size());
			}

			inline const math::vec2 & getSize() const { return m_Size; }
			inline const math::vec3 & getPoision() const { return m_Position; }
			inline const math::vec4 & getColor() const { return m_Color; }
			inline const math::mat4 & getModelMatrix() const { return m_ModelMatrix; }

			inline Shader& getShader() const { return m_Shader; }

			// https://stackoverflow.com/questions/15648844/using-smart-pointers-for-class-members
			inline const VertexArray* getVAO() const { return m_VertexArray.get(); }
			inline const IndexBuffer* getIBO() const { return m_IndexBuffer.get(); }

		};
	}
}
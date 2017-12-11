#pragma once

#include <glew.h>
#include <stdexcept>

namespace granite {
	namespace graphics {
		class VertexBuffer {
			GLuint m_BufferID;
			GLuint m_ComponentCount;
			GLsizei m_Size;

		public:
			VertexBuffer(GLfloat* data, GLsizei count, GLuint componentCount, GLsizei size = 1);
			~VertexBuffer() { glDeleteBuffers(m_Size, &m_BufferID); }

			inline void bind() const { glBindBuffer(GL_ARRAY_BUFFER, m_BufferID); }
			inline void unbind() const { glBindBuffer(GL_ARRAY_BUFFER, 0); }

			inline GLuint getComponentCount() const { return m_ComponentCount; }
		};

	}
}

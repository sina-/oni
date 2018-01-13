#pragma once

#include <GL/glew.h>
#include <stdexcept>
#include <vector>

namespace granite {
	namespace graphics {
		class IndexBuffer {
			GLuint m_BufferID;
			GLuint m_Count;
			GLsizei m_Size;

		public:
			IndexBuffer(const std::vector<GLushort>& data, GLuint count, GLsizei size = 1);
			~IndexBuffer() { glDeleteBuffers(m_Size, &m_BufferID); }

			inline void bind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID); }
			inline void unbind() const { glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); }

			inline GLuint getCount() const { return m_Count; }
		};

	}
}

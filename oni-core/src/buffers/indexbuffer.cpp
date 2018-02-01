#include "buffers/indexbuffer.h"

namespace oni {
	namespace buffers {
		IndexBuffer::IndexBuffer(const std::vector<GLuint>& data, GLuint count, GLsizei size): m_Count(count), m_Size(size)
		{
			auto dataSize = count * sizeof(GLuint);

			glGenBuffers(size, &m_BufferID);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_BufferID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, dataSize, data.data(), GL_STATIC_DRAW);

			GLint actualSize = 0;
			glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &actualSize);
			if (dataSize != actualSize)
			{
				glDeleteBuffers(m_Size, &m_BufferID);
				throw std::runtime_error("Could not allocate index buffer!");
			}

			unbind();

		}

	}
}

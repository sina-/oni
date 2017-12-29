#include "buffers/vertexarray.h"

namespace granite {
	namespace graphics {
		VertexArray::VertexArray()
		{
			glGenVertexArrays(1, &m_ArrayID);
		}

		void VertexArray::addBuffer(const std::shared_ptr<Buffer> vertexBuffer, GLuint index)
		{
			m_VertexBuffers.push_back(vertexBuffer);

			bind();
			vertexBuffer->bind();

			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, vertexBuffer->getComponentCount(), GL_FLOAT, GL_FALSE, 0, nullptr);

			vertexBuffer->unbind();
			unbind();
		}

	}
}

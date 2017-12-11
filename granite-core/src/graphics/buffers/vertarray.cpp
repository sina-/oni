#include "vertarray.h"

namespace granite {
	namespace graphics {
		VertArray::VertArray()
		{
			glGenVertexArrays(1, &m_ArrayID);
		}

		VertArray::~VertArray()
		{
			for (auto buffer: m_VertexBuffers) 
				delete buffer;
		}

		void VertArray::addBuffer(VertexBuffer * vertexBuffer, GLuint index)
		{
			bind();
			vertexBuffer->bind();

			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, vertexBuffer->getComponentCount(), GL_FLOAT, GL_FALSE, 0, 0);

			vertexBuffer->unbind();
			unbind();
		}

	}
}

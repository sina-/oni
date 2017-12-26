#pragma once

//#include <windef.h>
#include <GL/glew.h>
#include <vector>
#include <memory>
#include <utility>
#include "buffers/buffer.h"

namespace granite {
	namespace graphics {

		class VertexArray {
			GLuint m_ArrayID;
			std::vector<std::shared_ptr<Buffer>> m_VertexBuffers;

		public:
			VertexArray();

			void addBuffer(const std::shared_ptr<Buffer> vertexBuffer, GLuint index);
			inline void bind() const { glBindVertexArray(m_ArrayID); }
			inline void unbind() const { glBindVertexArray(0);  }

		};

	}
}
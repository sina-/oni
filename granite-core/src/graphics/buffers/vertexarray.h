#pragma once

#include <glew.h>
#include <vector>
#include <memory>
#include <utility>
#include "buffer.h"

namespace granite {
	namespace graphics {

		class VertexArray {
			GLuint m_ArrayID;
			std::vector<std::shared_ptr<Buffer>> m_VertexBuffers;

		public:
			VertexArray();

			void addBuffer(std::shared_ptr<Buffer> vertexBuffer, GLuint index);
			inline void bind() { glBindVertexArray(m_ArrayID); }
			inline void unbind() { glBindVertexArray(0);  }

		};

	}
}
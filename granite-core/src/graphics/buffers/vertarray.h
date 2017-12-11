#pragma once

#include <glew.h>
#include <vector>
#include "vertexbuffer.h"

namespace granite {
	namespace graphics {

		class VertArray {
			GLuint m_ArrayID;
			// Notice ownership of Buffer by VertArray.
			// TODO: use unique_ptr and std::move to pass the ownership.
			std::vector<VertexBuffer*> m_VertexBuffers;

		public:
			VertArray();
			~VertArray();

			void addBuffer(VertexBuffer * vertexBuffer, GLuint index);
			inline void bind() { glBindVertexArray(m_ArrayID); }
			inline void unbind() { glBindVertexArray(0);  }

		};

	}
}
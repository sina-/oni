#pragma once

#include <glew.h>
#include <vector>
#include "../utils/file.h"

namespace granite {
	namespace graphics {
		class Shader {
			GLuint m_ShaderID;
			const char* m_VertPath;
			const char* m_FragPath;

			GLuint load();

		public:
			Shader(const char* vertPath, const char* fragPath);
			~Shader() { glDeleteProgram(m_ShaderID); };

			inline void enable() const { glUseProgram(m_ShaderID); }
				
			void disable() const { glUseProgram(0); }

		};
	}
}

#include "shader.h"

namespace granite {
	namespace graphics {
		Shader::Shader(const char* vertPath, const char* fragPath) :
			m_VertPath(vertPath), m_FragPath(fragPath)
		{
			m_ShaderID = load();
		}

		GLuint Shader::load()
		{
			GLuint program = glCreateProgram();
			GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
			GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

			auto vertSource = read_file(m_VertPath);
			auto fragSource = read_file(m_FragPath);

			auto vertSourceChar = vertSource.c_str();
			auto fragSourceChar = fragSource.c_str();

			glShaderSource(vertex, 1, &vertSourceChar, NULL);
			glCompileShader(vertex);

			glShaderSource(fragment, 1, &fragSourceChar, NULL);
			glCompileShader(fragment);

			auto checkStatusOrThrow = [](GLuint shaderID) {
				GLint status;
				glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);

				if (status == GL_FALSE) {
					GLint length;
					glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
					std::vector<char> error(length);
					glGetShaderInfoLog(shaderID, length, &length, &error[0]);
					glDeleteShader(shaderID);
					throw std::runtime_error(&error[0]);
				}

			};

			checkStatusOrThrow(vertex);
			checkStatusOrThrow(fragment);

			glAttachShader(program, vertex);
			glAttachShader(program, fragment);

			glLinkProgram(program);
			glValidateProgram(program);

			glDeleteShader(vertex);
			glDeleteShader(fragment);

			return program;
		}

		GLint Shader::getUniformLocation(const GLchar * name)
		{
			// TODO: Slow operation, need caching
			return glGetUniformLocation(m_ShaderID, name);
		}
	}
}

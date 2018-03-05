#include <graphics/utils/check-ogl-errors.h>
#include <utils/io.h>
#include "graphics/shader.h"

namespace oni {
    namespace graphics {
        Shader::Shader(std::string &&vertPath, std::string &&fragPath) :
                m_VertPath(std::move(vertPath)), m_FragPath(std::move(fragPath)) {

            glEnable(GL_DEBUG_OUTPUT);
            glDebugMessageCallback((GLDEBUGPROC) &messageCallback, nullptr);

            GLuint program = glCreateProgram();
            GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
            GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

            auto vertSource = read_file(m_VertPath);
            auto fragSource = read_file(m_FragPath);

            auto vertSourceChar = vertSource.c_str();
            auto fragSourceChar = fragSource.c_str();

            glShaderSource(vertex, 1, &vertSourceChar, nullptr);
            glCompileShader(vertex);

            glShaderSource(fragment, 1, &fragSourceChar, nullptr);
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

            m_ShaderID = program;

            // Test if the shader can be linked.
            enable();
            disable();

        }

        GLint Shader::getUniformLocation(const GLchar *name) {
            // TODO: Slow operation, need caching
            auto location = glGetUniformLocation(m_ShaderID, name);
            if (location == -1) {
                throw std::runtime_error("Invalid uniform name.");
            }
            return location;
        }


        void Shader::messageCallback(GLenum source,
                                     GLenum type,
                                     GLuint id,
                                     GLenum severity,
                                     GLsizei length,
                                     const GLchar *message,
                                     const void *userParam) {
            if (type == GL_DEBUG_TYPE_ERROR) {
                fprintf(stderr, "GL CALLBACK: type = 0x%x, severity = 0x%x, message = %s\n",
                        type, severity, message);
                throw std::runtime_error("OpenGL error!");
            }
            else {
                // TODO: Other type of errors can be logged depending on logging level.
            }
        }

    }
}

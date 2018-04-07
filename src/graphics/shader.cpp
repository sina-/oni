#include <oni-core/graphics/utils/check-ogl-errors.h>
#include <oni-core/io/input.h>
#include <oni-core/graphics/shader.h>

namespace oni {
    namespace graphics {
        Shader::Shader(std::string &&vertPath, std::string &&fragPath) :
                mVertPath(std::move(vertPath)), mFragPath(std::move(fragPath)) {

            GLuint program = glCreateProgram();
            GLuint vertex = glCreateShader(GL_VERTEX_SHADER);
            GLuint fragment = glCreateShader(GL_FRAGMENT_SHADER);

            auto vertSource = read_file(mVertPath);
            auto fragSource = read_file(mFragPath);

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

            mProgram = program;

            // Test if the shader can be linked.
            enable();
            disable();

        }

        GLint Shader::getUniformLocation(const GLchar *name) {
            // TODO: Slow operation, need caching
            auto location = glGetUniformLocation(mProgram, name);
            if (location == -1) {
                throw std::runtime_error("Invalid uniform name.");
            }
            return location;
        }

    }
}

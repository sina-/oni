#include <oni-core/graphic/oni-graphic-shader.h>

#include <GL/glew.h>
#include <cassert>

namespace oni {
    namespace graphic {
        Shader::Shader(std::string_view vertPath,
                       std::string_view geomPath,
                       std::string_view fragPath) :
                mVertPath(vertPath), mGeomPath(geomPath), mFragPath(fragPath) {
            auto program = glCreateProgram();

            auto vertex = glCreateShader(GL_VERTEX_SHADER);
            auto fragment = glCreateShader(GL_FRAGMENT_SHADER);

            auto vertSource = read_file(mVertPath);
            auto fragSource = read_file(mFragPath);

            auto vertSourceChar = vertSource.c_str();
            auto fragSourceChar = fragSource.c_str();

            glShaderSource(vertex, 1, &vertSourceChar, nullptr);
            glCompileShader(vertex);

            glShaderSource(fragment, 1, &fragSourceChar, nullptr);
            glCompileShader(fragment);

            auto checkStatusOrThrow = [](common::oniGLuint shaderID) {
                common::oniGLint status;
                glGetShaderiv(shaderID, GL_COMPILE_STATUS, &status);

                if (status == GL_FALSE) {
                    common::oniGLint length;
                    glGetShaderiv(shaderID, GL_INFO_LOG_LENGTH, &length);
                    std::vector<char> error(static_cast<common::u32>(length));
                    glGetShaderInfoLog(shaderID, length, &length, &error[0]);
                    glDeleteShader(shaderID);
                    std::cout << std::string(error.data()) << "\n";
                    assert(false);
                }
            };

            common::oniGLuint geometry = 0;
            bool doGeometryShading = !geomPath.empty();
            if (doGeometryShading) {
                geometry = glCreateShader(GL_GEOMETRY_SHADER);
                auto geomSource = read_file(mGeomPath);
                auto geomSourceChar = geomSource.c_str();
                glShaderSource(geometry, 1, &geomSourceChar, nullptr);
                glCompileShader(geometry);
                checkStatusOrThrow(geometry);
            }

            checkStatusOrThrow(vertex);
            checkStatusOrThrow(fragment);

            glAttachShader(program, vertex);
            glAttachShader(program, fragment);
            if (doGeometryShading) {
                glAttachShader(program, geometry);
            }

            glLinkProgram(program);
            glValidateProgram(program);

            mProgram = program;

            // Test if the shader can be linked.
            enable();
            disable();
        }

        Shader::~Shader() {
            glDeleteProgram(mProgram);
        }

        common::oniGLint
        Shader::getUniformLocation(const common::oniGLchar *name) const {
            // TODO: Slow operation, need caching
            auto location = glGetUniformLocation(mProgram, name);
            assert(location != -1);
            return location;
        }

        void
        Shader::enable() const {
            glUseProgram(mProgram);
        }

        void
        Shader::disable() const {
            glUseProgram(0);
        }

        common::oniGLuint
        Shader::getProgram() const {
            return mProgram;
        }

        void
        Shader::setUniform1f(const common::oniGLchar *name,
                             oni::common::r32 value) const {
            glUniform1f(getUniformLocation(name), value);
        }

        void
        Shader::setUniform2f(const common::oniGLchar *name,
                             const math::vec2 &vector) const {
            glUniform2f(getUniformLocation(name), vector.x, vector.y);
        }

        void
        Shader::setUniform3f(const common::oniGLchar *name,
                             const math::vec3 &vector) const {
            glUniform3f(getUniformLocation(name), vector.x, vector.y, vector.z);
        }

        void
        Shader::setUniform4f(const common::oniGLchar *name,
                             const math::vec4 &vector) const {
            glUniform4f(getUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
        }

        void
        Shader::setUniform1i(const common::oniGLchar *name,
                             common::i32 value) const {
            glUniform1i(getUniformLocation(name), value);
        }

        void
        Shader::setUniformMat4(const common::oniGLchar *name,
                               const math::mat4 &matrix) const {
            glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, matrix.getArray());
        }

        void
        Shader::setUniformiv(const common::oniGLchar *name,
                             const std::vector<GLint> &textureIDs) const {
            glUniform1iv(getUniformLocation(name), static_cast<common::oniGLsizei>(textureIDs.size()),
                         textureIDs.data());
        }

        void
        Shader::setUniformuiv(const common::oniGLchar *name,
                              const std::vector<common::oniGLuint> &textureIDs) const {
            glUniform1uiv(getUniformLocation(name), static_cast<common::oniGLsizei>(textureIDs.size()),
                          textureIDs.data());
        }

        void
        Shader::setUniformui(const common::oniGLchar *name,
                             common::oniGLuint textureID) const {
            glUniform1ui(getUniformLocation(name), textureID);
        }

    }
}

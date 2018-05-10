#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>

#include <oni-core/utils/file.h>
#include <oni-core/math/vec2.h>
#include <oni-core/math/vec3.h>
#include <oni-core/math/vec4.h>
#include <oni-core/math/mat4.h>
#include <oni-core/graphics/utils/check-ogl-errors.h>

namespace oni {
    namespace graphics {
        class Shader {
            GLuint mProgram;
            const std::string mVertPath;
            const std::string mFragPath;

            GLint getUniformLocation(const GLchar *name) const;

        public:
            Shader(std::string &&vertPath, std::string &&fragPath);

            ~Shader() { glDeleteProgram(mProgram); };

            Shader(const Shader &) = delete;

            Shader &operator=(Shader &) = delete;

            /*
             * Have to call this before using a shader.
             */
            void enable() const { glUseProgram(mProgram); }

            void disable() const { glUseProgram(0); }

            GLuint getProgram() const { return mProgram; }

            void setUniform1f(const GLchar *name, float value) const {
                glUniform1f(getUniformLocation(name), value);
            };

            void setUniform2f(const GLchar *name, const math::vec2 &vector) const {
                glUniform2f(getUniformLocation(name), vector.x, vector.y);
            };

            void setUniform3f(const GLchar *name, const math::vec3 &vector) const {
                glUniform3f(getUniformLocation(name), vector.x, vector.y, vector.z);
            };

            void setUniform4f(const GLchar *name, const math::vec4 &vector) const {
                glUniform4f(getUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
            };

            void setUniform1i(const GLchar *name, common::int32 value) const {
                glUniform1i(getUniformLocation(name), value);
            };

            void setUniformMat4(const GLchar *name, const math::mat4 &matrix) const {
                glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, matrix.getArray());
            };

            void setUniformiv(const GLchar *name, const std::vector<GLint> &textureIDs) const {
                glUniform1iv(getUniformLocation(name), static_cast<GLsizei>(textureIDs.size()), textureIDs.data());
            }

            void setUniformuiv(const GLchar *name, const std::vector<GLuint> &textureIDs) const {
                glUniform1uiv(getUniformLocation(name), static_cast<GLsizei>(textureIDs.size()), textureIDs.data());
            }

            void setUniformui(const GLchar *name, const GLuint textureID) const {
                glUniform1ui(getUniformLocation(name), textureID);
            }

        };
    }
}

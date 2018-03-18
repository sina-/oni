#pragma once

#include <GL/glew.h>
#include <vector>
#include <string>

#include <utils/file.h>
#include <math/vec2.h>
#include <math/vec3.h>
#include <math/vec4.h>
#include <math/mat4.h>
#include <graphics/utils/check-ogl-errors.h>

namespace oni {
    namespace graphics {
        class Shader {
            GLuint mProgram;
            const std::string mVertPath;
            const std::string mFragPath;

            static void messageCallback(GLenum source,
                                        GLenum type,
                                        GLuint id,
                                        GLenum severity,
                                        GLsizei length,
                                        const GLchar *message,
                                        const void *userParam);

            GLint getUniformLocation(const GLchar *name);

        public:
            Shader(std::string &&vertPath, std::string &&fragPath);

            ~Shader() { glDeleteProgram(mProgram); };

            /*
             * Have to call this before using a shader.
             */
            void enable() const { glUseProgram(mProgram); }

            void disable() const { glUseProgram(0); }

            const GLuint getProgram() const { return mProgram; }

            void setUniform1f(const GLchar *name, float value) {
                glUniform1f(getUniformLocation(name), value);
            };

            void setUniform2f(const GLchar *name, const math::vec2 &vector) {
                glUniform2f(getUniformLocation(name), vector.x, vector.y);
            };

            void setUniform3f(const GLchar *name, const math::vec3 &vector) {
                glUniform3f(getUniformLocation(name), vector.x, vector.y, vector.z);
            };

            void setUniform4f(const GLchar *name, const math::vec4 &vector) {
                glUniform4f(getUniformLocation(name), vector.x, vector.y, vector.z, vector.w);
            };

            void setUniform1i(const GLchar *name, int value) {
                glUniform1i(getUniformLocation(name), value);
            };

            void setUniformMat4(const GLchar *name, const math::mat4 &matrix) {
                glUniformMatrix4fv(getUniformLocation(name), 1, GL_FALSE, matrix.getArray());
            };

            void setUniformiv(const GLchar *name, const std::vector<GLint> &textureIDs) {
                glUniform1iv(getUniformLocation(name), static_cast<GLsizei>(textureIDs.size()), textureIDs.data());
            }

            void setUniformuiv(const GLchar *name, const std::vector<GLuint> &textureIDs) {
                glUniform1uiv(getUniformLocation(name), static_cast<GLsizei>(textureIDs.size()), textureIDs.data());
            }

        };
    }
}

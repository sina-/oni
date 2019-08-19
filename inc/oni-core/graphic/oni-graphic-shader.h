#pragma once

#include <vector>
#include <string>

#include <oni-core/util/oni-util-file.h>
#include <oni-core/math/oni-math-vec2.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/math/oni-math-vec4.h>
#include <oni-core/math/oni-math-mat4.h>
#include <oni-core/common/oni-common-typedefs-graphic.h>


namespace oni {
    class Shader {
    public:
        Shader(std::string_view vertPath,
               std::string_view geomPath,
               std::string_view fragPath);

        ~Shader();

        Shader(const Shader &) = delete;

        Shader &
        operator=(Shader &) = delete;

        void
        enable() const;

        void
        disable() const;

        oniGLuint
        getProgram() const;

        void
        setUniform1f(const oniGLchar *name,
                     r32 value) const;

        void
        setUniform2f(const oniGLchar *name,
                     const vec2 &vector) const;

        void
        setUniform3f(const oniGLchar *name,
                     const vec3 &vector) const;

        void
        setUniform4f(const oniGLchar *name,
                     const vec4 &vector) const;

        void
        setUniform1i(const oniGLchar *name,
                     i32 value) const;

        void
        setUniformMat4(const oniGLchar *name,
                       const mat4 &matrix) const;

        void
        setUniformiv(const oniGLchar *name,
                     const std::vector<oniGLint> &textureIDs) const;

        void
        setUniformuiv(const oniGLchar *name,
                      const std::vector<oniGLuint> &textureIDs) const;

        void
        setUniformui(const oniGLchar *name,
                     oniGLuint textureID) const;

    private:
        oniGLint
        getUniformLocation(const oniGLchar *name) const;

    private:
        oniGLuint mProgram;
        std::string mVertPath;
        std::string mGeomPath;
        std::string mFragPath;
    };
}

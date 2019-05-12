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
    namespace graphic {
        class Shader {
        public:
            Shader(const std::string &vertPath,
                   const std::string &geomPath,
                   const std::string &fragPath);

            ~Shader();

            Shader(const Shader &) = delete;

            Shader &
            operator=(Shader &) = delete;

            void
            enable() const;

            void
            disable() const;

            common::oniGLuint
            getProgram() const;

            void
            setUniform1f(const common::oniGLchar *name,
                         oni::common::real32 value) const;

            void
            setUniform2f(const common::oniGLchar *name,
                         const math::vec2 &vector) const;

            void
            setUniform3f(const common::oniGLchar *name,
                         const math::vec3 &vector) const;

            void
            setUniform4f(const common::oniGLchar *name,
                         const math::vec4 &vector) const;

            void
            setUniform1i(const common::oniGLchar *name,
                         common::int32 value) const;

            void
            setUniformMat4(const common::oniGLchar *name,
                           const math::mat4 &matrix) const;

            void
            setUniformiv(const common::oniGLchar *name,
                         const std::vector<common::oniGLint> &textureIDs) const;

            void
            setUniformuiv(const common::oniGLchar *name,
                          const std::vector<common::oniGLuint> &textureIDs) const;

            void
            setUniformui(const common::oniGLchar *name,
                         common::oniGLuint textureID) const;

        private:
            common::oniGLint
            getUniformLocation(const common::oniGLchar *name) const;

        private:
            common::oniGLuint mProgram;
            std::string mVertPath;
            std::string mGeomPath;
            std::string mFragPath;
        };
    }
}

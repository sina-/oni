#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-mat4.h>

namespace oni {
    namespace component {
        class Appearance;

        class Age;

        union Scale;

        class Velocity;

        class Texture;

        class Text;

        union WorldP3D;

        class Heading;
    }

    namespace math {
        class mat4;
    }

    namespace graphic {
        enum class PrimitiveType : common::u8 {
            UNKNOWN,

            POINT,
            LINE,
            TRIANGLE,
            TRIANGLE_STRIP,

            LAST
        };

        class Renderer2D {
        protected:
            Renderer2D();

            virtual ~Renderer2D();

        public:
            void
            begin(const math::mat4 &model,
                  const math::mat4 &view,
                  const math::mat4 &proj);

            void
            end();

        protected:
            virtual void
            _begin(const math::mat4 &model,
                   const math::mat4 &view,
                   const math::mat4 &proj) = 0;

            virtual void
            _flush() = 0;

            virtual void
            _end() = 0;

        private:
            bool mBegun{false};
        };
    }
}
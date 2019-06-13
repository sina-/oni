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
            POINT = 0,
            LINE = 1,
            TRIANGLE = 2,
            UNKNOWN = 3
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
            submit(const component::WorldP3D &,
                   const component::Heading &,
                   const component::Scale &,
                   const component::Appearance &,
                   const component::Texture &);

            void
            submit(const component::Text &,
                   const component::WorldP3D &);

            void
            flush();

            void
            end();

        protected:
            virtual void
            _begin(const math::mat4 &model,
                   const math::mat4 &view,
                   const math::mat4 &proj) = 0;

            virtual void
            _submit(const component::WorldP3D &,
                    const component::Heading &,
                    const component::Scale &,
                    const component::Appearance &,
                    const component::Texture &) = 0;

            virtual void
            _submit(const component::Text &,
                    const component::WorldP3D &) = 0;

            virtual void
            _flush() = 0;

            virtual void
            _end() = 0;
        };
    }
}
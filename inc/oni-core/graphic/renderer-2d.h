#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {

    namespace component {
        class Appearance;

        class Tessellation;

        class Age;

        class Velocity;

        class Texture;

        class Text;

        class Shape;

        union WorldP3D;

        class Heading;
    }

    namespace graphic {
        enum class PrimitiveType : common::uint8 {
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
            begin();

            void
            submit(const component::Shape &,
                   const component::Appearance &);

            void
            submit(const component::Tessellation &,
                   const component::WorldP3D &,
                   const component::Heading &,
                   const component::Age &,
                   const component::Velocity &,
                   const component::Appearance &);

            void
            submit(const component::Tessellation &,
                   const component::WorldP3D &,
                   const component::Heading &,
                   const component::Age &,
                   const component::Velocity &,
                   const component::Texture &);

            void
            submit(const component::Shape &,
                   const component::Texture &);

            void
            submit(const component::Text &,
                   const component::WorldP3D &);

            void
            flush();

            void
            end();

        private:
            virtual void
            _begin() = 0;

            virtual void
            _submit(const component::Shape &,
                    const component::Appearance &) = 0;

            virtual void
            _submit(const component::Tessellation &,
                    const component::WorldP3D &,
                    const component::Heading &,
                    const component::Age &,
                    const component::Velocity &,
                    const component::Appearance &) = 0;

            virtual void
            _submit(const component::Tessellation &,
                    const component::WorldP3D &,
                    const component::Heading &,
                    const component::Age &,
                    const component::Velocity &,
                    const component::Texture &) = 0;

            virtual void
            _submit(const component::Shape &,
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
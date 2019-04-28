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

        class Placement;
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
            /**
             * Preparation work, if needed.
             */
            void
            begin();

            void
            submit(const component::Shape &,
                   const component::Appearance &);

            void
            submit(const component::Tessellation &,
                   const component::Placement &,
                   const component::Age &,
                   const component::Velocity &,
                   const component::Appearance &);

            void
            submit(const component::Tessellation &,
                   const component::Placement &,
                   const component::Age &,
                   const component::Velocity &,
                   const component::Texture &);

            void
            submit(const component::Shape &,
                   const component::Texture &);

            void
            submit(const component::Text &);

            /**
             * Draw the element(s).
             */
            void
            flush();

            /**
             * Clean up, if needed.
             */
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
                    const component::Placement &,
                    const component::Age &,
                    const component::Velocity &,
                    const component::Appearance &) = 0;

            virtual void
            _submit(const component::Tessellation &,
                    const component::Placement &,
                    const component::Age &,
                    const component::Velocity &,
                    const component::Texture &) = 0;

            virtual void
            _submit(const component::Shape &,
                    const component::Texture &) = 0;

            virtual void
            _submit(const component::Text &) = 0;

            /**
             * Draw the element(s).
             */
            virtual void
            _flush() = 0;

            /**
             * Clean up, if needed.
             */
            virtual void
            _end() = 0;
        };
    }
}
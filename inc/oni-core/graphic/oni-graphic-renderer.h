#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-mat4.h>

namespace oni {
    namespace component {
        class Color;

        class Age;

        union Scale;

        class Velocity;

        class Texture;

        struct CanvasTexture;

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

            POINTS,
            LINES,
            TRIANGLES,
            TRIANGLE_STRIP,

            LAST
        };

        class Renderer {
        protected:
            Renderer();

            virtual ~Renderer();

        public:
            void
            begin(const math::mat4 &model,
                  const math::mat4 &view,
                  const math::mat4 &proj,
                  const math::vec2 &screenSize,
                  common::r32 zoom,
                  component::Texture *renderTarget);

            void
            end();

        protected:
            struct WindowSize {
                common::u32 width{0};
                common::u32 height{0};
            };

        protected:
            virtual void
            _begin(const math::mat4 &model,
                   const math::mat4 &view,
                   const math::mat4 &proj,
                   const math::vec2 &screenSize,
                   common::r32 zoom) = 0;

            virtual void
            _flush(component::Texture *renderTarget) = 0;

            virtual void
            _end() = 0;

            virtual void
            setViewportSize(const WindowSize &) = 0;

            virtual WindowSize
            getViewportSize() = 0;

        protected:
            component::Texture *mRenderTarget{};

        private:
            bool mBegun{false};
            WindowSize mViewportSize{};
        };
    }
}
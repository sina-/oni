#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace entities {
        class EntityManager;
    }
    namespace component {
        class Color;

        class Age;

        union Scale;

        class Velocity;

        class Texture;

        struct Quad;

        class Text;

        union WorldP3D;

        class Heading;
    }

    namespace math {
        class mat4;
        class vec2;
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

        struct Renderable {
            Renderable(common::EntityID _id,
                       entities::EntityManager *_manager,
                       const component::WorldP3D *_pos,
                       const component::Heading *_heading,
                       const component::Scale *_scale,
                       const component::Color *_color,
                       const component::Texture *_texture);

            friend bool
            operator<(const Renderable &left,
                      const Renderable &right);

            friend bool
            operator>(const Renderable &left,
                      const Renderable &right);

            common::EntityID id{};
            entities::EntityManager *manager{};
            const component::WorldP3D *pos{};
            const component::Heading *heading{};
            const component::Scale *scale{};
            const component::Color *color{};
            const component::Texture *texture{};
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

            virtual void
            submit(const Renderable &) = 0;

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
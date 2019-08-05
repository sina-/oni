#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-mat4.h>

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

        struct TextureAnimated;

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

        // TODO: I can probably just let a mix of these instead of creating one enum per combo
        enum class RenderEffect : common::u8 {
            COLOR = 0,
            TEXTURE = 1,
            TINTED = 2,
            SHINNY_COLOR = 3,
            SHINNY_TEXTURE = 4,
            FADE = 5,

            LAST
        };

        struct Renderable {
            Renderable(common::EntityID _id,
                       const entities::EntityManager *_manager,
                       const component::WorldP3D *_pos,
                       const component::Heading *_heading,
                       const graphic::RenderEffect _effect,
                       const component::Scale *_scale,
                       const component::Color *_color,
                       const component::Texture *_texture,
                       const component::TextureAnimated *_animatedTexture);

            friend bool
            operator<(const Renderable &left,
                      const Renderable &right);

            friend bool
            operator>(const Renderable &left,
                      const Renderable &right);

            common::EntityID id{};
            graphic::RenderEffect effect{RenderEffect::COLOR};
            const entities::EntityManager *manager{};
            const component::WorldP3D *pos{};
            const component::Heading *heading{};
            const component::Scale *scale{};
            const component::Color *color{};
            const component::Texture *texture{};
            const component::TextureAnimated *animatedTexture{};
        };

        struct RenderSpec {
            math::mat4 model{};
            math::mat4 view{};
            math::mat4 proj{};
            math::vec2 screenSize{};
            common::r32 zoom{};

            component::Texture *renderTarget{};
            RenderEffect effect{RenderEffect::COLOR};
        };

        class Renderer {
        protected:
            Renderer();

            virtual ~Renderer();

        public:
            void
            begin(const RenderSpec &);

            virtual void
            submit(const Renderable &) = 0;

            void
            end();

        protected:
            struct WindowSize {
                common::u32 width{0};
                common::u32 height{0};
            };

            enum class BlendMode : common::u8 {
                ZERO,
                ONE,
                ONE_MINUS_SRC_ALPHA,

                LAST
            };

            struct BlendSpec {
                BlendMode src{BlendMode::ONE};
                BlendMode dest{BlendMode::ONE};
            };

            component::Texture *mRenderTarget{};

        protected:
            virtual void
            _begin(const RenderSpec &,
                   const BlendSpec &) = 0;

            virtual void
            _flush(component::Texture *renderTarget) = 0;

            virtual void
            _end() = 0;

            virtual void
            setViewportSize(const WindowSize &) = 0;

            virtual WindowSize
            getViewportSize() = 0;

        private:
            static BlendSpec
            getBlendSpec(RenderEffect);

        private:
            bool mBegun{false};
            WindowSize mViewportSize{};
        };
    }
}
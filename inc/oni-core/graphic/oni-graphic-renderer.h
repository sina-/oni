#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/math/oni-math-mat4.h>
#include <oni-core/component/oni-component-visual.h>

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

        struct Renderable {
            friend bool
            operator<(const Renderable &left,
                      const Renderable &right);

            friend bool
            operator>(const Renderable &left,
                      const Renderable &right);

            common::EntityID id{};
            const entities::EntityManager *manager;

            const component::WorldP3D *pos;
            const component::Heading *heading;
            const component::Scale *scale;

            const component::MaterialSurface *material;
            const component::MaterialFinish *finish;
            const component::MaterialTransition_Fade *transitionFade;
            const component::MaterialTransition_Animated *transitionAnimated;
            const component::MaterialTransition_Tint *transitionTint;
            component::MaterialTransition_Type transitionType;
        };

        struct RenderSpec {
            math::mat4 model{};
            math::mat4 view{};
            math::mat4 proj{};
            math::vec2 screenSize{};
            common::r32 zoom{};
            component::Texture *renderTarget{};

            ///

            component::MaterialFinishType finishType{};
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

            struct DepthSpec {
                bool depthWrite{false};
                bool depthRead{false};
            };

            component::Texture *mRenderTarget{};

        protected:
            virtual void
            _begin(const RenderSpec &,
                   const BlendSpec &,
                   const DepthSpec &) = 0;

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
            getBlendSpec(component::MaterialFinishType);

            static DepthSpec
            getDepthSpec(component::MaterialFinishType);

        private:
            bool mBegun{false};
            WindowSize mViewportSize{};
        };
    }
}
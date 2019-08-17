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
            entities::EntityType type{};
            const entities::EntityManager *manager;

            const component::WorldP3D *pos;
            const component::Heading *heading;
            const component::Scale *scale;

            component::MaterialDefinition def;

            const component::MaterialSkin *skin;
            const component::MaterialTransition_Fade *transitionFade;
            const component::MaterialTransition_Animation *transitionAnimation;
            const component::MaterialTransition_Tint *transitionTint;
        };

        struct RenderSpec {
            math::mat4 model{};
            math::mat4 view{};
            math::mat4 proj{};
            math::vec2 screenSize{};
            common::r32 zoom{};
            component::Texture *renderTarget{};

            ///

            component::MaterialFinish_Type finishType{};
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
            getBlendSpec(component::MaterialFinish_Type);

            static DepthSpec
            getDepthSpec(component::MaterialFinish_Type);

        private:
            bool mBegun{false};
            WindowSize mViewportSize{};
        };
    }
}
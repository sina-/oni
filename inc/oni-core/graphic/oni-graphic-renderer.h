#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/component/oni-component-fwd.h>
#include <oni-core/entities/oni-entities-fwd.h>
#include <oni-core/math/oni-math-fwd.h>
#include <oni-core/math/oni-math-mat4.h>

namespace oni {
    enum class PrimitiveType : oni::u8 {
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

        EntityID id{};
        EntityType type{};
        const EntityManager *manager{};

        const WorldP3D *pos{};
        const Orientation *ornt{};
        const Scale *scale{};

        const MaterialSkin *skin{};
        const MaterialText *text{};
        const MaterialTransition_Def *trans{};
    };

    struct RenderSpec {
        mat4 model{};
        mat4 view{};
        mat4 proj{};
        vec2 screenSize{};
        r32 zoom{};
        Texture *renderTarget{};

        ///

        MaterialFinish_Type finishType{};
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
            u32 width{0};
            u32 height{0};
        };

        enum class BlendMode : oni::u8 {
            ZERO,
            ONE,
            SRC_ALPHA,
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

        Texture *mRenderTarget{};

    protected:
        virtual void
        _begin(const RenderSpec &,
               const BlendSpec &,
               const DepthSpec &) = 0;

        virtual void
        _flush(Texture *renderTarget) = 0;

        virtual void
        _end() = 0;

        virtual void
        setViewportSize(const WindowSize &) = 0;

        virtual WindowSize
        getViewportSize() = 0;

    private:
        static BlendSpec
        getBlendSpec(MaterialFinish_Type);

        static DepthSpec
        getDepthSpec(MaterialFinish_Type);

    private:
        bool mBegun{false};
        WindowSize mViewportSize{};
    };
}
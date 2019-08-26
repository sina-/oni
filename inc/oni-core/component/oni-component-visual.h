#pragma once

#include <utility>
#include <vector>
#include <unordered_map>

#include <oni-core/math/oni-math-vec2.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/math/oni-math-vec4.h>
#include <oni-core/math/oni-math-mat4.h>
#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-physics.h>


namespace oni {
    struct Image {
        u16 width{};
        u16 height{};
        std::vector<u8> data{};
        std::string path{};
    };

    enum class NumAnimationFrames : oni::u8 {
        TWO = 2,
        FOUR = 4,
        FIVE = 5,
        TEN = 10,
        TWENTY = 20,
        TWENTYFIVE = 25,
        FIFTY = 50,

        LAST
    };

    enum class AnimationEndingBehavior : oni::u8 {
        LOOP,
        PLAY_AND_STOP,
        PLAY_AND_REMOVE_ENTITY,
    };

    struct UV {
        std::array<vec2, 4> values{vec2{0.f, 0.f}, vec2{0.f, 1.f},
                                   vec2{1.f, 1.f}, vec2{1.f, 0.f}};
    };

    struct Texture {
        Image image{};
        bool clear{false};
        oniGLuint id{0};
        // GL_BGRA          0x80E1
        oniGLenum format{0x80E1};
        // GL_UNSIGNED_BYTE 0x1401
        // GL_FLOAT         0x1406
        oniGLenum type{0x1401};
        UV uv{};
    };

    struct TextureAnimated {
        Texture texture{};
        NumAnimationFrames numFrames{NumAnimationFrames::TWO};
        u8 nextFrame{0};
        bool playing{true};
        AnimationEndingBehavior endingBehaviour{AnimationEndingBehavior::LOOP};
        r64 timeElapsed{0.f};
        r64 frameDuration{0.1f};
        std::vector<UV> frameUV{};

        inline static TextureAnimated
        make(NumAnimationFrames numFrames,
             r32 fps) noexcept {
            TextureAnimated result;
            init(result, numFrames, fps);
            return result;
        }

        inline static void
        init(TextureAnimated &output,
             NumAnimationFrames numFrames,
             r32 fps) {
            output.numFrames = numFrames;
            output.frameDuration = 1.0 / fps;
            output.nextFrame = 0;
            output.timeElapsed = 0;
            output.texture = {};

            auto countFrame = enumCast(numFrames);
            auto xOffset = 0.f;
            auto xWidth = 1.f / countFrame;

            output.frameUV.resize(countFrame);
            for (auto &&uv: output.frameUV) {
                uv.values[0] = {xOffset, 0.f};
                uv.values[1] = {xOffset, 1.f};
                uv.values[2] = {xOffset + xWidth, 1.f};
                uv.values[3] = {xOffset + xWidth, 0.f};

                xOffset += xWidth;
            }
        }
    };

    struct Text {
        EntityID entityID{0};
        r32 xScaling{1.f};
        r32 yScaling{1.f};
        oniGLuint textureID{0};
        std::string textContent{};
        std::vector<size_t> width{};
        std::vector<size_t> height{};
        std::vector<u32> offsetX{};
        std::vector<u32> offsetY{};
        std::vector<r32> advanceX{};
        std::vector<r32> advanceY{};
        std::vector<vec4> uv{};
    };

    struct Color {
        u8
        r() const {
            return value >> 24u;
        }

        r32
        r_r32() const {
            return r() / 255.f;
        }

        u8
        g() const {
            return (value << 8u) >> 24u;
        }

        r32
        g_r32() const {
            return g() / 255.f;
        }

        u8
        b() const {
            return (value << 16u) >> 24u;
        }

        r32
        b_r32() const {
            return b() / 255.f;
        }

        r32
        a_r32() const {
            return a() / 255.f;
        }

        u8
        a() const {
            return (value << 24u) >> 24u;
        }

        inline vec3
        rgb() const {
            return vec3{r() / 255.f, g() / 255.f, b() / 255.f};
        }

        inline vec4
        rgba() const {
            return vec4{r() / 255.f, g() / 255.f, b() / 255.f, a() / 255.f};
        }

        void
        set_r(u8 r) {
            value &= ~R_MASK;
            value |= (u32(r) << 24u);
        }

        void
        set_r(r32 r) {
            value &= ~R_MASK;
            value |= (u32(r * 255) << 24u);
        }

        void
        set_g(u8 g) {
            value &= ~G_MASK;
            value |= (u32(g) << 16u);
        }

        void
        set_g(r32 g) {
            value &= ~G_MASK;
            value |= (u32(g * 255) << 16u);
        }

        void
        set_b(u8 b) {
            value &= ~B_MASK;
            value |= (u32(b) << 8u);
        }

        void
        set_b(r32 b) {
            value &= ~B_MASK;
            value |= (u32(b * 255) << 8u);
        }

        void
        set_a(u8 a) {
            value &= ~A_MASK;
            value |= (u32(a));
        }

        void
        set_a(r32 a) {
            value &= ~A_MASK;
            value |= (u32(a * 255));
        }

        void
        set_rgb(r32 _r,
                r32 _g,
                r32 _b) {
            value &= ~R_MASK;
            value &= ~B_MASK;
            value &= ~G_MASK;
            set_r(_r);
            set_g(_g);
            set_b(_b);
        }

        void
        set_rgb(u8 _r,
                u8 _g,
                u8 _b) {
            value &= ~R_MASK;
            value &= ~G_MASK;
            value &= ~B_MASK;
            set_r(_r);
            set_g(_g);
            set_b(_b);
        }

        void
        set_rgba(r32 _r,
                 r32 _g,
                 r32 _b,
                 r32 _a) {
            value = 0;
            set_rgb(_r, _g, _b);
            set_a(_a);
        }

        void
        set_rgba(u8 _r,
                 u8 _g,
                 u8 _b,
                 u8 _a) {
            value = 0;
            set_rgb(_r, _g, _b);
            set_a(_a);
        }

        static constexpr
        Color
        WHITE() {
            auto color = Color{};
            color.value = (255u << 24u) | (255u << 16u) | (255u << 8u) | (255u);
            return color;
        }

        static constexpr
        Color
        BLACK() {
            auto color = Color{};
            color.value = (0u << 24u) | (0u << 16u) | (0u << 8u) | (255u);
            return color;
        }

        u32 value{255u};
    };

    enum class BrushType : oni::u8 {
        COLOR,
        TEXTURE,
        TEXTURE_TAG,
    };

    // TODO: Probably should be merged with ParticleEmitter with the goal of generic enough Particle Emitter
    // component that covers most of the games needs.
    struct CoolDown {
        r64 current{0.f};
        r64 initial{0.2f};
    };

    struct BrushTrail {
        bool initialized{false};
        bool active{true};
        Texture texture{};
        r32 mass{1.f};
        r32 width{0.4f};
        Heading2D heading{};
        WorldP2D last{};
        WorldP2D lastDelta{};
        WorldP2D current{};
        Velocity2D velocity2d{};
        Velocity velocity{};
        Acceleration acceleration{};
        Acceleration2D acceleration2d{};
        std::vector<Quad> quads{};
    };

    // TODO: Game code
    enum class EntityPreset : oni::u32 {
        UNKNOWN,

        RACE_CAR_DEFAULT,
        RACE_CAR_TIRE_DEFAULT,
        RACE_CAR_TIRE_WITH_TRAIL,
        VEHICLE_GUN_DEFAULT,
        VEHICLE_DEFAULT,
        ROCKET_DEFAULT,
        ROCKET_TRAIL_DEFAULT,

        WALL_VERTICAL,
        WALL_HORIZONTAL,

        ROAD_DEFAULT,

        EXPLOSION_DEFAULT,
        BLAST_ANIMATION_DEFAULT,
        BLAST_PARTICLE_DEFAULT,

        // TODO: I probably want to split the variations into another enum.

        CLOUD_BLACK,
        CLOUD_WHITE,

        ROCKET_FLAME_DEFAULT,

        SMOKE_BLACK,
        SMOKE_WHITE,

        BACKGROUND_DEFAULT,
        BACKGROUND_DEBUG,

        CANVAS,

        LAST
    };

    struct MaterialSkin {
        Texture texture{};
        Color color{};
    };

    enum class MaterialTransition_Type : oni::u8 {
        NONE,
        FADE,
        COLOR,
        TEXTURE,
    };

    enum class FadeFunc : oni::u8 {
        LINEAR,
        TAIL,
    };

    struct MaterialTransition_Fade {
        FadeFunc fadeFunc{FadeFunc::LINEAR};
        r32 factor{1.f};
    };

    struct MaterialTransition_Color {
        Color begin{};
        Color end{};
    };

    struct MaterialTransition_Texture {
        TextureAnimated value{};
    };

    // TODO: Better name
    // TODO: Split this up, MaterialTransparency_Type: SOLID and TRANSLUECENT
    // and SHINNY could turn into MaterialGloss_Type: SHINNY, MATT, or maybe even just MaterialGloss with a
    // float definning how shinny it is. Although I have to keep in mind for shinny entities I do switch the
    // blend function so it can't just be a range of values, it has to be a Type hmmm...
    enum class MaterialFinish_Type : oni::u8 {
        SOLID,
        TRANSLUCENT,
        SHINNY,

        LAST
    };

//        struct MaterialFinish {
//            union {
//                common::r32 solidness;
//                common::r32 translecency;
//                common::r32 shinniness;
//            };
//            MaterialFinishType type;
//
//            auto
//            typeID() const {
//                return enumCast(type);
//            }
//        };
//
    struct MaterialDefinition {
        MaterialTransition_Type transition{};
        MaterialFinish_Type finish{};
    };

    struct GrowInTime {
        duration period{0.2f}; // NOTE: Grow every period
        duration elapsed{0.f}; // NOTE: Since last growth
        r32 factor{0.1f}; // NOTE: add this much to current size
        Scale initialSize{1, 1};
        Scale maxSize{1, 1};
    };

    struct ParticleEmitter {
        EntityPreset tag{};
        r32 size = 0.1f;
        u8 count = 1;
        GrowInTime growth{};
    };

    struct AfterMark {
        Scale scale{1, 1};
        BrushType type{BrushType::COLOR};
        union {
            EntityPreset tag = {};
            Color color;
        };
    };
}
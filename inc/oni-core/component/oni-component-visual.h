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
    namespace component {
        struct Image {
            common::u16 width{};
            common::u16 height{};
            std::vector<common::u8> data{};
            std::string path{};
        };

        // TODO: This seems to be just a way to get around the fact that server can not set values on Complementary
        // components, so imagine that was possible, then this would be part of Texture component as it is pretty
        // much useless on its own.
        enum class TextureTag : common::u32 {
            UNKNOWN,

            BACKGROUND_CHUNK,
            BACKGROUND_WHITE,

            RACE_CAR,
            TRUCK,
            TIRE,
            VEHICLE_GUN,
            SMOKE_WHITE,
            SMOKE_BLACK,
            CLOUD_WHITE,
            CLOUD_BLACK,
            ROCKET,
            ROCKET_TRAIL,
            ROAD,
            WALL_VERTICAL,
            WALL_HORIZONTAL,
            EXPLOSION,

            TEST_TEXTURE,
            ROCK,
            SPARK,

            BLAST_TEXTURE_ANIMATED,

            LAST
        };

        enum class NumAnimationFrames : common::u8 {
            TWO = 2,
            FOUR = 4,
            FIVE = 5,
            TEN = 10,
            TWENTY = 20,
            TWENTYFIVE = 25,
            FIFTY = 50,

            LAST
        };

        enum class AnimationEndingBehavior : common::u8 {
            LOOP,
            PLAY_AND_STOP,
            PLAY_AND_REMOVE_ENTITY,

            LAST
        };

        struct UV {
            std::array<math::vec2, 4> values{math::vec2{0.f, 0.f}, math::vec2{0.f, 1.f},
                                             math::vec2{1.f, 1.f}, math::vec2{1.f, 0.f}};
        };

        struct Texture {
            Image image{};
            bool clear{false};
            common::oniGLuint textureID{0};
            // GL_BGRA          0x80E1
            common::oniGLenum format{0x80E1};
            // GL_UNSIGNED_BYTE 0x1401
            // GL_FLOAT         0x1406
            common::oniGLenum type{0x1401};
            UV uv{};
        };

        struct TextureAnimated {
            Texture texture{};
            NumAnimationFrames numFrames{NumAnimationFrames::TWO};
            common::u8 nextFrame{0};
            bool playing{true};
            AnimationEndingBehavior endingBehaviour{AnimationEndingBehavior::LOOP};
            common::r64 timeElapsed{0.f};
            common::r64 frameDuration{0.1f};
            std::vector<UV> frameUV{};

            inline static TextureAnimated
            make(NumAnimationFrames numFrames,
                 common::r32 fps) noexcept {
                TextureAnimated result;
                init(result, numFrames, fps);
                return result;
            }

            inline static void
            init(TextureAnimated &output,
                 NumAnimationFrames numFrames,
                 common::r32 fps) {
                output.numFrames = numFrames;
                output.frameDuration = 1.0 / fps;
                output.nextFrame = 0;
                output.timeElapsed = 0;
                output.texture = {};

                auto countFrame = math::enumCast(numFrames);
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
            common::EntityID entityID{0};
            common::r32 xScaling{1.f};
            common::r32 yScaling{1.f};
            common::oniGLuint textureID{0};
            std::string textContent{};
            std::vector<size_t> width{};
            std::vector<size_t> height{};
            std::vector<common::u32> offsetX{};
            std::vector<common::u32> offsetY{};
            std::vector<common::r32> advanceX{};
            std::vector<common::r32> advanceY{};
            std::vector<math::vec4> uv{};
        };

        struct ParticleEmitter {
            TextureTag textureTag = TextureTag::UNKNOWN;
            common::r32 size = 0.1f;
        };

        struct Color {
            common::u8
            r() const {
                return value >> 24u;
            }

            common::r32
            r_r32() const {
                return r() / 255.f;
            }

            common::u8
            g() const {
                return (value << 8u) >> 24u;
            }

            common::r32
            g_r32() const {
                return g() / 255.f;
            }

            common::u8
            b() const {
                return (value << 16u) >> 24u;
            }

            common::r32
            b_r32() const {
                return b() / 255.f;
            }

            common::r32
            a_r32() const {
                return a() / 255.f;
            }

            common::u8
            a() const {
                return (value << 24u) >> 24u;
            }

            inline math::vec3
            rgb() const {
                return math::vec3{r() / 255.f, g() / 255.f, b() / 255.f};
            }

            inline math::vec4
            rgba() const {
                return math::vec4{r() / 255.f, g() / 255.f, b() / 255.f, a() / 255.f};
            }

            void
            set_r(common::u8 r) {
                value &= ~common::R_MASK;
                value |= (common::u32(r) << 24u);
            }

            void
            set_r(common::r32 r) {
                value &= ~common::R_MASK;
                value |= (common::u32(r * 255) << 24u);
            }

            void
            set_G(common::u8 g) {
                value &= ~common::G_MASK;
                value |= (common::u32(g) << 16u);
            }

            void
            set_g(common::r32 g) {
                value &= ~common::G_MASK;
                value |= (common::u32(g * 255) << 16u);
            }

            void
            set_b(common::u8 b) {
                value &= ~common::B_MASK;
                value |= (common::u32(b) << 8u);
            }

            void
            set_b(common::r32 b) {
                value &= ~common::B_MASK;
                value |= (common::u32(b * 255) << 8u);
            }

            void
            set_a(common::u8 a) {
                value &= ~common::A_MASK;
                value |= (common::u32(a));
            }

            void
            set_a(common::r32 a) {
                value &= ~common::A_MASK;
                value |= (common::u32(a * 255));
            }

            void
            set_rgb(common::r32 _r,
                    common::r32 _g,
                    common::r32 _b) {
                value &= ~common::R_MASK;
                value &= ~common::B_MASK;
                value &= ~common::G_MASK;
                set_r(_r);
                set_g(_g);
                set_b(_b);
            }

            void
            set_rgb(common::u8 _r,
                    common::u8 _g,
                    common::u8 _b) {
                value &= ~common::R_MASK;
                value &= ~common::G_MASK;
                value &= ~common::B_MASK;
                set_r(_r);
                set_g(_g);
                set_b(_b);
            }

            void
            set_rgba(common::r32 _r,
                     common::r32 _g,
                     common::r32 _b,
                     common::r32 _a) {
                value = 0;
                set_rgb(_r, _g, _b);
                set_a(_a);
            }

            void
            set_rgba(common::u8 _r,
                     common::u8 _g,
                     common::u8 _b,
                     common::u8 _a) {
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

            common::u32 value{255u};
        };

        enum class BrushType : common::u8 {
            COLOR,
            TEXTURE,
            TEXTURE_TAG,

            LAST
        };

        enum class FadeFunc : common::u8 {
            LINEAR,
            TAIL,

            LAST
        };

        struct FadeWithAge {
            FadeFunc fadeFunc{};
            common::r32 factor{1.f};
        };

        // TODO: Probably should be merged with ParticleEmitter with the goal of generic enough Particle Emitter
        // component that covers most of the games needs.
        struct CoolDown {
            common::r64 current{0.f};
            common::r64 initial{0.2f};
        };

        struct BrushTrail {
            bool initialized{false};
            bool active{true};
            component::Texture texture{};
            common::r32 mass{1.f};
            common::r32 width{0.4f};
            component::Heading2D heading{};
            component::WorldP2D last{};
            component::WorldP2D lastDelta{};
            component::WorldP2D current{};
            component::Velocity2D velocity2d{};
            component::Velocity velocity{};
            component::Acceleration acceleration{};
            component::Acceleration2D acceleration2d{};
            std::vector<component::Quad> quads;
        };

        struct AfterMark {
            component::Scale scale{1, 1};
            component::BrushType type{component::BrushType::COLOR};
            union {
                TextureTag textureTag = TextureTag::UNKNOWN;
                component::Color color;
            };
        };
    }
}
#pragma once

#include <utility>
#include <vector>
#include <unordered_map>
#include <cassert>

#include <oni-core/math/oni-math-vec2.h>
#include <oni-core/math/oni-math-vec3.h>
#include <oni-core/math/oni-math-vec4.h>
#include <oni-core/math/oni-math-mat4.h>
#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-physics.h>
#include <oni-core/graphic/oni-graphic-fwd.h>
#include <oni-core/common/oni-common-const.h>
#include <oni-core/util/oni-util-hash.h>
#include <oni-core/util/oni-util-enum.h>
#include <oni-core/asset/oni-asset-structure.h>


namespace oni {
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

        template<class Archive>
        void
        save(Archive &archive) const {
            auto r = r_r32();
            auto g = r_r32();
            auto b = r_r32();
            auto a = r_r32();
            archive(r, g, b, a);
        }

        template<class Archive>
        void
        load(Archive &archive) {
            auto color = vec4();
            archive(color.x, color.y, color.z, color.w);
            set_rgba(color.x, color.y, color.z, color.w);
        }
    };

    struct Image {
        ImageName name{};

        u32 width{};
        u32 height{};

        template<class Archive>
        void
        save(Archive &archive) const {
            // TODO: Not super happy about this, would it allocate?
            archive("name", std::string(name.str));
        }

        template<class Archive>
        void
        load(Archive &archive) {
            auto name_ = std::string();
            archive("name", name_);
            name = {HashedString::makeFromStr(std::move(name_))};
        }

        inline static constexpr auto GENERATED = ImageName{"__GENERATED__"};
    };

    struct UV {
        std::array<vec2, 4> values{vec2{0.f, 0.f}, vec2{0.f, 1.f},
                                   vec2{1.f, 1.f}, vec2{1.f, 0.f}};
    };

    struct Texture {
        Image image{};
        // TODO: NOOooooooooooooooooooo what is this.
        bool clear{false};
        oniGLuint id{0};
        // TODO: Remove these ogl dependent stuff and use engine enums
        // GL_BGRA          0x80E1
        oniGLenum format{0x80E1};
        // GL_UNSIGNED_BYTE 0x1401
        // GL_FLOAT         0x1406
        oniGLenum type{0x1401};
        UV uv{};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive("image", image);
        }
    };

    enum class BrushType : oni::u8 {
        COLOR,
        TEXTURE,
    };

    enum class NumAnimationFrames : oni::FrameID {
        TWO = 2,
        FOUR = 4,
        FIVE = 5,
        TEN = 10,
        TWENTY = 20,
        TWENTYFIVE = 25,
        FIFTY = 50,

        LAST
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
        Image image{};
        r32 mass{1.f};
        r32 width{0.4f};
        Direction ornt{};
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
    enum class __EntityAssetsPack : oni::u32 {
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

        CANVAS_DEFAULT,

        TEXT_DEFAULT,

        GENERATED,

        LAST
    };

    struct TextAttachment {
        EntityContext attachee;
    };

    struct Material_Skin {
        Color color{};
        Texture texture{};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive("color", color);
            archive("texture", texture);
        }
    };

    struct Material_Text {
        r32 xGameScaleDenom{1.f};
        r32 yGameScaleDenom{1.f};
        r32 fontSize{1.f};
        oniGLuint textureID{0};
        std::string textContent{};
        std::vector<size_t> width{};
        std::vector<size_t> height{};
        std::vector<u32> offsetX{};
        std::vector<u32> offsetY{};
        std::vector<r32> advanceX{};
        std::vector<r32> advanceY{};
        std::vector<vec4> uv{};
        Material_Skin skin{}; // TODO: only the color is used for now, but for ease of use with Renderable I have skin, but when MaterialText is merged with the rest I can remove this field.

        template<class Archive>
        void
        serialize(Archive &archive) {
        }
    };

    // TODO: All these ENUMS needs UNKNOWN since I'm reading them from files.
    enum class MaterialTransition_Type : oni::u8 {
        TEXTURE = 0,
        FADE = 1,
        TINT = 2,
        NONE = 3,
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
        AnimationID animID{};
        NumAnimationFrames numFrames{NumAnimationFrames::TWO};
        FrameID nextFrame{0};
        bool playing{true};
        TimeToLive ttl;
    };

    struct MaterialTransition_Def {
        MaterialTransition_Type type{};
        TimeToLive ttl;

        union {
            MaterialTransition_Texture texture{};
            MaterialTransition_Fade fade;
            MaterialTransition_Color color;
        };

        template<class Archive>
        void
        serialize(Archive &archive) {
            // NOTE: Taken from: https://old.reddit.com/r/programming/comments/1i3qi9/cereal_a_c11_library_for_serialization_binary_xml/cb10q08/
            archive(type, ttl);
            // assert(Enums.valid(type));
            switch (type) {
                case MaterialTransition_Type::TEXTURE: {
                    archive("texture", texture);
                    break;
                }
                case MaterialTransition_Type::FADE: {
                    archive("fade", fade);
                    break;
                }
                case MaterialTransition_Type::TINT: {
                    archive("color", color);
                    break;
                }
                case MaterialTransition_Type::NONE:
                    break;
            }
        }
    };

    enum class MaterialTransition_EndBehavior : oni::u8 {
        LOOP,
        PLAY_AND_STOP,
        PLAY_AND_REMOVE_ENTITY,
    };

    struct MaterialTransition_List {
        size activeTransIdx{0};
        MaterialTransition_EndBehavior ending{};
        bool ended{false};
        std::vector<MaterialTransition_Def> transitions{};
    };

    // TODO: Better name
    // TODO: Split this up, MaterialTransparency_Type: SOLID and TRANSLUECENT
    // and SHINNY could turn into MaterialGloss_Type: SHINNY, MATT, or maybe even just MaterialGloss with a
    // float definning how shinny it is. Although I have to keep in mind for shinny entities I do switch the
    // blend function so it can't just be a range of values, it has to be a Type hmmm...

    // NOTE: The order of the enums defines the order in which they are rendered!
    // NOTE: For translucent and shinny entities since depth writes are disabled
    // if an entity that is translucent but has higher z is rendered then a shinny
    // entity with lower z is rendered it will still be drawn over the higher z
    // entity!
    ONI_ENUM_DEF(Material_Finish, { 0, "solid" }, { 1, "translucent" }, { 2, "shinny" })

    struct Material_Definition {
        Material_Finish finish{};
        Material_Skin skin{};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive("finish", finish);
            archive("skin", skin);
        }
    };

    struct GrowOverTime {
        duration32 period{0.2f}; // NOTE: Grow every period
        duration32 elapsed{0.f}; // NOTE: Since last growth
        r32 factor{0.1f}; // NOTE: add this much to current size
        Scale maxSize{1, 1, 1};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive("period", period);
            archive("elapsed", elapsed);
            archive("factor", factor);
            archive("maxSize", maxSize);
        }
    };

    struct ParticleEmitter {
        Material_Definition material{};
        r32 size = 0.1f;
        r32 initialVMin = 1.f;
        r32 initialVMax = 2.f;
        r32 orientMin = 0.f;
        r32 orientMax = TWO_PI;
        r32 acc = 0.f;
        u8 count = 1;
        GrowOverTime growth{};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive("material", material);
            archive("size", size);
            archive("initialVMin", initialVMin);
            archive("initialVMax", initialVMax);
            archive("orientMin", orientMin);
            archive("orientMax", orientMax);
            archive("acc", acc);
            archive("count", count);
            archive("growth", growth);
        }
    };

    struct AfterMark {
        Scale scale{1, 1};
        BrushType type{BrushType::COLOR};
        Material_Definition material{};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive(material, type);
        }
    };

    struct SplatOnDeath {
        Material_Definition md{};
    };

    struct SplatOnRest {
        Material_Definition md{};
    };
}
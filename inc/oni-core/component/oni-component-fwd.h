#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    /// Audio
    enum class SoundPlaybackState : oni::u8;

    struct Sound;
    struct SoundPitch;

    /// Geometry
    union OriginP2D;
    union OriginP3D;
    union Point;
    union Scale;
    union WorldP2D;
    union WorldP3D;
    union WorldP4D;
    union Screen2D;

    struct AABB;
    struct EntityAttachee;
    struct EntityAttachment;
    struct Orientation;
    struct Quad;
    struct Rectangle;
    struct WorldP3D_History;

    /// Physics
    struct Acceleration;
    struct Car;
    struct CarConfig;
    struct JetForce;
    struct PhysicalProperties;
    struct PhysicalBody;
    struct TimeToLive;
    struct Velocity;

    union Acceleration2D;
    union Direction;
    union Force;
    union Velocity2D;

    /// Visual
    enum class NumAnimationFrames : oni::u8;
    enum class MaterialTransition_EndBehavior : oni::u8;
    enum class BrushType : oni::u8;
    enum class MaterialTransition_Type : oni::u8;
    enum class FadeFunc : oni::u8;

    struct AfterMark;
    struct AnimatedColor;
    struct BrushTrail;
    struct Color;
    struct CoolDown;
    struct GrowOverTime;
    struct Image;
    struct Material_Definition;
    struct Material_Skin;
    struct Material_Text;
    struct MaterialTransition_Color;
    struct MaterialTransition_Fade;
    struct MaterialTransition_Texture;
    struct ParticleEmitter;
    struct TextAttachment;
    struct Texture;
    struct MaterialTransition_Texture;
    struct UV;
}
#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    /// Audio
    enum class ChannelGroup : oni::u8;
    enum class SoundPlaybackState : oni::u8;
    enum class Sound_Tag : oni::u32;

    struct Sound;
    struct SoundPitch;

    /// Geometry
    union Heading2D;
    union OriginP2D;
    union OriginP3D;
    union Point;
    union Scale;
    union WorldP2D;
    union WorldP3D;
    union WorldP4D;

    struct AABB;
    struct EntityAttachee;
    struct EntityAttachment;
    struct Heading;
    struct Quad;
    struct Rectangle;
    struct WorldP3D_History;

    /// Physics
    struct Acceleration;
    struct Age;
    struct Car;
    struct CarConfig;
    struct JetForce;
    struct PhysicalProperties;
    struct Velocity;

    union Acceleration2D;
    union Force;
    union Velocity2D;

    /// Visual
    enum class NumAnimationFrames : oni::u8;
    enum class AnimationEndingBehavior : oni::u8;
    enum class BrushType : oni::u8;
    enum class EntityPreset : oni::u32;
    enum class MaterialTransition_Type : oni::u8;
    enum class FadeFunc : oni::u8;

    struct AfterMark;
    struct AnimatedColor;
    struct BrushTrail;
    struct Color;
    struct CoolDown;
    struct Image;
    struct MaterialDefinition;
    struct MaterialSkin;
    struct MaterialTransition_Animation;
    struct MaterialTransition_Fade;
    struct MaterialTransition_Tint;
    struct ParticleEmitter;
    struct Text;
    struct Texture;
    struct TextureAnimated;
    struct UV;
}
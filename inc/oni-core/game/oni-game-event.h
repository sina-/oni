#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-entity.h>


namespace oni {
    enum class EventType : oni::u8 {
        COLLISION,
        SPAWN_PARTICLE,
        SOUND_PLAY,
        ROCKET_LAUNCH,
    };

    // TODO: Doent look like the right place
    struct CollidingEntity {
        EntityType a{EntityType::UNKNOWN};
        EntityType b{EntityType::UNKNOWN};
    };

    struct Event_SplatOnDeath {
        Event_SplatOnDeath() = default;

        Event_SplatOnDeath(const WorldP3D &pos,
                           const Scale &scale,
                           const Orientation &ornt,
                           const EntityPreset &tag,
                           std::function<void()> func
        ) : pos(pos), scale(scale), tag(tag), ornt(ornt), callback(std::move(func)) {}

        WorldP3D pos{};
        Scale scale{};
        Orientation ornt{};
        EntityPreset tag{};
        std::function<void()> callback{};
    };

    struct Event_SplatOnRest {
        Event_SplatOnRest() = default;

        Event_SplatOnRest(const WorldP3D &pos,
                          const Scale &scale,
                          const Orientation &ornt,
                          const EntityPreset &tag,
                          std::function<void()> func
        ) : pos(pos), scale(scale), tag(tag), ornt(ornt), callback(std::move(func)) {}

        WorldP3D pos{};
        Scale scale{};
        Orientation ornt{};
        EntityPreset tag{};
        std::function<void()> callback{};
    };

    struct Event_RocketLaunch {
        Event_RocketLaunch() = default;

        Event_RocketLaunch(const WorldP3D &pos) : pos(pos) {}

        WorldP3D pos{};
    };

    struct Event_Collision {
        Event_Collision() = default;

        Event_Collision(const WorldP3D &pos,
                        const CollidingEntity &colliding) : pos(pos), colliding(colliding) {}

        WorldP3D pos{};
        CollidingEntity colliding{};
    };

    struct Event_SoundPlay {
        Event_SoundPlay() = default;

        Event_SoundPlay(const WorldP3D &pos,
                        const Sound_Tag &soundTag) : pos(pos), tag(soundTag) {}

        WorldP3D pos{};
        Sound_Tag tag{};
    };
}
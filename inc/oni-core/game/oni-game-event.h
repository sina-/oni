#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-entity.h>


namespace oni {
    namespace game {
        enum class EventType : common::u8 {
            COLLISION,
            SPAWN_PARTICLE,
            SOUND_PLAY,
            ROCKET_LAUNCH,
        };

        // TODO: Doent look like the right place
        struct CollidingEntity {
            entities::EntityType entityA{entities::EntityType::UNKNOWN};
            entities::EntityType entityB{entities::EntityType::UNKNOWN};
        };

        struct Event_SplatOnDeath {
            Event_SplatOnDeath() = default;

            Event_SplatOnDeath(const component::WorldP3D &pos,
                               const component::Scale &scale,
                               const component::TextureTag &tag) : pos(pos), scale(scale), tag(tag) {}

            component::WorldP3D pos{};
            component::Scale scale{};
            component::TextureTag tag{};
        };

        struct Event_RocketLaunch {
            Event_RocketLaunch() = default;

            Event_RocketLaunch(const component::WorldP3D &pos) : pos(pos) {}

            component::WorldP3D pos{};
        };

        struct Event_Collision {
            Event_Collision() = default;

            Event_Collision(const component::WorldP3D &pos,
                            const CollidingEntity &colliding) : pos(pos), colliding(colliding) {}

            component::WorldP3D pos{};
            CollidingEntity colliding{};
        };

        struct Event_SoundPlay {
            Event_SoundPlay() = default;

            Event_SoundPlay(const component::WorldP3D &pos,
                            const component::SoundTag &soundTag) : pos(pos), tag(soundTag) {}

            component::WorldP3D pos{};
            component::SoundTag tag{};
        };
    }
}
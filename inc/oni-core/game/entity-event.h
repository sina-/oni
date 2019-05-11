#pragma once

#include <oni-core/common/typedefs.h>
#include <oni-core/component/geometry.h>
#include <oni-core/component/visual.h>
#include <oni-core/entities/entity.h>


namespace oni {
    namespace game {
        enum class EventType : common::uint8 {
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
                               const component::Size &size,
                               const std::string &textureId) : pos(pos), size(size), textureID(textureId) {}

            component::WorldP3D pos{};
            component::Size size{};
            std::string textureID{};
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
                            const component::SoundID &soundId) : pos(pos), soundID(soundId) {}

            component::WorldP3D pos{};
            component::SoundID soundID{};
        };
    }
}
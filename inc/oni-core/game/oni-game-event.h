#pragma once

#include <functional>

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/component/oni-component-geometry.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/entities/oni-entities-structure.h>
#include <oni-core/math/oni-math-function.h>


namespace oni {
    enum class EventDispatcherType : u8 {
        LOCAL,
        SYNCED,

        LAST
    };

    constexpr auto NumEventDispatcher = enumCast(EventDispatcherType::LAST);

    struct Event_SplatOnDeath {
        Event_SplatOnDeath() = default;

        Event_SplatOnDeath(const WorldP3D &pos,
                           const Scale &scale,
                           const Orientation &ornt,
                           const Material_Definition &md,
                           std::function<void()> func
        ) : pos(pos), scale(scale), md(md), ornt(ornt), callback(std::move(func)) {}

        WorldP3D pos{};
        Scale scale{};
        Orientation ornt{};
        Material_Definition md{};
        std::function<void()> callback{};

        template<class Archive>
        void
        serialize(Archive &archive) {
            archive(pos, scale, ornt, md);
        }
    };

    struct Event_SplatOnRest {
        Event_SplatOnRest() = default;

        Event_SplatOnRest(const WorldP3D &pos,
                          const Scale &scale,
                          const Orientation &ornt,
                          const Material_Definition &md,
                          std::function<void()> func
        ) : pos(pos), scale(scale), md(md), ornt(ornt), callback(std::move(func)) {}

        WorldP3D pos{};
        Scale scale{};
        Orientation ornt{};
        Material_Definition md{};
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
                        const Impulse2D &impulse,
                        const EntityPair &pair,
                        const PhysicalCatPair &pcPair) : pos(pos), pair(pair), pcPair(pcPair), impulse(impulse) {}


        EntityPair pair{};
        PhysicalCatPair pcPair{};
        WorldP3D pos{};
        Impulse2D impulse{};
    };

    struct Event_SoundPlay {
        Event_SoundPlay() = default;

        Event_SoundPlay(const WorldP3D &pos,
                        const Sound_Tag &soundTag) : pos(pos), tag(soundTag) {}

        WorldP3D pos{};
        Sound_Tag tag{};
    };

    struct Event_EntityDeath {
        Event_EntityDeath() = default;

        EntityID id{};
    };
}
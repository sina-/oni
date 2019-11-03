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

        Event_SplatOnDeath(const WorldP3D &pos_,
                           const Scale &scale_,
                           const Orientation &ornt_,
                           const Material_Definition &md_,
                           std::function<void()> func_
        ) : pos(pos_), scale(scale_), md(md_), ornt(ornt_), callback(std::move(func_)) {}

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

        Event_SplatOnRest(const WorldP3D &pos_,
                          const Scale &scale_,
                          const Orientation &ornt_,
                          const Material_Definition &md_,
                          std::function<void()> func_
        ) : pos(pos_), scale(scale_), md(md_), ornt(ornt_), callback(std::move(func_)) {}

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

        Event_Collision(const WorldP3D &pos_,
                        const Impulse2D &impulse_,
                        const EntityPair &pair_,
                        const PhysicalCatPair &pcPair_) : pos(pos_), pair(pair_), pcPair(pcPair_), impulse(impulse_) {}


        EntityPair pair{};
        PhysicalCatPair pcPair{};
        WorldP3D pos{};
        Impulse2D impulse{};
    };

    struct Event_SoundPlay {
        Event_SoundPlay() = default;

        Event_SoundPlay(const WorldP3D &pos_,
                        const SoundName &name_) : pos(pos_), name(name_) {}

        WorldP3D pos{};
        SoundName name{};
    };

    struct Event_EntityDeath {
        Event_EntityDeath() = default;

        EntityID id{};
    };
}
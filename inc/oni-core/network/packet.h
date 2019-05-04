#pragma once

#include <string>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/packet-types.h>
#include <oni-core/component/physic.h>
#include <oni-core/component/audio.h>
#include <oni-core/component/geometry.h>
#include <oni-core/game/entity-event.h>


namespace oni {
    namespace network {
        struct EntityPacket {
            common::uint32 entity{0};
        };

        struct DataPacket {
            std::string data{};
        };

        struct CollisionEventPacket {
            game::CollidingEntity collidingEntity{};
            component::WorldP3D pos{};
        };

        struct SoundPlayEventPacket {
            component::SoundID soundID{};
            component::WorldP3D pos{};
        };

        struct RocketLaunchEventPacket {
            component::WorldP3D pos{};
        };
    }
}

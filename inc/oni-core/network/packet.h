#pragma once

#include <string>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/packet-types.h>
#include <oni-core/component/physic.h>
#include <oni-core/component/audio.h>
#include <oni-core/component/visual.h>


namespace oni {
    namespace network {
        struct EntityPacket {
            common::uint32 entity{0};
        };

        struct DataPacket {
            std::string data{};
        };

        struct CollisionEventPacket {
            component::CollidingEntity collidingEntity{};
            component::CollisionPos collisionPos{};
        };

        struct SoundPlayEventPacket {
            component::SoundID soundID{};
            component::WorldPos pos{};
        };

        struct RocketLaunchEventPacket {
            math::vec2 pos{};
        };
    }
}

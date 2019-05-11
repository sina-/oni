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
        struct Packet_EntityID {
            common::uint32 entity{0};
        };

        struct Packet_Data {
            std::string data{};
        };
    }
}

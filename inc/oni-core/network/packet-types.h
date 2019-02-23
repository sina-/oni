#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace network {
        enum class PacketType : common::uint8 {
            UNKNOWN = 0,
            PING = 1,
            MESSAGE = 2,
            SETUP_SESSION = 3,
            CAR_ENTITY_ID = 4,
            CLIENT_INPUT = 5,

            REPLACE_ALL_ENTITIES = 6,
            ONLY_COMPONENT_UPDATE = 7,
            ADD_NEW_ENTITIES = 8,
            DESTROYED_ENTITIES = 9,

            SPAWN_PARTICLE = 11,
        };
    }
}

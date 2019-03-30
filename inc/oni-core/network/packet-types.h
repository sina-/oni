#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace network {
        enum class PacketType : common::uint8 {
            UNKNOWN = 0,
            PING = 1,
            MESSAGE = 2,
            SETUP_SESSION = 3,
            // TODO: Is this needed?
            CAR_ENTITY_ID = 4,
            CLIENT_INPUT = 5,

            REGISTRY_REPLACE_ALL_ENTITIES = 6,
            REGISTRY_ONLY_COMPONENT_UPDATE = 7,
            REGISTRY_ADD_NEW_ENTITIES = 8,
            REGISTRY_DESTROYED_ENTITIES = 9,

            EVENT_ONE_SHOT_SOUND_EFFECT = 10,
            EVENT_COLLISION = 11,
            EVENT_ROCKET_LAUNCH = 12,
        };
    }
}

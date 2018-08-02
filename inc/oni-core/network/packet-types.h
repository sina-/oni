#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace network {
        enum class PacketType : common::uint8 {
            UNKNOWN = 0,
            PING = 1,
            MESSAGE = 2,
            ENTITY = 3,
            CAR_ENTITY_ID = 4,
            FOREGROUND_ENTITIES = 5,
            BACKGROUND_ENTITIES = 6,
            INPUT = 7,
        };
    }
}

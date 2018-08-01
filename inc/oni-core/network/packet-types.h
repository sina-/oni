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
            WORLD_DATA = 5,
            INPUT = 6,
        };
    }
}

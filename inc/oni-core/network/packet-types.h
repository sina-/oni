#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace network {
        enum class PacketType : common::uint8 {
            PING = 0,
            MESSAGE = 1,
            ENTITY = 2,
            CAR_ENTITY_ID = 3,
            WORLD_DATA = 4,

            UNKNOWN = 5
        };
    }
}

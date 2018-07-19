#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace network {
        enum class PacketType : common::uint8 {
            PING = 0,
            MESSAGE = 1,
            ENTITY = 2,
            WORLD_DATA = 3,

            UNKNOWN = 4
        };
    }
}

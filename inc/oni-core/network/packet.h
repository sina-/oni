#pragma once

#include <string>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/packet-types.h>


namespace oni {
    namespace network {
        struct EntityPacket {
            common::uint32 entity{};
        };

        struct DataPacket {
            std::string data{};
        };

        struct ZLevelDeltaPacket {
            common::real32 major{};
            common::real32 minor{};
        };
    }
}

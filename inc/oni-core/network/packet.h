#pragma once

#include <string>

#include <oni-core/common/typedefs.h>
#include <oni-core/network/packet-types.h>


namespace oni {
    namespace network {
        struct EntityPacket {
            common::uint32 entity{0};
        };

        struct DataPacket {
            std::string data{};
        };

        struct ZLevelDeltaPacket {
            common::real32 major{0.f};
            common::real32 minor{0.f};
        };
    }
}

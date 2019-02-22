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

        struct ZLayerPacket {
            common::real32 level_0{0.f};
            common::real32 level_1{0.f};
            common::real32 level_2{0.f};
            common::real32 level_3{0.f};
            common::real32 level_4{0.f};
            common::real32 level_5{0.f};
            common::real32 level_6{0.f};
            common::real32 level_7{0.f};
            common::real32 level_8{0.f};
            common::real32 level_9{0.f};
        };
    }
}

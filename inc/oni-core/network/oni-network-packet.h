#pragma once

#include <string>

#include <oni-core/common/oni-common-typedef.h>


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

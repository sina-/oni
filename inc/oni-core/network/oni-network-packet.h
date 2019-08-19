#pragma once

#include <string>

#include <oni-core/common/oni-common-typedef.h>


namespace oni {
    struct Packet_EntityID {
        u32 entity{0};
    };

    struct Packet_Data {
        std::string data{};
    };
}

#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace component {

        struct CarLapInfo {
            common::EntityID entityID{0};
            common::uint16 lap{0};
            common::uint32 lapTimeS{0};
            common::uint32 bestLapTimeS{0};
        };
    }
}

#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace components {

        struct CarLap {
            common::EntityID entityID{0};
            common::uint16 lap{0};
            common::uint32 lapTimeS{0};
            common::uint32 currentBestLapTimeS{0};

            template<class Archive>
            void serialize(Archive &archive) {
                archive(entityID, lap, lapTimeS, currentBestLapTimeS);
            }
        };
    }
}

#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace component {
        typedef common::uint16 oniKeyPress;

        struct CarInput {
            common::real32 left{0.0f};
            common::real32 right{0.0f};
            common::real32 throttle{0.0f};
            common::real32 brake{0.0f};
            common::real32 eBrake{0.0f};
        };
    }
}

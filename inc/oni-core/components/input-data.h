#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace components {
        typedef common::uint16 oniKeyPress;

        struct CarInput {
            float left{0.0f};
            float right{0.0f};
            float throttle{0.0f};
            float brake{0.0f};
            float eBrake{0.0f};
        };
    }
}

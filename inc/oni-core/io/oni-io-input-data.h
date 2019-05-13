#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    namespace io {
        typedef common::u16 oniKeyPress;

        struct CarInput {
            common::r32 left{0.0f};
            common::r32 right{0.0f};
            common::r32 throttle{0.0f};
            common::r32 brake{0.0f};
            common::r32 eBrake{0.0f};
        };
    }
}

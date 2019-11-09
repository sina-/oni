#pragma once

#include <oni-core/common/oni-common-typedef.h>

namespace oni {
    typedef u16 oniKeyPress;

    struct CarInput {
        r32 left{0.f};
        r32 right{0.f};
        r32 throttle{0.f};
        r32 brake{0.f};
        r32 eBrake{0.f};

        bool nitro{false};
    };
}

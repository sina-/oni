#pragma once

namespace oni {
    namespace components {
        typedef int oniKeyPress;

        struct CarInput {
            float left{0.0f};
            float right{0.0f};
            float throttle{0.0f};
            float brake{0.0f};
            float eBrake{0.0f};
        };
    }
}

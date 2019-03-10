#pragma once

#include <string>
#include <oni-core/math/vec2.h>

namespace oni {
    namespace component {

        struct SoundEffect {
            std::string soundID;
            math::vec2 pos{0.f, 0.f};
        };
    }
}
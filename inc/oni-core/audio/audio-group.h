#pragma once

#include <oni-core/common/typedefs.h>

namespace oni {
    namespace audio {
        enum class AudioGroup : common::uint8 {
            MASTER,
            MUSIC,
            EFFECTS,
        };
    }
}

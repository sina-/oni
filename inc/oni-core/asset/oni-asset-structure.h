#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/util/oni-util-file.h>
#include <oni-core/util/oni-util-hash.h>

namespace oni {
    struct ImageName : public HashedString {

    };

    struct SoundName : public HashedString {
    };

    struct ImageAsset {
        FilePath path{};
        ImageName name{};
    };

    struct SoundAsset {
        FilePath path{};
        SoundName name{};
    };
}


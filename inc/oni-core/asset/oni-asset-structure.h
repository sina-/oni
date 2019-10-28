#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/util/oni-util-file.h>
#include <oni-core/util/oni-util-hash.h>

namespace oni {
    struct AssetName : public HashedString {
    };

    struct Asset {
        FilePath path{};
        AssetName name{};
    };

    struct
    ImageAsset : public Asset {
    };

    struct

    SoundAsset : public Asset {
    };
}

DEFINE_STD_HASH_FUNCTIONS(oni::AssetName)


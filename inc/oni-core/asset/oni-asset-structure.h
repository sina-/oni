#pragma once

#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/util/oni-util-file.h>
#include <oni-core/util/oni-util-hash.h>

namespace oni {
    // TODO: It is a mess, what is an AssetName? What is ImageAsset? What is SoundName? Why textures
    // use Image.AssetName as the user facing identifier? And How can I make sure users can use an
    // Enum to pre-define the known types?
    struct AssetName : public HashedString {
    };

    struct Asset {
        FilePath path{};
        AssetName name{};
    };

    struct ImageAsset : public Asset {
    };

    struct SoundAsset : public Asset {
    };
}

DEFINE_STD_HASH_FUNCTIONS(oni::AssetName)


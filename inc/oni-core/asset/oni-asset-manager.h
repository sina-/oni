#pragma once

#include <unordered_map>
#include <string_view>

#include <oni-core/asset/oni-asset-structure.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/util/oni-util-file.h>
#include <oni-core/util/oni-util-structure.h>


namespace oni {
    class AssetFilesIndex {
    public:
        explicit AssetFilesIndex(ImageIndexFilePath &&);

        auto
        imageAssetsBegin() const {
            return mImageAssetMap.cbegin();
        }

        auto
        imageAssetsEnd() const {
            return mImageAssetMap.cend();
        }

    private:
        void
        _indexImages();

    private:
        std::unordered_map<Sound_Tag, std::string> mSoundAssetPath{};
        std::unordered_map<Hash, ImageAsset> mImageAssetMap{};
        ImageIndexFilePath mImageIndexFilePath{};
    };
}

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
        explicit AssetFilesIndex(ImageIndexFilePath &&,
                                 SoundIndexFilePath &&);

        auto
        soundAssetsBegin() const {
            return mSoundAssetMap.cbegin();
        }

        auto
        soundAssetsEnd() const {
            return mSoundAssetMap.cend();
        }

        auto
        imageAssetsBegin() const {
            return mImageAssetMap.cbegin();
        }

        auto
        imageAssetsEnd() const {
            return mImageAssetMap.cend();
        }

        bool
        validSound(const SoundName &) const;

        bool
        validImage(const ImageName&) const;

    private:
        void
        _indexImages();

        void
        _indexSounds();

    private:
        std::unordered_map<Hash, SoundAsset> mSoundAssetMap{};
        std::unordered_map<Hash, ImageAsset> mImageAssetMap{};
        ImageIndexFilePath mImagesParentDir{};
        SoundIndexFilePath mAudioParentDir{};
    };
}

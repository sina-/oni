#pragma once

#include <map>
#include <string_view>

#include <oni-core/asset/oni-asset-type.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/component/oni-component-visual.h>
#include <oni-core/util/oni-util-file.h>

namespace oni {
    using ImageNameHash = Hash;

    class AssetManager {
    public:
        explicit AssetManager(const ImageIndexFilePath &);

        void
        setPath(Sound_Tag,
                std::string_view);

        const std::string &
        getAssetFilePath(Sound_Tag);

        const FilePath &
        getAssetFilePath(const ImageName &name);

        std::vector<ImageName>
        knownImages();

    private:
        void indexImages();

    private:
        std::map<Sound_Tag, std::string> mSoundAssetPath{};
        std::map<ImageNameHash, FilePath> mImageNameToPath{};
        std::map<ImageNameHash, ImageName> mImageNameLookup{};
        ImageIndexFilePath mImageIndexFilePath{};
    };
}

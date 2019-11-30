#include <oni-core/asset/oni-asset-manager.h>

#include <oni-core/json/oni-json.h>


namespace oni {
    AssetFilesIndex::AssetFilesIndex(ImageIndexFilePath &&imagePath,
                                     SoundIndexFilePath &&soundPath) {
        mImagesParentDir = std::move(imagePath);
        mAudioParentDir = std::move(soundPath);

        // TODO better split this up from the constructor and give the user possibility of loading these files
        // when they think it is the good time.
        _indexImages();
        _indexSounds();
    }

    bool
    AssetFilesIndex::validSound(const SoundName &name) const {
        return mSoundAssetMap.find(name.hash) != mSoundAssetMap.end();
    }

    bool
    AssetFilesIndex::validImage(const ImageName &name) const {
        return mImageAssetMap.find(name.hash) != mImageAssetMap.end();
    }

    void
    AssetFilesIndex::_indexImages() {
        auto textures = parseDirectoryTree(mImagesParentDir);
        for (auto &&texture : textures) {
            if (texture.extension == ".png") {
                auto name = texture.getFullPath();
                auto nameHash = HashedString::makeFromCStr(name.data());
                auto result = mImageAssetMap.emplace(nameHash.hash, ImageAsset{texture, nameHash});
                assert(result.second);
            } else {
            }
        }
    }

    void
    AssetFilesIndex::_indexSounds() {
        // TODO:
//        auto audios = parseDirectoryTree(mAudioParentDir);
//        for (auto &&audio: audios) {
//
//        }
    }
}
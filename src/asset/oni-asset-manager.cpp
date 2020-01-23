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
            if (texture.extension == "png") {
                auto fullPath = texture.getFullPath();
                auto pathHashed = HashedString::makeFromCStr(fullPath.data());
                auto result = mImageAssetMap.emplace(pathHashed.hash, ImageAsset{texture, pathHashed});
                assert(result.second);
            } else {
            }
        }
    }

    void
    AssetFilesIndex::_indexSounds() {
        auto sounds = parseDirectoryTree(mAudioParentDir);
        for (auto &&soundFile : sounds) {
            if (soundFile.extension == "wav") {
                auto fullPath = soundFile.getFullPath();
                auto pathHashed = HashedString::makeFromCStr(fullPath.data());
                auto result = mSoundAssetMap.emplace(pathHashed.hash, SoundAsset{soundFile, pathHashed});
                assert(result.second);
            } else {
            }
        }
    }
}
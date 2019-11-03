#include <oni-core/asset/oni-asset-manager.h>

#include <oni-core/json/oni-json.h>


namespace oni {
    AssetFilesIndex::AssetFilesIndex(ImageIndexFilePath &&imagePath,
                                     SoundIndexFilePath &&soundPath) {
        mImageIndexFilePath = std::move(imagePath);
        mSoundIndexFilePath = std::move(soundPath);

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
        auto maybeDoc = readJson(mImageIndexFilePath);
        if (!maybeDoc.has_value()) {
            assert(false);
            return;
        }

        auto document = std::move(maybeDoc.value());

        auto textures = document.FindMember("textures");
        if (textures != document.MemberEnd() && textures->value.IsObject()) {
            for (auto texture = textures->value.MemberBegin(); texture != textures->value.MemberEnd(); ++texture) {
                if (texture->value.IsString()) {
                    auto name = texture->name.GetString();
                    auto relativeFileAndPath = texture->value.GetString();
                    auto filePath = FilePath{mImageIndexFilePath.path.data(), relativeFileAndPath};
                    auto nameHash = HashedString::makeFromCStr(name);
                    auto image = mImageAssetMap.find(nameHash.hash);
                    if (image != mImageAssetMap.end()) {
                        assert(false);
                        continue;
                    }
                    mImageAssetMap.emplace(nameHash.hash, ImageAsset{filePath, nameHash});
                } else {
                    assert(false);
                }
            }
        } else {
            assert(false);
        }
    }

    void
    AssetFilesIndex::_indexSounds() {
        auto maybeDoc = readJson(mSoundIndexFilePath);
        if (!maybeDoc.has_value()) {
            assert(false);
            return;
        }

        auto document = std::move(maybeDoc.value());

        auto sounds = document.FindMember("sounds");
        if (sounds != document.MemberEnd() && sounds->value.IsObject()) {
            // TODO: generalize read of these objects and read the sounds
        }
    }
}
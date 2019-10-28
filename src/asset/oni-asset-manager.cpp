#include <oni-core/asset/oni-asset-manager.h>

#include <cassert>
#include <fstream>

#include <cereal/external/rapidjson/document.h>

namespace oni {
    AssetFilesIndex::AssetFilesIndex(const ImageIndexFilePath &imagePath) {
        mImageIndexFilePath = imagePath;
        // TODO: Make this portable to other platforms
        if (mImageIndexFilePath.value.back() != '/') {
            mImageIndexFilePath.value.append("/");
        }

        // TODO better split this up from the constructor and give the user possibility of loading these files
        // when they think it is the good time.
        _indexImages();
    }

    void
    AssetFilesIndex::_indexImages() {
        auto jsonPath = mImageIndexFilePath.value;
        // TODO: accept index.json value as param
        std::ifstream jsonStream(jsonPath.append("index.json"));
        if (!jsonStream.is_open()) {
            assert(false);
            return;
        }

        auto jsonString = std::string((std::istreambuf_iterator<char>(jsonStream)), std::istreambuf_iterator<char>());

        auto document = rapidjson::Document();
        if (document.Parse(jsonString.c_str()).HasParseError()) {
            assert(false);
            return;
        }

        if (document.IsObject()) {
            auto textures = document.FindMember("textures");
            if (textures != document.MemberEnd() && textures->value.IsObject()) {
                for (auto texture = textures->value.MemberBegin(); texture != textures->value.MemberEnd(); ++texture) {
                    if (texture->value.IsString()) {
                        auto name = texture->name.GetString();
                        auto path = texture->value.GetString();
                        auto nameHash = HashedString::makeFromCStr(name);
                        auto fullPath = mImageIndexFilePath.value;
                        fullPath.append(path);
                        auto image = mImageAssetMap.find(nameHash.hash);
                        if (image != mImageAssetMap.end()) {
                            assert(false);
                            continue;
                        }
                        mImageAssetMap.emplace(nameHash.hash, ImageAsset{FilePath{fullPath}, nameHash});
                    } else {
                        assert(false);
                    }
                }
            } else {
                assert(false);
            }
        } else {
            assert(false);
        }
    }

    const std::string &
    AssetFilesIndex::getAssetFilePath(Sound_Tag tag) {
        const auto &result = mSoundAssetPath[tag];
        return result;
    }

    void
    AssetFilesIndex::setPath(Sound_Tag tag,
                             std::string_view path) {
        mSoundAssetPath.emplace(tag, path);
    }
}
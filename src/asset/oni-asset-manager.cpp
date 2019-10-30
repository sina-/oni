#include <oni-core/asset/oni-asset-manager.h>

#include <cassert>
#include <fstream>

#include <cereal/external/rapidjson/document.h>

namespace oni {
    AssetFilesIndex::AssetFilesIndex(ImageIndexFilePath &&imagePath) {
        mImageIndexFilePath = std::move(imagePath);

        // TODO better split this up from the constructor and give the user possibility of loading these files
        // when they think it is the good time.
        _indexImages();
    }

    void
    AssetFilesIndex::_indexImages() {
        std::ifstream jsonStream(mImageIndexFilePath.getFullPath());
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
        } else {
            assert(false);
        }
    }
}
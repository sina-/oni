#include <oni-core/asset/oni-asset-manager.h>

#include <cassert>
#include <fstream>

#include <cereal/external/rapidjson/document.h>

namespace oni {
    AssetManager::AssetManager(const ImageIndexFilePath &imagePath) {
        mImageIndexFilePath = imagePath;
        // TODO: Make this portable to other platforms
        if (mImageIndexFilePath.value.back() != '/') {
            mImageIndexFilePath.value.append("/");
        }

        indexImages();
    }

    void
    AssetManager::indexImages() {
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
                        auto nameHash = HashedString(name);
                        auto fullPath = mImageIndexFilePath.value;
                        fullPath.append(path);
                        mImageNameLookup.emplace(nameHash.hash, ImageName{nameHash});
                        mImageNameToPath.emplace(nameHash.hash, FilePath{fullPath});
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
    AssetManager::getAssetFilePath(Sound_Tag tag) {
        const auto &result = mSoundAssetPath[tag];
        return result;
    }

    const FilePath &
    AssetManager::getAssetFilePath(const ImageName &name) {
        auto path = mImageNameToPath.find(name.value.hash);
        if (path == mImageNameToPath.end()) {
            assert(false);
            return INVALID_FILE_PATH;
        }
        return path->second;
    }

    void
    AssetManager::setPath(Sound_Tag tag,
                          std::string_view path) {
        mSoundAssetPath.emplace(tag, path);
    }

    std::vector<ImageName>
    AssetManager::knownImages() {
        // TODO: You can use a view instead of the full copy
        auto names = std::vector<ImageName>();
        for (auto &&name: mImageNameLookup) {
            names.emplace_back(name.second);
        }

        return names;
    }
}

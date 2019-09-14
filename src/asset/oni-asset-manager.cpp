#include <oni-core/asset/oni-asset-manager.h>

#include <cassert>

namespace oni {
    AssetManager::AssetManager() {
    }

    const std::string &
    AssetManager::getAssetFilePath(Sound_Tag tag) {
        const auto &result = mSoundAssetPath[tag];
        return result;
    }

    const std::string &
    AssetManager::getAssetFilePath(EntityAssetsPack tag) {
        const auto &result = mTextureAssetPath[tag];
        return result;
    }

    void
    AssetManager::setPath(Sound_Tag tag,
                          std::string_view path) {
        mSoundAssetPath.emplace(tag, path);
    }

    void
    AssetManager::setPath(EntityAssetsPack tag,
                          std::string_view path) {
        mTextureAssetPath.emplace(tag, path);
    }

    std::vector<EntityAssetsPack>
    AssetManager::knownTags() {
        auto tags = std::vector<EntityAssetsPack>();
        for (auto &&asset: mTextureAssetPath) {
            tags.emplace_back(asset.first);
        }

        return tags;
    }
}

#include <oni-core/asset/oni-asset-manager.h>

#include <cassert>

namespace oni {
    namespace asset {
        AssetManager::AssetManager() {
        }

        const std::string &
        AssetManager::getAssetFilePath(component::Sound_Tag tag) {
            const auto &result = mSoundAssetPath[tag];
            return result;
        }

        const std::string &
        AssetManager::getAssetFilePath(component::EntityPreset tag) {
            const auto &result = mTextureAssetPath[tag];
            return result;
        }

        void
        AssetManager::setPath(component::Sound_Tag tag,
                              std::string_view path) {
            mSoundAssetPath.emplace(tag, path);
        }

        void
        AssetManager::setPath(component::EntityPreset tag,
                              std::string_view path) {
            mTextureAssetPath.emplace(tag, path);
        }

        std::vector<component::EntityPreset>
        AssetManager::knownTags() {
            auto tags = std::vector<component::EntityPreset>();
            for (auto &&asset: mTextureAssetPath) {
                tags.emplace_back(asset.first);
            }

            return tags;
        }
    }
}
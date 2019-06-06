#include <oni-core/asset/oni-asset-manager.h>

#include <cassert>

namespace oni {
    namespace asset {
        AssetManager::AssetManager() {
        }

        const std::string &
        AssetManager::getAssetFilePath(component::SoundTag tag) {
            const auto &result = mSoundAssetPath[tag];
            return result;
        }

        const std::string &
        AssetManager::getAssetFilePath(component::TextureTag tag) {
            const auto &result = mTextureAssetPath[tag];
            return result;
        }

        void
        AssetManager::setAssetFilePath(component::SoundTag tag,
                                       std::string_view path) {
            mSoundAssetPath.emplace(tag, path);
        }

        void
        AssetManager::setAssetFilePath(component::TextureTag tag,
                                       std::string_view path) {
            mTextureAssetPath.emplace(tag, path);
        }
    }
}
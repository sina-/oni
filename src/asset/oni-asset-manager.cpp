#include <oni-core/asset/oni-asset-manager.h>

#include <cassert>

namespace oni {
    namespace asset {
        AssetManager::AssetManager() {
        }

        std::string_view
        AssetManager::getAssetFilePath(component::SoundTag tag) {
            auto result = mSoundAssetPath[tag];
            assert(!result.empty());
            return result;
        }

        std::string_view
        AssetManager::getAssetFilePath(component::TextureTag tag) {
            auto result = mTextureAssetPath[tag];
            assert(!result.empty());
            return result;
        }

        void
        AssetManager::setAssetFilePath(component::SoundTag tag,
                                       std::string_view path) {
            mSoundAssetPath[tag] = path;
        }

        void
        AssetManager::setAssetFilePath(component::TextureTag tag,
                                       std::string_view path) {
            mTextureAssetPath[tag] = path;
        }
    }
}
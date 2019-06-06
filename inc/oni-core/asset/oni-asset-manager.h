#pragma once

#include <string_view>

#include <oni-core/asset/oni-asset-type.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/component/oni-component-visual.h>

namespace oni {
    namespace asset {
        class AssetManager {
        public:
            AssetManager();

            void
            setAssetFilePath(component::SoundTag,
                             std::string_view);

            void
            setAssetFilePath(component::TextureTag,
                             std::string_view);

            const std::string&
            getAssetFilePath(component::SoundTag);

            const std::string&
            getAssetFilePath(component::TextureTag);

        private:
            std::unordered_map<component::SoundTag, std::string> mSoundAssetPath;
            std::unordered_map<component::TextureTag, std::string> mTextureAssetPath;
        };
    }
}


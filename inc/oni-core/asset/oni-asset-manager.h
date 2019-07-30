#pragma once

#include <map>
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
            setPath(component::SoundTag,
                    std::string_view);

            void
            setPath(component::TextureTag,
                    std::string_view);

            const std::string &
            getAssetFilePath(component::SoundTag);

            const std::string &
            getAssetFilePath(component::TextureTag);

            std::vector<component::TextureTag>
            knownTags();

        private:
            std::map<component::SoundTag, std::string> mSoundAssetPath;
            std::map<component::TextureTag, std::string> mTextureAssetPath;
        };
    }
}


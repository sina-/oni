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
            setPath(component::Sound_Tag,
                    std::string_view);

            void
            setPath(component::EntityPreset,
                    std::string_view);

            const std::string &
            getAssetFilePath(component::Sound_Tag);

            const std::string &
            getAssetFilePath(component::EntityPreset);

            std::vector<component::EntityPreset>
            knownTags();

        private:
            std::map<component::Sound_Tag, std::string> mSoundAssetPath;
            std::map<component::EntityPreset, std::string> mTextureAssetPath;
        };
    }
}


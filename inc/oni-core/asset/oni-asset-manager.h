#pragma once

#include <map>
#include <string_view>

#include <oni-core/asset/oni-asset-type.h>
#include <oni-core/component/oni-component-audio.h>
#include <oni-core/component/oni-component-visual.h>

namespace oni {
    class AssetManager {
    public:
        AssetManager();

        void
        setPath(Sound_Tag,
                std::string_view);

        void
        setPath(EntityPreset,
                std::string_view);

        const std::string &
        getAssetFilePath(Sound_Tag);

        const std::string &
        getAssetFilePath(EntityPreset);

        std::vector<EntityPreset>
        knownTags();

    private:
        std::map<Sound_Tag, std::string> mSoundAssetPath;
        std::map<EntityPreset, std::string> mTextureAssetPath;
    };
}


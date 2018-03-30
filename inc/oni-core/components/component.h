#pragma once

#include <bitset>
#include <vector>

namespace oni {
    namespace components {
        enum Component {
            NONE,
            // Ready to be writen to. This is a temporary state. Very similar to NONE, but could be
            // useful if interested in knowing how many entities are created but never
            // initialized.
            READY,

            PLACEMENT,      // 2 -> 8
            APPEARANCE,     // 3 -> 16
            DYNAMIC,       // 4 -> 32
            TEXTURE,        // ...
            TEXT,

            // NOTE: Always keep it last to determine size.
            MAX
        };

        typedef std::bitset<MAX> Mask;
        typedef std::vector<std::bitset<MAX>> EntityMasks;

        const static components::Mask PlacementComponent = components::Mask().set(components::PLACEMENT);
        const static components::Mask AppearanceComponent = components::Mask().set(components::APPEARANCE);
        const static components::Mask DynamicComponent = components::Mask().set(components::DYNAMIC);
        const static components::Mask TextureComponent = components::Mask().set(components::TEXTURE);
        const static components::Mask TextComponent = components::Mask().set(components::TEXT);
    }

}

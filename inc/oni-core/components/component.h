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
            ORIENTATION,
            STATIC,

            // NOTE: Always keep it last to determine size.
            MAX

        };

        typedef std::bitset<MAX> Mask;
        typedef std::vector<std::bitset<MAX>> EntityMasks;

        const static Mask PlacementComponent = Mask().set(components::PLACEMENT);
        const static Mask AppearanceComponent = Mask().set(components::APPEARANCE);
        const static Mask DynamicComponent = Mask().set(components::DYNAMIC);
        const static Mask TextureComponent = Mask().set(components::TEXTURE);
        const static Mask OrientationComponent = Mask().set(components::ORIENTATION);
        const static Mask TextComponent = Mask().set(components::TEXT);
    }

}

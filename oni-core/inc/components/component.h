#pragma once

namespace oni {
    namespace components {
        enum Component {
            NONE,
            RESERVED,
            SPRITE,

            // NOTE: Always keep it last  to determine size.
            MAX
        };

        typedef std::bitset<MAX> Mask;
        typedef std::vector<std::bitset<components::Component::MAX>> EntityMask;
    }
}

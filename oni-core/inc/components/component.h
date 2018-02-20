#pragma once

namespace oni {
    namespace components {
        enum Component {
            NONE,
            // Ready to be writen to. This is a temporary state. Very similar to NONE, but could be
            // useful if interested in knowing how many entities are created but never
            // initialized.
            READY,

            POSITION,
            APPEARANCE,

            // NOTE: Always keep it last to determine size.
            MAX
        };

        typedef std::bitset<MAX> Mask;
        typedef std::vector<std::bitset<components::Component::MAX>> EntityMask;

    }

}

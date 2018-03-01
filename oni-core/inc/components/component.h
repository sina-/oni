#pragma once

namespace oni {
    namespace components {
        enum Component {
            NONE,
            // Ready to be writen to. This is a temporary state. Very similar to NONE, but could be
            // useful if interested in knowing how many entities are created but never
            // initialized.
            READY,

            PLACEMENT,      // 8
            APPEARANCE,     // 16
            VELOCITY,       // 32
            TEXTURE,
            SHADER,

            // NOTE: Always keep it last to determine size.
            MAX
        };

        typedef std::bitset<MAX> Mask;
        typedef std::vector<std::bitset<MAX>> EntityMask;

        const static components::Mask PlacementComponent = components::Mask().set(components::PLACEMENT);
        const static components::Mask AppearanceComponent = components::Mask().set(components::APPEARANCE);
        const static components::Mask VelocityComponent = components::Mask().set(components::VELOCITY);
        const static components::Mask TextureComponent = components::Mask().set(components::TEXTURE);
        const static components::Mask ShaderComponent = components::Mask().set(components::SHADER);
    }

}

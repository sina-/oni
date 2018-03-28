#pragma once

#include <string>
#include <vector>

#include <oni-core/components/input-data.h>

namespace oni {
    namespace io {
        class Input {
        public:

            Input();

            void update(components::oniKey keyPressed, components::oniKey keyReleased);

            bool isPressed(components::oniKey key) const;

            bool isReleased(components::oniKey key) const;

            void setPressed(components::oniKey key);

            void setReleased(components::oniKey key);

        private:
            std::vector<components::oniKey> mKeysPressed;
            std::vector<components::oniKey> mKeysReleased;
        };
    }
}

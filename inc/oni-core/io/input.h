#pragma once

#include <vector>

#include <oni-core/components/input-data.h>

namespace oni {
    namespace io {
        class Input {
        public:
            Input();

            template<class Archive>
            void serialize(Archive &archive) {
                archive(mKeysPressed, mKeysReleased);
            }

            void update(components::oniKeyPress keyPressed, components::oniKeyPress keyReleased);

            bool isPressed(components::oniKeyPress key) const;

            bool isReleased(components::oniKeyPress key) const;

            void setPressed(components::oniKeyPress key);

            void setReleased(components::oniKeyPress key);

        private:
            std::vector<components::oniKeyPress> mKeysPressed;
            std::vector<components::oniKeyPress> mKeysReleased;
        };
    }
}

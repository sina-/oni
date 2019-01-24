#pragma once

#include <vector>

#include <oni-core/component/input-data.h>

namespace oni {
    namespace io {
        class Input {
        public:
            Input();

            template<class Archive>
            void serialize(Archive &archive) {
                archive(mKeysPressed, mKeysReleased);
            }

            void update(component::oniKeyPress keyPressed, component::oniKeyPress keyReleased);

            bool isPressed(component::oniKeyPress key) const;

            bool isReleased(component::oniKeyPress key) const;

            void setPressed(component::oniKeyPress key);

            void setReleased(component::oniKeyPress key);

            void reset();

        private:
            std::vector<component::oniKeyPress> mKeysPressed;
            std::vector<component::oniKeyPress> mKeysReleased;
        };
    }
}

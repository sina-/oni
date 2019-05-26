#pragma once

#include <vector>

#include <oni-core/io/oni-io-input-data.h>

namespace oni {
    namespace io {
        class Input {
        public:
            Input();

            template<class Archive>
            void
            serialize(Archive &archive) {
                archive(mKeysPressed, mKeysReleased);
            }

            void
            update(io::oniKeyPress keyPressed,
                   io::oniKeyPress keyReleased);

            bool
            isPressed(io::oniKeyPress key) const;

            bool
            isReleased(io::oniKeyPress key) const;

            void
            setPressed(io::oniKeyPress key);

            void
            setReleased(io::oniKeyPress key);

            void
            reset();

            bool
            hasData() const;

        private:
            std::vector<io::oniKeyPress> mKeysPressed{};
            std::vector<io::oniKeyPress> mKeysReleased{};
        };
    }
}

#pragma once

#include <vector>

#include <oni-core/io/oni-io-input-data.h>
#include <oni-core/component/oni-component-geometry.h>

namespace oni {
    namespace io {
        enum class ScrollDirection {
            UNKNOWN,
            UP,
            DOWN,
            LAST
        };

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

            void
            reset();

            bool
            hasData() const;

            bool
            isPressed(io::oniKeyPress key) const;

            bool
            isReleased(io::oniKeyPress key) const;

            void
            setPressed(io::oniKeyPress key);

            void
            setReleased(io::oniKeyPress key);

            void
            addScrollDirectionX(io::ScrollDirection);

            void
            addScrollDirectionY(io::ScrollDirection);

            const std::vector<io::ScrollDirection> &
            getScrollDirectionX() const;

            const std::vector<io::ScrollDirection> &
            getScrollDirectionY() const;

            void
            setMouseButton(common::i32 button);

            void
            addCursor(oni::common::r64 x,
                      oni::common::r64 y);

            const std::vector<component::WorldP2D>&
            getCursor() const;

            const common::i32 &
            getMouseButton() const;

        private:
            std::vector<io::oniKeyPress> mKeysPressed{};
            std::vector<io::oniKeyPress> mKeysReleased{};

            std::vector<io::ScrollDirection> mScrollDirectionX{};
            std::vector<io::ScrollDirection> mScrollDirectionY{};

            common::i32 mMouseButton{-1};
            std::vector<component::WorldP2D> mCursorPos{};
        };
    }
}

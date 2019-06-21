#pragma once

#include <vector>

#include <oni-core/io/oni-io-input-data.h>

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
            setCursorY(oni::common::r64 y);

            void
            setCursorX(oni::common::r64 x);

            const common::i32 &
            getMouseButton() const;

            const oni::common::r64 &
            getCursorX() const;

            const oni::common::r64 &
            getCursorY() const;

        private:
            std::vector<io::oniKeyPress> mKeysPressed{};
            std::vector<io::oniKeyPress> mKeysReleased{};

            std::vector<io::ScrollDirection> mScrollDirectionX{};
            std::vector<io::ScrollDirection> mScrollDirectionY{};

            common::i32 mMouseButton{};
            common::r64 mCursorX{};
            common::r64 mCursorY{};
        };
    }
}

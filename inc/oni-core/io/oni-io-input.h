#pragma once

#include <vector>

#include <oni-core/io/oni-io-input-structure.h>
#include <oni-core/component/oni-component-geometry.h>


namespace oni {
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
        update(oniKeyPress keyPressed,
               oniKeyPress keyReleased);

        void
        reset();

        bool
        hasData() const;

        bool
        isPressed(oniKeyPress key) const;

        bool
        isReleased(oniKeyPress key) const;

        void
        setPressed(oniKeyPress key);

        void
        setReleased(oniKeyPress key);

        void
        addScrollDirectionX(ScrollDirection);

        void
        addScrollDirectionY(ScrollDirection);

        const std::vector<ScrollDirection> &
        getScrollDirectionX() const;

        const std::vector<ScrollDirection> &
        getScrollDirectionY() const;

        void
        setMosButtonPressed(i32 button);

        void
        setMosButtonReleased(i32 button);

        void
        addCursor(r64 x,
                  r64 y);

        const std::vector<Screen2D> &
        getCursor() const;

        i32
        getMosButtonPressed() const;

        i32
        getMosButtonReleased() const;

        bool
        isMosButtonPressed() const;

        bool
        isMosButtonReleased() const;

    private:
        std::vector<oniKeyPress> mKeysPressed{};
        std::vector<oniKeyPress> mKeysReleased{};

        std::vector<ScrollDirection> mScrollDirectionX{};
        std::vector<ScrollDirection> mScrollDirectionY{};

        i32 mMosButtonPressed{-1};
        i32 mMosButtonReleased{-1};
        std::vector<Screen2D> mCursorPos{};
    };
}

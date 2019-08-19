#include <algorithm>
#include <cassert>

#include <oni-core/io/oni-io-input.h>


namespace oni {
    Input::Input() = default;

    void
    Input::update(oniKeyPress keyPressed,
                  oniKeyPress keyReleased) {
        assert(keyPressed != keyReleased);

        if (keyPressed > 0) {
            setPressed(keyPressed);
        }
        if (keyReleased > 0) {
            setReleased(keyReleased);
        }

        assert(mKeysPressed.size() < 50);
        assert(mKeysReleased.size() < 50);
    }

    void
    Input::reset() {
        mScrollDirectionX.clear();
        mScrollDirectionY.clear();
        mCursorPos.clear();
    }

    bool
    Input::hasData() const {
        return !mKeysPressed.empty() || !mKeysReleased.empty();
    }

    bool
    Input::isPressed(oniKeyPress key) const {
        return std::find(mKeysPressed.begin(), mKeysPressed.end(), key) != mKeysPressed.end();
    }

    bool
    Input::isReleased(oniKeyPress key) const {
        return std::find(mKeysReleased.begin(), mKeysReleased.end(), key) != mKeysReleased.end();
    }

    void
    Input::setPressed(oniKeyPress key) {
        if (!isPressed(key)) {
            mKeysPressed.push_back(key);
        }
        if (isReleased(key)) {
            mKeysReleased.erase(std::find(mKeysReleased.begin(), mKeysReleased.end(), key));
        }
    }

    void
    Input::setReleased(oniKeyPress key) {
        if (!isReleased(key)) {
            mKeysReleased.push_back(key);
        }
        if (isPressed(key)) {
            mKeysPressed.erase(std::find(mKeysPressed.begin(), mKeysPressed.end(), key));
        }
    }

    void
    Input::addScrollDirectionX(ScrollDirection sd) {
        mScrollDirectionX.push_back(sd);
    }

    void
    Input::addScrollDirectionY(ScrollDirection sd) {
        mScrollDirectionY.push_back(sd);
    }

    const std::vector<ScrollDirection> &
    Input::getScrollDirectionX() const {
        return mScrollDirectionX;
    }

    const std::vector<ScrollDirection> &
    Input::getScrollDirectionY() const {
        return mScrollDirectionY;
    }

    void
    Input::setMouseButton(i32 button) { mMouseButton = button; }

    const i32 &
    Input::getMouseButton() const { return mMouseButton; }

    void
    Input::addCursor(r64 x,
                     r64 y) {
        mCursorPos.push_back({r32(x), r32(y)});
    }

    const std::vector<WorldP2D> &
    Input::getCursor() const {
        return mCursorPos;
    }
}

#include <iostream>
#include <algorithm>
#include <cassert>

#include <oni-core/io/oni-io-input.h>


namespace oni {
    namespace io {
        Input::Input() = default;

        void
        Input::update(io::oniKeyPress keyPressed,
                      io::oniKeyPress keyReleased) {
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
        Input::isPressed(io::oniKeyPress key) const {
            return std::find(mKeysPressed.begin(), mKeysPressed.end(), key) != mKeysPressed.end();
        }

        bool
        Input::isReleased(io::oniKeyPress key) const {
            return std::find(mKeysReleased.begin(), mKeysReleased.end(), key) != mKeysReleased.end();
        }

        void
        Input::setPressed(io::oniKeyPress key) {
            if (!isPressed(key)) {
                mKeysPressed.push_back(key);
            }
            if (isReleased(key)) {
                mKeysReleased.erase(std::find(mKeysReleased.begin(), mKeysReleased.end(), key));
            }
        }

        void
        Input::setReleased(io::oniKeyPress key) {
            if (!isReleased(key)) {
                mKeysReleased.push_back(key);
            }
            if (isPressed(key)) {
                mKeysPressed.erase(std::find(mKeysPressed.begin(), mKeysPressed.end(), key));
            }
        }

        void
        Input::addScrollDirectionX(io::ScrollDirection sd) {
            mScrollDirectionX.push_back(sd);
        }

        void
        Input::addScrollDirectionY(io::ScrollDirection sd) {
            mScrollDirectionY.push_back(sd);
        }

        const std::vector<io::ScrollDirection> &
        Input::getScrollDirectionX() const {
            return mScrollDirectionX;
        }

        const std::vector<io::ScrollDirection> &
        Input::getScrollDirectionY() const {
            return mScrollDirectionY;
        }

        void
        Input::setMouseButton(common::i32 button) { mMouseButton = button; }

        const common::i32 &
        Input::getMouseButton() const { return mMouseButton; }

        void
        Input::addCursor(oni::common::r64 x,
                         oni::common::r64 y) {
            mCursorPos.push_back({oni::common::r32(x), oni::common::r32(y)});
        }

        const std::vector<component::WorldP2D> &
        Input::getCursor() const {
            return mCursorPos;
        }
    }
}

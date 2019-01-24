#include <iostream>
#include <algorithm>
#include <cassert>

#include <oni-core/io/input.h>


namespace oni {
    namespace io {
        Input::Input() = default;

        void Input::update(component::oniKeyPress keyPressed, component::oniKeyPress keyReleased) {
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

        bool Input::isPressed(component::oniKeyPress key) const {
            return std::find(mKeysPressed.begin(), mKeysPressed.end(), key) != mKeysPressed.end();
        }

        bool Input::isReleased(component::oniKeyPress key) const {
            return std::find(mKeysReleased.begin(), mKeysReleased.end(), key) != mKeysReleased.end();
        }

        void Input::setPressed(component::oniKeyPress key) {
            if (!isPressed(key)) {
                mKeysPressed.push_back(key);
            }
            if (isReleased(key)) {
                mKeysReleased.erase(std::find(mKeysReleased.begin(), mKeysReleased.end(), key));
            }
        }

        void Input::setReleased(component::oniKeyPress key) {
            if (!isReleased(key)) {
                mKeysReleased.push_back(key);
            }
            if (isPressed(key)) {
                mKeysPressed.erase(std::find(mKeysPressed.begin(), mKeysPressed.end(), key));
            }
        }

        void Input::reset() {
            mKeysPressed.clear();
            mKeysReleased.clear();
        }
    }
}

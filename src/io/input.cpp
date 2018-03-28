#include <iostream>
#include <algorithm>

#include <oni-core/io/input.h>
#include <oni-core/utils/oni-assert.h>


namespace oni {
    namespace io {
        Input::Input() = default;

        void Input::update(components::oniKey keyPressed, components::oniKey keyReleased) {
            ONI_DEBUG_ASSERT(keyPressed != keyReleased);

            if (keyPressed > 0) {
                setPressed(keyPressed);
            }
            if (keyReleased > 0) {
                setReleased(keyReleased);
            }

            ONI_DEBUG_ASSERT(mKeysPressed.size() < 50);
            ONI_DEBUG_ASSERT(mKeysReleased.size() < 50);
        }

        bool Input::isPressed(components::oniKey key) const {
            return std::find(mKeysPressed.begin(), mKeysPressed.end(), key) != mKeysPressed.end();
        }

        bool Input::isReleased(components::oniKey key) const {
            return std::find(mKeysReleased.begin(), mKeysReleased.end(), key) != mKeysReleased.end();
        }

        void Input::setPressed(components::oniKey key) {
            if (!isPressed(key)) {
                mKeysPressed.push_back(key);
            }
            if (isReleased(key)) {
                mKeysReleased.erase(std::find(mKeysReleased.begin(), mKeysReleased.end(), key));
            }
        }

        void Input::setReleased(components::oniKey key) {
            if (!isReleased(key)) {
                mKeysReleased.push_back(key);
            }
            if (isPressed(key)) {
                mKeysPressed.erase(std::find(mKeysPressed.begin(), mKeysPressed.end(), key));
            }
        }
    }
}

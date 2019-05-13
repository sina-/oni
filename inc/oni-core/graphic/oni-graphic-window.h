#pragma once

#include <oni-core/io/oni-io-input.h>
#include <oni-core/common/oni-common-typedefs-graphic.h>

class GLFWwindow;

namespace oni {
    namespace graphic {


        /*
         * Creates a windows of given name and size. Currently handles:
         *  - Resize
         *  - Key, or button, -press
         *  - Mouse position
         *  by registering a call-back that is called on such events.
         */
        class Window {
            std::string mTitle{};
            common::i32 mWidth{};
            common::i32 mHeight{};
            common::i32 mGameWidth{};
            common::i32 mGameHeight{};
            GLFWwindow *mWindow{};
            common::i32 mMouseButton{};
            common::r64 mCursorX{};
            common::r64 mCursorY{};
            std::vector<common::i32> mKeysPressed{};
            std::vector<common::i32> mKeysReleased{};


        public:
            Window(std::string &&name,
                   common::i32 gameWidth,
                   common::i32 gameHeight);

            ~Window();

            Window(const Window &) = delete;

            Window &
            operator=(const Window &) = delete;

            void
            tick(io::Input &input);

            void
            display();

            bool
            closed() const;

            void
            clear() const;

            void
            setWidth(common::i32 width) { mWidth = width; }

            const common::i32 &
            getWidth() const { return mWidth; }

            void
            setHeight(common::i32 height) { mHeight = height; }

            const common::i32 &
            getHeight() const { return mHeight; }

            common::i32
            getGameWidth() { return mGameWidth; }

            common::i32
            getGameHeight() { return mGameHeight; }

            void
            addKeyPressed(common::i32 key) { mKeysPressed.push_back(key); }

            void
            addKeyReleased(common::i32 key) { mKeysReleased.push_back(key); }

            void
            setMouseButton(common::i32 button) { mMouseButton = button; }

            const common::i32 &
            getMouseButton() const { return mMouseButton; }

            void
            setCursorX(oni::common::r64 x) { mCursorX = x; }

            const oni::common::r64 &
            getCursorX() const { return mCursorX; }

            void
            setCursorY(oni::common::r64 y) { mCursorY = y; }

            const oni::common::r64 &
            getCursorY() const { return mCursorY; }

            static Window *
            getThisFromGLFWWindow(GLFWwindow *window);

            static void
            windowResizeCallback(GLFWwindow *window,
                                 common::i32 width,
                                 common::i32 height);

            static void
            keyCallback(GLFWwindow *window,
                        common::i32 key,
                        common::i32 scancode,
                        common::i32 action,
                        common::i32 mods);

            static void
            mouseCallback(GLFWwindow *window,
                          common::i32 button,
                          common::i32 action,
                          common::i32 mods);

            static void
            cursorPosCallback(GLFWwindow *window,
                              oni::common::r64 x,
                              oni::common::r64 y);

        private:
            static void
            messageCallback(common::oniGLenum source,
                            common::oniGLenum type,
                            common::oniGLuint id,
                            common::oniGLenum severity,
                            common::oniGLsizei length,
                            const common::oniGLchar *message,
                            const void *userParam);

        };
    }
}
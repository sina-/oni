#pragma once

#include <oni-core/io/input.h>
#include <oni-core/common/typedefs-graphics.h>

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
            common::int32 mWidth{};
            common::int32 mHeight{};
            common::int32 mGameWidth{};
            common::int32 mGameHeight{};
            GLFWwindow *mWindow{};
            common::int32 mMouseButton{};
            common::real64 mCursorX{};
            common::real64 mCursorY{};
            std::vector<common::int32> mKeysPressed{};
            std::vector<common::int32> mKeysReleased{};


        public:
            Window(std::string &&name, common::int32 gameWidth, common::int32 gameHeight);

            ~Window();

            Window(const Window &) = delete;

            Window &operator=(const Window &) = delete;

            void tick(io::Input &input);

            void display();

            bool closed() const;

            void clear() const;

            void setWidth(common::int32 width) { mWidth = width; }

            const common::int32 &getWidth() const { return mWidth; }

            void setHeight(common::int32 height) { mHeight = height; }

            const common::int32 &getHeight() const { return mHeight; }

            common::int32 getGameWidth() { return mGameWidth; }

            common::int32 getGameHeight() { return mGameHeight; }

            void addKeyPressed(common::int32 key) { mKeysPressed.push_back(key); }

            void addKeyReleased(common::int32 key) { mKeysReleased.push_back(key); }

            void setMouseButton(common::int32 button) { mMouseButton = button; }

            const common::int32 &getMouseButton() const { return mMouseButton; }

            void setCursorX(oni::common::real64 x) { mCursorX = x; }

            const oni::common::real64 &getCursorX() const { return mCursorX; }

            void setCursorY(oni::common::real64 y) { mCursorY = y; }

            const oni::common::real64 &getCursorY() const { return mCursorY; }

            static Window *getThisFromGLFWWindow(GLFWwindow *window);

            static void windowResizeCallback(GLFWwindow *window, common::int32 width, common::int32 height);

            static void keyCallback(GLFWwindow *window, common::int32 key, common::int32 scancode, common::int32 action,
                                    common::int32 mods);

            static void
            mouseCallback(GLFWwindow *window, common::int32 button, common::int32 action, common::int32 mods);

            static void cursorPosCallback(GLFWwindow *window, oni::common::real64 x, oni::common::real64 y);

        private:
            static void messageCallback(common::oniGLenum source,
                                        common::oniGLenum type,
                                        common::oniGLuint id,
                                        common::oniGLenum severity,
                                        common::oniGLsizei length,
                                        const common::oniGLchar *message,
                                        const void *userParam);

        };
    }
}
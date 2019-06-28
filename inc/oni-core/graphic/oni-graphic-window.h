#pragma once

#include <string>

#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/common/oni-common-typedef.h>

class GLFWwindow;

namespace oni {
    namespace io {
        class Input;
    }
    namespace graphic {
        class Window {
        public:
            Window(io::Input &,
                   std::string &&name,
                   common::i32 gameWidth,
                   common::i32 gameHeight);

            ~Window();

            Window(const Window &) = delete;

            Window &
            operator=(const Window &) = delete;

            static void
            tick(io::Input &input);

            void
            display();

            bool
            closed() const;

            void
            clear() const;

            const common::i32 &
            getWidth() const;

            const common::i32 &
            getHeight() const;

        private:
            void
            addKeyPressed(common::i32 key);

            void
            addKeyReleased(common::i32 key);

            void
            setHeight(common::i32 height);

            void
            setWidth(common::i32 width);

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
            scrollCallback(GLFWwindow *window,
                           common::r64 xOffset,
                           common::r64 yOffset);

            static void
            cursorPosCallback(GLFWwindow *window,
                              oni::common::r64 x,
                              oni::common::r64 y);

            static void
            messageCallback(common::oniGLenum source,
                            common::oniGLenum type,
                            common::oniGLuint id,
                            common::oniGLenum severity,
                            common::oniGLsizei length,
                            const common::oniGLchar *message,
                            const void *userParam);

        private:
            io::Input &mInput;

            std::string mTitle{};
            common::i32 mWidth{};
            common::i32 mHeight{};
            GLFWwindow *mWindow{};
        };
    }
}
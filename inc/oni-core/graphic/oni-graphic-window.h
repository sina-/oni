#pragma once

#include <string>

#include <oni-core/component/oni-component-fwd.h>
#include <oni-core/common/oni-common-typedefs-graphic.h>
#include <oni-core/common/oni-common-typedef.h>
#include <oni-core/io/oni-io-fwd.h>

class GLFWwindow;

namespace oni {
    class Window {
    public:
        Window(Input &,
               std::string &&name,
               i32 gameWidth,
               i32 gameHeight);

        ~Window();

        Window(const Window &) = delete;

        Window &
        operator=(const Window &) = delete;

        static void
        tick(Input &input);

        void
        display();

        bool
        closed() const;

        void
        clear() const;

        void
        setClear(const Color &) const;

        const i32 &
        getWidth() const;

        const i32 &
        getHeight() const;

    private:
        void
        addKeyPressed(i32 key);

        void
        addKeyReleased(i32 key);

        void
        setHeight(i32 height);

        void
        setWidth(i32 width);

        static Window *
        getThisFromGLFWWindow(GLFWwindow *window);

        static void
        windowResizeCallback(GLFWwindow *window,
                             i32 width,
                             i32 height);

        static void
        keyCallback(GLFWwindow *window,
                    i32 key,
                    i32 scancode,
                    i32 action,
                    i32 mods);

        static void
        mouseCallback(GLFWwindow *window,
                      i32 button,
                      i32 action,
                      i32 mods);

        static void
        scrollCallback(GLFWwindow *window,
                       r64 xOffset,
                       r64 yOffset);

        static void
        cursorPosCallback(GLFWwindow *window,
                          r64 x,
                          r64 y);

        static void
        openGLErrorCallback(oniGLenum source,
                            oniGLenum type,
                            oniGLuint id,
                            oniGLenum severity,
                            oniGLsizei length,
                            const oniGLchar *message,
                            const void *userParam);

        static void
        glfwErrorCallback(i32 error,
                          const c8 *msg);

    private:
        Input &mInput;

        std::string mTitle{};
        i32 mWidth{};
        i32 mHeight{};
        GLFWwindow *mWindow{};
    };
}
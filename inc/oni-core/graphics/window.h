#pragma once

#include <stdexcept>
#include <functional>

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <oni-core/utils/io.h>

namespace oni {
    namespace graphics {


        /*
         * Creates a windows of given name and size. Currently handles:
         *  - Resize
         *  - Key, or button, -press
         *  - Mouse position
         *  by registering a call-back that is called on such events.
         */
        class Window {
            const char *mTitle;
            int mWidth, mHeight;
            int mXScaling, mYScaling;
            GLFWwindow *mWindow;
            int mKeyPressed;
            int mMouseButton;
            double mCursorX, mCursorY;

        public:
            Window(const char *name, int width, int height, int xScaling, int yScaling);

            ~Window();

            void update() const;

            bool closed() const;

            void clear() const;

            void setWidth(int width) { mWidth = width; }

            const int &getWidth() const { return mWidth; }

            void setHeight(int height) { mHeight = height; }

            const int &getHeight() const { return mHeight; }

            int getXScaling() { return mXScaling; }

            int getYScaling() { return mYScaling; }

            void setKeyPressed(int key) { mKeyPressed = key; }

            const int &getKeyPressed() const { return mKeyPressed; }

            void setMouseButton(int button) { mMouseButton = button; }

            const int &getMouseButton() const { return mMouseButton; }

            void setCursorX(double x) { mCursorX = x; }

            const double &getCursorX() const { return mCursorX; }

            void setCursorY(double y) { mCursorY = y; }

            const double &getCursorY() const { return mCursorY; }

            static Window *getThisFromGLFWWindow(GLFWwindow *window) {
                return reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
            }

            static void windowResizeCallback(GLFWwindow *window, int width, int height);

            static void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

            static void mouseCallback(GLFWwindow *window, int button, int action, int mods);

            static void cursorPosCallback(GLFWwindow *window, double x, double y);
        };
    }
}
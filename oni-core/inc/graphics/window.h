#pragma once

#include <stdexcept>
#include <functional>

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <utils/io.h>

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
            const char *m_Title;
            int m_Width, m_Height;
            GLFWwindow *m_Window;
            int m_KeyPressed;
            int m_MouseButton;
            double m_CursorX, m_CursorY;

        public:
            Window(const char *name, int width, int height);

            ~Window();

            void update() const;

            bool closed() const;

            void clear() const;

            void setWidth(int width) { m_Width = width; }

            const int &getWidth() const { return m_Width; }

            void setHeight(int height) { m_Height = height; }

            const int &getHeight() const { return m_Height; }

            void setKeyPressed(int key) { m_KeyPressed = key; }

            const int &getKeyPressed() const { return m_KeyPressed; }

            void setMouseButton(int button) { m_MouseButton = button; }

            const int &getMouseButton() const { return m_MouseButton; }

            void setCursorX(double x) { m_CursorX = x; }

            const double &getCursorX() const { return m_CursorX; }

            void setCursorY(double y) { m_CursorY = y; }

            const double &getCursorY() const { return m_CursorY; }

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
#pragma once

#include <stdexcept>
#include <functional>

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <oni-core/io/input.h>

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
            std::string mTitle;
            int mWidth, mHeight;
            int mGameWidth, mGameHeight;
            GLFWwindow *mWindow;
            int mMouseButton;
            double mCursorX, mCursorY;
            std::vector<int> mKeysPressed;
            std::vector<int> mKeysReleased;


        public:
            Window(std::string &&name, int width, int height, int gameWidth, int gameHeight);

            ~Window();

            Window(const Window &) = delete;

            Window &operator=(const Window &) = delete;

            void tick(io::Input &input);

            bool closed() const;

            void clear() const;

            void setWidth(int width) { mWidth = width; }

            const int &getWidth() const { return mWidth; }

            void setHeight(int height) { mHeight = height; }

            const int &getHeight() const { return mHeight; }

            int getGameWidth() { return mGameWidth; }

            int getGameHeight() { return mGameHeight; }

            void addKeyPressed(int key) { mKeysPressed.push_back(key); }

            void addKeyReleased(int key) { mKeysReleased.push_back(key); }

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

        private:
            static void messageCallback(GLenum source,
                                        GLenum type,
                                        GLuint id,
                                        GLenum severity,
                                        GLsizei length,
                                        const GLchar *message,
                                        const void *userParam);

        };
    }
}
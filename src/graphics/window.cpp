#include <AntTweakBar.h>

#include <oni-core/graphics/window.h>
#include <oni-core/common/defines.h>

namespace oni {
    namespace graphics {

        Window::Window(std::string &&title, common::int32 gameWidth, common::int32 gameHeight) :
                mTitle{std::move(title)},
                mGameWidth{gameWidth},
                mGameHeight{gameHeight},
                mWindow{nullptr},
                mMouseButton{GLFW_KEY_UNKNOWN}, mCursorX{0.0f}, mCursorY{0.0f},
                mKeysPressed{}, mKeysReleased{} {

            if (!glfwInit())
                throw std::runtime_error("Failed to init GLFW!");

            common::int32 monitorCount{};
            auto monitors = glfwGetMonitors(&monitorCount);
            const GLFWvidmode *mode = glfwGetVideoMode(monitors[0]);
            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

            mWindow = glfwCreateWindow(mode->width, mode->height, mTitle.c_str(), monitors[0], nullptr);

            mWidth = mode->width;
            mHeight = mode->height;

            if (!mWindow) {
                glfwTerminate();
                throw std::runtime_error("Failed to create window!");
            }

            glfwMakeContextCurrent(mWindow);

            if (glewInit() != GLEW_OK)
                throw std::runtime_error("GLEW failed to initialize!");

            printf("OpenGL version supported by this platform (%s): \n", glGetString(GL_VERSION));

            /*
            Final solution for accessing (Window*) from C callback function.
            (Inspired from: https://codereview.stackexchange.com/a/119374)
            Generally do this:
                1) call glfwSetWindowUserPointer to associate a pointer to user object and a window;
                2) define a static callback function and register it as a call back;
                3) gain access to Window instance by casting GLFWWindow* to a pointer to user object!
            */
            glfwSetWindowUserPointer(mWindow, reinterpret_cast<void *>(this));
            /*
            1) First try
            auto windowResizeCallback = [](GLFWwindow* w, common::int32 width, common::int32 height)
            {
                static_cast<Window*>(glfwGetWindowUserPointer(w))->windowResizeCallback(w, width, height);
            };
            2) Second try. It didn't work :(
            std::function<void(GLFWwindow*, common::int32, common::int32)> windowResizeCallback = std::bind(&Window::windowResizeCallback,
                this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
            */

            glfwSetWindowSizeCallback(mWindow, windowResizeCallback);
            glfwSetKeyCallback(mWindow, keyCallback);
            glfwSetMouseButtonCallback(mWindow, mouseCallback);
            glfwSetCursorPosCallback(mWindow, cursorPosCallback);
            // This will disable v-sync.
            glfwSwapInterval(0.0);


            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glEnable(GL_DEBUG_OUTPUT);
            glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
            glDebugMessageCallback(&messageCallback, nullptr);
        }

        Window::~Window() {
            glfwTerminate();
            mWindow = nullptr;
        }

        void Window::messageCallback(GLenum source,
                                     GLenum type,
                                     GLuint id,
                                     GLenum severity,
                                     GLsizei length,
                                     const GLchar *message,
                                     const void *userParam) {
            UNUSED(id);
            UNUSED(length);
            UNUSED(userParam);
            if (type == GL_DEBUG_TYPE_ERROR) {
                fprintf(stderr, "GL CALLBACK: type = 0x%x, severity = 0x%x, message = %s\n",
                        type, severity, message);
                throw std::runtime_error("OpenGL error!");
            } else {
                fprintf(stderr, "GL CALLBACK: type = 0x%x, severity = 0x%x, source = 0x%x,message = %s\n",
                        type, severity, source, message);
            }
        }


        void Window::tick(io::Input &input) {
            glfwPollEvents();

            for (auto key: mKeysPressed) {
                input.setPressed(key);
            }
            for (auto key: mKeysReleased) {
                input.setReleased(key);
            }
            mKeysPressed.clear();
            mKeysReleased.clear();
        }

        bool Window::closed() const {
            return glfwWindowShouldClose(mWindow) == 1;
        }

        void Window::clear() const {
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glClearColor(0.2f, 0.2f, 0.2f, 0.2f);
        }

        void Window::windowResizeCallback(GLFWwindow *window, common::int32 width, common::int32 height) {
            auto thiz = getThisFromGLFWWindow(window);
            TwWindowSize(width, height);

            glViewport(0, 0, width, height);
            thiz->setHeight(height);
            thiz->setWidth(width);
        }

        void Window::keyCallback(GLFWwindow *window, common::int32 key, common::int32 scancode, common::int32 action,
                                 common::int32 mods) {
            UNUSED(scancode);
            UNUSED(mods);
            auto thiz = getThisFromGLFWWindow(window);
            if (action == GLFW_PRESS)
                thiz->addKeyPressed(key);
            if (action == GLFW_RELEASE)
                thiz->addKeyReleased(key);
        }

        void
        Window::mouseCallback(GLFWwindow *window, common::int32 button, common::int32 action, common::int32 mods) {
            UNUSED(mods);
            TwEventMouseButtonGLFW(button, action);
            auto thiz = getThisFromGLFWWindow(window);
            if (action == GLFW_PRESS)
                thiz->setMouseButton(button);
            if (action == GLFW_RELEASE)
                thiz->setMouseButton(GLFW_KEY_UNKNOWN);
        }

        void Window::cursorPosCallback(GLFWwindow *window, double x, double y) {
            TwEventMousePosGLFW(x, y);
            auto thiz = getThisFromGLFWWindow(window);
            thiz->setCursorX(x);
            thiz->setCursorY(y);
        }

        void Window::display() {
            glfwSwapBuffers(mWindow);
        }

    }
}

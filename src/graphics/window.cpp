#include <AntTweakBar.h>

#include <oni-core/graphics/window.h>
#include <oni-core/common/defines.h>

namespace oni {
    namespace graphics {

        Window::Window(const char *title, int width, int height, int xScaling, int yScaling) {
            mTitle = title;
            mWidth = width;
            mHeight = height;
            mGameWidth = xScaling;
            mGameHeight = yScaling;
            mMouseButton = GLFW_KEY_UNKNOWN;
            mCursorX = 0.0;
            mCursorY = 0.0;

            if (!glfwInit())
                throw std::runtime_error("Failed to init GLFW!");

            mWindow = glfwCreateWindow(mWidth, mHeight, mTitle, nullptr, nullptr);
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
            auto windowResizeCallback = [](GLFWwindow* w, int width, int height)
            {
                static_cast<Window*>(glfwGetWindowUserPointer(w))->windowResizeCallback(w, width, height);
            };
            2) Second try. It didn't work :(
            std::function<void(GLFWwindow*, int, int)> windowResizeCallback = std::bind(&Window::windowResizeCallback,
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
            glDebugMessageCallback((GLDEBUGPROC) &messageCallback, nullptr);
        }

        Window::~Window() {
            glfwTerminate();
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
            glfwSwapBuffers(mWindow);

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

        void Window::windowResizeCallback(GLFWwindow *window, int width, int height) {
            auto thiz = getThisFromGLFWWindow(window);
            TwWindowSize(width, height);

            glViewport(0, 0, width, height);
            thiz->setHeight(height);
            thiz->setWidth(width);
        }

        void Window::keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
            auto thiz = getThisFromGLFWWindow(window);
            if (action == GLFW_PRESS)
                thiz->addKeyPressed(key);
            if (action == GLFW_RELEASE)
                thiz->addKeyReleased(key);
        }

        void Window::mouseCallback(GLFWwindow *window, int button, int action, int mods) {
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

    }
}

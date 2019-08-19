#include <oni-core/graphic/oni-graphic-window.h>

#include <cassert>

#define GLEW_STATIC

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <AntTweakBar.h>

#include <oni-core/common/oni-common-define.h>
#include <oni-core/io/oni-io-input.h>
#include <oni-core/math/oni-math-function.h>


namespace oni {
    Window::Window(Input &input,
                   std::string &&title,
                   i32 gameWidth,
                   i32 gameHeight) :
            mInput(input),
            mTitle{std::move(title)} {

        if (!glfwInit()) {
            assert(false);
        }

/*            common::i32 monitorCount{};
            auto monitors = glfwGetMonitors(&monitorCount);
            const GLFWvidmode *mode = glfwGetVideoMode(monitors[0]);
            glfwWindowHint(GLFW_RED_BITS, mode->redBits);
            glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
            glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
            glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

            mWindow = glfwCreateWindow(mode->width, mode->height, mTitle.c_str(), monitors[0], nullptr);
            mWidth = mode->width;
            mHeight = mode->height;

            */
        mWindow = glfwCreateWindow(gameWidth, gameHeight, mTitle.c_str(), nullptr, nullptr);
        mWidth = gameWidth;
        mHeight = gameHeight;

        if (!mWindow) {
            glfwTerminate();
            assert(false);
        }

        glfwMakeContextCurrent(mWindow);

        if (glewInit() != GLEW_OK) {
            assert(false);
        }

        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(&messageCallback, nullptr);

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
        auto windowResizeCallback = [](GLFWwindow* w, common::i32 width, common::i32 height)
        {
            static_cast<Window*>(glfwGetWindowUserPointer(w))->windowResizeCallback(w, width, height);
        };
        2) Second try. It didn't work :(
        std::function<void(GLFWwindow*, common::i32, common::i32)> windowResizeCallback = std::bind(&Window::windowResizeCallback,
            this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
        */

        glfwSetWindowSizeCallback(mWindow, windowResizeCallback);
        glfwSetKeyCallback(mWindow, keyCallback);
        glfwSetMouseButtonCallback(mWindow, mouseCallback);
        glfwSetScrollCallback(mWindow, scrollCallback);
        glfwSetCursorPosCallback(mWindow, cursorPosCallback);
        // This will disable v-sync.
        glfwSwapInterval(0.0);

        glEnable(GL_BLEND);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glDepthMask(GL_TRUE);
        glDepthRange(0.f, 1.f);

        // NOTE: All rgb values are pre-multiplied by the alpha
        glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

        glViewport(0, 0, mWidth, mHeight);
        glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
        glClearDepth(1.f);
    }

    Window::~Window() {
        glfwTerminate();
        mWindow = nullptr;
    }

    void
    Window::messageCallback(GLenum source,
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
            assert(false);
        } else {
            //fprintf(stderr, "GL CALLBACK: type = 0x%x, severity = 0x%x, source = 0x%x,message = %s\n",
            //       type, severity, source, message);
        }
    }


    void
    Window::tick(Input &input) {
        glfwPollEvents();
    }

    bool
    Window::closed() const {
        return glfwWindowShouldClose(mWindow) == 1;
    }

    void
    Window::clear() const {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void
    Window::windowResizeCallback(GLFWwindow *window,
                                 i32 width,
                                 i32 height) {
        // TODO: Can't handle viewport size changes as the renderer relies on it and this change is not communicated
        // to it. Have to re-design Window and Renderer and SceneManager to better isolate OGL calls and viewport
        // ownership
        return;
        auto thiz = getThisFromGLFWWindow(window);
        TwWindowSize(width, height);

        glViewport(0, 0, width, height);
        thiz->setHeight(height);
        thiz->setWidth(width);
    }

    void
    Window::keyCallback(GLFWwindow *window,
                        i32 key,
                        i32 scancode,
                        i32 action,
                        i32 mods) {
        UNUSED(scancode);
        UNUSED(mods);
        auto thiz = getThisFromGLFWWindow(window);
        if (action == GLFW_PRESS)
            thiz->addKeyPressed(key);
        if (action == GLFW_RELEASE)
            thiz->addKeyReleased(key);
    }

    void
    Window::mouseCallback(GLFWwindow *window,
                          i32 button,
                          i32 action,
                          i32 mods) {
        UNUSED(mods);
        TwEventMouseButtonGLFW(button, action);
        auto thiz = getThisFromGLFWWindow(window);
        if (action == GLFW_PRESS) {
            thiz->mInput.setMouseButton(button);
        }
        if (action == GLFW_RELEASE) {
            thiz->mInput.setMouseButton(GLFW_KEY_UNKNOWN);
        }
    }

    void
    Window::scrollCallback(GLFWwindow *window,
                           r64 xOffset,
                           r64 yOffset) {
        constexpr auto SENSITIVITY = 0.01;
        auto thiz = getThisFromGLFWWindow(window);
        if (almost_Greater(yOffset, SENSITIVITY)) {
            thiz->mInput.addScrollDirectionY(ScrollDirection::UP);
        }
        if (almost_Less(yOffset, -SENSITIVITY)) {
            thiz->mInput.addScrollDirectionY(ScrollDirection::DOWN);
        }
    }

    void
    Window::cursorPosCallback(GLFWwindow *window,
                              double x,
                              double y) {
        TwEventMousePosGLFW(x, y);
        auto thiz = getThisFromGLFWWindow(window);
        thiz->mInput.addCursor(x, thiz->mHeight - y);
    }

    void
    Window::display() {
        glfwSwapBuffers(mWindow);
    }

    Window *
    Window::getThisFromGLFWWindow(GLFWwindow *window) {
        return reinterpret_cast<Window *>(glfwGetWindowUserPointer(window));
    }

    void
    Window::addKeyPressed(i32 key) { mInput.setPressed(key); }

    void
    Window::addKeyReleased(i32 key) { mInput.setReleased(key); }

    const i32 &
    Window::getHeight() const { return mHeight; }

    void
    Window::setHeight(i32 height) { mHeight = height; }

    const i32 &
    Window::getWidth() const { return mWidth; }

    void
    Window::setWidth(i32 width) { mWidth = width; }
}

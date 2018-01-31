#include "graphics/window.h"

namespace oni {
	namespace graphics {

		Window::Window(const char * title, int width, int height)
		{
			m_Title = title;
			m_Width = width;
			m_Height = height;
			m_KeyPressed = GLFW_KEY_UNKNOWN;
			m_MouseButton = 0;
			m_CursorX = 0.0;
			m_CursorY = 0.0;

			if (!glfwInit())
				throw std::runtime_error("Failed to init GLFW!");

			m_Window = glfwCreateWindow(m_Width, m_Height, m_Title, nullptr, nullptr);
			if (!m_Window) {
				glfwTerminate();
				throw std::runtime_error("Failed to create window!");
			}

			glfwMakeContextCurrent(m_Window);

			if (glewInit() != GLEW_OK)
				throw std::runtime_error("GLEW failed to initialize!");

			/* 
			Final solution for accessing (Window*) from C callback function.
			(Inspired from: https://codereview.stackexchange.com/a/119374)
			Generally do this:
				1) call glfwSetWindowUserPointer to associate a pointer to user object and a window;
				2) define a static callback function and register it as a call back;
				3) gain access to Window instance by casting GLFWWindow* to a pointer to user object!
			*/
			glfwSetWindowUserPointer(m_Window, reinterpret_cast<void*>(this));
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

			glfwSetWindowSizeCallback(m_Window, windowResizeCallback);
			glfwSetKeyCallback(m_Window, keyCallback);
			glfwSetMouseButtonCallback(m_Window, mouseCallback);
			glfwSetCursorPosCallback(m_Window, cursorPosCallback);
			// This will disable v-sync.
            glfwSwapInterval(0.0);
		}

		Window::~Window()
		{
			glfwTerminate();
		}

		void Window::update() const
		{
			glfwPollEvents();
			glfwSwapBuffers(m_Window);
		}

		bool Window::closed() const
		{
			return glfwWindowShouldClose(m_Window) == 1;
		}

		void Window::clear() const
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}

		void Window::windowResizeCallback(GLFWwindow * window, int width, int height)
		{
			auto thiz = getThisFromGLFWWindow(window);

			glViewport(0, 0, width, height);
			thiz->setHeight(height);
			thiz->setWidth(width);
		}

		void Window::keyCallback(GLFWwindow * window, int key, int scancode, int action, int mods)
		{
			auto thiz = getThisFromGLFWWindow(window);
			if (action == GLFW_PRESS)
				thiz->setKeyPressed(key);
			if (action == GLFW_RELEASE)
				thiz->setKeyPressed(GLFW_KEY_UNKNOWN);

		}

		void Window::mouseCallback(GLFWwindow * window, int button, int action, int mods)
		{
			auto thiz = getThisFromGLFWWindow(window);
			if (action == GLFW_PRESS)
				thiz->setMouseButton(button);
			if (action == GLFW_RELEASE)
				thiz->setMouseButton(GLFW_KEY_UNKNOWN);
		}

		void Window::cursorPosCallback(GLFWwindow * window, double x, double y)
		{
			auto thiz = getThisFromGLFWWindow(window);
			thiz->setCursorX(x);
			thiz->setCursorY(y);
		}

	}
}

#pragma once
#include <stdexcept>
#include <functional>

#define GLEW_STATIC
#include <windef.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils/io.h"

namespace granite {
	namespace graphics {


		class Window {
			const char * m_Title;
			int m_Width, m_Height;
			GLFWwindow * m_Window;
			int m_KeyPressed;
			int m_MouseButton;
			double m_CursorX, m_CursorY;

		public:
			Window(const char *name, int width, int height);
			~Window();

			void update() const;
			bool closed() const;
			void clear() const;

			inline void setWidth(int width) { m_Width = width; }
			inline int getWidth() { return m_Width; }
			inline void setHeight(int height) { m_Height = height; }
			inline int getHeight() { return m_Height; }
			inline void setKeyPressed(int key) { m_KeyPressed = key; }
			inline int getKeyPressed() { return m_KeyPressed; }
			inline void setMouseButton(int button) { m_MouseButton = button; }
			inline int getMouseButton() { return m_MouseButton; }
			inline void setCursorX(double x) { m_CursorX = x; }
			inline double getCursorX() { return m_CursorX; }
			inline void setCursorY(double y) { m_CursorY = y; }
			inline double getCursorY() { return m_CursorY; }
			inline static Window* getThisFromGLFWWindow(GLFWwindow * window) {
				return reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
			}
			static void windowResizeCallback(GLFWwindow * window, int width, int height);
			static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
			static void mouseCallback(GLFWwindow* window, int button, int action, int mods);
			static void cursorPosCallback(GLFWwindow* window, double x, double y);
		};
	}
}
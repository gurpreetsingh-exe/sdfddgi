#include "Window.hh"
#include <GLFW/glfw3.h>
#include <stdexcept>

Window::Window(uint32_t width, uint32_t height, const char* name)
    : m_Width(width), m_Height(height), m_Name(name) {
  if (!glfwInit()) {
    throw std::runtime_error("cannot initialize glfw");
  }

  m_Window = glfwCreateWindow(m_Width, m_Height, m_Name, nullptr, nullptr);
  if (!m_Window) {
    throw std::runtime_error("failed to create window");
  }

  glfwMakeContextCurrent(m_Window);
}

void Window::isRunning(std::function<void()> func) {
  while (!glfwWindowShouldClose(m_Window)) {
    func();
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
  }
}

Window::~Window() {
  glfwDestroyWindow(m_Window);
  glfwTerminate();
}

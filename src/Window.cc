#include "Window.hh"
#include <GLFW/glfw3.h>
#include <iostream>
#include <stdexcept>

static void keyCallback(GLFWwindow* window, int key, int scancode, int action,
                        int mods) {
  auto event = reinterpret_cast<Event*>(glfwGetWindowUserPointer(window));
  if (action == GLFW_PRESS) {
    if (key == GLFW_KEY_ESCAPE) {
      event->disableCursor = !event->disableCursor;
    }
    event->pressed[key] = true;
  } else if (action == GLFW_RELEASE) {
    event->pressed[key] = false;
  }
}

static void mouseCallback(GLFWwindow* window, double x, double y) {
  auto event = reinterpret_cast<Event*>(glfwGetWindowUserPointer(window));
  event->mousePos = glm::vec2(x, y);
}

Window::Window(uint32_t width, uint32_t height, const char* name)
    : m_Width(width), m_Height(height), m_Name(name) {
  if (!glfwInit()) {
    throw std::runtime_error("cannot initialize glfw");
  }

  glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
  glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);
  m_Window = glfwCreateWindow(m_Width, m_Height, m_Name, nullptr, nullptr);
  if (!m_Window) {
    throw std::runtime_error("failed to create window");
  }

  m_Event = new Event();
  glfwMakeContextCurrent(m_Window);
  glfwSetKeyCallback(m_Window, keyCallback);
  glfwSetCursorPosCallback(m_Window, mouseCallback);
  glfwSetWindowUserPointer(m_Window, m_Event);
}

void Window::isRunning(std::function<void()> func) {
  while (!glfwWindowShouldClose(m_Window)) {
    auto time = std::chrono::steady_clock::now();
    func();
    glfwPollEvents();
    glfwSwapBuffers(m_Window);
    m_Event->deltaTime =
        std::chrono::duration_cast<std::chrono::milliseconds>(time - m_LastTime)
            .count();
    m_LastTime = time;
  }
}

Window::~Window() {
  delete m_Event;
  glfwDestroyWindow(m_Window);
  glfwTerminate();
}

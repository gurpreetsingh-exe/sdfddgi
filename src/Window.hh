#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <cstdint>
#include <functional>

class Window {
public:
  Window(uint32_t width, uint32_t height, const char* name);
  ~Window();

public:
  void isRunning(std::function<void()>);
  GLFWwindow* getHandle() { return m_Window; }

private:
  uint32_t m_Width, m_Height;
  const char* m_Name;
  GLFWwindow* m_Window;
};

#endif // !WINDOW_H

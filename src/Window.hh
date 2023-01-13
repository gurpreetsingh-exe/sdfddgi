#ifndef WINDOW_H
#define WINDOW_H

#include "Event.hh"
#include <GLFW/glfw3.h>
#include <chrono>
#include <cstdint>
#include <functional>

class Window {
public:
  Window(uint32_t width, uint32_t height, const char* name);
  ~Window();

public:
  void isRunning(std::function<void()>);
  GLFWwindow* getHandle() { return m_Window; }
  uint32_t getWidth() { return m_Width; }
  uint32_t getHeight() { return m_Height; }
  Event* getEvent() { return m_Event; }

private:
  uint32_t m_Width, m_Height;
  const char* m_Name;
  GLFWwindow* m_Window;
  Event* m_Event;
  std::chrono::time_point<
      std::chrono::steady_clock,
      std::chrono::duration<long, std::ratio<1, 1000000000>>>
      m_LastTime;
};

#endif // !WINDOW_H

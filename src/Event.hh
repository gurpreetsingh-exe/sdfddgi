#ifndef EVENT_H
#define EVENT_H

#include <GLFW/glfw3.h>
#include <cstdint>
#include <glm/glm.hpp>

struct Event {
  float deltaTime;
  bool pressed[GLFW_KEY_LAST + 1];
};

#endif // !EVENT_H

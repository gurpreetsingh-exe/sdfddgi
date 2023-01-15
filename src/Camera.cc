#include "Camera.hh"
#include "Window.hh"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <iostream>

#define SPEED 0.005f
#define ROT_SPEED 0.8f

void Camera::updateModel() { m_Model = glm::mat4(1.0f); }

void Camera::updateView() {
  m_View = glm::lookAt(m_Position, m_Position + m_Direction,
                       glm::vec3(0.0f, 0.0f, 1.0f));
}

void Camera::updateProjection() {
  m_Projection =
      glm::perspective(glm::radians(m_Fov), m_ViewportWidth / m_ViewportHeight,
                       m_ClipNear, m_ClipFar);

  m_Projection[1][1] *= -1;
}

void Camera::onResize(uint32_t width, uint32_t height) {
  if (m_ViewportWidth != width || m_ViewportHeight != height) {
    m_ViewportWidth = width;
    m_ViewportHeight = height;
    m_NeedsUpdate = true;
  }
}

extern Window window;

void Camera::onUpdate(Event* event) {
  glm::vec2 delta = (event->mousePos - m_LastMousePos) * 0.0025f;

  glm::vec3 upDir(0.0, 0.0, 1.0);
  m_Right = glm::cross(upDir, m_Direction);

  if (event->pressed['W']) {
    m_Position += m_Direction * event->deltaTime * SPEED;
    m_NeedsUpdate = true;
  }
  if (event->pressed['S']) {
    m_Position -= m_Direction * event->deltaTime * SPEED;
    m_NeedsUpdate = true;
  }
  if (event->pressed['A']) {
    m_Position += m_Right * event->deltaTime * SPEED;
    m_NeedsUpdate = true;
  }
  if (event->pressed['D']) {
    m_Position -= m_Right * event->deltaTime * SPEED;
    m_NeedsUpdate = true;
  }

  if (event->disableCursor) {
    glfwSetInputMode(window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_LookAround = true;
  } else {
    glfwSetInputMode(window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    m_LookAround = false;
  }

  if ((delta.x != 0.0 || delta.y != 0.0) && m_LookAround) {
    float pitch = delta.y * ROT_SPEED;
    float yaw = delta.x * ROT_SPEED;
    glm::quat q = glm::normalize(glm::cross(glm::angleAxis(pitch, m_Right),
                                            glm::angleAxis(-yaw, upDir)));
    m_Direction = glm::rotate(q, m_Direction);
    m_NeedsUpdate = true;
  }

  m_LastMousePos = event->mousePos;

  if (m_NeedsUpdate) {
    updateProjection();
    updateView();

    m_NeedsUpdate = false;
    return;
  }
}

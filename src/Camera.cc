#include "Camera.hh"
#include "Window.hh"
#include <iostream>

#define SPEED 0.02f

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

void Camera::onUpdate(Event* event) {
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

  if (m_NeedsUpdate) {
    updateProjection();
    updateView();

    m_NeedsUpdate = false;
    return;
  }
}

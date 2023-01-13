#ifndef CAMERA_H
#define CAMERA_H

#include "Event.hh"
#include <cstdint>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
  Camera(uint32_t width, uint32_t height, float fov, float clipNear,
         float clipFar)
      : m_ViewportWidth(width), m_ViewportHeight(height), m_Fov(fov),
        m_ClipNear(clipNear), m_ClipFar(clipFar) {
    updateModel();
    updateProjection();
    updateView();
  }

  ~Camera() = default;

public:
  const glm::mat4& getModel() const { return m_Model; }
  const glm::mat4& getView() const { return m_View; }
  const glm::mat4& getProjection() const { return m_Projection; }
  void onUpdate(Event* event);
  void onResize(uint32_t width, uint32_t height);

private:
  void updateModel();
  void updateView();
  void updateProjection();

private:
  float m_ViewportWidth, m_ViewportHeight;
  glm::mat4 m_Model;
  glm::mat4 m_View;
  glm::mat4 m_Projection;

  float m_Fov;
  float m_ClipNear;
  float m_ClipFar;

  bool m_NeedsUpdate = false;

  glm::vec3 m_Position = glm::vec3(2.0f, 2.0f, 2.0f);
  glm::vec3 m_Direction = glm::vec3(-1.0f);
  glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);

  glm::vec2 m_LastMousePos = glm::vec2(0.0f);
};

#endif // !CAMERA_H

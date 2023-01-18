#ifndef APPLICATION_H
#define APPLICATION_H

#include "Buffer.hh"
#include "Camera.hh"
#include "Framebuffer.hh"
#include "ImGuiLayer.hh"
#include "Mesh.hh"
#include "Shader.hh"
#include "VertexArray.hh"
#include "Window.hh"
#include <cstdint>
#include <string_view>

class Application {
  static constexpr uint32_t width = 600;
  static constexpr uint32_t height = 600;
  static constexpr std::string_view name = "sdfddgi";
  const char* vert = "../shaders/vertex.glsl";
  const char* frag = "../shaders/fragment.glsl";

public:
  Application();
  ~Application();

public:
  void run();

private:
  ImGuiLayer* m_ImGuiLayer;
  Mesh* m_Mesh;
  Shader* m_Shader;
  Camera* m_Camera;
  VertexArray* m_VertexArray;
  bool VAOSetupCompleted = false;
  bool invalidateFuture = false;

  Framebuffer<GL_TEXTURE_2D_MULTISAMPLE>* m_MsaaFb;
  Framebuffer<GL_TEXTURE_2D>* m_Framebuffer;
};

#endif // !APPLICATION_H

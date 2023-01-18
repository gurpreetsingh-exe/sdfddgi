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
  const char* vert = R"(
    #version 450

    layout (location = 0) in vec3 position;
    layout (location = 1) in vec3 normal;
    layout (location = 2) in vec2 texCoord;
    out vec3 pos;
    out vec3 nor;
    out vec2 uv;

    uniform mat4 modelViewProjection;
    uniform mat4 transform;

    void main() {
        gl_Position = modelViewProjection * transform * vec4(position, 1.0f);
        pos = position;
        nor = normal;
        uv = texCoord;
    }
  )";

  const char* frag = R"(
    #version 450

    in vec3 pos;
    in vec3 nor;
    in vec2 uv;
    out vec4 color;

    void main() {
        vec3 col = mix(vec3(uv, 0.0), 0.5 + 0.5 * nor, step(pos.y + 35, 0.0));
        color = vec4(col, 1.0f);
    }
  )";

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

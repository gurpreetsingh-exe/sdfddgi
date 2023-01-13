// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Camera.hh"
#include "Framebuffer.hh"
#include "Texture.hh"
#include <stdexcept>
#define TINYOBJLOADER_IMPLEMENTATION
#include "../vendor/tiny_obj_loader.h"
#include "Buffer.hh"
#include "Shader.hh"
#include "VertexArray.hh"
#include "Window.hh"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <cstdint>
#include <iostream>
#include <unordered_map>
#include <vector>

static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                       GLenum severity, GLsizei length,
                                       const GLchar* message,
                                       const void* userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}

void loadObj(const char* filepath, std::vector<Vertex>& vertices,
             std::vector<uint32_t>& indices) {
  tinyobj::ObjReader reader;
  tinyobj::ObjReaderConfig config;
  reader.ParseFromFile(filepath, config);

  if (!reader.Valid()) {
    throw std::runtime_error(reader.Warning() + reader.Error());
  }

  std::unordered_map<Vertex, uint32_t> uniqueVertices;
  const auto& attributes = reader.GetAttrib();

  for (const auto& shape : reader.GetShapes()) {
    for (const auto& index : shape.mesh.indices) {
      Vertex vertex;
      vertex.pos = {attributes.vertices[3 * index.vertex_index + 0],
                    attributes.vertices[3 * index.vertex_index + 1],
                    attributes.vertices[3 * index.vertex_index + 2]};

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }

      indices.push_back(uniqueVertices[vertex]);
    }
  }
}

Window window(640, 640, "sdfddgi");

int main() {
  // Window window(640, 640, "sdfddgi");
  if (glewInit() == GLEW_OK) {
    std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
  }

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window.getHandle(), true);
  ImGui_ImplOpenGL3_Init("#version 450");

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  std::vector<Vertex> vertices = {};
  std::vector<uint32_t> indices = {};
  loadObj("model.obj", vertices, indices);
  Buffer<Vertex, GL_ARRAY_BUFFER> pos(vertices.data(), vertices.size());
  Buffer<uint32_t, GL_ELEMENT_ARRAY_BUFFER> ind(indices.data(), indices.size());

  VertexArray arr;
  arr.bind();
  arr.addVertexBuffer(&pos);
  arr.setIndexBuffer(&ind);

  const char* vert = R"(
    #version 450

    layout (location = 0) in vec3 position;
    out vec3 pos;

    uniform mat4 modelViewProjection;

    void main() {
        gl_Position = modelViewProjection * vec4(position, 1.0f);
        pos = position;
    }
  )";

  const char* frag = R"(
    #version 450

    in vec3 pos;
    out vec4 color;

    void main() {
        vec3 normal = normalize(cross(dFdx(pos), dFdy(pos)));
        color = vec4(normal, 1.0f);
    }
  )";

  Shader shader(vert, frag);
  shader.bind();
  uint32_t width = window.getWidth();
  uint32_t height = window.getHeight();
  Framebuffer framebuffer(width, height);
  framebuffer.bind();
  if (!framebuffer.isComplete()) {
    throw std::runtime_error("framebuffer setup not completed");
  }
  framebuffer.unbind();

  Camera camera(width, height, 90.0, 0.1, 100.0);

  window.isRunning([&indices, &io, &framebuffer, &camera, &shader] {
    auto event = window.getEvent();
    camera.onUpdate(event);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    static bool opt_fullscreen = true;
    static bool opt_padding = false;
    static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

    ImGuiWindowFlags window_flags =
        ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
    if (opt_fullscreen) {
      const ImGuiViewport* viewport = ImGui::GetMainViewport();
      ImGui::SetNextWindowPos(viewport->WorkPos);
      ImGui::SetNextWindowSize(viewport->WorkSize);
      ImGui::SetNextWindowViewport(viewport->ID);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
      ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
      window_flags |= ImGuiWindowFlags_NoTitleBar |
                      ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize |
                      ImGuiWindowFlags_NoMove;
      window_flags |=
          ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
    }

    if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
      window_flags |= ImGuiWindowFlags_NoBackground;

    if (!opt_padding)
      ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    bool p_open = true;

    ImGui::Begin("DockSpace", &p_open, window_flags);

    if (!opt_padding)
      ImGui::PopStyleVar();

    if (opt_fullscreen)
      ImGui::PopStyleVar(2);

    if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
      ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
      ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
    }

    ImGui::End();

    ImGui::Begin("Debug");
    ImGui::Text("Delta Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
    ImGui::End();

    ImGui::Begin("Properties");
    ImGui::End();

    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
    ImGui::Begin("Viewport");
    ImVec2 dim = ImGui::GetWindowSize();
    framebuffer.onResize(dim.x, dim.y);
    camera.onResize(dim.x, dim.y);
    ImGui::Image((void*)(intptr_t)framebuffer.getColorAttachments()[0]->getId(),
                 dim);
    ImGui::End();
    ImGui::PopStyleVar();

    framebuffer.bind();
    shader.uploadUniformMat4("modelViewProjection",
                             camera.getProjection() * camera.getView());

    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
    glViewport(0, 0, dim.x, dim.y);
    glDisable(GL_DEPTH_TEST);
    framebuffer.unbind();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
      GLFWwindow* backup_current_context = glfwGetCurrentContext();
      ImGui::UpdatePlatformWindows();
      ImGui::RenderPlatformWindowsDefault();
      glfwMakeContextCurrent(backup_current_context);
    }
  });

  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

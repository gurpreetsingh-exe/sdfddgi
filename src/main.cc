// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Buffer.hh"
#include "Camera.hh"
#include "Framebuffer.hh"
#include "ImGuiLayer.hh"
#include "Mesh.hh"
#include "Shader.hh"
#include "Texture.hh"
#include "VertexArray.hh"
#include "Window.hh"
#include <cstdint>
#include <future>
#include <iostream>
#include <stdexcept>
#include <thread>
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

std::string openFileBrowser() {
  char filename[512];
  FILE* f = popen("zenity --file-selection", "r");
  fgets(filename, 512, f);
  auto obj = std::string(filename);
  obj[obj.size() - 1] = '\0';
  return obj;
}

Window window(640, 640, "sdfddgi");
int main() {
  Properties properties;

  if (glewInit() == GLEW_OK) {
    std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
  }

  ImGuiLayer imguiLayer;

  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  // auto mesh = Mesh<uint16_t>::fromObj("model.obj");
  Mesh* mesh = new Mesh();
  auto future = std::async(loadGLTF, "sponza/Sponza.gltf", mesh);

  const char* vert = R"(
    #version 450

    layout (location = 0) in vec3 position;
    out vec3 pos;

    uniform mat4 modelViewProjection;
    uniform mat4 transform;

    void main() {
        gl_Position = modelViewProjection * transform * vec4(position, 1.0f);
        pos = position;
    }
  )";

  const char* frag = R"(
    #version 450

    in vec3 pos;
    out vec4 color;

    void main() {
        vec3 normal = normalize(cross(dFdx(pos), dFdy(pos)));
        color = vec4(0.5 + 0.5 * normal, 1.0f);
    }
  )";

  Shader shader(vert, frag);
  shader.bind();
  uint32_t width = window.getWidth();
  uint32_t height = window.getHeight();
  Framebuffer<GL_TEXTURE_2D_MULTISAMPLE> msaaFramebuffer(width, height);
  msaaFramebuffer.bind();
  if (!msaaFramebuffer.isComplete()) {
    throw std::runtime_error("msaaFramebuffer setup not completed");
  }
  msaaFramebuffer.unbind();

  Framebuffer<GL_TEXTURE_2D> framebuffer(width, height);
  framebuffer.bind();
  if (!framebuffer.isComplete()) {
    throw std::runtime_error("framebuffer setup not completed");
  }
  framebuffer.unbind();

  float fov = 90.0;
  Camera camera(width, height, fov, 0.01, 100.0);

  Buffer<Vertex, GL_ARRAY_BUFFER>* pos;
  Buffer<uint32_t, GL_ELEMENT_ARRAY_BUFFER>* ind;
  VertexArray<uint32_t>* arr;
  bool VAOSetupCompleted = false;
  bool invalidateFuture = false;

  window.isRunning([&] {
    imguiLayer.props.meshVertices = mesh->vertices.size();
    camera.setFov(imguiLayer.props.cameraFov);

    if (mesh->status == MeshStatus::Loaded && !VAOSetupCompleted) {
      pos = new Buffer<Vertex, GL_ARRAY_BUFFER>(mesh->vertices);
      ind = new Buffer<uint32_t, GL_ELEMENT_ARRAY_BUFFER>(mesh->indices);

      arr = new VertexArray<uint32_t>();
      arr->bind();
      arr->addVertexBuffer(pos);
      arr->setIndexBuffer(ind);
      VAOSetupCompleted = true;
    }

    auto event = window.getEvent();
    camera.onUpdate(event);

    imguiLayer.beginFrame();
    imguiLayer.onUpdate(framebuffer);

    ImVec2 dim = imguiLayer.getViewportDimensions();
    msaaFramebuffer.onResize(dim.x, dim.y);
    framebuffer.onResize(dim.x, dim.y);
    camera.onResize(dim.x, dim.y);

    if (mesh->status == MeshStatus::Loaded) {
      shader.bind();
      msaaFramebuffer.bind();
      arr->bind();
      shader.uploadUniformMat4("transform", mesh->localTransform);
      shader.uploadUniformMat4("modelViewProjection",
                               camera.getProjection() * camera.getView());

      glEnable(GL_DEPTH_TEST);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glDrawElements(GL_TRIANGLES, mesh->indices.size(), GL_UNSIGNED_INT,
                     nullptr);
      glViewport(0, 0, dim.x, dim.y);
      glDisable(GL_DEPTH_TEST);

      glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFramebuffer.getId());
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.getId());
      glBlitFramebuffer(0, 0, dim.x, dim.y, 0, 0, dim.x, dim.y,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
      msaaFramebuffer.unbind();
    } else {
      msaaFramebuffer.bind();
      glClear(GL_COLOR_BUFFER_BIT);
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glViewport(0, 0, dim.x, dim.y);

      glBindFramebuffer(GL_READ_FRAMEBUFFER, msaaFramebuffer.getId());
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer.getId());
      glBlitFramebuffer(0, 0, dim.x, dim.y, 0, 0, dim.x, dim.y,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
      msaaFramebuffer.unbind();
    }

    imguiLayer.endFrame();

    if (!invalidateFuture &&
        future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      future.get();
      invalidateFuture = true;
    }
  });

  delete mesh;
  delete arr;
  delete pos;
  delete ind;
}

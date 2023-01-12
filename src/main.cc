// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Buffer.hh"
#include "Shader.hh"
#include "VertexArray.hh"
#include "Window.hh"
#include <cstdint>
#include <iostream>
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

int main() {
  Window window(640, 640, "sdfddgi");
  if (glewInit() == GLEW_OK) {
    std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
  }
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);

  // clang-format off
  std::vector<float> positions = {
      -1.0, -1.0,  0.0,
       1.0, -1.0,  0.0,
       1.0,  1.0,  0.0,
      -1.0,  1.0,  0.0,
  };
  // clang-format on
  std::vector<uint32_t> index = {
      0, 1, 2, 2, 3, 0,
  };
  Buffer<float, GL_ARRAY_BUFFER> pos(positions.data(), positions.size());
  Buffer<uint32_t, GL_ELEMENT_ARRAY_BUFFER> ind(index.data(), index.size());

  VertexArray arr;
  arr.bind();
  arr.addVertexBuffer(&pos);
  arr.setIndexBuffer(&ind);

  const char* vert = R"(
    #version 450

    layout (location = 0) in vec3 position;
    out vec3 pos;

    void main() {
        gl_Position = vec4(position, 1.0f);
        pos = position;
    }
  )";

  const char* frag = R"(
    #version 450

    in vec3 pos;
    out vec4 color;

    void main() {
        color = vec4(pos, 1.0f);
    }
  )";
  Shader shader(vert, frag);
  shader.bind();

  window.isRunning([&index] {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glDrawElements(GL_TRIANGLES, index.size(), GL_UNSIGNED_INT, nullptr);
  });
}

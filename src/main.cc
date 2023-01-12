// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Buffer.hh"
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

  window.isRunning([&index] {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glDrawElements(GL_TRIANGLES, index.size(), GL_UNSIGNED_INT, nullptr);
  });
}

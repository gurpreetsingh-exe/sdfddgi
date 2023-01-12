// clang-format off
#include <GL/glew.h>
// clang-format on
#define TINYOBJLOADER_IMPLEMENTATION
#include "../vendor/tiny_obj_loader.h"
#include "Buffer.hh"
#include "Shader.hh"
#include "VertexArray.hh"
#include "Window.hh"
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
                    attributes.vertices[3 * index.vertex_index + 2],
                    attributes.vertices[3 * index.vertex_index + 1]};

      if (uniqueVertices.count(vertex) == 0) {
        uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
        vertices.push_back(vertex);
      }

      indices.push_back(uniqueVertices[vertex]);
    }
  }
}

int main() {
  Window window(640, 640, "sdfddgi");
  if (glewInit() == GLEW_OK) {
    std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
  }
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

    void main() {
        gl_Position = vec4(position + vec3(0.0, 0.0, 0.0), 1.0f);
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

  window.isRunning([&indices] {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, nullptr);
  });
}

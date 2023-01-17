#ifndef VERTEX_H
#define VERTEX_H

#include "GL/glew.h"
#include <array>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#define FLOAT GL_FLOAT

struct BufferLayout {
  uint32_t type;
  bool normalized;
  uint32_t offset;
  uint32_t elements;
  void* data;
  size_t size;
};

struct Vertex {
  glm::vec3 pos;
  bool operator==(const Vertex& other) const { return pos == other.pos; }
};

namespace std {
template <>
struct hash<Vertex> {
  size_t operator()(Vertex const& vertex) const {
    return (hash<glm::vec3>()(vertex.pos) >> 1);
  }
};
} // namespace std

#endif // !VERTEX_H

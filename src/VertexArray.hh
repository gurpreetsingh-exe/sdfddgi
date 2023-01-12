#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H
// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Buffer.hh"
#include <cstdint>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#include <iostream>
#include <unordered_map>
#include <vector>

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

class VertexArray {
public:
  VertexArray();
  ~VertexArray();

public:
  void bind();
  void unbind();
  void addVertexBuffer(Buffer<Vertex, GL_ARRAY_BUFFER>* buffer);
  void setIndexBuffer(Buffer<uint32_t, GL_ELEMENT_ARRAY_BUFFER>* buffer);

private:
  uint32_t m_Id;
  std::vector<Buffer<Vertex, GL_ARRAY_BUFFER>*> m_VertexBuffers;
  Buffer<uint32_t, GL_ELEMENT_ARRAY_BUFFER>* m_IndexBuffer;
};

#endif // !VERTEX_ARRAY_H

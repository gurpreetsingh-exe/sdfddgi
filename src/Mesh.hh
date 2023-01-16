#ifndef MESH_H
#define MESH_H

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>
#include <string>
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

enum MeshStatus : uint8_t {
  Undefined = 0,
  Loaded,
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<uint32_t> indices;
  glm::mat4 localTransform;
  MeshStatus status = MeshStatus::Undefined;

  static Mesh* fromObj(const std::string& filepath);
  static Mesh* fromGLTF(const std::string& filepath);
};

void loadGLTF(const std::string&, Mesh*);

#endif // !MESH_H

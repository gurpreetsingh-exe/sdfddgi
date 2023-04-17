#ifndef MESH_H
#define MESH_H
// clang-format off
#include "VertexArray.hh"
#include <GL/glew.h>
// clang-format on

#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>
#define GLM_ENABLE_EXPERIMENTAL
#include "Texture.hh"
#include <glm/glm.hpp>
#include <glm/gtx/hash.hpp>

#define FLOAT GL_FLOAT

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

struct GLTFTexture {
  std::vector<unsigned char> image;
  int width, height, pixel_type;
};

struct RMesh {
  std::vector<Vertex> vertices;
  std::vector<glm::vec3> normals;
  std::vector<glm::vec2> texCoords;
  std::vector<uint32_t> indices;
  GLTFTexture image;
  uint32_t texture;
  VertexArray* vertexArray;
};

struct Model {
  std::unordered_map<std::string, uint32_t> images;
  std::unordered_map<uint32_t, RMesh> mesh;
  uint32_t imageId;
};

struct Mesh {
  glm::mat4 localTransform;
  MeshStatus status = MeshStatus::Undefined;
  Model model;

  // static Mesh* fromObj(const std::string& filepath);
  static Mesh* fromGLTF(const std::string& filepath);
};

void loadGLTF(const std::string&, Mesh*);

#endif // !MESH_H

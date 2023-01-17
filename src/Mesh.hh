#ifndef MESH_H
#define MESH_H

#include "Vertex.hh"
#include <glm/glm.hpp>
#include <string>
#include <unordered_map>
#include <vector>

enum MeshStatus : uint8_t {
  Undefined = 0,
  Loaded,
};

struct Mesh {
  std::vector<Vertex> vertices;
  std::vector<glm::vec3> normals;
  std::vector<uint32_t> indices;
  glm::mat4 localTransform;
  MeshStatus status = MeshStatus::Undefined;

  static Mesh* fromObj(const std::string& filepath);
  static Mesh* fromGLTF(const std::string& filepath);
};

void loadGLTF(const std::string&, Mesh*);

#endif // !MESH_H

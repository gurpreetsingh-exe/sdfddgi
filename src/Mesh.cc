#include "Mesh.hh"
#include <cstring>
#include <iostream>
#include <stdexcept>
#define TINYOBJLOADER_IMPLEMENTATION
#include "../vendor/tiny_obj_loader.h"
#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "tiny_gltf.h"
#include <glm/gtx/quaternion.hpp>

Mesh* Mesh::fromObj(const std::string& filepath) {
  auto mesh = new Mesh();
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
        uniqueVertices[vertex] = static_cast<uint32_t>(mesh->vertices.size());
        mesh->vertices.push_back(vertex);
      }

      mesh->indices.push_back(uniqueVertices[vertex]);
    }
  }
  return mesh;
}

static std::string GetFilePathExtension(const std::string& FileName) {
  if (FileName.find_last_of(".") != std::string::npos)
    return FileName.substr(FileName.find_last_of(".") + 1);
  return "";
}

// Adapted from Blender
// https://github.com/blender/blender/blob/master/intern/ghost/intern/GHOST_XrControllerModel.cpp

template <typename T>
static void readIndices(const tinygltf::Accessor& accessor,
                        const tinygltf::BufferView& buffer_view,
                        const tinygltf::Buffer& buffer, Mesh* mesh) {
  if ((accessor.count % 3) != 0) {
    throw std::runtime_error(
        "unexpected number of indices for triangle primitive");
  }

  const T* indexBuffer = reinterpret_cast<const T*>(
      buffer.data.data() + buffer_view.byteOffset + accessor.byteOffset);
  size_t indexLen = mesh->vertices.size();
  for (uint32_t i = 0; i < accessor.count; i++) {
    mesh->indices.push_back(indexBuffer[i] + indexLen);
  }
}

static void processAttributeAccessor(const tinygltf::Model& model,
                                     const std::string& name,
                                     const tinygltf::Accessor& accessor,
                                     Mesh* r_Mesh) {
  const auto& bufferView = model.bufferViews.at(accessor.bufferView);
  const auto& __buffer = model.buffers.at(bufferView.buffer);

  const float* buffer = reinterpret_cast<const float*>(
      __buffer.data.data() + bufferView.byteOffset + accessor.byteOffset);

  if (name == "POSITION") {
    for (size_t i = 0; i < accessor.count; ++i) {
      Vertex v;
      v.pos = {
          buffer[i * 3 + 0],
          buffer[i * 3 + 2],
          buffer[i * 3 + 1],
      };
      r_Mesh->vertices.push_back(v);
    }
  } else if (name == "NORMAL") {
    for (size_t i = 0; i < accessor.count; ++i) {
      glm::vec3 normal = {
          buffer[i * 3 + 0],
          buffer[i * 3 + 1],
          buffer[i * 3 + 2],
      };
      r_Mesh->normals.push_back(normal);
    }
  } else if (name == "TEXCOORD_0") {
    for (size_t i = 0; i < accessor.count; ++i) {
      glm::vec2 texCoord = {
          buffer[i * 2 + 0],
          buffer[i * 2 + 1],
      };
      r_Mesh->texCoords.push_back(texCoord);
    }
  }
}

static void processIndexAccessor(const tinygltf::Model& model,
                                 const tinygltf::Accessor& accessor,
                                 Mesh* r_Mesh) {
  const auto& bufferView = model.bufferViews.at(accessor.bufferView);
  const auto& buffer = model.buffers.at(bufferView.buffer);

  switch (accessor.componentType) {
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
    readIndices<uint8_t>(accessor, bufferView, buffer, r_Mesh);
    break;
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
    readIndices<uint16_t>(accessor, bufferView, buffer, r_Mesh);
    break;
  case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
    readIndices<uint32_t>(accessor, bufferView, buffer, r_Mesh);
  default:
    throw std::runtime_error("unknown component type");
  }
}

static void processPrimitive(const tinygltf::Model& model,
                             const tinygltf::Primitive& prim, Mesh* r_Mesh) {
  if (prim.indices != -1) {
    processIndexAccessor(model, model.accessors.at(prim.indices), r_Mesh);
  }
  for (const auto& [name, id] : prim.attributes) {
    processAttributeAccessor(model, name, model.accessors.at(id), r_Mesh);
  }
}

static void processMesh(const tinygltf::Model& model,
                        const tinygltf::Mesh& mesh, Mesh* r_Mesh) {
  for (const auto& prim : mesh.primitives) {
    processPrimitive(model, prim, r_Mesh);
  }
}

static void calcTransform(const tinygltf::Node& node, Mesh* r_Mesh,
                          glm::mat4& worldTransform,
                          const glm::mat4 parentTransform) {
  if (node.matrix.size() == 16) {
    const auto t = node.matrix;
    for (size_t i = 0; i < 3; ++i) {
      for (size_t j = 0; j < 3; ++j) {
        r_Mesh->localTransform[i][j] = t[i + j * 4];
      }
    }
  } else {
    auto translation = node.translation;
    auto rotation = node.rotation;
    auto scale = node.scale;
    glm::quat q;
    glm::mat3 scaleMat = glm::identity<glm::mat3>();

    if (translation.size() != 3) {
      translation.resize(3);
      translation[0] = translation[1] = translation[2] = 0.0;
    }

    if (rotation.size() != 4) {
      rotation.resize(4);
      rotation[0] = rotation[1] = rotation[2] = 0.0;
      rotation[3] = 1.0;
    }

    if (scale.size() != 3) {
      scale.resize(3);
      scale[0] = scale[1] = scale[2] = 1.0;
    }

    for (size_t i = 0; i < 4; ++i) {
      q[i] = float(rotation[i]);
    }
    q = glm::normalize(q);

    for (size_t i = 0; i < 3; ++i) {
      scaleMat[i][i] = float(scale[i]);
    }

    glm::mat4 transform = glm::mat4(glm::toMat3(q) * scaleMat);
    r_Mesh->localTransform[0] = transform[0];
    r_Mesh->localTransform[1] = transform[1];
    r_Mesh->localTransform[2] = transform[2];
    r_Mesh->localTransform[3] =
        glm::vec4(float(translation[0]), float(translation[1]),
                  float(translation[2]), 1.0f);
  }
  worldTransform = parentTransform * r_Mesh->localTransform;
}

static void processNodes(const tinygltf::Model& model,
                         const tinygltf::Node& node, Mesh* r_Mesh,
                         const glm::mat4 parentTransform) {
  glm::mat4 worldTransform;
  calcTransform(node, r_Mesh, worldTransform, parentTransform);

  processMesh(model, model.meshes[node.mesh], r_Mesh);

  for (auto i : node.children) {
    processNodes(model, model.nodes[i], r_Mesh, worldTransform);
  }
}

Mesh* Mesh::fromGLTF(const std::string& filepath) {
  auto mesh = new Mesh();
  loadGLTF(filepath, mesh);
  return mesh;
}

void loadGLTF(const std::string& filepath, Mesh* r_Mesh) {
  tinygltf::Model model;
  tinygltf::TinyGLTF gltf_ctx;
  std::string err;
  std::string warn;
  std::string ext = GetFilePathExtension(filepath);

  gltf_ctx.SetStoreOriginalJSONForExtrasAndExtensions(true);

  bool ret = false;
  if (ext.compare("glb") == 0) {
    std::cout << "Reading binary glTF" << std::endl;
    ret = gltf_ctx.LoadBinaryFromFile(&model, &err, &warn, filepath.c_str());
  } else {
    std::cout << "Reading ASCII glTF" << std::endl;
    ret = gltf_ctx.LoadASCIIFromFile(&model, &err, &warn, filepath.c_str());
  }

  if (!warn.empty()) {
    std::cout << "Warn: " << warn << std::endl;
  }

  if (!err.empty()) {
    std::cout << "Err: " << err << std::endl;
  }

  if (!ret) {
    std::cout << "Failed to parse glTF" << std::endl;
  }

  const auto& scene = model.scenes[model.defaultScene];
  glm::mat4 transform = glm::identity<glm::mat4>();
  for (auto i : scene.nodes) {
    processNodes(model, model.nodes[i], r_Mesh, transform);
  }

  r_Mesh->status = MeshStatus::Loaded;
}

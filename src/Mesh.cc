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

Mesh* Mesh::fromGLTF(const std::string& filepath) {
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

  auto mesh = new Mesh();
  for (const auto& _mesh : model.meshes) {
    for (const auto& prim : _mesh.primitives) {
      const auto& indexAccessor = model.accessors.at(prim.indices);

      const auto& indexBufferView =
          model.bufferViews.at(indexAccessor.bufferView);
      const auto& indexBuffer = model.buffers.at(indexBufferView.buffer);

      switch (indexAccessor.componentType) {
      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE:
        readIndices<uint8_t>(indexAccessor, indexBufferView, indexBuffer, mesh);
        break;
      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT:
        readIndices<uint16_t>(indexAccessor, indexBufferView, indexBuffer,
                              mesh);
        break;
      case TINYGLTF_COMPONENT_TYPE_UNSIGNED_INT:
        readIndices<uint32_t>(indexAccessor, indexBufferView, indexBuffer,
                              mesh);
      default:
        throw std::runtime_error("unknown component type");
      }

      auto accessorIndex = prim.attributes.at("POSITION");
      const auto& accessor = model.accessors.at(accessorIndex);
      const auto& bufferView = model.bufferViews.at(accessor.bufferView);
      const auto& buffer = model.buffers.at(bufferView.buffer);

      const float* posBuffer = reinterpret_cast<const float*>(
          buffer.data.data() + bufferView.byteOffset + accessor.byteOffset);

      for (size_t i = 0; i < accessor.count; ++i) {
        Vertex v;
        v.pos = {
            posBuffer[i * 3 + 0],
            posBuffer[i * 3 + 1],
            posBuffer[i * 3 + 2],
        };
        mesh->vertices.push_back(v);
      }
    }
  }

  return mesh;
}

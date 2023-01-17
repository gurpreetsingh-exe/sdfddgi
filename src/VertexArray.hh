#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H
// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Buffer.hh"
#include <cstdint>
#include <vector>

struct BufferLayout {
  uint32_t type;
  bool normalized;
  uint32_t stride;
  uint32_t size;
  void* data;
  size_t byteSize;
};

class VertexArray {
public:
  VertexArray();
  ~VertexArray();

public:
  void bind();
  void unbind();
  void addVertexBuffers(std::vector<BufferLayout>& layouts);
  void setIndexBuffer(Buffer<GL_ELEMENT_ARRAY_BUFFER>* buffer);

private:
  uint32_t m_Id;
  std::vector<Buffer<GL_ARRAY_BUFFER>*> m_VertexBuffers;
  Buffer<GL_ELEMENT_ARRAY_BUFFER>* m_IndexBuffer;
};

#endif // !VERTEX_ARRAY_H

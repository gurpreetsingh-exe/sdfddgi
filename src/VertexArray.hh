#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H
// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Buffer.hh"
#include <cstdint>
#include <vector>

class VertexArray {
public:
  VertexArray();
  ~VertexArray();

public:
  void bind();
  void unbind();
  void addVertexBuffer(Buffer<float, GL_ARRAY_BUFFER>* buffer);
  void setIndexBuffer(Buffer<uint32_t, GL_ELEMENT_ARRAY_BUFFER>* buffer);

private:
  uint32_t m_Id;
  std::vector<Buffer<float, GL_ARRAY_BUFFER>*> m_VertexBuffers;
  Buffer<uint32_t, GL_ELEMENT_ARRAY_BUFFER>* m_IndexBuffer;
};

#endif // !VERTEX_ARRAY_H

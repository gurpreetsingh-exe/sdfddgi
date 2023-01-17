#ifndef VERTEX_ARRAY_H
#define VERTEX_ARRAY_H
// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Buffer.hh"
#include "Mesh.hh"
#include <cstdint>

template <typename T>
class VertexArray {
public:
  VertexArray() {
    glCreateVertexArrays(1, &m_Id);
    glBindVertexArray(m_Id);
  }

  ~VertexArray() { glDeleteVertexArrays(1, &m_Id); }

public:
  void bind() { glBindVertexArray(m_Id); }

  void unbind() { glBindVertexArray(0); }

  void addLayout(std::vector<BufferLayout>& layouts) {
    for (const auto& layout : layouts) {
      auto* buffer =
          new Buffer<void, GL_ARRAY_BUFFER>(layout.data, layout.size);
      glEnableVertexAttribArray(m_VertexBuffers.size());
      glVertexAttribPointer(m_VertexBuffers.size(), layout.elements,
                            layout.type, layout.normalized, layout.offset,
                            nullptr);
      m_VertexBuffers.push_back(buffer);
    }
  }

  void addVertexBuffer(Buffer<Vertex, GL_ARRAY_BUFFER>* buffer) {
    buffer->bind();
    glEnableVertexAttribArray(m_VertexBuffers.size());
    glVertexAttribPointer(m_VertexBuffers.size(), 3, GL_FLOAT, GL_FALSE, 0,
                          nullptr);
    m_VertexBuffers.push_back((Buffer<void, GL_ARRAY_BUFFER>*)buffer);
  }

  void setIndexBuffer(Buffer<T, GL_ELEMENT_ARRAY_BUFFER>* buffer) {
    buffer->bind();
    m_IndexBuffer = buffer;
  }

private:
  uint32_t m_Id;
  std::vector<Buffer<void, GL_ARRAY_BUFFER>*> m_VertexBuffers;
  Buffer<T, GL_ELEMENT_ARRAY_BUFFER>* m_IndexBuffer;
};

#endif // !VERTEX_ARRAY_H

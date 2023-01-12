#include "VertexArray.hh"

VertexArray::VertexArray() {
  glCreateVertexArrays(1, &m_Id);
  glBindVertexArray(m_Id);
}

void VertexArray::bind() { glBindVertexArray(m_Id); }

void VertexArray::unbind() { glBindVertexArray(0); }

void VertexArray::addVertexBuffer(Buffer<Vertex, GL_ARRAY_BUFFER>* buffer) {
  buffer->bind();
  glEnableVertexAttribArray(m_VertexBuffers.size());
  glVertexAttribPointer(m_VertexBuffers.size(), 3, GL_FLOAT, GL_FALSE,
                        sizeof(Vertex), nullptr);
  m_VertexBuffers.push_back(buffer);
}

void VertexArray::setIndexBuffer(
    Buffer<uint32_t, GL_ELEMENT_ARRAY_BUFFER>* buffer) {
  buffer->bind();
  m_IndexBuffer = buffer;
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &m_Id);
}


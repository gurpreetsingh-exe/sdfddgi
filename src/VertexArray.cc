#include "VertexArray.hh"

VertexArray::VertexArray() {
  glCreateVertexArrays(1, &m_Id);
  glBindVertexArray(m_Id);
}

void VertexArray::bind() { glBindVertexArray(m_Id); }

void VertexArray::unbind() { glBindVertexArray(0); }

void VertexArray::addVertexBuffers(std::vector<BufferLayout>& layouts) {
  for (const auto& layout : layouts) {
    auto* buffer = new Buffer<GL_ARRAY_BUFFER>(layout.data, layout.byteSize);
    glEnableVertexAttribArray(m_VertexBuffers.size());
    glVertexAttribPointer(m_VertexBuffers.size(), layout.size, layout.type,
                          layout.normalized, layout.stride, nullptr);
    m_VertexBuffers.push_back(buffer);
  }
}

void VertexArray::setIndexBuffer(Buffer<GL_ELEMENT_ARRAY_BUFFER>* buffer) {
  buffer->bind();
  m_IndexBuffer = buffer;
}

VertexArray::~VertexArray() {
  for (auto& buf : m_VertexBuffers) {
    delete buf;
  }
  delete m_IndexBuffer;
  glDeleteVertexArrays(1, &m_Id);
}

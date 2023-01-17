#ifndef BUFFER_H
#define BUFFER_H

// clang-format off
#include <GL/glew.h>
// clang-format on
#include <cstddef>
#include <cstdint>

template <uint32_t target>
class Buffer {
public:
  Buffer() = default;

  Buffer(void* data, size_t size) {
    glCreateBuffers(1, &m_Id);
    glBindBuffer(target, m_Id);
    glBufferData(target, size, data, GL_STATIC_DRAW);
  }

  ~Buffer() { glDeleteBuffers(1, &m_Id); }

public:
  void bind() { glBindBuffer(target, m_Id); }
  void unbind() { glBindBuffer(target, 0); }

private:
  uint32_t m_Id;
};

#endif // !BUFFER_H

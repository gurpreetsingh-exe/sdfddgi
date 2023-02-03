#ifndef TEXTURE_H
#define TEXTURE_H

// clang-format off
#include <GL/glew.h>
// clang-format on
#include <cassert>
#include <cstdint>

template <uint32_t target>
class Texture {
public:
  Texture() = default;
  Texture(uint32_t width, uint32_t height, uint32_t format) {
    glCreateTextures(target, 1, &m_Id);
    glBindTexture(target, m_Id);
    switch (target) {
    case GL_TEXTURE_2D: {
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(target, 0, format, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, nullptr);
      break;
    }
    case GL_TEXTURE_2D_MULTISAMPLE: {
      glTexImage2DMultisample(target, 4, format, width, height, GL_TRUE);
      break;
    }
    default:
      assert(false && "Not implemented");
    }
  }

  Texture(uint32_t width, uint32_t height, uint32_t format, void* data) {
    glCreateTextures(target, 1, &m_Id);
    glBindTexture(target, m_Id);
    switch (target) {
    case GL_TEXTURE_2D: {
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(target, 0, format, width, height, 0, GL_RGBA,
                   GL_UNSIGNED_BYTE, data);
      break;
    }
    case GL_TEXTURE_2D_MULTISAMPLE: {
      glTexImage2DMultisample(target, 4, format, width, height, GL_TRUE);
      break;
    }
    default:
      assert(false && "Not implemented");
    }
  }

  ~Texture() { glDeleteTextures(1, &m_Id); }

public:
  void bind() { glBindTexture(target, m_Id); }
  void unbind() { glBindTexture(target, 0); }
  const uint32_t getId() const { return m_Id; }

private:
  uint32_t m_Id;
};

#endif // !TEXTURE_H

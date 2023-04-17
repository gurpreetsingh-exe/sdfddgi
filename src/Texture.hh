#ifndef TEXTURE_H
#define TEXTURE_H

// clang-format off
#include <GL/glew.h>
// clang-format on
#include "stb_image.h"
#include <cassert>
#include <cstdint>
#include <string>

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

  Texture(uint32_t width, uint32_t height, uint32_t format, int pixel_type,
          void* data) {
    glCreateTextures(target, 1, &m_Id);
    glBindTexture(target, m_Id);
    switch (target) {
    case GL_TEXTURE_2D: {
      glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
      glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
      glTexImage2D(target, 0, format, width, height, 0, GL_RGBA, pixel_type,
                   data);
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

  Texture(const std::string& filepath) {
    int w = 0;
    int h = 0;
    int channels = 0;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(
        stbi_loadf(filepath.c_str(), &w, &h, &channels, 0));
    m_Width = w;
    m_Height = h;

    glCreateTextures(target, 1, &m_Id);
    glBindTexture(target, m_Id);

    uint32_t internal_format = 0x00;
    uint32_t byte_format;
    switch (channels) {
    case 3: {
      internal_format = GL_RGB32F;
      byte_format = GL_FLOAT;

      glTexImage2D(target, 0, internal_format, w, h, 0, GL_RGB, byte_format,
                   data);
    } break;
    case 4: {
      internal_format = GL_RGBA32F;
      byte_format = GL_FLOAT;

      glTexImage2D(target, 0, internal_format, w, h, 0, GL_RGBA, byte_format,
                   data);
    } break;
    default: {
      assert(0);
    }
    }

    glTexParameteri(target, GL_TEXTURE_WRAP_S, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_WRAP_T, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glGenerateMipmap(target);
  }

  ~Texture() { glDeleteTextures(1, &m_Id); }

public:
  void bind() { glBindTexture(target, m_Id); }
  void unbind() { glBindTexture(target, 0); }
  [[nodiscard]] const uint32_t getId() const { return m_Id; }
  [[nodiscard]] const uint32_t getWidth() const { return m_Width; }
  [[nodiscard]] const uint32_t getHeight() const { return m_Height; }

private:
  uint32_t m_Id;
  uint32_t m_Width, m_Height;
};

#endif // !TEXTURE_H

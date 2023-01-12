#ifndef TEXTURE_H
#define TEXTURE_H

// clang-format off
#include <GL/glew.h>
// clang-format on
#include <cstdint>

class Texture {
public:
  Texture(uint32_t width, uint32_t height, uint32_t format);
  ~Texture();

public:
  const uint32_t getId() const { return m_Id; }

private:
  uint32_t m_Id;
};

#endif // !TEXTURE_H

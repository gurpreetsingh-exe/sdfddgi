#include "Texture.hh"

Texture::Texture(uint32_t width, uint32_t height, uint32_t format) {
  glCreateTextures(GL_TEXTURE_2D, 1, &m_Id);
  glBindTexture(GL_TEXTURE_2D, m_Id);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, nullptr);
}

Texture::~Texture() { glDeleteTextures(1, &m_Id); }

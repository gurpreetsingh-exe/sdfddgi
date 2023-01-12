#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Texture.hh"
#include <cstdint>
#include <vector>

class Framebuffer {
public:
  Framebuffer(uint32_t width, uint32_t height);
  ~Framebuffer();

public:
  void bind();
  void unbind();
  void addColorAttachment(const Texture& texture);
  void setDepthAttachment();
  bool isComplete();
  std::vector<uint32_t> getColorAttachments() { return m_ColorAttachments; }

private:
  uint32_t m_Id;
  uint32_t m_Width, m_Height;
  std::vector<uint32_t> m_ColorAttachments;
  uint32_t m_DepthAttachment;
};

#endif // !FRAMEBUFFER_H

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
  void addColorAttachment();
  void setDepthAttachment();
  bool isComplete();
  std::vector<Texture*> getColorAttachments() { return m_ColorAttachments; }
  void onResize(uint32_t width, uint32_t height);
  void invalidate();

private:
  uint32_t m_Id;
  uint32_t m_Width, m_Height;
  std::vector<Texture*> m_ColorAttachments;
  uint32_t m_DepthAttachment;
};

#endif // !FRAMEBUFFER_H

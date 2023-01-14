#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H
// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Texture.hh"
#include <cstdint>
#include <vector>

template <uint32_t target>
class Framebuffer {
public:
  Framebuffer(uint32_t width, uint32_t height)
      : m_Width(width), m_Height(height) {
    glCreateFramebuffers(1, &m_Id);
    glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
    addColorAttachment();
    setDepthAttachment();
  }

  ~Framebuffer() {
    for (const auto& t : m_ColorAttachments) {
      delete t;
    }
    glDeleteFramebuffers(1, &m_Id);
  }

public:
  void bind() { glBindFramebuffer(GL_FRAMEBUFFER, m_Id); }

  void unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

  uint32_t getId() { return m_Id; }

  void addColorAttachment() {
    auto texture = new Texture<target>(m_Width, m_Height, GL_RGB8);
    glFramebufferTexture2D(GL_FRAMEBUFFER,
                           GL_COLOR_ATTACHMENT0 + m_ColorAttachments.size(),
                           target, texture->getId(), 0);
    m_ColorAttachments.push_back(texture);
    texture->unbind();
  }

  void setDepthAttachment() {
    glCreateTextures(target, 1, &m_DepthAttachment);
    glBindTexture(target, m_DepthAttachment);
    switch (target) {
    case GL_TEXTURE_2D: {
      glTexStorage2D(target, 1, GL_DEPTH24_STENCIL8, m_Width, m_Height);
      break;
    }
    case GL_TEXTURE_2D_MULTISAMPLE: {
      glTexStorage2DMultisample(target, 4, GL_DEPTH24_STENCIL8, m_Width,
                                m_Height, GL_TRUE);
      break;
    }
    default:
      assert(false && "Not implemented");
    }
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, target,
                           m_DepthAttachment, 0);
    glBindTexture(target, 0);
  }

  bool isComplete() {
    return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
  }

  std::vector<Texture<target>*> getColorAttachments() {
    return m_ColorAttachments;
  }

  void onResize(uint32_t width, uint32_t height) {
    if (width != m_Width || height != m_Height) {
      m_Width = width;
      m_Height = height;
      invalidate();
    }
  }

  void invalidate() {
    bind();
    for (const auto& t : m_ColorAttachments) {
      delete t;
    }
    m_ColorAttachments.clear();
    glDeleteTextures(1, &m_Id);
    addColorAttachment();
    setDepthAttachment();
    unbind();
  }

private:
  uint32_t m_Id;
  uint32_t m_Width, m_Height;
  std::vector<Texture<target>*> m_ColorAttachments;
  uint32_t m_DepthAttachment;
};

#endif // !FRAMEBUFFER_H

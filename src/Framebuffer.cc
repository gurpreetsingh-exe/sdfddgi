#include "Framebuffer.hh"
#include <GL/gl.h>
#include <iostream>

Framebuffer::Framebuffer(uint32_t width, uint32_t height)
    : m_Width(width), m_Height(height) {
  glCreateFramebuffers(1, &m_Id);
  glBindFramebuffer(GL_FRAMEBUFFER, m_Id);
  addColorAttachment();
  setDepthAttachment();
}

void Framebuffer::bind() { glBindFramebuffer(GL_FRAMEBUFFER, m_Id); }

void Framebuffer::unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

void Framebuffer::addColorAttachment() {
  auto texture = new Texture(m_Width, m_Height, GL_RGB8);
  glFramebufferTexture2D(GL_FRAMEBUFFER,
                         GL_COLOR_ATTACHMENT0 + m_ColorAttachments.size(),
                         GL_TEXTURE_2D, texture->getId(), 0);
  m_ColorAttachments.push_back(texture);
}

void Framebuffer::setDepthAttachment() {
  glCreateTextures(GL_TEXTURE_2D, 1, &m_DepthAttachment);
  glBindTexture(GL_TEXTURE_2D, m_DepthAttachment);
  glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH24_STENCIL8, m_Width, m_Height);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                         GL_TEXTURE_2D, m_DepthAttachment, 0);
}

bool Framebuffer::isComplete() {
  return glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE;
}

void Framebuffer::onResize(uint32_t width, uint32_t height) {
  if (width != m_Width || height != m_Height) {
    m_Width = width;
    m_Height = height;
    invalidate();
  }
}

void Framebuffer::invalidate() {
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

Framebuffer::~Framebuffer() {
  for (const auto& t : m_ColorAttachments) {
    delete t;
  }
  glDeleteFramebuffers(1, &m_Id);
}

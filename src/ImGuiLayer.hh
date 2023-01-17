#ifndef IMGUI_LAYER_H
#define IMGUI_LAYER_H
#include "Framebuffer.hh"
#include "Window.hh"
#include "imgui.h"
#include <string>

struct Properties {
  float cameraFov = 90.0f;
  size_t meshVertices;
};

class ImGuiLayer {
public:
  ImGuiLayer();
  ~ImGuiLayer();

public:
  void onUpdate();
  void onUpdate(Framebuffer<GL_TEXTURE_2D>& framebuffer);
  void beginFrame();
  void endFrame();
  ImVec2 getViewportDimensions() { return m_Dim; }

private:
  bool m_Open;
  bool m_OptFullscreen = true;
  bool m_OptPadding = false;
  ImGuiDockNodeFlags m_DockspaceFlags = ImGuiDockNodeFlags_None;
  ImGuiWindowFlags m_WindowFlags =
      ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
  ImVec2 m_Dim;

public:
  Properties props;
};

#endif // !IMGUI_LAYER_H

#include "ImGuiLayer.hh"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

extern Window* window;

ImGuiLayer::ImGuiLayer() {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO& io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
  io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window->getHandle(), true);
  ImGui_ImplOpenGL3_Init("#version 450");
}

void ImGuiLayer::beginFrame() {
  ImGui_ImplOpenGL3_NewFrame();
  ImGui_ImplGlfw_NewFrame();
  ImGui::NewFrame();

  if (m_OptFullscreen) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    ImGui::SetNextWindowPos(viewport->WorkPos);
    ImGui::SetNextWindowSize(viewport->WorkSize);
    ImGui::SetNextWindowViewport(viewport->ID);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    m_WindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse |
                     ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
    m_WindowFlags |=
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
  }

  if (m_DockspaceFlags & ImGuiDockNodeFlags_PassthruCentralNode)
    m_WindowFlags |= ImGuiWindowFlags_NoBackground;

  if (!m_OptPadding)
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  m_Open = true;
}

void ImGuiLayer::onUpdate(Framebuffer<GL_TEXTURE_2D>& framebuffer) {
  ImGuiIO& io = ImGui::GetIO();
  ImGui::Begin("DockSpace", &m_Open, m_WindowFlags);

  if (!m_OptPadding)
    ImGui::PopStyleVar();

  if (m_OptFullscreen)
    ImGui::PopStyleVar(2);

  if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable) {
    ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
    ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), m_DockspaceFlags);
  }

  ImGui::End();

  ImGui::Begin("Debug");
  ImGui::Text("Delta Time: %.3f ms", 1000.0f / ImGui::GetIO().Framerate);
  ImGui::Text("Vertices: %zu", props.meshVertices);
  ImGui::End();

  ImGui::Begin("Properties");
  ImGui::Text("Camera:");
  ImGui::SliderFloat("Fov", &props.cameraFov, 10.0, 120.0);
  ImGui::End();

  ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
  ImGui::Begin("Viewport");

  m_Dim = ImGui::GetContentRegionAvail();
  ImGui::Image((void*)(intptr_t)framebuffer.getColorAttachments()[0]->getId(),
               m_Dim);

  ImGui::End();
  ImGui::PopStyleVar();
}

void ImGuiLayer::endFrame() {
  ImGuiIO& io = ImGui::GetIO();
  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

  if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
    GLFWwindow* backup_current_context = glfwGetCurrentContext();
    ImGui::UpdatePlatformWindows();
    ImGui::RenderPlatformWindowsDefault();
    glfwMakeContextCurrent(backup_current_context);
  }
}

ImGuiLayer::~ImGuiLayer() {
  ImGui_ImplOpenGL3_Shutdown();
  ImGui_ImplGlfw_Shutdown();
  ImGui::DestroyContext();
}

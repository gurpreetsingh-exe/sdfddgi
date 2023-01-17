// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Application.hh"
#include <future>
#include <iostream>
#include <thread>

Window* window;

#if NDEBUG
static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id,
                                       GLenum severity, GLsizei length,
                                       const GLchar* message,
                                       const void* userParam) {
  fprintf(stderr,
          "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
          (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type, severity,
          message);
}
#endif

Application::Application() {
  window = new Window(width, height, name.data());
  if (glewInit() == GLEW_OK) {
    std::cout << "GL version: " << glGetString(GL_VERSION) << std::endl;
  }
  m_ImGuiLayer = new ImGuiLayer();
#if NDEBUG
  glEnable(GL_DEBUG_OUTPUT);
  glDebugMessageCallback(MessageCallback, 0);
#endif
  m_Mesh = new Mesh();
  m_Shader = new Shader(vert, frag);
  m_Shader->bind();

  uint32_t width = window->getWidth();
  uint32_t height = window->getHeight();
  m_MsaaFb = new Framebuffer<GL_TEXTURE_2D_MULTISAMPLE>(width, height);
  m_MsaaFb->bind();
  if (!m_MsaaFb->isComplete()) {
    throw std::runtime_error("multisampled framebuffer setup not completed");
  }
  m_MsaaFb->unbind();

  m_Framebuffer = new Framebuffer<GL_TEXTURE_2D>(width, height);
  m_Framebuffer->bind();
  if (!m_Framebuffer->isComplete()) {
    throw std::runtime_error("framebuffer setup not completed");
  }
  m_Framebuffer->unbind();

  m_Camera = new Camera(width, height, 90.0, 0.01, 100.0);
}

void Application::run() {
  auto future = std::async(loadGLTF, "sponza/Sponza.gltf", m_Mesh);

  window->isRunning([&] {
    m_ImGuiLayer->props.meshVertices = m_Mesh->vertices.size();
    m_Camera->setFov(m_ImGuiLayer->props.cameraFov);

    if (m_Mesh->status == MeshStatus::Loaded && !VAOSetupCompleted) {
      m_VertexArray = new VertexArray();
      m_VertexArray->bind();
      std::vector<BufferLayout> layouts = {
          {
              .type = FLOAT,
              .normalized = false,
              .stride = 12,
              .size = 3,
              .data = m_Mesh->vertices.data(),
              .byteSize = m_Mesh->vertices.size() * 12,
          },
          {
              .type = FLOAT,
              .normalized = true,
              .stride = 12,
              .size = 3,
              .data = m_Mesh->normals.data(),
              .byteSize = m_Mesh->normals.size() * 12,
          }};
      m_VertexArray->addVertexBuffers(layouts);
      m_VertexArray->setIndexBuffer(new Buffer<GL_ELEMENT_ARRAY_BUFFER>(
          m_Mesh->indices.data(), m_Mesh->indices.size() * sizeof(uint32_t)));
      VAOSetupCompleted = true;
    }

    auto event = window->getEvent();
    m_Camera->onUpdate(event);

    m_ImGuiLayer->beginFrame();
    m_ImGuiLayer->onUpdate(*m_Framebuffer);

    ImVec2 dim = m_ImGuiLayer->getViewportDimensions();
    m_MsaaFb->onResize(dim.x, dim.y);
    m_Framebuffer->onResize(dim.x, dim.y);
    m_Camera->onResize(dim.x, dim.y);

    if (m_Mesh->status == MeshStatus::Loaded) {
      m_Shader->bind();
      m_MsaaFb->bind();
      m_VertexArray->bind();
      m_Shader->uploadUniformMat4("transform", m_Mesh->localTransform);
      m_Shader->uploadUniformMat4("modelViewProjection",
                                  m_Camera->getProjection() *
                                      m_Camera->getView());

      glEnable(GL_DEPTH_TEST);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glDrawElements(GL_TRIANGLES, m_Mesh->indices.size(), GL_UNSIGNED_INT,
                     nullptr);
      glViewport(0, 0, dim.x, dim.y);
      glDisable(GL_DEPTH_TEST);

      glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MsaaFb->getId());
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer->getId());
      glBlitFramebuffer(0, 0, dim.x, dim.y, 0, 0, dim.x, dim.y,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
      m_MsaaFb->unbind();
    } else {
      m_MsaaFb->bind();
      glClear(GL_COLOR_BUFFER_BIT);
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      glViewport(0, 0, dim.x, dim.y);

      glBindFramebuffer(GL_READ_FRAMEBUFFER, m_MsaaFb->getId());
      glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_Framebuffer->getId());
      glBlitFramebuffer(0, 0, dim.x, dim.y, 0, 0, dim.x, dim.y,
                        GL_COLOR_BUFFER_BIT, GL_NEAREST);
      m_MsaaFb->unbind();
    }

    m_ImGuiLayer->endFrame();

    if (!invalidateFuture &&
        future.wait_for(std::chrono::seconds(0)) == std::future_status::ready) {
      future.get();
      invalidateFuture = true;
    }
  });
}

Application::~Application() {
  delete m_VertexArray;
  delete m_Camera;
  delete m_Shader;
  delete m_MsaaFb;
  delete m_Framebuffer;
  delete m_Mesh;
  delete m_ImGuiLayer;
  delete window;
}

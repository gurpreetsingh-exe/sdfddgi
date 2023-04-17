// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Application.hh"
#include <future>
#include <iostream>
#include <thread>

Window* window;
#define NDEBUG 1

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
  m_Shader->bind();

  window->isRunning([&] {
    m_Camera->setFov(m_ImGuiLayer->props.cameraFov);

    if (m_Mesh->status == MeshStatus::Loaded && !VAOSetupCompleted) {
      for (auto& [id, mesh] : m_Mesh->model.mesh) {
        std::vector<Vertex>& vertices = mesh.vertices;
        std::vector<uint32_t>& indices = mesh.indices;
        m_ImGuiLayer->props.meshVertices += vertices.size();
        uint32_t i = 0;
        glGenTextures(1, &i);
        GLenum err;
        while ((err = glGetError()) != GL_NO_ERROR) {
          switch (err) {
          case GL_INVALID_ENUM:
            std::cout << "GL_INVALID_ENUM" << std::endl;
            break;
          case GL_INVALID_VALUE:
            std::cout << "GL_INVALID_VALUE" << std::endl;
            break;
          case GL_INVALID_OPERATION:
            std::cout << "GL_INVALID_OPERATION" << std::endl;
            break;
          case GL_STACK_OVERFLOW:
            std::cout << "GL_STACK_OVERFLOW" << std::endl;
            break;
          case GL_STACK_UNDERFLOW:
            std::cout << "GL_STACK_UNDERFLOW" << std::endl;
            break;
          case GL_OUT_OF_MEMORY:
            std::cout << "GL_OUT_OF_MEMORY" << std::endl;
            break;
          case GL_INVALID_FRAMEBUFFER_OPERATION:
            std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION" << std::endl;
            break;
          case GL_CONTEXT_LOST:
            std::cout << "GL_CONTEXT_LOST" << std::endl;
            break;
          case GL_TABLE_TOO_LARGE:
            std::cout << "GL_TABLE_TOO_LARGE" << std::endl;
            break;
          default:
            std::cout << "what?" << std::endl;
            break;
          }
        }
        glBindTexture(GL_TEXTURE_2D, i);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, mesh.image.width,
                     mesh.image.height, 0, GL_RGBA, mesh.image.pixel_type,
                     mesh.image.image.data());
        mesh.texture = i;

        m_VertexArray = new VertexArray();
        m_VertexArray->bind();
        std::vector<BufferLayout> layouts = {
            {
                .type = FLOAT,
                .normalized = false,
                .stride = 12,
                .size = 3,
                .data = (void*)vertices.data(),
                .byteSize = vertices.size() * 12,
            },
            {
                .type = FLOAT,
                .normalized = true,
                .stride = 12,
                .size = 3,
                .data = mesh.normals.data(),
                .byteSize = mesh.normals.size() * 12,
            },
            {
                .type = FLOAT,
                .normalized = false,
                .stride = 8,
                .size = 2,
                .data = (void*)mesh.texCoords.data(),
                .byteSize = mesh.texCoords.size() * 8,
            },
        };
        m_VertexArray->addVertexBuffers(layouts);
        m_VertexArray->setIndexBuffer(new Buffer<GL_ELEMENT_ARRAY_BUFFER>(
            indices.data(), indices.size() * sizeof(uint32_t)));
        mesh.vertexArray = m_VertexArray;
      }
      VAOSetupCompleted = true;
    }

    if (m_ImGuiLayer->props.reloadShaders) {
      delete m_Shader;
      m_Shader = new Shader(vert, frag);
      m_Shader->bind();
      m_ImGuiLayer->props.reloadShaders = false;
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
      m_MsaaFb->bind();
      m_Shader->uploadUniformMat4("transform", m_Mesh->localTransform);
      m_Shader->uploadUniformMat4("modelViewProjection",
                                  m_Camera->getProjection() *
                                      m_Camera->getView());
      m_Shader->uploadUniformSampler("baseColor", 0);
      glEnable(GL_DEPTH_TEST);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
      glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
      for (const auto& [id, mesh] : m_Mesh->model.mesh) {
        mesh.vertexArray->bind();
        glBindTexture(GL_TEXTURE_2D, mesh.texture);
        glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT,
                       nullptr);
      }
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

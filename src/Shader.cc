// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Shader.hh"
#include <stdexcept>
#include <vector>

Shader::Shader(const std::string& vertSrc, const std::string& fragSrc) {
  const char* vert = vertSrc.c_str();
  const char* frag = fragSrc.c_str();

  uint32_t vertShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertShader, 1, &vert, 0);
  glCompileShader(vertShader);

  int result;
  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> infoLog(length);
    glGetShaderInfoLog(vertShader, length, &length, &infoLog[0]);
    glDeleteShader(vertShader);
    throw std::runtime_error(infoLog.data());
  }

  uint32_t fragShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragShader, 1, &frag, 0);
  glCompileShader(fragShader);

  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);
  if (result == GL_FALSE) {
    int length;
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &length);
    std::vector<char> infoLog(length);
    glGetShaderInfoLog(fragShader, length, &length, &infoLog[0]);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    throw std::runtime_error(infoLog.data());
  }

  m_Id = glCreateProgram();
  glAttachShader(m_Id, vertShader);
  glAttachShader(m_Id, fragShader);
  glLinkProgram(m_Id);

  int isLinked = 0;
  glGetProgramiv(m_Id, GL_LINK_STATUS, (int*)&isLinked);
  if (isLinked == GL_FALSE) {
    int maxLength = 0;
    glGetProgramiv(m_Id, GL_INFO_LOG_LENGTH, &maxLength);

    std::vector<char> infoLog(maxLength);
    glGetProgramInfoLog(m_Id, maxLength, &maxLength, &infoLog[0]);

    glDeleteProgram(m_Id);
    glDeleteShader(vertShader);
    glDeleteShader(fragShader);
    throw std::runtime_error(infoLog.data());
  }

  glDetachShader(m_Id, vertShader);
  glDetachShader(m_Id, fragShader);
}

void Shader::bind() { glUseProgram(m_Id); }

void Shader::unbind() { glUseProgram(0); }

Shader::~Shader() { glDeleteProgram(m_Id); }

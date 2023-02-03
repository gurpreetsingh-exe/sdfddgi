// clang-format off
#include <GL/glew.h>
// clang-format on
#include "Shader.hh"
#include "utils.hh"
#include <glm/gtc/type_ptr.hpp>
#include <stdexcept>
#include <vector>

Shader::Shader(const std::string& vertPath, const std::string& fragPath) {
  const auto vertSrc = readFile(vertPath);
  const auto fragSrc = readFile(fragPath);

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

void Shader::uploadUniformFloat(const std::string& name, float value) {
  uint32_t valLoc = glGetUniformLocation(m_Id, name.c_str());
  glUniform1f(valLoc, value);
}

void Shader::uploadUniformVec2(const std::string& name, const glm::vec2& vec) {
  uint32_t vecLoc = glGetUniformLocation(m_Id, name.c_str());
  glUniform2f(vecLoc, vec.x, vec.y);
}

void Shader::uploadUniformVec3(const std::string& name, const glm::vec3& vec) {
  uint32_t vecLoc = glGetUniformLocation(m_Id, name.c_str());
  glUniform3f(vecLoc, vec.x, vec.y, vec.z);
}

void Shader::uploadUniformVec4(const std::string& name, const glm::vec4& vec) {
  uint32_t vecLoc = glGetUniformLocation(m_Id, name.c_str());
  glUniform4f(vecLoc, vec.x, vec.y, vec.z, vec.w);
}

void Shader::uploadUniformMat3(const std::string& name, const glm::mat3& mat) {
  uint32_t matLoc = glGetUniformLocation(m_Id, name.c_str());
  glUniformMatrix3fv(matLoc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::uploadUniformMat4(const std::string& name, const glm::mat4& mat) {
  uint32_t matLoc = glGetUniformLocation(m_Id, name.c_str());
  glUniformMatrix4fv(matLoc, 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::uploadUniformSampler(const std::string& name, uint32_t textureId) {
  uint32_t samplerLoc = glGetUniformLocation(m_Id, name.c_str());
  glUniform1i(samplerLoc, textureId);
}

void Shader::uploadUniformSamplers(const std::string& name,
                                   const std::vector<int32_t>& textureIds) {
  uint32_t samplerLoc = glGetUniformLocation(m_Id, name.c_str());
  glUniform1iv(samplerLoc, textureIds.size(), textureIds.data());
}

Shader::~Shader() { glDeleteProgram(m_Id); }

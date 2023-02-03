#ifndef SHADER_H
#define SHADER_H

#include <cstdint>
#include <glm/glm.hpp>
#include <string>
#include <vector>

class Shader {
public:
  Shader(const std::string& vertSrc, const std::string& fragSrc);
  ~Shader();

public:
  void bind();
  void unbind();
  const uint32_t getId() const { return m_Id; }

  void uploadUniformFloat(const std::string& name, float value);

  void uploadUniformVec2(const std::string& name, const glm::vec2& vec);
  void uploadUniformVec3(const std::string& name, const glm::vec3& vec);
  void uploadUniformVec4(const std::string& name, const glm::vec4& vec);

  void uploadUniformMat3(const std::string& name, const glm::mat3& mat);
  void uploadUniformMat4(const std::string& name, const glm::mat4& mat);

  void uploadUniformSampler(const std::string& name, uint32_t textureId);
  void uploadUniformSamplers(const std::string& name,
                             const std::vector<int32_t>& textureIds);

private:
  uint32_t m_Id;
};

#endif // !SHADER_H

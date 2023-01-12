#ifndef SHADER_H
#define SHADER_H

#include <cstdint>
#include <string>

class Shader {
public:
  Shader(const std::string& vertSrc, const std::string& fragSrc);
  ~Shader();

public:
  void bind();
  void unbind();
  const uint32_t getId() const { return m_Id; }

private:
  uint32_t m_Id;
};

#endif // !SHADER_H

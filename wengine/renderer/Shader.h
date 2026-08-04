#pragma once

#include <string>
#include <glm/glm.hpp>

class Shader
{
public:
    Shader(const std::string& vertexPath, const std::string& fragmentPath);
    ~Shader();

    Shader(const Shader&)            = delete;
    Shader& operator=(const Shader&) = delete;

    Shader(Shader&& other) noexcept;
    Shader& operator=(Shader&& other) noexcept;

    void bind() const;
    void unbind() const;

    void setInt   (const std::string& name, int value) const;
    void setFloat (const std::string& name, float value) const;
    void setVec3  (const std::string& name, const glm::vec3& v) const;
    void setMat4  (const std::string& name, const glm::mat4& v) const;

private:
    uint32_t m_id = 0;
    static uint32_t compileShader(uint32_t type, const std::string& source);
};
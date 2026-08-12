#include "wengine/render/Shader.h"

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <fstream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const std::string& vertexPath, const std::string& fragmentPath)
{
    auto readFile = [](const std::string& path) -> std::string
    {
        std::ifstream file(path);
        if (!file.is_open())
            throw std::runtime_error("Shader: failed to open file: " + path);
        std::stringstream ss;
        ss << file.rdbuf();
        return ss.str();
    };

    std::string vertexSource = readFile(vertexPath);
    std::string fragmentSource = readFile(fragmentPath);

    uint32_t vertexShader = compileShader(GL_VERTEX_SHADER,   vertexSource);
    uint32_t fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSource);

    m_id = glCreateProgram();
    glAttachShader(m_id, vertexShader);
    glAttachShader(m_id, fragmentShader);
    glLinkProgram(m_id);

    int success;
    glGetProgramiv(m_id, GL_LINK_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetProgramInfoLog(m_id, 512, nullptr, log);
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
        glDeleteProgram(m_id);
        m_id = 0;
        throw std::runtime_error(std::string("Shader: link failed: ") + log);
    }

    // already linked to program
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    spdlog::info("Shader: compiled '{}' , '{}'", vertexPath, fragmentPath);
}

Shader::~Shader()
{
    if (m_id) glDeleteProgram(m_id);
}

// move object
Shader::Shader(Shader&& other) noexcept
    : m_id(other.m_id)
{
    other.m_id = 0;
}

// move object
Shader& Shader::operator=(Shader&& other) noexcept
{
    if (this != &other)
    {
        if (m_id) glDeleteProgram(m_id);
        m_id       = other.m_id;
        other.m_id = 0;
    }
    return *this;
}

void Shader::bind()   const { glUseProgram(m_id); }
void Shader::unbind() const { glUseProgram(0); }

void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& v) const
{
    glUniform2f(glGetUniformLocation(m_id, name.c_str()), v.x, v.y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& v) const
{
    glUniform3f(glGetUniformLocation(m_id, name.c_str()), v.x, v.y, v.z);
}

void Shader::setMat4(const std::string& name, const glm::mat4& m) const
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, &m[0][0]);
}

uint32_t Shader::compileShader(uint32_t type, const std::string& source)
{
    uint32_t    id  = glCreateShader(type);
    const char* src = source.c_str();
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int success;
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(id, 512, nullptr, log);
        glDeleteShader(id);
        throw std::runtime_error(std::string("Shader: compile failed: ") + log);
    }

    return id;
}

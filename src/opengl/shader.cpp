//
// Created by Gianni on 30/06/2024.
//

#include "shader.hpp"

Shader::Shader()
    : m_renderer_id()
{
}

Shader::Shader(const char* vs_file, const char* fs_file)
{
    std::string vs_src = parse_shader(vs_file);
    std::string fs_src = parse_shader(fs_file);

    uint32_t vs = compile_shader(GL_VERTEX_SHADER, vs_src.c_str());
    uint32_t fs = compile_shader(GL_FRAGMENT_SHADER, fs_src.c_str());

    m_renderer_id = glCreateProgram();

    glAttachShader(m_renderer_id, vs);
    glAttachShader(m_renderer_id, fs);
    glLinkProgram(m_renderer_id);

    glDeleteShader(vs);
    glDeleteShader(fs);
}

Shader::~Shader()
{
    if (m_renderer_id)
    {
        glDeleteProgram(m_renderer_id);
    }
}

Shader::Shader(Shader &&other) noexcept
{
    m_renderer_id = other.m_renderer_id;
    m_uniform_location_cache.swap(other.m_uniform_location_cache);

    other.m_renderer_id = 0;
}

Shader &Shader::operator=(Shader &&other) noexcept
{
    if (this != &other)
    {
        if (m_renderer_id)
        {
            glDeleteProgram(m_renderer_id);
        }

        m_renderer_id = other.m_renderer_id;
        m_uniform_location_cache.swap(other.m_uniform_location_cache);

        other.m_renderer_id = 0;
    }

    return *this;
}

void Shader::bind() const
{
    glUseProgram(m_renderer_id);
}

void Shader::unbind() const
{
    glUseProgram(0);
}

void Shader::set_int(const char* name, int v0) const
{
    glUniform1i(get_uniform_location(name), v0);
}

void Shader::set_float(const char* name, float v0) const
{
    glUniform1f(get_uniform_location(name), v0);
}

void Shader::set_float2(const char* name, float v0, float v1) const
{
    glUniform2f(get_uniform_location(name), v0, v1);
}

void Shader::set_float2(const char* name, const glm::vec2 &vec2) const
{
    glUniform2f(get_uniform_location(name), vec2.x, vec2.y);
}

void Shader::set_float3(const char* name, float v0, float v1, float v2) const
{
    glUniform3f(get_uniform_location(name), v0, v1, v2);
}

void Shader::set_float3(const char* name, const glm::vec3 &vec3) const
{
    glUniform3f(get_uniform_location(name), vec3.x, vec3.y, vec3.z);
}

void Shader::set_float4(const char* name, float v0, float v1, float v2, float v3) const
{
    glUniform4f(get_uniform_location(name), v0, v1, v2, v3);
}

void Shader::set_float4(const char* name, const glm::vec4 &vec4) const
{
    glUniform4f(get_uniform_location(name), vec4.x, vec4.y, vec4.z, vec4.w);
}

void Shader::set_mat4(const char* name, const glm::mat4 &matrix) const
{
    glUniformMatrix4fv(get_uniform_location(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

uint32_t Shader::id() const
{
    return m_renderer_id;
}

std::string Shader::parse_shader(const char* file_name)
{
    std::ifstream file(file_name);

    if (!file.is_open())
    {
        throw std::runtime_error("Shader::parse_shader: Failed to open file " + std::string(file_name));
    }

    std::ostringstream oss;
    oss << file.rdbuf();

    return oss.str();
}

uint32_t Shader::compile_shader(uint32_t type, const char *source)
{
    uint32_t shader_id = glCreateShader(type);
    glShaderSource(shader_id, 1, &source, nullptr);
    glCompileShader(shader_id);
    return shader_id;
}

int Shader::get_uniform_location(const char* name) const
{
    if (auto result = m_uniform_location_cache.find(name);
            result != m_uniform_location_cache.end())
    {
        return result->second;
    }

    int location = glGetUniformLocation(m_renderer_id, name);

    if (location == -1)
    {
        printf("Warning: Uniform %s doesn't exist or it may not be used", name);
    }

    m_uniform_location_cache[name] = location;

    return location;
}

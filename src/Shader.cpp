#define DBG_MACRO_NO_WARNING
#include <dbg.h>
#include <fmt/format.h>
#include <glm/gtc/type_ptr.hpp>

#include "Light.h"
#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath): m_programId{0} {
    const std::string vertexSource = readFile(vertexPath);
    const std::string fragmentSource = readFile(fragmentPath);

    m_programId = glCreateProgram();

    if (const auto vertexShader = compile(vertexSource, GL_VERTEX_SHADER); vertexShader != 0) {
        glAttachShader(m_programId, vertexShader);
        glDeleteShader(vertexShader);
    }

    if (const auto fragmentShader = compile(fragmentSource, GL_FRAGMENT_SHADER); fragmentShader != 0) {
        glAttachShader(m_programId, fragmentShader);
        glDeleteShader(fragmentShader);
    }

    glLinkProgram(m_programId);
    int success;
    glGetProgramiv(m_programId, GL_LINK_STATUS, &success);
    if (!success) {
        char message[512];
        glGetProgramInfoLog(m_programId, 512, nullptr, message);
        auto str = fmt::format("Could not link shader program:\n{}", message);
        throw std::runtime_error(str);
    }
}

void Shader::use() const {
    glUseProgram(m_programId);
}

void Shader::setBool(const std::string &name, const bool value) {
    const auto loc = getUniformLocation(name);
    glUniform1i(loc, value);
}

void Shader::setInt(const std::string &name, const int value) {
    const auto loc = getUniformLocation(name);
    glUniform1i(loc, value);
}

void Shader::setFloat(const std::string &name, const float value) {
    const auto loc = getUniformLocation(name);
    glUniform1f(loc, value);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) {
    const auto loc = getUniformLocation(name);
    glUniform3fv(loc, 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) {
    const auto loc = getUniformLocation(name);
    glUniform4fv(loc, 1, glm::value_ptr(value));
}

void Shader::setMat3(const std::string &name, const glm::mat3 &value) {
    const auto loc = getUniformLocation(name);
    glUniformMatrix3fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) {
    const auto loc = getUniformLocation(name);
    glUniformMatrix4fv(loc, 1, GL_FALSE, glm::value_ptr(value));
}

GLuint Shader::compile(const std::string &source, const GLuint type) {
    const auto shader = glCreateShader(type);
    const char *shaderCode = source.c_str();
    glShaderSource(shader, 1, &shaderCode, nullptr);
    glCompileShader(shader);

    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char message[512];
        glGetShaderInfoLog(shader, 512, nullptr, message);
        const auto typeStr = shaderTypeStr(type);
        const auto str = fmt::format("Could not compile {} shader:\n{}", shaderTypeStr(type), message);
        throw std::runtime_error(str);
    }

    return shader;
}

GLuint Shader::getUniformLocation(const std::string &name) {
    if (const auto it = m_uniformLocationCache.find(name); it != m_uniformLocationCache.end()) {
        return it->second;
    } else {
        const auto loc = glGetUniformLocation(m_programId, name.c_str());
        if (loc == -1) {
            const auto str = fmt::format("Uniform '{}' not found in shader program", name);
            throw std::runtime_error(str);
        }
        m_uniformLocationCache[name] = loc;
        return loc;
    }
}

std::string readFile(const std::string &path) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    // Might throw an exception if the file cannot be opened.
    file.open(path);
    std::stringstream stream;
    stream << file.rdbuf();
    file.close();
    return stream.str();
}

std::string shaderTypeStr(const GLuint type) {
    switch (type) {
        case GL_VERTEX_SHADER:
            return "vertex";
        case GL_FRAGMENT_SHADER:
            return "fragment";
        case GL_GEOMETRY_SHADER:
            return "geometry";
        default:
            return "unknown";
    }
}

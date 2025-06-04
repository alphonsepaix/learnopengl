#define DBG_MACRO_NO_WARNING
#include <dbg.h>
#include <glm/gtc/type_ptr.hpp>

#include "Light.h"
#include "Shader.h"

#include <fstream>
#include <iostream>
#include <sstream>

Shader::Shader(const std::string &vertexPath, const std::string &fragmentPath): m_programId{0} {
    std::string vertexSource, fragmentSource;
    try {
        vertexSource = read_content(vertexPath);
        fragmentSource = read_content(fragmentPath);
    } catch (const std::exception &e) {
        std::cerr << e.what() << '\n';
        return;
    }

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
        std::cerr << "Could not link shader program:\n" << message << '\n';
        m_programId = 0;
    }
}

void Shader::use() const {
    glUseProgram(m_programId);
}

void Shader::setBool(const std::string &name, const bool value) const {
    glUniform1i(glGetUniformLocation(m_programId, name.c_str()), value);
}

void Shader::setInt(const std::string &name, const int value) const {
    glUniform1i(glGetUniformLocation(m_programId, name.c_str()), value);
}

void Shader::setFloat(const std::string &name, const float value) const {
    glUniform1f(glGetUniformLocation(m_programId, name.c_str()), value);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &value) const {
    glUniform3fv(glGetUniformLocation(m_programId, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setVec4(const std::string &name, const glm::vec4 &value) const {
    glUniform4fv(glGetUniformLocation(m_programId, name.c_str()), 1, glm::value_ptr(value));
}

void Shader::setMat3(const std::string &name, const glm::mat3 &value) const {
    glUniformMatrix3fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
}

void Shader::setMat4(const std::string &name, const glm::mat4 &value) const {
    glUniformMatrix4fv(glGetUniformLocation(m_programId, name.c_str()), 1, GL_FALSE, glm::value_ptr(value));
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
        std::cerr << "Could not compile " << typeStr << " shader:\n" << message << '\n';
        glDeleteShader(shader);
        return 0;
    }

    return shader;
}

std::string read_content(const std::string &path) {
    std::ifstream file;
    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try {
        file.open(path);
        std::stringstream stream;
        stream << file.rdbuf();
        file.close();
        return stream.str();
    } catch (std::ifstream::failure &e) {
        std::cerr << "Could not read file '" << path << "': " << e.what() << '\n';
        return "";
    }
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

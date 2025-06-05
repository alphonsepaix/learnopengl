#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

class Light;

#include <string>
#include <unordered_map>

class Shader {
public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);

    void use() const;

    [[nodiscard]] GLuint getProgramId() const {
        return m_programId;
    }

    void setBool(const std::string &name, bool value);

    void setInt(const std::string &name, int value);

    void setFloat(const std::string &name, float value);

    void setVec3(const std::string &name, const glm::vec3 &value);

    void setVec4(const std::string &name, const glm::vec4 &value);

    void setMat3(const std::string &name, const glm::mat3 &value);

    void setMat4(const std::string &name, const glm::mat4 &value);

private:
    GLuint m_programId;

    static GLuint compile(const std::string &source, GLuint type);

    GLuint getUniformLocation(const std::string &name);

    std::unordered_map<std::string, GLint> m_uniformLocationCache;
};

std::string readFile(const std::string &path);

std::string shaderTypeStr(GLuint type);

#endif

#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>

class Light;

#include <string>

class Shader {
public:
    Shader(const std::string &vertexPath, const std::string &fragmentPath);

    void use() const;

    [[nodiscard]] GLuint getProgramId() const {
        return m_programId;
    }

    void setBool(const std::string &name, bool value) const;

    void setInt(const std::string &name, int value) const;

    void setFloat(const std::string &name, float value) const;

    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void setVec4(const std::string &name, const glm::vec4 &value) const;

    void setMat3(const std::string &name, const glm::mat3 &value) const;

    void setMat4(const std::string &name, const glm::mat4 &value) const;

    void setLight(const Light *light) const;

private:
    GLuint m_programId;

    static GLuint compile(const std::string &source, GLuint type);
};

std::string read_content(const std::string &path);

std::string shaderTypeStr(GLuint type);

#endif

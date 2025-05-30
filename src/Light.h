#ifndef LIGHT_H
#define LIGHT_H

#include <glm/glm.hpp>

#include <string>

class Shader;

constexpr auto AMBIENT = glm::vec4(0.2f);
constexpr auto DIFFUSE = glm::vec4(0.8f);
constexpr auto SPECULAR = glm::vec4(1.0f);

constexpr auto CONSTANT = 1.0f;
constexpr auto LINEAR = 0.09f;
constexpr auto QUADRATIC = 0.032f;

constexpr auto CUTOFF = 12.0f; // in degrees
constexpr auto OUTER_CUTOFF = 20.0f; // in degrees

class Light {
public:
    virtual ~Light() = default;

    virtual void setShaderUniforms(const Shader *shader) const = 0;

    virtual void widgets();

protected:
    Light(glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular);

    void setColorUniforms(const Shader *const shader, const std::string &name) const;

private:
    glm::vec3 m_ambient;
    glm::vec3 m_diffuse;
    glm::vec3 m_specular;
};

class DirectionalLight final : public Light {
public:
    explicit DirectionalLight(glm::vec3 direction, glm::vec3 ambient = AMBIENT,
                              glm::vec3 diffuse = DIFFUSE,
                              glm::vec3 specular = SPECULAR);

    void widgets() override;

    void setShaderUniforms(const Shader *const shader) const override;

    [[nodiscard]] const glm::vec3 &getDirection() const { return m_direction; };

private:
    glm::vec3 m_direction;
};

class PointLight final : public Light {
public:
    explicit PointLight(glm::vec3 position, glm::vec3 ambient = AMBIENT, glm::vec3 diffuse = DIFFUSE,
                        glm::vec3 specular = SPECULAR, float constant = CONSTANT,
                        float linear = LINEAR, float quadratic = QUADRATIC);

    void widgets() override;

    void setShaderUniforms(const Shader *const shader) const override;

    [[nodiscard]] const glm::vec3 &getPosition() const { return m_position; };

    [[nodiscard]] float getConstant() const { return m_constant; }

    [[nodiscard]] float getLinear() const { return m_linear; }

    [[nodiscard]] float getQuadratic() const { return m_quadratic; }

private:
    glm::vec3 m_position;

    float m_constant;
    float m_linear;
    float m_quadratic;
};

class SpotLight final : public Light {
public:
    SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient = AMBIENT, glm::vec3 diffuse = DIFFUSE,
              glm::vec3 specular = SPECULAR,
              float cutOff = CUTOFF, float outerCutOff = OUTER_CUTOFF, float constant = CONSTANT, float linear = LINEAR,
              float quadratic = QUADRATIC);

    void widgets() override;

    void setShaderUniforms(const Shader *const shader) const override;

    [[nodiscard]] const glm::vec3 &getPosition() const { return m_position; }

    [[nodiscard]] const glm::vec3 &getDirection() const { return m_direction; }

    void setPosition(const glm::vec3 &position) { m_position = position; }

    void setDirection(const glm::vec3 &direction) { m_direction = direction; }

    [[nodiscard]] float getCutOff() const { return glm::cos(glm::radians(m_cutOff)); }

    [[nodiscard]] float getOuterCutOff() const { return glm::cos(glm::radians(m_outerCutOff)); }

    [[nodiscard]] float getConstant() const { return m_constant; }

    [[nodiscard]] float getLinear() const { return m_linear; }

    [[nodiscard]] float getQuadratic() const { return m_quadratic; }

private:
    glm::vec3 m_position;
    glm::vec3 m_direction;

    float m_cutOff; // in degrees
    float m_outerCutOff; // in degrees

    float m_constant;
    float m_linear;
    float m_quadratic;
};

void attenuationWidgets(float &c, float &l, float &q);

#endif

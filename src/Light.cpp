#include <fmt/format.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "Light.h"
#include "Shader.h"

Light::Light(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular)
    : m_ambient{ambient}, m_diffuse{diffuse}, m_specular{specular} {
}

void Light::widgets() {
    ImGui::ColorEdit3(fmt::format("Light ambient##{}", reinterpret_cast<uintptr_t>(this)).c_str(),
                      glm::value_ptr(m_ambient));
    ImGui::ColorEdit3(fmt::format("Light diffuse##{}", reinterpret_cast<uintptr_t>(this)).c_str(),
                      glm::value_ptr(m_diffuse));
    ImGui::ColorEdit3(fmt::format("Light specular##{}", reinterpret_cast<uintptr_t>(this)).c_str(),
                      glm::value_ptr(m_specular));
}

void Light::setColorUniforms(const Shader *const shader, const std::string &name) const {
    shader->setVec3(name + ".ambient", m_ambient);
    shader->setVec3(name + ".diffuse", m_diffuse);
    shader->setVec3(name + ".specular", m_specular);
}

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,
                                   glm::vec3 specular): Light(ambient, diffuse, specular), m_direction{direction} {
}

void DirectionalLight::widgets() {
    Light::widgets();
    ImGui::SliderFloat3("Direction", glm::value_ptr(m_direction), -1.0f, 1.0f);
}

void DirectionalLight::setShaderUniforms(const Shader *const shader) const {
    const std::string name = "dirLight";
    setColorUniforms(shader, name);

    shader->setVec3(name + ".direction", m_direction);
}

PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant,
                       float linear, float quadratic) : Light(ambient, diffuse, specular), m_position{position},
                                                        m_constant{constant}, m_linear{linear}, m_quadratic{quadratic} {
}

void PointLight::widgets() {
    Light::widgets();
    ImGui::SliderFloat3("Position", glm::value_ptr(m_position), -10.0f, 10.0f);
    attenuationWidgets(m_constant, m_linear, m_quadratic);
}

void PointLight::setShaderUniforms(const Shader *const shader) const {
    const std::string name = "pointLight";
    setColorUniforms(shader, name);
    shader->setVec3(name + ".position", m_position);

    shader->setFloat(name + ".constant", m_constant);
    shader->setFloat(name + ".linear", m_linear);
    shader->setFloat(name + ".quadratic", m_quadratic);
}

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
                     float cutOff, float outerCutOff, float constant, float linear,
                     float quadratic) : Light(ambient, diffuse, specular),
                                        m_position{position}, m_direction{direction},
                                        m_cutOff{cutOff}, m_outerCutOff{outerCutOff}, m_constant{constant},
                                        m_linear{linear}, m_quadratic{quadratic} {
}

void SpotLight::widgets() {
    Light::widgets();
    ImGui::SliderFloat("Cut-off", &m_cutOff, 0.0f, 90.0f);
    ImGui::SliderFloat("Outer cut-off", &m_outerCutOff, m_cutOff, 90.0f);
    attenuationWidgets(m_constant, m_linear, m_quadratic);
}

void SpotLight::setShaderUniforms(const Shader *const shader) const {
    const std::string name = "spotLight";
    setColorUniforms(shader, name);

    shader->setVec3(name + ".position", m_position);
    shader->setVec3(name + ".direction", m_direction);

    shader->setFloat(name + ".cutOff", getCutOff());
    shader->setFloat(name + ".outerCutOff", getOuterCutOff());

    shader->setFloat(name + ".constant", m_constant);
    shader->setFloat(name + ".linear", m_linear);
    shader->setFloat(name + ".quadratic", m_quadratic);
}

void attenuationWidgets(float &c, float &l, float &q) {
    const auto text = fmt::format("Attenuation: c: {}, l: {}, q: {}", c, l, q);
    ImGui::Text(text.c_str());
}

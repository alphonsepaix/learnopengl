#define DBG_MACRO_NO_WARNING
#include <dbg.h>
#include <fmt/format.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "Light.h"
#include "Shader.h"

#include <array>
#include <iostream>

Light::Light(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular, const Type type)
    : m_ambient{ambient}, m_diffuse{diffuse}, m_specular{specular}, m_type{type} {
}

std::string Light::getTypeStr(const Type type) {
    switch (type) {
        case Light::Type::Directional:
            return "Directional";
        case Light::Type::Point:
            return "Point";
        case Light::Type::Spot:
            return "Spot";
        default:
            return "Unknown";
    }
}

void Light::widgets() {
    ImGui::ColorEdit3(fmt::format("Light ambient##{}", reinterpret_cast<uintptr_t>(this)).c_str(),
                      glm::value_ptr(m_ambient));
    ImGui::ColorEdit3(fmt::format("Light diffuse##{}", reinterpret_cast<uintptr_t>(this)).c_str(),
                      glm::value_ptr(m_diffuse));
    ImGui::ColorEdit3(fmt::format("Light specular##{}", reinterpret_cast<uintptr_t>(this)).c_str(),
                      glm::value_ptr(m_specular));
}

Light::Type Light::getType() const {
    return m_type;
}

void Light::setBaseUniforms(const Shader *const shader, const std::string &name) const {
    shader->setVec3(name + ".ambient", m_ambient);
    shader->setVec3(name + ".diffuse", m_diffuse);
    shader->setVec3(name + ".specular", m_specular);
    shader->setInt(name + ".type", static_cast<int>(m_type));
}

DirectionalLight::DirectionalLight(glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse,
                                   glm::vec3 specular): Light(ambient, diffuse, specular, Type::Directional),
                                                        m_direction{direction} {
}

void DirectionalLight::widgets() {
    Light::widgets();
    ImGui::SliderFloat3("Direction", glm::value_ptr(m_direction), -1.0f, 1.0f);
}

void DirectionalLight::setShaderUniforms(const Shader *const shader, const std::string &name) const {
    setBaseUniforms(shader, name);

    shader->setVec3(name + ".direction", m_direction);
}

void DirectionalLight::draw(const Shader *shader) {
    // no-op
}

PointLight::PointLight(glm::vec3 position, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular, float constant,
                       float linear, float quadratic) : Light(ambient, diffuse, specular, Type::Point),
                                                        m_position{position},
                                                        m_constant{constant}, m_linear{linear}, m_quadratic{quadratic} {
}

void PointLight::widgets() {
    Light::widgets();
    ImGui::SliderFloat3("Position", glm::value_ptr(m_position), -10.0f, 10.0f);
    attenuationWidgets(m_constant, m_linear, m_quadratic);
}

void PointLight::setShaderUniforms(const Shader *const shader, const std::string &name) const {
    setBaseUniforms(shader, name);
    shader->setVec3(name + ".position", m_position);

    shader->setFloat(name + ".constant", m_constant);
    shader->setFloat(name + ".linear", m_linear);
    shader->setFloat(name + ".quadratic", m_quadratic);
}

void PointLight::draw(const Shader *shader) {
    auto model = glm::translate(glm::mat4(1.0f), m_position);
    model = glm::scale(model, glm::vec3(0.2f));
    shader->setMat4("model", model);
    shader->setVec3("lightColor", m_diffuse);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

SpotLight::SpotLight(glm::vec3 position, glm::vec3 direction, glm::vec3 ambient, glm::vec3 diffuse, glm::vec3 specular,
                     float cutOff, float outerCutOff, float constant, float linear,
                     float quadratic) : Light(ambient, diffuse, specular, Type::Spot),
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

void SpotLight::setShaderUniforms(const Shader *const shader, const std::string &name) const {
    setBaseUniforms(shader, name);

    shader->setVec3(name + ".position", m_position);
    shader->setVec3(name + ".direction", m_direction);

    shader->setFloat(name + ".cutOff", getCutOff());
    shader->setFloat(name + ".outerCutOff", getOuterCutOff());

    shader->setFloat(name + ".constant", m_constant);
    shader->setFloat(name + ".linear", m_linear);
    shader->setFloat(name + ".quadratic", m_quadratic);
}

void SpotLight::draw(const Shader *shader) {
}

void attenuationWidgets(float &c, float &l, float &q) {
    const auto text = fmt::format("Attenuation: c: {}, l: {}, q: {}", c, l, q);
    ImGui::Text(text.c_str());
}

LightManager::LightManager(): m_activeLightsCount{0},
                              m_flashlight{SpotLight{glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f)}},
                              m_selectedLight{0}, m_flashLightOn(false) {
    // clang-format off
    constexpr std::array vertices = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f
    };
    // clang-format on

    glGenVertexArrays(1, &m_lightVao);
    glBindVertexArray(m_lightVao);
    GLuint lightVbo;
    glGenBuffers(1, &lightVbo);
    glBindBuffer(GL_ARRAY_BUFFER, lightVbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          static_cast<GLvoid *>(nullptr));
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void LightManager::widgets() {
    if (ImGui::CollapsingHeader("Lights")) {
        ImGui::SeparatorText("Add a new light");
        constexpr std::array lightTypes = {"Directional", "Point", "Spot"};
        ImGui::Combo("Type", &m_selectedLight, lightTypes.data(), lightTypes.size());
        ImGui::SameLine();
        if (ImGui::Button("Add")) {
            switch (m_selectedLight) {
                case 0:
                    add(std::make_unique<DirectionalLight>(glm::vec3(0.0f, -1.0f, 0.0f)));
                    break;
                case 1:
                    add(std::make_unique<PointLight>(glm::vec3(0.0f, 0.0f, 0.0f)));
                    break;
                case 2:
                    add(std::make_unique<SpotLight>(glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
                    break;
                default:
                    break;
            }
        }

        ImGui::SeparatorText("Lights");
        int removeIndex = -1;
        for (auto i = 0; i < m_lights.size(); ++i) {
            ImGui::PushID(i);
            const auto light = m_lights[i].get();
            const auto treeNode = ImGui::TreeNode(
                fmt::format("Light #{} ({})", i, Light::getTypeStr(light->getType())).c_str());
            ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0 / 7.0f, 0.6f, 0.6f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0 / 7.0f, 0.7f, 0.7f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0 / 7.0f, 0.8f, 0.8f)));
            ImGui::SameLine();
            if (ImGui::Button("Remove")) {
                removeIndex = i;
            }
            ImGui::PopStyleColor(3);
            ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(1 / 7.0f, 0.6f, 0.6f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(1 / 7.0f, 0.7f, 0.7f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(1 / 7.0f, 0.8f, 0.8f)));
            ImGui::SameLine();
            if (ImGui::Button("Hide")) {
                m_activeLights[i] = !m_activeLights[i];
                if (m_activeLights[i]) {
                    m_activeLightsCount++;
                } else {
                    m_activeLightsCount--;
                }
            }
            ImGui::PopStyleColor(3);
            if (treeNode) {
                light->widgets();
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        if (removeIndex != -1) {
            m_lights.erase(m_lights.begin() + removeIndex);
            m_activeLights.erase(m_activeLights.begin() + removeIndex);
        }

        if (ImGui::TreeNode("Flashlight")) {
            ImGui::Checkbox("Toggle", &m_flashLightOn);
            m_flashlight.widgets();
            ImGui::TreePop();
        }
    }
}

void LightManager::add(std::unique_ptr<Light> light) {
    m_lights.push_back(std::move(light));
    m_activeLights.push_back(true);
    m_activeLightsCount++;
}

void LightManager::update(const Camera *const camera) {
    m_flashlight.setDirection(camera->getFront());
    m_flashlight.setPosition(camera->getPosition());
}

void LightManager::setShaderUniforms(const Shader *shader) const {
    for (int i = 0; auto &light: m_lights) {
        if (!m_activeLights[i]) continue; // skip inactive lights
        const auto name = fmt::format("lights[{}]", i++);
        light.get()->setShaderUniforms(shader, name);
    }
    auto size = m_activeLightsCount;
    if (m_flashLightOn) {
        const auto name = fmt::format("lights[{}]", size++);
        m_flashlight.setShaderUniforms(shader, name);
    }
    shader->setInt("lightCount", size);
}

void LightManager::draw(const Shader *const shader) const {
    glBindVertexArray(m_lightVao);
    for (auto i = 0; i < m_lights.size(); ++i) {
        if (!m_activeLights[i]) continue;
        m_lights[i]->draw(shader);
    }
}

#include <fmt/format.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "Light.h"
#include "Shader.h"

#include <array>
#include <iostream>

Light::Light(const glm::vec3 ambient, const glm::vec3 diffuse, const glm::vec3 specular)
    : m_ambient{ambient}, m_diffuse{diffuse}, m_specular{specular} {
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

Light::Type DirectionalLight::getType() const {
    return Type::Directional;
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

Light::Type PointLight::getType() const {
    return Type::Point;
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

Light::Type SpotLight::getType() const {
    return Type::Spot;
}

void attenuationWidgets(float &c, float &l, float &q) {
    const auto text = fmt::format("Attenuation: c: {}, l: {}, q: {}", c, l, q);
    ImGui::Text(text.c_str());
}

LightManager::LightManager(): m_flashlight{SpotLight{glm::vec3(0.0f), glm::vec3(0.0f, 0.0f, -1.0f)}},
                              m_selectedLight{0} {
}

void LightManager::widgets() {
    if (ImGui::CollapsingHeader("Lights")) {
        ImGui::SeparatorText("Add a new light");
        constexpr std::array lightTypes = {"Directional", "Point", "Spot"};
        ImGui::Combo("Type", &m_selectedLight, lightTypes.data(), lightTypes.size());
        ImGui::SameLine();
        if (ImGui::SmallButton("Add")) {
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

        ImGui::SeparatorText("Active lights");
        int removeIndex = -1;
        for (auto i = 0; i < m_lights.size(); ++i) {
            const auto light = m_lights[i].get();
            ImGui::PushID(i);
            auto open = ImGui::TreeNode(fmt::format("Light #{} ({})", i, Light::getTypeStr(light->getType())).c_str());
            ImGui::PushStyleColor(ImGuiCol_Button, static_cast<ImVec4>(ImColor::HSV(0 / 7.0f, 0.6f, 0.6f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonHovered, static_cast<ImVec4>(ImColor::HSV(0 / 7.0f, 0.7f, 0.7f)));
            ImGui::PushStyleColor(ImGuiCol_ButtonActive, static_cast<ImVec4>(ImColor::HSV(0 / 7.0f, 0.8f, 0.8f)));
            ImGui::PopStyleColor(3);
            ImGui::SameLine();
            if (ImGui::Button("Remove")) {
                removeIndex = i;
            }
            if (open) {
                light->widgets();
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        if (removeIndex != -1) {
            m_lights.erase(m_lights.begin() + removeIndex);
            std::cout << "Removing light at index: " << removeIndex << '\n';
        }

        if (ImGui::TreeNode("Flashlight")) {
            m_flashlight.widgets();
            ImGui::TreePop();
        }
    }
}

void LightManager::add(std::unique_ptr<Light> light) {
    m_lights.push_back(std::move(light));
}

void LightManager::update(const Camera *const camera) {
    m_flashlight.setDirection(camera->getFront());
    m_flashlight.setPosition(camera->getPosition());
}

void LightManager::setShaderUniforms(const Shader *shader) const {
    for (const auto &light: m_lights) {
        light->setShaderUniforms(shader);
    }
    m_flashlight.setShaderUniforms(shader);
}

void LightManager::remove(auto index) {
    m_lights.erase(index);
}

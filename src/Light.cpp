#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "Light.h"

void PointLight::widgets() {
    ImGui::SliderFloat3("Position", glm::value_ptr(position), -10.0f, 10.0f);
    ImGui::ColorEdit3("Light ambient", glm::value_ptr(ambient));
    ImGui::ColorEdit3("Light diffuse", glm::value_ptr(diffuse));
    ImGui::ColorEdit3("Light specular", glm::value_ptr(specular));
}

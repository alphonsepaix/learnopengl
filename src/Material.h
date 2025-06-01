#ifndef MATERIAL_H
#define MATERIAL_H

#include <fmt/format.h>
#include <imgui.h>

#include "Texture.h"

struct Material {
    Texture diffuse;
    Texture specular;
    int shininess;

    void widgets() {
        ImGui::SliderInt("Shininess", &shininess, 1, 8);
        ImGui::SameLine();
        const auto value = static_cast<float>(std::pow(2, shininess));
        const auto text = fmt::format("({})", value);
        ImGui::Text("%s", text.c_str());
    }

    [[nodiscard]] float getShininess() const {
        return static_cast<float>(std::pow(2, shininess));
    }
};

#endif

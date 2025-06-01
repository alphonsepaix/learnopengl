#include <fmt/format.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#define DBG_MACRO_NO_WARNING
#include <dbg.h>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Shader.h"
#include "Camera.h"
#include "Light.h"
#include "Material.h"
#include "Texture.h"
#include "events.h"
#include "globals.h"
#include "Model.h"
#include "Window.h"

#include <array>
#include <iostream>
#include <tuple>

int WINDOW_WIDTH = 1600;
int WINDOW_HEIGHT = 900;

const std::string SHADER_DIR = "assets/shaders/";
const std::string TEXTURE_DIR = "assets/textures/";
const std::string MODELS_DIR = "assets/models/";

float deltaTime = 0.0f;
float deltaTimeAdded = 0.0f;
float lastFrame = 0.0f;
std::string performanceStr = "Starting...";

bool firstMouse = true;
double lastX = WINDOW_WIDTH / 2.0;
double lastY = WINDOW_HEIGHT / 2.0;

bool cursorLocked = true;
bool cursorJustUnlocked = false;

float fov = 45.0f;

int main() {
    // auto alone will strip the reference, giving errors (we deleted the copy and assignment constructors).
    auto &cameraManager = CameraManager::getInstance();

    auto window = Window{WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_TITLE};

    // Setup Dear ImGui context and backends.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;;
    ImGui_ImplGlfw_InitForOpenGL(window.getHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    // clang-format off
    constexpr std::array vertices = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };
    // clang-format on

    GLuint cubeVao;
    glGenVertexArrays(1, &cubeVao);
    glBindVertexArray(cubeVao);
    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float),
                 vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          static_cast<GLvoid *>(nullptr));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<GLvoid *>(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          reinterpret_cast<GLvoid *>(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLuint lightVao;
    glGenVertexArrays(1, &lightVao);
    glBindVertexArray(lightVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          static_cast<GLvoid *>(nullptr));
    glEnableVertexAttribArray(0);

    // Unbind everything.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    const auto objectShader = Shader(SHADER_DIR + "object.vert", SHADER_DIR + "object.frag");
    const auto lightShader = Shader(SHADER_DIR + "light.vert", SHADER_DIR + "light.frag");

    bool isPaused = true;
    bool emissionOn = false;
    auto backgroundColor = glm::vec3(0.0f);
    auto wireframe = false;

    auto containerDiffuseMap = Texture{TEXTURE_DIR + "container.png"};
    auto containerSpecularMap = Texture{TEXTURE_DIR + "container_specular.png"};
    auto containerEmissionMap = Texture(TEXTURE_DIR + "matrix.jpg");
    auto cubeGlobalScale = 1.0f;
    auto cubeRotationAngle = 0.0f; // in degrees
    auto cubeTranslation = glm::vec3(0.0f);
    auto objectMaterial = Material{containerDiffuseMap, containerSpecularMap, 7};

    LightManager lightManager;
    lightManager.add(std::make_unique<DirectionalLight>(glm::vec3(-1.0f)));
    lightManager.add(std::make_unique<PointLight>(glm::vec3(1.0f, 2.0f, -2.0f)));

    auto cubes = std::to_array<std::tuple<glm::vec3, float> >({
        {glm::vec3(0.0f, 0.0f, 0.0f), 1.2f},
        {glm::vec3(2.0f, 5.0f, -15.0f), 0.8f},
        {glm::vec3(-1.5f, -2.2f, -2.5f), 1.0f},
        {glm::vec3(-3.8f, -2.0f, -12.3f), 1.1f},
        {glm::vec3(2.4f, -0.4f, -3.5f), 1.3f},
        {glm::vec3(-1.7f, 3.0f, -7.5f), 1.4f},
        {glm::vec3(1.3f, -2.0f, -2.5f), 0.7f},
        {glm::vec3(1.5f, 2.0f, -2.5f), 0.6f},
        {glm::vec3(1.5f, 0.2f, -1.5f), 0.9f},
        {glm::vec3(-1.3f, 1.0f, -1.5f), 1.0f},
    });

    // Load the backpack model.
    dbg("Loading model...");
    auto path = MODELS_DIR + "backpack/backpack.obj";
    auto backpack = Model{path};
    dbg("Model loaded: {}", path);
    auto backpackModel = glm::mat4(1.0f);
    backpackModel = glm::translate(backpackModel, glm::vec3(-6.0f, -2.0f, 1.0f));
    backpackModel = glm::rotate(backpackModel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    auto backpackNormalMatrix = glm::mat3(glm::transpose(glm::inverse(backpackModel)));

    while (!window.shouldClose()) {
        const auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        deltaTimeAdded += deltaTime;

        processInput(window.getHandle());

        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0); // state-setting
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // state-using

        // Start the Dear ImGui frame.
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::Begin("Performances");
        if (deltaTimeAdded > 1.0f) {
            std::stringstream ss;
            deltaTimeAdded -= 1.0f;
            auto fps = static_cast<int>(1 / deltaTime);
            ss << "Application average: " << deltaTime * 1000 << "ms/frame (" << fps << " FPS)\n";
            performanceStr = ss.str();
        }
        ImGui::Text("%s", performanceStr.c_str());
        ImGui::End();

        ImGui::Begin("Settings");

        if (ImGui::CollapsingHeader("Window")) {
            auto s = fmt::format("Window size: ({}, {})", WINDOW_WIDTH, WINDOW_HEIGHT);
            ImGui::Text("%s", s.c_str());
            ImGui::ColorPicker3("##Background", glm::value_ptr(backgroundColor));
        }

        if (ImGui::CollapsingHeader("Objects")) {
            ImGui::Checkbox("Wireframe", &wireframe);
            ImGui::SliderFloat3("Offset", glm::value_ptr(cubeTranslation), -10.0f, 10.0f);
            ImGui::SliderFloat("Scale", &cubeGlobalScale, 0.0f, 10.0f);
            ImGui::Checkbox("Pause rotation", &isPaused);
            ImGui::Checkbox("Emission map", &emissionOn);
            objectMaterial.widgets();
        }

        lightManager.widgets();

        cameraManager.widgets();

        ImGui::End();

        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        auto view = cameraManager.getActiveCamera()->lookAt();
        auto projection = glm::perspective(glm::radians(fov),
                                           static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f,
                                           100.0f);

        // Render the light sources.
        lightShader.use();
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        glBindVertexArray(lightVao);
        lightManager.draw(&lightShader);

        // Render the cubes.
        objectShader.use();
        objectShader.setInt("material.texture_diffuse1", 0);
        objectShader.setInt("material.texture_specular1", 1);
        objectShader.setInt("material.emission", 2);
        containerDiffuseMap.setUnit(0);
        containerSpecularMap.setUnit(1);
        containerEmissionMap.setUnit(2);
        objectShader.use();
        objectShader.setFloat("material.shininess", objectMaterial.getShininess());
        objectShader.setBool("emissionOn", emissionOn);
        lightManager.update(cameraManager.getActiveCamera());
        lightManager.setShaderUniforms(&objectShader);
        auto viewPos = cameraManager.getActiveCamera()->getPosition();
        objectShader.setVec3("viewPos", viewPos);
        for (std::size_t i = 0; const auto &[cubeCenter, cubeScale]: cubes) {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, cubeCenter);
            model = glm::translate(model, cubeTranslation);
            glm::vec3 rotAxis;
            switch (i % 3) {
                case 0:
                    rotAxis = glm::vec3(1.0f, 0.0f, 0.0f);
                    break;
                case 1:
                    rotAxis = glm::vec3(0.0f, 1.0f, 0.0f);
                    break;
                default:
                    rotAxis = glm::vec3(0.0f, 0.0f, 1.0f);
                    break;
            }
            if (!isPaused)
                cubeRotationAngle += 1.0f;
            auto speed = 0.1f;
            model = glm::rotate(model, glm::radians(cubeRotationAngle) * speed,
                                rotAxis);
            model = glm::scale(model, glm::vec3(cubeGlobalScale));

            auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));

            objectShader.setMat3("normalMatrix", normalMatrix);
            objectShader.setMat4("model", model);
            objectShader.setMat4("view", view);
            objectShader.setMat4("projection", projection);
            glBindVertexArray(cubeVao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            ++i;
        }

        // Render the backpack model.
        objectShader.setMat3("normalMatrix", backpackNormalMatrix);
        objectShader.setMat4("model", backpackModel);
        backpack.draw(objectShader);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        window.swapBuffers();
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    return 0;
}

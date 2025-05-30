#include <fmt/format.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
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

#include <array>
#include <iostream>
#include <tuple>

int main() {
    // auto alone will strip the reference, giving errors (we deleted the copy and assignment constructors).
    auto &instance = CameraManager::getInstance();

    // Create a GLFW window.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    // Tell GLFW we want to use the core-profile (a smaller and more modern subset of OpenGL).
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *window = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT,
                                          WINDOW_TITLE, nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    // Make the context of the window the main context on the current thread.
    glfwMakeContextCurrent(window);

    // Initialize GLAD.
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD\n";
        return -1;
    }

    // Tell OpenGL the size of our rendering window so it can display data and coordinates correctly.
    glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetScrollCallback(window, scrollCallback);
    glEnable(GL_DEPTH_TEST);

    // Setup Dear ImGui context and backends.
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui_ImplGlfw_InitForOpenGL(window, true);
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

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
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

    const auto cubeShader = Shader(SHADER_DIR + "cube.vert", SHADER_DIR + "cube.frag");
    cubeShader.use();
    cubeShader.setInt("material.diffuse", 0);
    cubeShader.setInt("material.specular", 1);
    cubeShader.setInt("material.emission", 2);
    // const auto lightShader = Shader(SHADER_DIR + "light.vert", SHADER_DIR + "light.frag");

    bool isPaused = true;
    bool emissionOn = false;
    auto backgroundColor = glm::vec3(0.0f);
    auto wireframe = false;

    auto containerDiffuseMap = Texture{TEXTURE_DIR + "container.png"};
    auto containerSpecularMap = Texture{TEXTURE_DIR + "container_specular.png"};
    auto containerEmissionMap = Texture(TEXTURE_DIR + "matrix.jpg");
    containerDiffuseMap.setUnit(0);
    containerSpecularMap.setUnit(1);
    containerEmissionMap.setUnit(2);
    auto cubeGlobalScale = 1.0f;
    auto cubeRotationAngle = 0.0f; // in degrees
    auto cubeTranslation = glm::vec3(0.0f);
    auto objectMaterial = Material{containerDiffuseMap, containerSpecularMap, 7};

    // auto light = DirectionalLight{
    //     glm::vec3(-0.2f, -1.0f, -0.3f),
    //     glm::vec3(0.1f),
    //     glm::vec3(1.0f),
    //     glm::vec3(1.0f),
    // };

    std::vector<std::unique_ptr<Light> > lights;
    lights.push_back(std::make_unique<DirectionalLight>(glm::vec3(-1.0f)));
    lights.push_back(std::make_unique<PointLight>(glm::vec3(1.0f, 2.0f, -2.0f)));
    lights.push_back(std::make_unique<SpotLight>(
        instance.getActiveCamera()->getPosition(),
        instance.getActiveCamera()->getFront()
    ));

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

    while (!glfwWindowShouldClose(window)) {
        const auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        deltaTimeAdded += deltaTime;

        processInput(window);

        glClearColor(backgroundColor.r, backgroundColor.g, backgroundColor.b, 1.0); // state-setting
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // state-using

        // Start the Dear ImGui frame.
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        if (deltaTimeAdded > 1.0f) {
            std::stringstream ss;
            deltaTimeAdded -= 1.0f;
            auto fps = static_cast<int>(1 / deltaTime);
            ss << "Application average: " << deltaTime * 1000 << "ms/frame (" << fps << " FPS)\n";
            performanceStr = ss.str();
        }
        ImGui::Text(performanceStr.c_str());

        ImGui::SeparatorText("Window");
        ImGui::Text(("Width: " + std::to_string(WINDOW_WIDTH)).c_str());
        ImGui::SameLine();
        ImGui::Text(("Height: " + std::to_string(WINDOW_HEIGHT)).c_str());
        ImGui::ColorEdit3("Background", glm::value_ptr(backgroundColor));

        // Objects
        ImGui::SeparatorText("Cubes");
        ImGui::Checkbox("Wireframe", &wireframe);
        ImGui::SliderFloat3("Offset", glm::value_ptr(cubeTranslation), -10.0f, 10.0f);
        ImGui::SliderFloat("Scale", &cubeGlobalScale, 0.0f, 10.0f);
        ImGui::Checkbox("Pause rotation", &isPaused);
        ImGui::Checkbox("Emission map", &emissionOn);
        objectMaterial.widgets();

        // Light
        ImGui::SeparatorText("Lights");
        for (auto &light: lights)
            light.get()->widgets();

        ImGui::SeparatorText("Camera");
        instance.widgets(); // switch camera
        instance.getActiveCamera()->widgets(); // camera widget

        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        auto view = instance.getActiveCamera()->lookAt();
        auto projection = glm::perspective(glm::radians(fov),
                                           static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f,
                                           100.0f);

        // auto lightModel = glm::mat4(1.0f);
        // lightModel = glm::translate(lightModel, light.position);
        // lightModel = glm::scale(lightModel, glm::vec3(0.2f));

        // Render the light sources.
        // lightShader.use();
        // lightShader.setMat4("model", lightModel);
        // lightShader.setMat4("view", view);
        // lightShader.setMat4("projection", projection);
        // lightShader.setVec3("lightColor", light.diffuse);
        // glBindVertexArray(lightVao);
        // glDrawArrays(GL_TRIANGLES, 0, 36);

        // Render the cubes.
        cubeShader.use();
        cubeShader.setFloat("material.shininess", objectMaterial.getShininess());
        cubeShader.setBool("emissionOn", emissionOn);
        for (const auto &light: lights)
            cubeShader.setLight(light.get());
        auto viewPos = instance.getActiveCamera()->getPosition();
        cubeShader.setVec3("viewPos", viewPos);
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

            cubeShader.setMat3("normalMatrix", normalMatrix);
            cubeShader.setMat4("model", model);
            cubeShader.setMat4("view", view);
            cubeShader.setMat4("projection", projection);
            glBindVertexArray(vao);
            glDrawArrays(GL_TRIANGLES, 0, 36);
            ++i;
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window); // double buffering
        glfwPollEvents();
    }

    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    glfwTerminate();
    return 0;
}

#include <dbg.h>
#include <fmt/format.h>
#include <glm//gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Application.h"

const std::string SHADER_DIR = "assets/shaders/";

constexpr auto UNLOCK_KEY = GLFW_KEY_LEFT_SHIFT;
constexpr auto FORWARD_KEY = GLFW_KEY_W;
constexpr auto BACKWARD_KEY = GLFW_KEY_S;
constexpr auto LEFT_KEY = GLFW_KEY_A;
constexpr auto RIGHT_KEY = GLFW_KEY_D;
constexpr auto UP_KEY = GLFW_KEY_SPACE;
constexpr auto DOWN_KEY = GLFW_KEY_LEFT_CONTROL;
constexpr auto EXIT_KEY = GLFW_KEY_ESCAPE;

Application::Application(): m_window{this}, m_cameraManager{CameraManager::getInstance()} {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;;
    ImGui_ImplGlfw_InitForOpenGL(m_window.getHandle(), true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    const auto objectShader = Shader(SHADER_DIR + "object.vert", SHADER_DIR + "object.frag");
    const auto lightShader = Shader(SHADER_DIR + "light.vert", SHADER_DIR + "light.frag");
    m_shaders["object"] = std::make_unique<Shader>(std::move(objectShader));
    m_shaders["light"] = std::make_unique<Shader>(std::move(lightShader));

    m_lightManager.add(std::make_unique<DirectionalLight>(glm::vec3(-1.0f)));
    m_lightManager.add(std::make_unique<PointLight>(glm::vec3(1.0f, 2.0f, -2.0f)));
}

Application::~Application() {
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
}

void Application::mainLoop() {
    const auto currentFrame = static_cast<float>(glfwGetTime());
    m_state.deltaTime = currentFrame - m_state.lastFrame;
    m_state.lastFrame = currentFrame;
    m_state.deltaTimeAdded += m_state.deltaTime;

    processInput();

    const auto &bgColor = m_window.getBgColor();
    glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    widgets();

    if (m_state.wireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    const auto viewPos = m_cameraManager->getActiveCamera()->getPosition();
    const auto view = m_cameraManager->getActiveCamera()->lookAt();
    const auto projection = glm::perspective(glm::radians(m_cameraManager->getFov()),
                                             static_cast<float>(m_window.getWidth()) / static_cast<float>(m_window.
                                                 getHeight()), 0.1f,
                                             100.0f);

    // Render the light sources.
    const auto &lightShader = m_shaders["light"];
    lightShader->use();
    lightShader->setMat4("view", view);
    lightShader->setMat4("projection", projection);
    m_lightManager.update(m_cameraManager->getActiveCamera());
    m_lightManager.draw(lightShader.get());

    // Draw the objects.
    const auto &objectShader = m_shaders["object"];
    objectShader->use();
    objectShader->setFloat("material.shininess", 32);
    objectShader->setVec3("viewPos", viewPos);
    objectShader->setBool("emission", m_state.emission);
    objectShader->setMat4("view", view);
    objectShader->setMat4("projection", projection);
    m_lightManager.setShaderUniforms(objectShader.get());
    m_modelManager.draw(objectShader.get());

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
    m_window.swapBuffers();
    glfwPollEvents();
}

bool Application::isRunning() const {
    return !m_window.shouldClose();
}

void Application::updateFov(const float yOffset) const {
    m_cameraManager->updateFov(yOffset);
}

void Application::resize(const int width, const int height) {
    m_window.resize(width, height);
}

void Application::widgets() {
    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("Performances");
    if (m_state.deltaTimeAdded > 1.0f) {
        m_state.deltaTimeAdded -= 1.0f;
        auto fps = static_cast<int>(1 / m_state.deltaTime);
        m_state.performanceStr = fmt::format("Application average: {:.2f} ms/frame ({:d} FPS)",
                                             m_state.deltaTime * 1000, fps);
    }
    ImGui::Text("%s", m_state.performanceStr.c_str());
    ImGui::End();

    ImGui::Begin("Settings");
    m_window.widgets();
    m_cameraManager->widgets();
    m_modelManager.widgets();
    m_lightManager.widgets();
    ImGui::End();
}

void Application::processInput() {
    if (m_window.getKey(EXIT_KEY) == GLFW_PRESS) {
        m_window.setShouldClose(true);
    }

    if (m_window.getKey(UNLOCK_KEY) == GLFW_PRESS && m_state.cursorLocked) {
        m_window.setInputMode(GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        m_state.cursorLocked = false;
    }
    if (m_window.getKey(UNLOCK_KEY) == GLFW_RELEASE && !m_state.cursorLocked) {
        m_window.setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        m_state.cursorLocked = m_state.cursorJustUnlocked = true;
    }

    const auto &deltaTime = m_state.deltaTime;
    const auto camera = CameraManager::getInstance()->getActiveCamera();
    if (m_window.getKey(FORWARD_KEY) == GLFW_PRESS)
        camera->move(Camera::Direction::Forward, deltaTime);
    if (m_window.getKey(BACKWARD_KEY) == GLFW_PRESS)
        camera->move(Camera::Direction::Backward, deltaTime);
    if (m_window.getKey(LEFT_KEY) == GLFW_PRESS)
        camera->move(Camera::Direction::Left, deltaTime);
    if (m_window.getKey(RIGHT_KEY) == GLFW_PRESS)
        camera->move(Camera::Direction::Right, deltaTime);
    if (m_window.getKey(UP_KEY) == GLFW_PRESS)
        camera->move(Camera::Direction::Up, deltaTime);
    if (m_window.getKey(DOWN_KEY) == GLFW_PRESS)
        camera->move(Camera::Direction::Down, deltaTime);
}

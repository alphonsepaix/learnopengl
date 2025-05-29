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

#include <array>
#include <iostream>
#include <tuple>

#include "Texture.h"

auto WINDOW_WIDTH = 1600;
auto WINDOW_HEIGHT = 900;
constexpr auto WINDOW_TITLE = "Learn OpenGL";

const std::string SHADER_DIR = "assets/shaders/";
const std::string TEXTURE_DIR = "assets/textures/";

float deltaTime = 0.0f;
float deltaTimeAdded = 0.0f;
float lastFrame = 0.0f;
std::string performanceStr = "Starting...";

bool firstMouse = true;
auto lastX = WINDOW_WIDTH / 2.0;
auto lastY = WINDOW_HEIGHT / 2.0;

bool cursorLocked = true;
bool cursorJustUnlocked = false;

float fov = 45.0f;

constexpr auto UNLOCK_KEY = GLFW_KEY_LEFT_SHIFT;
constexpr auto FORWARD_KEY = GLFW_KEY_W;
constexpr auto BACKWARD_KEY = GLFW_KEY_S;
constexpr auto LEFT_KEY = GLFW_KEY_A;
constexpr auto RIGHT_KEY = GLFW_KEY_D;
constexpr auto UP_KEY = GLFW_KEY_SPACE;
constexpr auto DOWN_KEY = GLFW_KEY_LEFT_CONTROL;

void framebufferSizeCallback(GLFWwindow *window, int width, int height);

void mouseCallback(GLFWwindow *window, double posX, double posY);

void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

void processInput(GLFWwindow *window);

struct Material {
    Texture diffuse;
    glm::vec3 specular;
    int shininessPower;
    float shininess;

    void widgets() {
        ImGui::ColorEdit3("Object specular", glm::value_ptr(specular));
        ImGui::SliderInt("Shininess", &shininessPower, 1, 8);
        ImGui::SameLine();
        shininess = static_cast<float>(std::pow(2, shininessPower));
        ImGui::Text(std::to_string(static_cast<int>(shininess)).c_str());
    }
};

struct Light {
    glm::vec3 position;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    void widgets() {
        ImGui::SliderFloat3("Position", glm::value_ptr(position), -10.0f, 10.0f);
        ImGui::ColorEdit3("Light ambient", glm::value_ptr(ambient));
        ImGui::ColorEdit3("Light diffuse", glm::value_ptr(diffuse));
        ImGui::ColorEdit3("Light specular", glm::value_ptr(specular));
    }
};

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
    const auto lightShader = Shader(SHADER_DIR + "light.vert", SHADER_DIR + "light.frag");

    bool isPaused = false;
    auto backgroundColor = glm::vec3(0.0f);
    auto wireframe = false;

    auto containerTex = Texture{TEXTURE_DIR + "container.png"};
    containerTex.setFilter(Texture::Filter::Linear, Texture::Filter::Linear);
    containerTex.setWrap(Texture::Wrap::Repeat, Texture::Wrap::Repeat);
    containerTex.setUnit(0);
    auto cubeGlobalScale = 1.0f;
    auto cubeRotationAngle = 0.0f; // in degrees
    auto cubeTranslation = glm::vec3(0.0f);
    auto objectMaterial = Material{containerTex, glm::vec3(0.5f), 7, 128};

    auto light = Light{
        glm::vec3(0.0f, 0.0, -2.0f),
        glm::vec3(0.2f),
        glm::vec3(0.5f),
        glm::vec3(1.0f),
    };

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
        objectMaterial.widgets();

        // Light
        ImGui::SeparatorText("Light");
        light.widgets();

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

        auto lightModel = glm::mat4(1.0f);
        lightModel = glm::translate(lightModel, light.position);
        lightModel = glm::scale(lightModel, glm::vec3(0.2f));

        // Render the light source.
        lightShader.use();
        lightShader.setMat4("model", lightModel);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("lightColor", light.diffuse);
        glBindVertexArray(lightVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Render the cubes.
        cubeShader.use();
        cubeShader.setInt("material.diffuse", 0);
        cubeShader.setVec3("material.specular", objectMaterial.specular);
        cubeShader.setFloat("material.shininess", objectMaterial.shininess);
        cubeShader.setVec3("light.ambient", light.ambient);
        cubeShader.setVec3("light.diffuse", light.diffuse);
        cubeShader.setVec3("light.specular", light.specular);
        cubeShader.setVec3("light.position", light.position);
        cubeShader.setVec3("viewPos", instance.getActiveCamera()->getPosition());
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

void framebufferSizeCallback(GLFWwindow *, const int width, const int height) {
    glViewport(0, 0, width, height);
    WINDOW_WIDTH = width;
    WINDOW_HEIGHT = height;
}

void mouseCallback(GLFWwindow *window, double posX, double posY) {
    if (!cursorLocked)
        return;
    if (firstMouse || cursorJustUnlocked) {
        lastX = posX;
        lastY = posY;
        firstMouse = false;
        cursorJustUnlocked = false;
    }
    const auto xOffset = posX - lastX;
    const auto yOffset = posY - lastY;
    lastX = posX;
    lastY = posY;
    CameraManager::getInstance().getActiveCamera()->mouseUpdate(
        static_cast<float>(xOffset),
        static_cast<float>(yOffset));
}

void scrollCallback(GLFWwindow *window, double, double yOffset) {
    fov -= static_cast<float>(yOffset);
    if (fov < 1.0f)
        fov = 1.0f;
    if (fov > 45.0f)
        fov = 45.0f;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // Unlock the window if LEFT_CTRL is pressed.
    if (glfwGetKey(window, UNLOCK_KEY) == GLFW_PRESS && cursorLocked) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        cursorLocked = false;
    }
    if (glfwGetKey(window, UNLOCK_KEY) == GLFW_RELEASE && !cursorLocked) {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        cursorLocked = cursorJustUnlocked = true;
    }

    // Camera movement.
    const auto camera = CameraManager::getInstance().getActiveCamera();
    if (glfwGetKey(window, FORWARD_KEY) == GLFW_PRESS)
        camera->move(Camera::Direction::Forward, deltaTime);
    if (glfwGetKey(window, BACKWARD_KEY) == GLFW_PRESS)
        camera->move(Camera::Direction::Backward, deltaTime);
    if (glfwGetKey(window, LEFT_KEY) == GLFW_PRESS)
        camera->move(Camera::Direction::Left, deltaTime);
    if (glfwGetKey(window, RIGHT_KEY) == GLFW_PRESS)
        camera->move(Camera::Direction::Right, deltaTime);
    if (glfwGetKey(window, UP_KEY) == GLFW_PRESS)
        camera->move(Camera::Direction::Up, deltaTime);
    if (glfwGetKey(window, DOWN_KEY) == GLFW_PRESS)
        camera->move(Camera::Direction::Down, deltaTime);
}

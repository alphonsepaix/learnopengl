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
#include "stb_image.h"

#include "Shader.h"
#include "Camera.h"
#include "Texture.h"

#include <array>
#include <iostream>
#include <tuple>

auto WINDOW_WIDTH = 1600;
auto WINDOW_HEIGHT = 900;
constexpr auto WINDOW_TITLE = "Learn OpenGL";

float deltaTime = 0.0f;
float lastFrame = 0.0f;

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

    constexpr std::array vertices = {
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,

        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        -0.5f, -0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, -0.5f, 0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f, 0.0f, -1.0f, 0.0f, 0.0f, 1.0f,

        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        -0.5f, 0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        -0.5f, 0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f
    };

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
                          reinterpret_cast<GLvoid *>(3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    GLuint lightVao;
    glGenVertexArrays(1, &lightVao);
    glBindVertexArray(lightVao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
                          static_cast<GLvoid *>(nullptr));
    glEnableVertexAttribArray(0);
    auto lightPos = glm::vec3(0.0f, 0.0f, -2.0f);

    // Unbind everything.
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    stbi_set_flip_vertically_on_load(true);

    const auto containerTexture = Texture("assets/textures/container.jpg",
                                          Texture::Type::Texture2D,
                                          Texture::Format::RGB);
    containerTexture.bind();
    containerTexture.setWrap(Texture::Wrap::Repeat, Texture::Wrap::Repeat);
    containerTexture.setFilter(Texture::Filter::LinearMipmapLinear,
                               Texture::Filter::Linear);

    const auto faceTexture = Texture("assets/textures/face.png",
                                     Texture::Type::Texture2D,
                                     Texture::Format::RGBA);
    faceTexture.bind();
    faceTexture.setWrap(Texture::Wrap::ClampToEdge,
                        Texture::Wrap::MirroredRepeat);
    faceTexture.setFilter(Texture::Filter::LinearMipmapLinear,
                          Texture::Filter::Linear);
    constexpr auto borderColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    faceTexture.setBorderColor(glm::value_ptr(borderColor));

    glBindTexture(GL_TEXTURE_2D, 0);

    auto lightColor = glm::vec3(0.0f, 1.0f, 1.0f);
    auto objectColor = glm::vec3(0.0f, 1.0f, 0.0f);

    const auto cubeShader = Shader("assets/shaders/cube.vert",
                                   "assets/shaders/cube.frag");
    cubeShader.use();
    cubeShader.setInt("container", 0);
    cubeShader.setInt("face", 1);

    const auto lightShader = Shader("assets/shaders/light.vert", "assets/shaders/light.frag");

    auto wireframe = false;
    auto mirrorX = false;
    auto mirrorY = false;
    auto bgColor = glm::vec3(0.0f);
    auto offsetX = 0.0f;
    auto offsetY = 0.0f;
    auto offsetZ = 0.0f;
    auto scale = 1.0f;
    auto mixValue = 0.5f;
    bool paused = false;
    auto angleCube = 0.0f; // degrees
    auto shininess = 128; // 2^7 = 128 by default
    auto shininessPower = 7;
    auto ambientStrength = 0.1f;
    auto specularStrength = 0.5f;

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
        processInput(window);

        // Start the Dear ImGui frame.
        ImGui_ImplGlfw_NewFrame();
        ImGui_ImplOpenGL3_NewFrame();
        ImGui::NewFrame();

        ImGui::SeparatorText("Window");
        ImGui::Text(("width: " + std::to_string(WINDOW_WIDTH)).c_str());
        ImGui::Text(("height: " + std::to_string(WINDOW_HEIGHT)).c_str());
        ImGui::ColorEdit3("##ColorPicker#1", glm::value_ptr(bgColor));

        // Objects
        ImGui::SeparatorText("Objects");
        ImGui::Checkbox("wireframe", &wireframe);
        ImGui::Checkbox("mirror x", &mirrorX);
        ImGui::SameLine();
        ImGui::Checkbox("mirror y", &mirrorY);
        ImGui::SliderFloat("offset x", &offsetX, -10.0f, 10.0f);
        ImGui::SliderFloat("offset y", &offsetY, -10.0f, 10.0f);
        ImGui::SliderFloat("offset z", &offsetZ, -10.0f, 10.0f);
        ImGui::SliderFloat("scale", &scale, 0.0f, 10.0f);
        ImGui::SliderFloat("mix", &mixValue, 0.0f, 1.0f);
        ImGui::Checkbox("rotation", &paused);
        ImGui::SliderInt("shininess", &shininessPower, 1, 8);
        ImGui::SameLine();
        shininess = static_cast<int>(std::pow(2, shininessPower));
        ImGui::Text(std::to_string(shininess).c_str());
        ImGui::ColorEdit3("##ColorPicker#2", glm::value_ptr(objectColor));

        // Light
        ImGui::SeparatorText("Light");
        ImGui::ColorEdit3("##ColorPicker#3", glm::value_ptr(lightColor));
        ImGui::SliderFloat("ambient", &ambientStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("specular", &specularStrength, 0.0f, 1.0f);

        instance.getActiveCamera()->renderWidgets(); // camera widget
        instance.renderWidgets(); // switch camera

        ImGui::SeparatorText("Other");
        ImGui::Text(("fov: " + std::to_string(fov)).c_str());

        glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0); // state-setting
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // state-using

        if (wireframe) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        } else {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        containerTexture.setUnit(0);
        faceTexture.setUnit(1);

        auto view = instance.getActiveCamera()->lookAt();
        auto projection = glm::perspective(glm::radians(fov),
                                           static_cast<float>(WINDOW_WIDTH) / static_cast<float>(WINDOW_HEIGHT), 0.1f,
                                           100.0f);

        auto lightModel = glm::mat4(1.0f);
        auto lightRadius = 5.0f;
        lightPos = glm::vec3(lightRadius * std::sin(lastFrame),
                             lightRadius * std::cos(lastFrame), -2.0f);
        lightModel = glm::translate(lightModel, lightPos);
        lightModel = glm::scale(lightModel, glm::vec3(0.2f));

        // Render the light source.
        lightShader.use();
        lightShader.setMat4("model", lightModel);
        lightShader.setMat4("view", view);
        lightShader.setMat4("projection", projection);
        lightShader.setVec3("lightColor", lightColor);
        glBindVertexArray(lightVao);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        // Render the cubes;
        cubeShader.use();
        cubeShader.setInt("shininess", shininess);
        cubeShader.setFloat("mixValue", mixValue);
        cubeShader.setFloat("ambientStrength", ambientStrength);
        cubeShader.setFloat("specularStrength", specularStrength);
        cubeShader.setVec3("objectColor", objectColor);
        cubeShader.setVec3("lightColor", lightColor);
        // cubeShader.setVec3("lightPos", lightPos);
        cubeShader.setVec3("lightPos", view * glm::vec4(lightPos, 1.0f)); // in view space
        cubeShader.setVec3("viewPos", instance.getActiveCamera()->getPosition());
        for (std::size_t i = 0; const auto &[cubeCenter, cubeScale]: cubes) {
            auto model = glm::mat4(1.0f);
            model = glm::translate(model, cubeCenter);
            model = glm::translate(model, glm::vec3(offsetX, offsetY, offsetZ));
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
            if (!paused)
                angleCube += 1.0f;
            auto speed = 0.1f;
            model = glm::rotate(model, glm::radians(angleCube) * speed,
                                rotAxis);
            model = glm::scale(
                model,
                glm::vec3(mirrorX ? -1.0f : 1.0f, mirrorY ? -1.0f : 1.0f,
                          1.0f) *
                scale * cubeScale);

            // auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(model)));
            auto normalMatrix = glm::mat3(glm::transpose(glm::inverse(view * model))); // in view space

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

        const auto currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
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

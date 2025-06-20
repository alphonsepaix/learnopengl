#define DBG_MACRO_NO_WARNING
#include <dbg.h>
#include <fmt/format.h>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "Application.h"

#include <array>

const std::string ASSETS_DIR = "assets/";
const std::string SHADER_DIR = ASSETS_DIR + "shaders/";
const std::string TEXTURE_DIR = ASSETS_DIR + "textures/";

constexpr auto UNLOCK_KEY = GLFW_KEY_LEFT_SHIFT;
constexpr auto FORWARD_KEY = GLFW_KEY_W;
constexpr auto BACKWARD_KEY = GLFW_KEY_S;
constexpr auto LEFT_KEY = GLFW_KEY_A;
constexpr auto RIGHT_KEY = GLFW_KEY_D;
constexpr auto UP_KEY = GLFW_KEY_SPACE;
constexpr auto DOWN_KEY = GLFW_KEY_LEFT_CONTROL;
constexpr auto EXIT_KEY = GLFW_KEY_ESCAPE;

Application::Application() : m_window{this}, m_grassTexture(TEXTURE_DIR + "grass.png") {
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;;
  ImGui_ImplGlfw_InitForOpenGL(m_window.getHandle(), true);
  ImGui_ImplOpenGL3_Init("#version 330 core");

  try {
    const auto objectShader =
        Shader(SHADER_DIR + "object.vert", SHADER_DIR + "object.frag");
    const auto lightShader =
        Shader(SHADER_DIR + "light.vert", SHADER_DIR + "light.frag");
    m_shaders["object"] = std::make_unique<Shader>(objectShader);
    m_shaders["light"] = std::make_unique<Shader>(lightShader);
  } catch (const std::exception &e) {
    std::cerr << e.what() << std::endl;
    throw;
  }

  m_lightManager.add(std::make_unique<DirectionalLight>(glm::vec3(-1.0f)));
  m_lightManager.add(
    std::make_unique<PointLight>(glm::vec3(1.0f, 2.0f, -2.0f)));

  // Grass rendering setup.
  m_grassTexture.setWrap(Texture::Wrap::ClampToEdge, Texture::Wrap::ClampToEdge);
  m_transparentVertices = {
    0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
    0.0f, -0.5f, 0.0f, 0.0f, 1.0f,
    1.0f, -0.5f, 0.0f, 1.0f, 1.0f,

    0.0f, 0.5f, 0.0f, 0.0f, 0.0f,
    1.0f, -0.5f, 0.0f, 1.0f, 1.0f,
    1.0f, 0.5f, 0.0f, 1.0f, 0.0f
  };
  glGenVertexArrays(1, &m_transparentVao);
  glGenBuffers(1, &m_transparentVbo);
  glBindVertexArray(m_transparentVao);
  glBindBuffer(GL_ARRAY_BUFFER, m_transparentVbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(m_transparentVertices), m_transparentVertices.data(), GL_STATIC_DRAW);
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), static_cast<void *>(nullptr));
  glEnableVertexAttribArray(2);
  glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), reinterpret_cast<void *>(3 * sizeof(float)));
  glBindVertexArray(0);
  m_vegetationPos =
  {
    glm::vec3(-1.5f, 0.0f, -0.48f),
    glm::vec3(1.5f, 0.0f, 0.51f),
    glm::vec3(0.0f, 0.0f, 0.7f),
    glm::vec3(-0.3f, 0.0f, -2.3f),
    glm::vec3(0.5f, 0.0f, -0.6f)
  };
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

  widgets();

  m_state.wireframe
    ? glPolygonMode(GL_FRONT_AND_BACK, GL_LINE)
    : glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

  m_state.depthTesting ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
  glDepthFunc(m_state.depthFn);
  glEnable(GL_STENCIL_TEST);

  const auto &bgColor = m_window.getBgColor();
  glClearColor(bgColor.r, bgColor.g, bgColor.b, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  const auto viewPos = m_cameraManager.getActiveCamera()->getPosition();
  const auto view = m_cameraManager.getActiveCamera()->lookAt();
  const auto projection =
      glm::perspective(glm::radians(m_cameraManager.getFov()),
                       static_cast<float>(m_window.getWidth()) /
                       static_cast<float>(m_window.getHeight()),
                       0.1f, 100.0f);

  // Render the light sources.
  const auto &lightShader = m_shaders["light"];
  lightShader->use();
  lightShader->setMat4("view", view);
  lightShader->setMat4("projection", projection);
  m_lightManager.update(m_cameraManager.getActiveCamera());
  m_lightManager.draw(lightShader.get());

  // Draw the objects.
  const auto &objectShader = m_shaders["object"];
  objectShader->use();
  objectShader->setFloat("material.shininess", 32);
  objectShader->setVec3("viewPos", viewPos);
  objectShader->setBool("emission", m_state.emission);
  objectShader->setMat4("view", view);
  objectShader->setMat4("projection", projection);
  objectShader->setBool("showDepth", m_state.showDepth);
  m_lightManager.setShaderUniforms(objectShader.get());
  m_modelManager.draw(objectShader.get());

  // Render grass (blending example).
  objectShader->use();
  m_grassTexture.setUnit(0);
  objectShader->setInt("grass", 0);
  objectShader->setBool("isGrass", true);
  glBindVertexArray(m_transparentVao);
  for (auto pos: m_vegetationPos) {
    auto model = glm::mat4(1.0f);
    model = glm::translate(model, pos);
    objectShader->setMat4("model", model);
    glDrawArrays(GL_TRIANGLES, 0, 6);
  }
  objectShader->setBool("isGrass", false);

  ImGui::Render();
  ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
  m_window.swapBuffers();
  glfwPollEvents();
}

bool Application::isRunning() const { return !m_window.shouldClose(); }

void Application::updateFov(const float yOffset) {
  m_cameraManager.updateFov(yOffset);
}

void Application::toggleCursor() {
  m_state.cursorLocked = !m_state.cursorLocked;
  if (m_state.cursorLocked) {
    glfwSetInputMode(m_window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    m_state.cursorJustLocked = true;
  } else {
    glfwSetInputMode(m_window.getHandle(), GLFW_CURSOR, GLFW_CURSOR_NORMAL);
  }
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
    m_state.performanceStr =
        fmt::format("Application average: {:.2f} ms/frame ({:d} FPS)",
                    m_state.deltaTime * 1000, fps);
  }
  ImGui::Text("%s", m_state.performanceStr.c_str());
  ImGui::End();

  ImGui::Begin("Options");
  ImGui::Checkbox("Wireframe", &m_state.wireframe);
  ImGui::Checkbox("Emission", &m_state.emission);
  ImGui::Checkbox("Show depth", &m_state.showDepth);
  ImGui::Checkbox("Depth testing", &m_state.depthTesting);
  if (m_state.depthTesting) {
    constexpr std::array<std::pair<const char *, GLenum>, 3> depthFuncs = {
      {{"Less", GL_LESS}, {"Always", GL_ALWAYS}, {"Greater", GL_GREATER}}
    };
    int fnIndex = 0;
    for (auto i = 0; i < depthFuncs.size(); ++i) {
      if (depthFuncs[i].second == m_state.depthFn) {
        fnIndex = i;
        break;
      }
    }
    std::array<const char *, depthFuncs.size()> depthFnNames{};
    for (auto i = 0; i < depthFuncs.size(); ++i) {
      depthFnNames[i] = depthFuncs[i].first;
    }
    if (ImGui::Combo("Depth function##Combo", &fnIndex, depthFnNames.data(),
                     depthFnNames.size())) {
      m_state.depthFn = depthFuncs[fnIndex].second;
    }
  }
  ImGui::End();

  ImGui::Begin("Settings");
  m_window.widgets();
  m_cameraManager.widgets();
  m_modelManager.widgets();
  m_lightManager.widgets();
  ImGui::End();
}

void Application::processInput() {
  if (m_window.getKey(EXIT_KEY) == GLFW_PRESS) {
    m_window.setShouldClose(true);
  }

  const auto &deltaTime = m_state.deltaTime;
  const auto camera = m_cameraManager.getActiveCamera();
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

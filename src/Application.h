#ifndef APPLICATION_H
#define APPLICATION_H

#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include "Window.h"

struct AppState {
  float deltaTime = 0.0f;
  float deltaTimeAdded = 0.0f;
  float lastFrame = 0.0f;
  std::string performanceStr = "Starting...";
  bool wireframe = false;
  bool emission = false;
  bool cursorLocked = true;
  bool cursorJustLocked = false;
  bool firstMouse = true;
  bool depthTesting = true;
  bool showDepth = false;
  GLenum depthFn = GL_LESS;
  float lastX = 400.0f;
  float lastY = 300.0f;
};

class Application {
public:
  Application();

  ~Application();

  void mainLoop();

  bool isRunning() const;

  void updateFov(float yOffset);

  bool &getFirstMouse() { return m_state.firstMouse; }
  bool &getCursorLocked() { return m_state.cursorLocked; }
  bool &getCursorJustUnlocked() { return m_state.cursorJustLocked; }
  float &getLastX() { return m_state.lastX; }
  float &getLastY() { return m_state.lastY; }
  void toggleFlashLight() { m_lightManager.toggleFlashLight(); }

  void toggleCursor();

  Camera *getActiveCamera() const { return m_cameraManager.getActiveCamera(); }

  void resize(int width, int height);

private:
  Window m_window;
  CameraManager m_cameraManager;
  LightManager m_lightManager;
  ModelManager m_modelManager;
  AppState m_state;
  std::unordered_map<std::string, std::unique_ptr<Shader>> m_shaders;

  void widgets();

  void processInput();
};

#endif

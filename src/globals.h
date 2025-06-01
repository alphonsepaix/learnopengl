#ifndef GLOBALS_H
#define GLOBALS_H

#include <GLFW/glfw3.h>

#include <string>

inline extern auto WINDOW_WIDTH = 1600;
inline extern auto WINDOW_HEIGHT = 900;
inline extern constexpr auto WINDOW_TITLE = "Learn OpenGL";

inline extern const std::string SHADER_DIR = "assets/shaders/";
inline extern const std::string TEXTURE_DIR = "assets/textures/";
inline extern const std::string MODELS_DIR = "assets/models/";

inline extern float deltaTime = 0.0f;
inline extern float deltaTimeAdded = 0.0f;
inline extern float lastFrame = 0.0f;
inline extern std::string performanceStr = "Starting...";

inline extern bool firstMouse = true;
inline extern auto lastX = WINDOW_WIDTH / 2.0;
inline extern auto lastY = WINDOW_HEIGHT / 2.0;

inline extern bool cursorLocked = true;
inline extern bool cursorJustUnlocked = false;

inline extern float fov = 45.0f;

inline extern constexpr auto UNLOCK_KEY = GLFW_KEY_LEFT_SHIFT;
inline extern constexpr auto FORWARD_KEY = GLFW_KEY_W;
inline extern constexpr auto BACKWARD_KEY = GLFW_KEY_S;
inline extern constexpr auto LEFT_KEY = GLFW_KEY_A;
inline extern constexpr auto RIGHT_KEY = GLFW_KEY_D;
inline extern constexpr auto UP_KEY = GLFW_KEY_SPACE;
inline extern constexpr auto DOWN_KEY = GLFW_KEY_LEFT_CONTROL;

#endif

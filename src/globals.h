#ifndef GLOBALS_H
#define GLOBALS_H

#include <string>

extern int WINDOW_WIDTH;
extern int WINDOW_HEIGHT;
inline constexpr std::string WINDOW_TITLE = "Learn OpenGL";

extern const std::string SHADER_DIR;
extern const std::string TEXTURE_DIR;
extern const std::string MODELS_DIR;

extern float deltaTime;
extern float deltaTimeAdded;
extern float lastFrame;
extern std::string performanceStr;

extern bool firstMouse;
extern double lastX;
extern double lastY;

extern bool cursorLocked;
extern bool cursorJustUnlocked;

extern float fov;

inline constexpr auto UNLOCK_KEY = GLFW_KEY_LEFT_SHIFT;
inline constexpr auto FORWARD_KEY = GLFW_KEY_W;
inline constexpr auto BACKWARD_KEY = GLFW_KEY_S;
inline constexpr auto LEFT_KEY = GLFW_KEY_A;
inline constexpr auto RIGHT_KEY = GLFW_KEY_D;
inline constexpr auto UP_KEY = GLFW_KEY_SPACE;
inline constexpr auto DOWN_KEY = GLFW_KEY_LEFT_CONTROL;

#endif

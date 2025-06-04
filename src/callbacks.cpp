#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#include "callbacks.h"
#include "Camera.h"
#include "Application.h"

constexpr auto TOGGLE_FLASHLIGHT_BUTTON = GLFW_MOUSE_BUTTON_RIGHT;
constexpr auto TOGGLE_CURSOR = GLFW_KEY_LEFT_SHIFT;

void framebufferSizeCallback(GLFWwindow *window, const int width, const int height) {
    const auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    app->resize(width, height);
}

void mouseCallback(GLFWwindow *window, double posX, double posY) {
    const auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    if (!app->getCursorLocked())
        return;
    auto &firstMouse = app->getFirstMouse();
    auto &cursorJustLocked = app->getCursorJustUnlocked();
    auto &lastX = app->getLastX();
    auto &lastY = app->getLastY();
    if (firstMouse || cursorJustLocked) {
        lastX = posX;
        lastY = posY;
        firstMouse = false;
        cursorJustLocked = false;
    }
    const auto xOffset = posX - lastX;
    const auto yOffset = posY - lastY;
    lastX = posX;
    lastY = posY;
    app->getActiveCamera()->mouseUpdate(
        static_cast<float>(xOffset),
        static_cast<float>(yOffset));
}

void mouseButtonCallback(GLFWwindow *window, const int button, const int action, int) {
    if (const auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        button == TOGGLE_FLASHLIGHT_BUTTON && action == GLFW_PRESS && app->getCursorLocked()) {
        app->toggleFlashLight();
    }
}

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    const auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    if (key == TOGGLE_CURSOR && action == GLFW_PRESS) {
        app->toggleCursor();
    }
}

void scrollCallback(GLFWwindow *window, double, double yOffset) {
    const auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    app->updateFov(static_cast<float>(yOffset));
}

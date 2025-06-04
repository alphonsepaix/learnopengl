#define DBG_MACRO_NO_WARNING
#include <dbg.h>

#include "callbacks.h"
#include "Camera.h"
#include "Application.h"

constexpr auto TOGGLE_FLASHLIGHT_BUTTON = GLFW_MOUSE_BUTTON_RIGHT;

void framebufferSizeCallback(GLFWwindow *window, const int width, const int height) {
    const auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    app->resize(width, height);
}

void mouseCallback(GLFWwindow *window, double posX, double posY) {
    const auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    if (!app->getCursorLocked())
        return;
    auto &firstMouse = app->getFirstMouse();
    auto &cursorJustUnlocked = app->getCursorJustUnlocked();
    auto &lastX = app->getLastX();
    auto &lastY = app->getLastY();
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
    CameraManager::getInstance()->getActiveCamera()->mouseUpdate(
        static_cast<float>(xOffset),
        static_cast<float>(yOffset));
}

void mouseButtonCallback(GLFWwindow *window, const int button, const int action, int) {
    if (const auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
        button == TOGGLE_FLASHLIGHT_BUTTON && action == GLFW_PRESS && app->getCursorLocked()) {
        app->toggleFlashLight();
    }
}

void scrollCallback(GLFWwindow *window, double, double yOffset) {
    const auto app = static_cast<Application *>(glfwGetWindowUserPointer(window));
    app->updateFov(static_cast<float>(yOffset));
}

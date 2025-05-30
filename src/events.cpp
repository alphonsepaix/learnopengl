#include "events.h"
#include "Camera.h"
#include "globals.h"

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

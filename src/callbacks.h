#ifndef EVENTS_H
#define EVENTS_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebufferSizeCallback(GLFWwindow *window, int width, int height);

void mouseCallback(GLFWwindow *window, double posX, double posY);

void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods);

void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

#endif

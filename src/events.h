#ifndef EVENTS_H
#define EVENTS_H

#include <GLFW/glfw3.h>

void framebufferSizeCallback(GLFWwindow *window, int width, int height);

void mouseCallback(GLFWwindow *window, double posX, double posY);

void scrollCallback(GLFWwindow *window, double xOffset, double yOffset);

void processInput(GLFWwindow *window);

#endif

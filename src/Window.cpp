#include <fmt/format.h>
#include <glm/gtc/type_ptr.hpp>
#include <imgui.h>

#include "Application.h"
#include "Window.h"
#include "callbacks.h"

#include <iostream>

Window::Window(Application *const user, const int width, const int height, const std::string &title): m_width{width},
    m_height{height},
    m_title{title},
    m_bgColor{glm::vec3(0.0f)} {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    m_window = glfwCreateWindow(m_width, m_height, title.c_str(), nullptr, nullptr);
    if (m_window == nullptr) {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
    }
    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initialize GLAD\n";
    }

    glfwSwapInterval(1);

    glfwSetWindowUserPointer(m_window, user);
    glfwSetFramebufferSizeCallback(m_window, framebufferSizeCallback);
    glfwSetCursorPosCallback(m_window, mouseCallback);
    glfwSetMouseButtonCallback(m_window, mouseButtonCallback);
    glfwSetScrollCallback(m_window, scrollCallback);

    setInputMode(GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glViewport(0, 0, m_width, m_height);
    glEnable(GL_DEPTH_TEST);
}

Window::~Window() {
    glfwTerminate();
}

bool Window::shouldClose() const {
    return glfwWindowShouldClose(m_window);
}

void Window::setShouldClose(int value) const {
    glfwSetWindowShouldClose(m_window, value);
}

void Window::setInputMode(int mode, int value) const {
    glfwSetInputMode(m_window, mode, value);
}

void Window::resize(int width, int height) {
    m_width = width;
    m_height = height;
    glViewport(0, 0, m_width, m_height);
}

void Window::swapBuffers() const {
    glfwSwapBuffers(m_window);
}

void Window::widgets() {
    if (ImGui::CollapsingHeader("Window")) {
        auto s = fmt::format("Window size: ({}, {})", m_width, m_height);
        ImGui::Text("%s", s.c_str());
        ImGui::ColorPicker3("##Background", glm::value_ptr(m_bgColor));
    }
}

#ifndef WINDOW_H
#define WINDOW_H

// ReSharper disable once CppUnusedIncludeDirective
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <GLFW/glfw3.h>

#include <string>

constexpr int WINDOW_WIDTH = 1600;
constexpr int WINDOW_HEIGHT = 900;

class Application;

class Window {
public:
    explicit Window(Application *const user, int width = WINDOW_WIDTH, int height = WINDOW_HEIGHT,
                    const std::string &title = "Learn OpenGL");

    ~Window();

    [[nodiscard]] bool shouldClose() const;

    void setShouldClose(int value) const;

    void setInputMode(int mode, int value) const;

    int getKey(const int key) const { return glfwGetKey(m_window, key); }

    int getMouseButton(const int button) const { return glfwGetMouseButton(m_window, button); }

    int getWidth() const { return m_width; }

    int getHeight() const { return m_height; }

    void resize(int width, int height);

    const glm::vec3 &getBgColor() const { return m_bgColor; }

    void swapBuffers() const;

    [[nodiscard]] GLFWwindow *getHandle() const { return m_window; }

    void widgets();

private:
    int m_width;
    int m_height;
    std::string m_title;
    GLFWwindow *m_window;
    glm::vec3 m_bgColor;
};

#endif

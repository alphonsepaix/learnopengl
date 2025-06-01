#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

#include <string>

class Window {
public:
    explicit Window(int width = 800, int height = 600, const std::string &title = "Learn OpenGL");

    ~Window();

    [[nodiscard]] bool shouldClose() const;

    void swapBuffers() const;

    [[nodiscard]] GLFWwindow *getHandle() const { return m_window; }

private:
    int m_width;
    int m_height;
    const std::string &m_title;
    GLFWwindow *m_window;
};

#endif

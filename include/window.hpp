#pragma once

#include <memory>

class GLFWwindow;

class Window {
protected:

    GLFWwindow *window_;

public:

    Window() noexcept;

    void Close() noexcept;
    bool Update() noexcept;

    void GetFramebufferSize(int& width, int& height) const noexcept;
    GLFWwindow* Access() noexcept;

    virtual ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
};

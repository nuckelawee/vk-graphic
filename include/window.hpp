#pragma once

#include <memory>

class GLFWwindow;

class Window {
protected:

    GLFWwindow *window_;

public:

    Window();

    void CloseWindow() noexcept;
    bool Update() noexcept;

    void SetWindowUserPointer(void *pointer) noexcept;
    void GetFramebufferSize(int& width, int& height) const noexcept;

    virtual ~Window();

    Window(const Window&) = delete;
    Window& operator=(const Window&) = delete;
};

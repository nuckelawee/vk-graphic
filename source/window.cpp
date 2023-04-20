#include "window.hpp"
#include "global_settings.hpp"
#include "input/input_controller.hpp"

#include <cassert>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

Window::Window() noexcept {

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(primary);

    window_ = glfwCreateWindow(mode->width, mode->height
        , GlobalSettings::GetInstance().applicationName, nullptr, nullptr);

    assert(window_ != nullptr);

    glfwSetWindowMonitor(window_, primary, 0.0f, 0.0f
        , mode->width, mode->height, mode->refreshRate);

    glfwMakeContextCurrent(window_);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Window::GetFramebufferSize(int& width, int& height) const noexcept {
    glfwGetFramebufferSize(window_, &width, &height);
}

void Window::Close() noexcept {
    glfwSetWindowShouldClose(window_, GLFW_TRUE);
}

bool Window::Update() noexcept {
    return glfwWindowShouldClose(window_);
}

GLFWwindow* Window::Access() noexcept {
    return window_;
}

Window::~Window() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}


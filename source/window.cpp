#include "window.hpp"
#include "global_settings.hpp"
#include "input/input_controller.hpp"

#include <stdexcept>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

void keyCallback(GLFWwindow* window, int key, int scancode
    , int action, int modes);

void cursorPosCallback(GLFWwindow* window, double x, double y);

void framebufferResize(GLFWwindow* window, int width, int height);

Window::Window() {

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode *mode = glfwGetVideoMode(primary);

    window_ = glfwCreateWindow(mode->width, mode->height
        , GlobalSettings::GetInstance().applicationName, nullptr, nullptr);

    if(window_ == nullptr) {
        throw std::runtime_error("Failed to create GLFW window");
    }

    glfwSetWindowMonitor(window_, primary, 0.0f, 0.0f
        , mode->width, mode->height, mode->refreshRate);

    glfwMakeContextCurrent(window_);
    glfwSetInputMode(window_, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    glfwSetKeyCallback(window_, keyCallback);
    glfwSetCursorPosCallback(window_, cursorPosCallback);
    glfwSetFramebufferSizeCallback(window_, framebufferResize);
    glfwSwapInterval(1);

}

void Window::SetWindowUserPointer(void *pointer) noexcept {
    glfwSetWindowUserPointer(window_, pointer);
}

void Window::GetFramebufferSize(int& width, int& height) const noexcept {
    glfwGetFramebufferSize(window_, &width, &height);
}

void Window::CloseWindow() noexcept {
    glfwSetWindowShouldClose(window_, GLFW_TRUE);
}

bool Window::Update() noexcept {
    return glfwWindowShouldClose(window_);
}

Window::~Window() {
    glfwDestroyWindow(window_);
    glfwTerminate();
}

void keyCallback(GLFWwindow* window, int key, int scancode
    , int action, int modes) {

    if(key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    input::Controller *controller = static_cast<input::Controller*>
        (glfwGetWindowUserPointer(window));

    controller->KeyInput(key, scancode, action, modes);

}

void cursorPosCallback(GLFWwindow* window, double x, double y) {
    input::Controller *controller = static_cast<input::Controller*>
        (glfwGetWindowUserPointer(window));

    controller->CursorPosition((float)(x), (float)(y));
}

void framebufferResize(GLFWwindow* window, int width, int height) {
    
}



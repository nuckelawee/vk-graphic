#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>

#include "app_setting.hpp"

class Window {

    GLFWwindow *pWindow = nullptr;
    VkSurfaceKHR surface;

public:
    void CreateWindow(const AppSetting& appSetting);
    VkResult CreateSurface(const VkInstance& instance);

    VkSurfaceKHR AccessSurface() const { return surface; }

    void DestroySurface(const VkInstance& instance);
    bool ShouldClosed();

    Window() {}
    ~Window();
};

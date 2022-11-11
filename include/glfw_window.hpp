#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <iostream>
#include <cassert>
#include <vector>

#include "app_setting.hpp"

struct SurfaceDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class Window {

    GLFWwindow *pWindow_ = nullptr;
    VkSurfaceKHR surface_;

public:
    void CreateWindow(const AppSetting& appSetting);
    VkResult CreateSurface(const VkInstance& instance);

    static SurfaceDetails SurfaceCapabilities(const VkPhysicalDevice& gpu
        , const VkSurfaceKHR& surface);


    VkSurfaceKHR AccessSurface() const { return surface_; }

    void DestroySurface(const VkInstance& instance);
    void CloseWindow();
    bool ShouldClosed() const;

    Window() {}
    ~Window();
};

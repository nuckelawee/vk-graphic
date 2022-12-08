#pragma once

#include "app_setting.hpp"

#include "vk/vk_surface.hpp"

class GlfwWindow : public vk::Surface {

    GLFWwindow *pWindow_ = nullptr;

public:
    virtual VkResult Create(const vk::Instance& instance);
    virtual vk::SurfaceDetails Capabilities(const VkPhysicalDevice& gpu) const;

    void CreateWindow(const AppSetting& appSetting);
    static void KeyCallback(GLFWwindow *pWindow, int key, int scancode
        , int action, int modes);
    
    void CloseWindow();
    bool ShouldClosed() const;

    GlfwWindow() {}
    ~GlfwWindow();
};

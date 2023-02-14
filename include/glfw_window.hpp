#pragma once

#include "app_setting.hpp"

#include "vk/vk_surface.hpp"

class GlfwWindow : public vk::Surface {

    GLFWwindow *pWindow_ = nullptr;

public:
    virtual VkResult Create(const vk::Instance& instance);
    virtual vk::SurfaceDetails Capabilities(const VkPhysicalDevice& gpu) const;

    void CreateWindow();
    static void KeyCallback(GLFWwindow *pWindow, int key, int scancode
        , int action, int modes);
    static void FramebufferResize(GLFWwindow *pWindow, int width, int height);
    
    void CloseWindow();
    bool ShouldClosed() const;

    GLFWwindow& AccessGLFW() { return *pWindow_; }

    GlfwWindow(AppSetting& setting) : Surface(setting) {}
    virtual ~GlfwWindow();
};

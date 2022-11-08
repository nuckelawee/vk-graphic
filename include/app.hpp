#pragma once

#include "vulkan_instance.hpp"
#include "vulkan_device.hpp"
#include "glfw_window.hpp"

class App {

    AppSetting globalSetting_;
    VulkanLayersAndExtensions attachments_;
    Window window_;
    VulkanInstance instance_;
    VulkanDevice device_;

public:

    void Run();

    App() {}
    ~App();

private:
    void Init();

};

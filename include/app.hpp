#pragma once

#include "vulkan_instance.hpp"
#include "vulkan_device.hpp"
#include "glfw_window.hpp"
#include "thread_pool.hpp"
#include "console.hpp"
#include "statistic.hpp"

class App {

    AppSetting globalSetting_;
    VulkanLayersAndExtensions attachments_;
    Window window_;
    VulkanInstance instance_;
    VulkanDevice device_;
    ThreadPool threadPool_;
    Statistic statistic_;

private:
    bool CheckState();
    void Update();
    void Init();

public:

    void Run();

    App();
    ~App();

};

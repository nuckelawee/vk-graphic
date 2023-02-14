#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk_surface.hpp"
#include "vk_instance.hpp"
#include "vk_device.hpp"
#include "vk_swapchain.hpp"
#include "vk_graphic_pipeline.hpp"
#include "vk_data_loader.hpp"
#include "vk_command_manager.hpp"

namespace vk {

class Engine {
    AppSetting& setting_;

    LayersAndExtensions *pAttachments_;
    Instance *pInstance_;
    Device *pDevice_;
    Swapchain *pSwapchain_;
    GraphicPipeline *pPipeline_;
    DataLoader *pDataLoader_;
    CommandManager *pCommandManager_;
    Regulator *pRegulator_;
public:

    void Init(Surface& surface); 
    void Update(Surface& surface);
    void Terminate(Surface& window);

private:
    
    void Acquire(Surface& surface);
    void Present(Surface& surface, VkPresentInfoKHR& presentInfo);

public:
    Engine operator=(const Engine& engine) = delete;
    Engine(const Engine& engine) = delete;
    Engine(AppSetting& setting) : setting_(setting) {}
    ~Engine() {}

}; 

} //vk

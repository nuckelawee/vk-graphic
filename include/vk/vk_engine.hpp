#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk_surface.hpp"
#include "vk_instance.hpp"
#include "vk_device.hpp"
#include "vk_swapchain.hpp"
#include "vk_graphic_pipeline.hpp"
#include "vk_command_manager.hpp"
#include "vk_regulator.hpp"
#include "vk_model_storage.hpp"

#include "camera.hpp"

namespace vk {

class Engine {

    Device device_;
    Swapchain swapchain_;
    LayersAndExtensions attachments_;
    Regulator regulator_;
    GraphicPipeline pipeline_;
    Instance instance_;
    DescriptorSet descriptorSet_;
    DescriptorPool descriptorPool_;
    CommandManager commandManager_;

    ModelStorage modelStorage_;
    BufferBuilder bufferBuilder_;
    ImageBuilder imageBuilder_;
    Buffer ubos_[vk::Setting::frames];

    VkCommandPool graphicPool_;
    std::vector<VkCommandBuffer> graphicCommands_;

    AppSetting& setting_;
public:

    void Init(Surface& surface); 
    void Update(Surface& surface, const Camera& camera);
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

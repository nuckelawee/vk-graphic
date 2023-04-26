#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "vk_instance.hpp"
#include "vk_device.hpp"
#include "vk_swapchain.hpp"
#include "vk_graphic_pipeline.hpp"
#include "vk_command_manager.hpp"
#include "vk_regulator.hpp"
#include "vk_model_storage.hpp"
#include "vk_descriptor_set.hpp"
#include "vk_image_builder.hpp"
#include "vk_buffer_builder.hpp"

#include "engine.hpp"

struct MvpMatrix;

namespace vk {

class Engine final : public IEngine {

    Device device_;
    Swapchain swapchain_;
    LayersAndExtensions attachments_;
    Regulator regulator_;
    GraphicPipeline pipeline_;
    Instance instance_;
    DescriptorSet descriptorSet_;
    CommandManager commandManager_;

    ModelStorage modelStorage_;
    BufferBuilder bufferBuilder_;
    ImageBuilder imageBuilder_;

    VkCommandPool graphicPool_;
    std::vector<VkCommandBuffer> graphicCommands_;

    std::vector<Buffer> ubos_;
    Surface* surface_;

    const MvpMatrix* cameraData_;

public:

    virtual Window* Init() override; 
    virtual void Render(const std::vector<Model>& models) override;
    virtual std::vector<Model> Bind(ModelSet& modelSet) override;

    virtual BaseCamera* CreateCamera() noexcept override;
    virtual void DestroyCamera(BaseCamera* camera) noexcept override;
private:
    
    void UpdateCamera() noexcept;
    void Acquire() noexcept;
    void Present(VkPresentInfoKHR& presentInfo);

public:

    ~Engine();
    Engine() = default;

    Engine& operator=(const Engine& engine) = delete;
    Engine(const Engine& engine) = delete;
}; 

} //vk

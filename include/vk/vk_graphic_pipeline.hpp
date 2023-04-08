#pragma once

#include <string>
#include <vector>

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

namespace vk {

class Swapchain;
class DescriptorSet;
class Device;

class GraphicPipeline {

    VkPipelineLayout pipelineLayout_;
    VkRenderPass renderPass_;
    VkPipeline pipeline_;
    VkDevice device_;

public:

    void Create(const Device& device, const Swapchain& swapchain
        , const std::vector<std::string>& shadersPath
        , const DescriptorSet& descriptorSet);

    VkPipelineLayout AccessLayout() const noexcept;
    VkPipeline Access() const noexcept;
    VkRenderPass AccessRenderPass() const noexcept;

    void RenderPassBegin(VkCommandBuffer commandBuffer
        , VkFramebuffer framebuffer) noexcept;

    void Destroy() noexcept;
private:

    void CreateRenderPass(VkFormat depthFormat, VkFormat imageFormat);

    VkShaderModule CreateShaderModule(const std::string& code) const;

};

} //vk

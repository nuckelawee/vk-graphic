#pragma once

#include "vk_pipeline_states.hpp"
#include "vk_vertex_binding.hpp"

namespace vk {

class Swapchain;
class DescriptorSet;

class GraphicPipeline {

    VkPipelineLayout pipelineLayout_;
    VkRenderPass renderPass_;
    VkPipeline pipeline_;

public:

    void Create(const Device& device, const Swapchain& swapchain
        , const std::vector<std::string>& shadersPath
        , const DescriptorSet& descriptorSet
        , std::function<PipelineStates(const Swapchain& swapchain
        , void *pUserData)> = DescribePipelineStates);

    const VkPipelineLayout& AccessLayout() const;
    VkPipelineLayout& AccessLayout();
    const VkPipeline& Access() const;
    VkPipeline& Access();
    const VkRenderPass& AccessRenderPass() const;
    VkRenderPass& AccessRenderPass();

    void RenderPassBegin(VkCommandBuffer commandBuffer, VkFramebuffer framebuffer
        , VkExtent2D extent, const Setting& setting);

    VkRenderPassBeginInfo RenderPassBegin(const Setting& setting, const Swapchain& swapchain);

private:

    static PipelineStates DescribePipelineStates(const Swapchain& swapchain
        , void *pUserData);
    void CreateRenderPass(const Device& device, const Swapchain& swapchain
        , void *pUserData);

    VkShaderModule CreateShaderModule(const VkDevice& device, const std::string& code);
public:

    void Destroy(const Device& device);

    GraphicPipeline() {}
    ~GraphicPipeline() {}

};

} //vk

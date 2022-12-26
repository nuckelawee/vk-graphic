#pragma once

#include "vk/vk_pipeline_states.hpp"
#include "vk/vk_vertex.hpp"

namespace vk {

class Swapchain;

class GraphicPipeline {

    VkPipelineLayout pipelineLayout_;
    VkRenderPass renderPass_;
    VkPipeline pipeline_;

public:

    void Create(const Device& device, const Swapchain& swapchain
        , const std::vector<std::string>& shadersPath
        , std::function<PipelineStates(const Swapchain& swapchain
        , void *pUserData)> = DescribePipelineStates);

    VkPipeline Access() const { return pipeline_; }
    VkRenderPass AccessRenderPass() const { return renderPass_; }
private:

    static PipelineStates DescribePipelineStates(const Swapchain& swapchain
        , void *pUserData);
    void CreateRenderPass(const VkDevice& device, const Swapchain& swapchain
        , void *pUserData);

    VkShaderModule CreateShaderModule(const VkDevice& device, const std::string& code);
public:
    void Destroy(const Device& device);

    GraphicPipeline() {}
    ~GraphicPipeline() {}

};

} //vk

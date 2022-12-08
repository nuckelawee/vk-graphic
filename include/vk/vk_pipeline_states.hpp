#pragma once

#include <fstream>

#include "vk_device.hpp"
#include "vk_swapchain.hpp"

namespace vk {

class PipelineStates {

    VkPipelineDynamicStateCreateInfo dynamicInfo_ {};
    VkPipelineVertexInputStateCreateInfo inputInfo_ {};
    VkPipelineInputAssemblyStateCreateInfo assemblyInfo_ {};
    VkPipelineViewportStateCreateInfo viewportInfo_ {};
    VkPipelineRasterizationStateCreateInfo rasterizerInfo_ {};
    VkPipelineMultisampleStateCreateInfo multisampleInfo_ {};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo_ {};
    VkPipelineColorBlendStateCreateInfo colorBlendInfo_ {};

private:
    std::vector<VkDynamicState> dynamics_;

    std::vector<VkVertexInputBindingDescription> bindings_;
    std::vector<VkVertexInputAttributeDescription> attributes_;

    std::vector<VkViewport> viewports_;
    std::vector<VkRect2D> scissors_;

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments_;

    std::vector<VkSampleMask> sampleMasks_;

public:

    void DynamicInfo(const VkPipelineDynamicStateCreateInfo& info);
    void InputInfo(const VkPipelineVertexInputStateCreateInfo& info);
    void AssemblyInfo(const VkPipelineInputAssemblyStateCreateInfo& info);
    void ViewportInfo(const VkPipelineViewportStateCreateInfo& info);
    void RasterizerInfo(const VkPipelineRasterizationStateCreateInfo& info);
    void MultisampleInfo(const VkPipelineMultisampleStateCreateInfo& info);
    void DepthStencilInfo(const VkPipelineDepthStencilStateCreateInfo& info);
    void ColorBlendInfo(const VkPipelineColorBlendStateCreateInfo& info);


    VkPipelineDynamicStateCreateInfo& AccessDynamic()
    { return dynamicInfo_; }
    VkPipelineVertexInputStateCreateInfo& AccessInput()
    { return inputInfo_; }
    VkPipelineInputAssemblyStateCreateInfo& AccessAssembly()
    { return assemblyInfo_; }
    VkPipelineViewportStateCreateInfo& AccessViewport()
    { return viewportInfo_; }
    VkPipelineRasterizationStateCreateInfo& AccessRasterizer()
    { return rasterizerInfo_; }
    VkPipelineMultisampleStateCreateInfo& AccessMultisample()
    { return multisampleInfo_; }
    VkPipelineDepthStencilStateCreateInfo& AccessDepthStencil()
    { return depthStencilInfo_; }
    VkPipelineColorBlendStateCreateInfo& AccessColorBlend()
    { return colorBlendInfo_; }

    PipelineStates() {} 
    ~PipelineStates() {}
};

}; //vk

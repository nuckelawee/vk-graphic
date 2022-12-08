#include "vk/vk_pipeline_states.hpp"

namespace vk {

void PipelineStates::DynamicInfo(const VkPipelineDynamicStateCreateInfo& info) {
    dynamics_.resize(info.dynamicStateCount);
    for(uint32_t i = 0; i < info.dynamicStateCount; i++) {
        dynamics_[i] = info.pDynamicStates[i];
    }
    dynamicInfo_ = info;
    dynamicInfo_.pDynamicStates = dynamics_.data();
}

void PipelineStates::InputInfo(const VkPipelineVertexInputStateCreateInfo& info) {
    bindings_.resize(info.vertexBindingDescriptionCount);
    attributes_.resize(info.vertexAttributeDescriptionCount);
    for(uint32_t i = 0; i < info.vertexBindingDescriptionCount; i++) {
        bindings_[i] = info.pVertexBindingDescriptions[i];
    for(uint32_t i = 0; i < info.vertexAttributeDescriptionCount; i++) {
        attributes_[i] = info.pVertexAttributeDescriptions[i];
        }
    }
    inputInfo_ = info;
    inputInfo_.pVertexBindingDescriptions = bindings_.data();
    inputInfo_.pVertexAttributeDescriptions = attributes_.data();
}

void PipelineStates::AssemblyInfo(const VkPipelineInputAssemblyStateCreateInfo& info) {
    assemblyInfo_ = info;
}

void PipelineStates::ViewportInfo(const VkPipelineViewportStateCreateInfo& info) {
    viewports_.resize(info.viewportCount);
    scissors_.resize(info.scissorCount);
    for(uint32_t i = 0; i < info.viewportCount; i++) {
        viewports_[i] = info.pViewports[i];
    }
    for(uint32_t i = 0; i < info.scissorCount; i++) {
        scissors_[i] = info.pScissors[i];
    }
    viewportInfo_ = info;
    viewportInfo_.pViewports = viewports_.data();
    viewportInfo_.pScissors = scissors_.data();
}

void PipelineStates::RasterizerInfo(const VkPipelineRasterizationStateCreateInfo& info) {
    rasterizerInfo_ = info;
}

void PipelineStates::MultisampleInfo(const VkPipelineMultisampleStateCreateInfo& info) {
    multisampleInfo_ = info; 
}

void PipelineStates::DepthStencilInfo(const VkPipelineDepthStencilStateCreateInfo& info) {
    depthStencilInfo_ = info;
}

void PipelineStates::ColorBlendInfo(const VkPipelineColorBlendStateCreateInfo& info) {
    colorBlendAttachments_.resize(info.attachmentCount);
    for(uint32_t i = 0; i < info.attachmentCount; i++) {
        colorBlendAttachments_[i] = info.pAttachments[i];
    }
    colorBlendInfo_ = info;
    colorBlendInfo_.pAttachments = colorBlendAttachments_.data();
}

}; //vk

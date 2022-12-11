#pragma once

#include "vk/vk_graphic_pipeline.hpp"
#include "vk/vk_command_pool.hpp"

namespace vk {

class CommandBuffer {

    VkCommandBuffer commandBuffer_;

public:
    void Create(const Device& device, const CommandPool& commandPool);

    void Record(const Device& device, const GraphicPipeline& pipeline
        , const Swapchain& swapchain, uint32_t imageIndex);

    VkCommandBuffer Access() const { return commandBuffer_; }

};

} // vk

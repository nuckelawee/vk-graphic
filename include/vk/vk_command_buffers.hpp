#pragma once

#include "vk/vk_graphic_pipeline.hpp"
#include "vk/vk_command_pool.hpp"
#include "vk/vk_buffer.hpp"

#include <forward_list>

namespace vk {

class Buffer;

class CommandBuffers {

    std::forward_list<VkCommandBuffer> graphicCommandBuffers_;
    std::forward_list<VkCommandBuffer> transferCommandBuffers_;

public:
    std::forward_list<VkCommandBuffer>::iterator Allocate(const Device& device
        , commandPoolType type, const CommandPool& commandPool, size_t quantity);

    void RecordDrawCommands(const Device& device, const GraphicPipeline& pipeline
        , const Swapchain& swapchain, uint32_t imageIndex
        , Buffer& buffer, const VkDeviceSize& offset
        , std::forward_list<VkCommandBuffer>::iterator& commandBuffer);

    void Free(const Device& device, const CommandPool& commandPool
        , std::forward_list<VkCommandBuffer>::iterator commandBuffer
        , unsigned int commandBufferCount);

    std::forward_list<VkCommandBuffer>::iterator AccessGraphic()
    { return graphicCommandBuffers_.begin(); }
    std::forward_list<VkCommandBuffer>::iterator AccessTransfer()
    { return transferCommandBuffers_.begin(); }

    CommandBuffers() {}
    ~CommandBuffers() {}

};

} // vk

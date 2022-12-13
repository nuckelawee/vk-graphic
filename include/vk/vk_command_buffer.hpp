#pragma once

#include "vk/vk_graphic_pipeline.hpp"
#include "vk/vk_command_pool.hpp"

namespace vk {

class CommandBuffer {

    std::vector<VkCommandBuffer> commandBuffers_;

public:
    void Create(const Device& device, const CommandPool& commandPool
        , size_t quantity);

    void Record(const Device& device, const GraphicPipeline& pipeline
        , const Swapchain& swapchain, uint32_t imageIndex
        , unsigned int bufferIndex);

    std::vector<VkCommandBuffer>& Access() { return commandBuffers_; }

    CommandBuffer() {}
    ~CommandBuffer() {}

};

} // vk

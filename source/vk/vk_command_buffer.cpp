#include "vk/vk_command_buffer.hpp"

namespace vk {

void CommandBuffer::Create(const Device& device
    , const CommandPool& commandPool, size_t quantity) {

    commandBuffers_.resize(quantity);
    VkCommandBufferAllocateInfo commandBufferInfo {};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferInfo.commandPool = commandPool.Access();
    commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferInfo.commandBufferCount = quantity;

    VkResult result = vkAllocateCommandBuffers(device.Access()
        , &commandBufferInfo, commandBuffers_.data());
    ErrorManager::Validate(result, "Command buffer creation");
}

void CommandBuffer::Record(const Device& device
    , const GraphicPipeline& pipeline
    , const Swapchain& swapchain, uint32_t imageIndex
    , unsigned int bufferIndex) {

    VkCommandBufferBeginInfo bufferInfo {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bufferInfo.flags = 0;
    bufferInfo.pInheritanceInfo = nullptr;
    VkResult result = vkBeginCommandBuffer(commandBuffers_[bufferIndex]
        , &bufferInfo);
    ErrorManager::Validate(result, "Command buffer recording");

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pipeline.AccessRenderPass();
    renderPassInfo.framebuffer = swapchain.AccessFramebuffer(imageIndex);
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchain.AccessExtent();
    VkClearValue clearValue = { 0.2f, 0.2f, 0.2f, 1.0f };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearValue;
    
    vkCmdBeginRenderPass(commandBuffers_[bufferIndex], &renderPassInfo
        , VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffers_[bufferIndex], VK_PIPELINE_BIND_POINT_GRAPHICS
        , pipeline.Access());

    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = swapchain.AccessExtent().width;
    viewport.height = swapchain.AccessExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffers_[bufferIndex], 0, 1, &viewport);

    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchain.AccessExtent();
    vkCmdSetScissor(commandBuffers_[bufferIndex], 0, 1, &scissor);

    vkCmdDraw(commandBuffers_[bufferIndex], 3, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffers_[bufferIndex]);
    result = vkEndCommandBuffer(commandBuffers_[bufferIndex]);
    ErrorManager::Validate(result, "Command buffer recording");
}

}

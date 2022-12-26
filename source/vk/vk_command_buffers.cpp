#include "vk/vk_command_buffers.hpp"

namespace vk {

std::forward_list<VkCommandBuffer>::iterator CommandBuffers::Allocate(
      const Device& device, commandPoolType flag
    , const CommandPool& commandPool, size_t quantity) {

    std::forward_list<VkCommandBuffer> *pCommandBuffers;
    switch(flag) {
    case GRAPHICS:
        pCommandBuffers = &graphicCommandBuffers_; 
        break;
    case TRANSFER:
        pCommandBuffers = &transferCommandBuffers_;
        break;
    default:
        ErrorManager::Validate(UNSOLVABLE, "Trying to create unsupported "\
            "command buffers", "Command buffers creation");
    }

    if(pCommandBuffers->empty() != true) {
        ErrorManager::Validate(WARNING, "This type of command buffers "\
            "were previously created", "Command buffers creation");
    }

    VkCommandBuffer pCommandNewBuffers[quantity];

    VkCommandBufferAllocateInfo commandBufferInfo {};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferInfo.commandPool = commandPool.Access();
    commandBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBufferInfo.commandBufferCount = quantity;

    VkResult result = vkAllocateCommandBuffers(device.Access()
        , &commandBufferInfo, pCommandNewBuffers);
    ErrorManager::Validate(result, "Command buffers creation");

    for(ptrdiff_t i = quantity-1; i >= 0; i--) {
        pCommandBuffers->push_front(pCommandNewBuffers[i]);
    }
     
    auto it = pCommandBuffers->begin();
    advance(pCommandBuffers, quantity);
    return it;
}


void CommandBuffers::RecordDrawCommands(const Device& device
    , const GraphicPipeline& pipeline
    , const Swapchain& swapchain, uint32_t imageIndex
    , Buffer& buffer, const VkDeviceSize& offset
    , std::forward_list<VkCommandBuffer>::iterator& commandBuffer) {

    VkCommandBufferBeginInfo bufferInfo {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    bufferInfo.flags = 0;
    bufferInfo.pInheritanceInfo = nullptr;
    VkResult result = vkBeginCommandBuffer(*commandBuffer
        , &bufferInfo);
    ErrorManager::Validate(result, "Command buffers recording");

    VkRenderPassBeginInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = pipeline.AccessRenderPass();
    renderPassInfo.framebuffer = swapchain.AccessFramebuffer(imageIndex);
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = swapchain.AccessExtent();
    VkClearValue clearValue = { 0.2f, 0.2f, 0.2f, 1.0f };
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearValue;
    
    vkCmdBeginRenderPass(*commandBuffer, &renderPassInfo
        , VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(*commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS
        , pipeline.Access());

    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = swapchain.AccessExtent().width;
    viewport.height = swapchain.AccessExtent().height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(*commandBuffer, 0, 1, &viewport);

    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = swapchain.AccessExtent();
    vkCmdSetScissor(*commandBuffer, 0, 1, &scissor);

    vkCmdBindVertexBuffers(*commandBuffer, 0, 1, &buffer.Access(), &offset);

    vkCmdDraw(*commandBuffer, 3, 1, 0, 0);
    vkCmdEndRenderPass(*commandBuffer);
    result = vkEndCommandBuffer(*commandBuffer);
    ErrorManager::Validate(result, "Command buffers recording");
}

void CommandBuffers::Free(const Device& device, const CommandPool& commandPool
    , std::forward_list<VkCommandBuffer>::iterator commandBuffer
    , unsigned int commandBufferCount) {

    VkCommandBuffer pCommandBuffers[commandBufferCount];
    for(size_t i = 0;  i < commandBufferCount; i++, commandBuffer++) {
        pCommandBuffers[i] = *commandBuffer;
    }
    vkFreeCommandBuffers(device.Access(), commandPool.Access()
        , commandBufferCount, pCommandBuffers);
}

}

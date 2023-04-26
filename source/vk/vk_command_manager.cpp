#include "vk/vk_command_manager.hpp"
#include "vk/vk_graphic_pipeline.hpp"
#include "vk/vk_model_storage.hpp"
#include "vk/vk_device.hpp"
#include "vk/vk_settings.hpp"

namespace vk {

VkViewport setViewport(const VkExtent2D& extent);
VkRect2D setScissor(const VkExtent2D& extent);
void beginCommand(VkCommandBuffer commandBuffer);

VkCommandPool CommandManager::CreatePool(VkDevice device, uint32_t queueIndex
    , VkCommandPoolCreateFlags flags) {

    VkCommandPool pool;
    VkCommandPoolCreateInfo commandPoolInfo {};
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.flags = flags;
    commandPoolInfo.queueFamilyIndex = queueIndex;
    VkResult result = vkCreateCommandPool(device, &commandPoolInfo
        , nullptr, &pool);
    ErrorManager::Validate(result, "Command pool creation");
    return pool;
}

VkCommandBuffer CommandManager::BeginSingleCommand(VkDevice device
    , VkCommandPool pool) {

    VkCommandBufferAllocateInfo allocateInfo {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandBufferCount = 1;
    allocateInfo.commandPool = pool;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(device, &allocateInfo, &commandBuffer);
    
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    return commandBuffer;
}

void CommandManager::EndSingleCommand(VkDevice device, VkCommandPool pool
    , VkQueue queue, VkCommandBuffer commandBuffer) {

    vkEndCommandBuffer(commandBuffer);
    
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(queue);

    vkFreeCommandBuffers(device, pool, 1, &commandBuffer);
}

std::vector<VkCommandBuffer> CommandManager::CreateCommandBuffers(VkDevice device
    , VkCommandPool pool, uint32_t bufferCount) {

    std::vector<VkCommandBuffer> commandBuffers(bufferCount);
    VkCommandBufferAllocateInfo commandBuffersInfo {};
    commandBuffersInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBuffersInfo.commandPool = pool;
    commandBuffersInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBuffersInfo.commandBufferCount = bufferCount;

    VkResult result = vkAllocateCommandBuffers(device
        , &commandBuffersInfo, commandBuffers.data());

    ErrorManager::Validate(result, "Command manager allocation");
    return commandBuffers;
}

void CommandManager::RecordDrawCommands(VkCommandBuffer& commandBuffer
    , const std::vector<Model>& models
    , VkFramebuffer framebuffer, VkDescriptorSet descriptorSet
    , GraphicPipeline& pipeline, ModelStorage& modelStorage) {

    VkExtent2D extent = vk::Settings::GetInstance().Extent();

    vkResetCommandBuffer(commandBuffer, 0);
    beginCommand(commandBuffer);

    pipeline.RenderPassBegin(commandBuffer, framebuffer);

    VkViewport viewport = setViewport(extent);
    VkRect2D scissor = setScissor(extent);
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    modelStorage.DrawModels(models, commandBuffer, pipeline.AccessLayout(), descriptorSet);

    vkCmdEndRenderPass(commandBuffer);
    VkResult result = vkEndCommandBuffer(commandBuffer);
    ErrorManager::Validate(result, "Command buffers recording");
}

void CommandManager::Submit(VkCommandBuffer *commandBuffers
    , uint32_t bufferCount, VkQueue queue, VkFence fence
    , VkSubmitInfo& submitInfo) {

    VkPipelineStageFlags waitStages[] = 
        { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = bufferCount;
    submitInfo.pCommandBuffers = commandBuffers;
   
    VkResult result = vkQueueSubmit(queue, 1, &submitInfo, fence);
    ErrorManager::Validate(result, "Drawing");
}


void beginCommand(VkCommandBuffer commandBuffer) {
    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    beginInfo.pInheritanceInfo = nullptr;

    VkResult result = vkBeginCommandBuffer(commandBuffer
        , &beginInfo);
    ErrorManager::Validate(result, "Command buffers recording");
}
     
VkViewport setViewport(const VkExtent2D& extent) {
    VkViewport viewport {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = extent.width;
    viewport.height = extent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    return viewport;
}

VkRect2D setScissor(const VkExtent2D& extent) {
    VkRect2D scissor {};
    scissor.offset = { 0, 0 };
    scissor.extent = extent;
    return scissor;
}

} //vk

#include "vk/vk_command_manager.hpp"

namespace vk {

CommandBundle& CommandManager::FindBundle(const Device& device
    , const CommandInfo& info) {

    bool wasPreviouslyAllocated = commands_.find(info.type) != commands_.end();
    if(wasPreviouslyAllocated) {
        ErrorManager::Validate(WARNING, "This type of commands was previously "\
            "allocated!", "Command manager allocation");

        return commands_.find(info.type)->second;
    } 
    CommandBundle bundle;
    bundle.commandPool.Create(device, info.type);
    commands_.emplace(info.type, bundle); 
    return commands_.find(info.type)->second;    
}
    
CommandInfo CommandManager::Allocate(const Device& device
    , const CommandInfo& info) {

    CommandBundle& commandBundle = FindBundle(device, info);

    size_t curBufferCount = commandBundle.commandBuffers.size();
    commandBundle.commandBuffers.resize(curBufferCount + info.bufferCount);

    VkCommandBufferAllocateInfo commandBuffersInfo {};
    commandBuffersInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBuffersInfo.commandPool = commandBundle.commandPool.Access();
    commandBuffersInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandBuffersInfo.commandBufferCount = info.bufferCount;

    VkResult result = vkAllocateCommandBuffers(device.Access()
        , &commandBuffersInfo, (commandBundle.commandBuffers.data()) + curBufferCount);

    ErrorManager::Validate(result, "Command manager allocation");
    CommandInfo newInfo = info;
    newInfo.offset = curBufferCount;
    
    return newInfo;
}

void CommandManager::RecordDrawCommands(const Device& device
    , const Setting& setting, const GraphicPipeline& pipeline
    , const Swapchain& swapchain, DataLoader& dataLoader
    , const CommandInfo& commandInfo
    , const DescriptorSet& descriptorSet) {

    const VkCommandBuffer& commandBuffer = *(Access(commandInfo));

    vkResetCommandBuffer(commandBuffer, 0);

    VkCommandBufferBeginInfo commandBufferInfo {};
    commandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferInfo.flags = 0;
    commandBufferInfo.pInheritanceInfo = nullptr;

    VkRenderPassBeginInfo renderPassInfo = pipeline.RenderPassBegin(
        setting, swapchain); 
    VkViewport viewport = swapchain.Viewport();
    VkRect2D scissor = swapchain.Scissor();

    VkResult result = vkBeginCommandBuffer(commandBuffer
        , &commandBufferInfo);
    ErrorManager::Validate(result, "Command buffers recording");
    
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo
        , VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS
        , pipeline.Access());

    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    const BufferData& buffer = dataLoader.Access(BUFFER_TYPE_COMPLEX);
    const BufferInfo& bufferInfo = buffer.infos[0];
  
    const VkDeviceSize& offset = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1
        , &(buffer.buffers[0]), &offset);

    vkCmdBindIndexBuffer(commandBuffer, buffer.buffers[0]
        , bufferInfo.indexMemoryShift, VK_INDEX_TYPE_UINT16);

    for(size_t i = 0; i < bufferInfo.objectCount; i++) {
        ObjectInfo& info = bufferInfo.pObjectInfos[i];
        
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS
            , pipeline.AccessLayout(), 0, 1
            , descriptorSet.Access()+setting.CurrentFrame()
            , 0, nullptr);
        
        vkCmdDrawIndexed(commandBuffer, info.indexCount, 1, info.indexShift, 0, 0);
    }

    vkCmdEndRenderPass(commandBuffer);
    result = vkEndCommandBuffer(commandBuffer);
    ErrorManager::Validate(result, "Command buffers recording");
}

void CommandManager::Submit(const Device& device, const Setting& setting
    , Regulator& regulator, VkSubmitInfo& submitInfo
    , const CommandInfo& commandInfo) {

    const VkCommandBuffer *pCommandBuffers = Access(commandInfo);

    VkPipelineStageFlags waitStages[] = 
        { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = commandInfo.bufferCount;
    submitInfo.pCommandBuffers = pCommandBuffers;
   
    VkResult result = vkQueueSubmit(device.AccessQueues().graphic.queue
        , 1, &submitInfo, regulator.AccessFence(setting.CurrentFrame()));
    ErrorManager::Validate(result, "Drawing");
}

const VkCommandBuffer* CommandManager::Access(const CommandInfo& info) const {
    if(commands_.find(info.type) == commands_.end()) {
        ErrorManager::Validate(UNSOLVABLE, "This type of command buffers "\
            "weren't allocated", "Command manager access");
        return nullptr;
    }
    return commands_.find(info.type)->second.commandBuffers.data() + info.offset;
}

const CommandBundle& CommandManager::Access(const commandType type) const {
    if(commands_.find(type) == commands_.end()) {
        ErrorManager::Validate(UNSOLVABLE, "This type of command buffers "\
            "weren't allocated", "Command manager access");
        return commands_.end()->second;
    }
    return commands_.find(type)->second;
}

void CommandManager::FreeCommandBuffers(const Device& device, commandType type) {
    CommandBundle& bundle = commands_.find(type)->second;
    vkFreeCommandBuffers(device.Access(), bundle.commandPool.Access()
        , bundle.commandBuffers.size(), bundle.commandBuffers.data());
}

void CommandManager::FreeCommandBuffers(const Device& device, const CommandInfo& info) {
    CommandBundle& bundle = commands_.find(info.type)->second;
    vkFreeCommandBuffers(device.Access(), bundle.commandPool.Access()
        , info.bufferCount, bundle.commandBuffers.data() + info.offset);
}

void CommandManager::DestroyCommandPool(const Device& device, commandType type) {
    CommandPool& commandPool = commands_.find(type)->second.commandPool;
    commandPool.Destroy(device);
    commands_.erase(type);
}

void CommandManager::Destroy(const Device& device) {
    for(auto& pair : commands_) {
        pair.second.commandPool.Destroy(device);
    }
}

} //vk

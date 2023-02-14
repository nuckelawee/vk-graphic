#include "vk/vk_data_loader.hpp"

namespace vk {

BufferInfo DataLoader::LoadData(const Device& device, const DataInfo& info
    , CommandManager& commandManager, bool useStagingBuffer) {

    VkBufferUsageFlags bufferFlags;
    VkMemoryPropertyFlags memoryFlags;
    switch(info.type) { 
    case VERTEX:
        bufferFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        useStagingBuffer = true;
        break;
    case INDEX:
        bufferFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
        memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        useStagingBuffer = true;
        break;
    case UNIFORM:
        //CreateUniformBuffer(device, size);
    default:
        ErrorManager::Validate(WARNING, "Trying to create unsupported "\
            "buffer", "Data loading");
        BufferInfo bufferInfo {};
        return bufferInfo;
    }
    VkBuffer buffer;
    VkDeviceMemory memory;
    Create(device, buffer, memory, info.pData, info.elementSize * info.elementCount
        , *(commandManager.Access(copyCommands_))
        , bufferFlags, memoryFlags, useStagingBuffer);

    auto pair = data_.find(info.type);
    if(pair == data_.end()) {
        data_.emplace(info.type, BufferData());
        pair = data_.find(info.type);
    }
    pair->second.buffers.push_back(buffer);
    pair->second.memory.push_back(memory);
    
    BufferInfo bufferInfo {};
    bufferInfo.type = info.type;
    bufferInfo.elementCount = info.elementCount;
    bufferInfo.bufferIndex = pair->second.infos.size();
    pair->second.infos.push_back(bufferInfo);
    return bufferInfo;
}

const BufferData& DataLoader::Access(bufferType type) {
    auto buffers = data_.find(type);
    if(buffers == data_.end()) {
        ErrorManager::Validate(WARNING, "This type of buffers doesn't exist"
            , "Data loader access");
        return buffers->second;
    }
    return buffers->second;
}

void DataLoader::Begin(const Device& device, CommandManager& commandManager) {
    copyCommands_ = commandManager.Allocate(device, CommandInfo(TRANSFER, 1));
}

void DataLoader::Begin(const CommandInfo& copyCommands) {
    copyCommands_ = copyCommands;
}

void DataLoader::End(const Device& device, CommandManager& commandManager) {
    commandManager.FreeCommandBuffers(device, copyCommands_);
}

void DataLoader::Destroy(const Device& device) {
    for(auto& pair : data_) {
        for(size_t i = 0; i < pair.second.buffers.size(); i++) {
            vkDestroyBuffer(device.Access(), pair.second.buffers[i], nullptr); 
            vkFreeMemory(device.Access(), pair.second.memory[i], nullptr);
        }
    }    
}

void DataLoader::CreateBuffer(const Device& device, VkDeviceSize size
    , VkBufferUsageFlags bufferFlags, VkMemoryPropertyFlags memoryFlags
    , VkBuffer& buffer, VkDeviceMemory& memory) {

    QueueFamilies queues = device.AccessQueues(); 
    uint32_t indexCount = 2;
    uint32_t pQueueIndices[indexCount];
    pQueueIndices[0] = queues.graphic.index.value();
    if(queues.graphic.index.value() == queues.transfer.index.value()) {
        indexCount = 1;
    } else {
        pQueueIndices[1] = queues.transfer.index.value();
    }

    VkBufferCreateInfo bufferInfo {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = size;
    bufferInfo.usage = bufferFlags;
    bufferInfo.sharingMode = VK_SHARING_MODE_CONCURRENT;
    bufferInfo.queueFamilyIndexCount = indexCount;
    bufferInfo.pQueueFamilyIndices = pQueueIndices;

    VkResult result = vkCreateBuffer(device.Access(), &bufferInfo, nullptr
        , &buffer);
    ErrorManager::Validate(result, "Vertex buffer creation");

    AllocateMemory(device, buffer, memoryFlags, memory);
    vkBindBufferMemory(device.Access(), buffer, memory, 0);
}

void DataLoader::AllocateMemory(const Device& device, VkBuffer& buffer
    , VkMemoryPropertyFlags propertyFlags, VkDeviceMemory& memory) {

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device.Access(), buffer, &memoryRequirements);
    int32_t memoryTypeIndex = device.FindMemoryProperties(
        memoryRequirements.memoryTypeBits, propertyFlags); 
    ErrorManager::Validate(Error(UNSOLVABLE, memoryTypeIndex == -1)
        , "Failed to find suitable memory type"
        , "Vertex buffer memory allocation");
    
    VkMemoryAllocateInfo allocateInfo {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;

    VkResult result = vkAllocateMemory(device.Access(), &allocateInfo, nullptr
        , &memory);
    ErrorManager::Validate(result, "Vertex buffer memory allocation");
}

void DataLoader::CopyBuffer(const Device& device, const VkCommandBuffer& commandBuffer
    , VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size) {

    VkCommandBufferBeginInfo beginInfo {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);
    
    VkBufferCopy copyInfo {};
    copyInfo.srcOffset = 0;
    copyInfo.dstOffset = 0;
    copyInfo.size = size;
    vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyInfo);
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO; 
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(device.AccessQueues().transfer.queue, 1
        , &submitInfo, VK_NULL_HANDLE);
    vkQueueWaitIdle(device.AccessQueues().transfer.queue);
}

void DataLoader::CreateUniformBuffer(const Device& device, VkBuffer& buffer
    , VkDeviceMemory& memory, VkDeviceSize size) {
    VkBufferUsageFlags bufferFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT; 
    VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
    CreateBuffer(device, size, bufferFlags, memoryFlags, buffer, memory);
}

/*
void DataLoader::MapMemory(const Device& device, VkDeviceSize size, void *ptr)
{ vkMapMemory(device.Access(), memory, 0, size, 0, &ptr); }
*/

void DataLoader::CreateWithStagingBuffer(const Device& device, VkBuffer& buffer
    , VkDeviceMemory& memory, const void *pData
    , VkDeviceSize size, const VkCommandBuffer& commandBuffer
    , VkBufferUsageFlags bufferFlags, VkMemoryPropertyFlags memoryFlags) {

    bufferFlags = bufferFlags | VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VkBufferUsageFlags stagingBufferFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags stagingMemoryFlags = 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
      | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    CreateBuffer(device, size, stagingBufferFlags, stagingMemoryFlags
        , stagingBuffer, stagingMemory);

    void* pTempPointer;
    vkMapMemory(device.Access(), stagingMemory, 0, size, 0, &pTempPointer);
    memcpy(pTempPointer, pData, static_cast<size_t>(size));
    vkUnmapMemory(device.Access(), stagingMemory);

    CreateBuffer(device, size, bufferFlags, memoryFlags, buffer, memory);
    CopyBuffer(device, commandBuffer, stagingBuffer, buffer, size);
    
    vkDestroyBuffer(device.Access(), stagingBuffer, nullptr);
    vkFreeMemory(device.Access(), stagingMemory, nullptr);
}

void DataLoader::Create(const Device& device, VkBuffer& buffer
    , VkDeviceMemory& memory, const void *pData
    , VkDeviceSize size, const VkCommandBuffer& commandBuffer
    , VkBufferUsageFlags bufferFlags, VkMemoryPropertyFlags memoryFlags
    , bool useStagingBuffer) {

    if(useStagingBuffer) {
        CreateWithStagingBuffer(device, buffer, memory, pData, size
            , commandBuffer, bufferFlags, memoryFlags);
        return;
    }
    CreateBuffer(device, size, bufferFlags, memoryFlags, buffer, memory);
    void* pTempPointer;
    vkMapMemory(device.Access(), memory, 0, size, 0, &pTempPointer);
    memcpy(pTempPointer, pData, static_cast<size_t>(size));
    vkUnmapMemory(device.Access(), memory);
}

} // vk

#include "vk/vk_buffer_loader.hpp"

namespace vk {

SentDataInfo BufferLoader::Load(const Device& device
    , const LoadBufferInfo& loadInfo, const VkCommandBuffer& commandBuffer) {

    SentDataInfo sentInfo;
    sentInfo.type = loadInfo.type;
    sentInfo.descriptor = buffers_.size();

    VkBuffer buffer;
    VkDeviceMemory memory;
    void *pBufferMapped;
    VkDeviceSize size = loadInfo.memorySize;

    if(loadInfo.useStagingBuffer) {
        CreateWithStagingBuffer(device, buffer, memory, loadInfo.pData, size
            , commandBuffer, loadInfo.usageFlags, loadInfo.memoryFlags);
    } else {
        CreateBuffer(device, size, loadInfo.usageFlags
            , loadInfo.memoryFlags, buffer, memory);
        void* pTempPointer;
        vkMapMemory(device.Access(), memory, 0, size, 0, &pTempPointer);
        memcpy(pTempPointer, loadInfo.pData, static_cast<size_t>(size));
        vkUnmapMemory(device.Access(), memory);
    }
    
    vkMapMemory(device.Access(), memory, 0, size, 0, &pBufferMapped);
    buffers_.push_back(buffer);

    memory_.push_back(memory);
    buffersMapped_.push_back(pBufferMapped); 

    return sentInfo;
}

void BufferLoader::CreateBuffer(const Device& device, VkDeviceSize size
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

void BufferLoader::AllocateMemory(const Device& device, VkBuffer& buffer
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

void BufferLoader::CopyBuffer(const Device& device, const VkCommandBuffer& commandBuffer
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

void BufferLoader::CreateWithStagingBuffer(const Device& device, VkBuffer& buffer
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

const VkBuffer& BufferLoader::AccessBuffer(const SentDataInfo& sentInfo) const {
    return buffer_[sentInfo.descriptor];
}
const VkDeviceMemory& BufferLoader::AccessMemory(const SentDataInfo& sentInfo) const {
    return memory_[sentInfo.descriptor];
}
const void* BufferLoader::AccessMappedBuffer(const SentDataInfo& sentInfo) const {
    return buffersMapped_[sentInfo.descriptor]; 
}

} // vk

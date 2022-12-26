#include "vk/vk_buffer.hpp"

namespace vk {

void Buffer::CreateBuffer(const Device& device, VkDeviceSize size
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

void Buffer::AllocateMemory(const Device& device, VkBuffer& buffer
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

void Buffer::CopyBuffer(const Device& device, CommandBuffers& commandBuffers
    , VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size) {
/*
    VkCommandBufferAllocateInfo allocateInfo {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocateInfo.commandPool = commandPool.Access();
    allocateInfo.commandBufferCount = 1;

    VkResult result = vkAllocateCommandBuffers(device.Access()
        , &allocateInfo, &commandBuffer);
    ErrorManager::Validate(result, "Buffer coping");
*/
    VkCommandBuffer& commandBuffer = *(commandBuffers.AccessTransfer());


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

void Buffer::Create(const Device& device, CommandBuffers& commandBuffers
    , BufferType type, const void *pData, VkDeviceSize size) {    

    VkBufferUsageFlags bufferFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    CreateBuffer(device, size, bufferFlags, memoryFlags
        , stagingBuffer, stagingMemory);

    void* pTempPointer;
    vkMapMemory(device.Access(), stagingMemory, 0, size, 0, &pTempPointer);
    memcpy(pTempPointer, pData, static_cast<size_t>(size));
    vkUnmapMemory(device.Access(), stagingMemory);

    switch(type) { 
    case VERTEX_BUFFER:
        bufferFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
            | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
        break;
    default:
        return;
    }

    CreateBuffer(device, size, bufferFlags, memoryFlags, buffer_, memory_);
    CopyBuffer(device, commandBuffers, stagingBuffer, buffer_, size);
    
    vkDestroyBuffer(device.Access(), stagingBuffer, nullptr);
    vkFreeMemory(device.Access(), stagingMemory, nullptr);
}

void Buffer::Destroy(const Device& device) {
    vkDestroyBuffer(device.Access(), buffer_, nullptr); 
    vkFreeMemory(device.Access(), memory_, nullptr);
}

} //vk

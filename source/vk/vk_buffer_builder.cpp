#include "vk/vk_buffer_builder.hpp"
#include "vk/vk_command_manager.hpp"

namespace vk {

void BufferBuilder::Init(const Device& device) {
    device_ = device;
    commandPool_ = CommandManager::CreatePool(device_.Access()
        , device_.AccessQueues().transfer.index.value()
        , VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
}

void BufferBuilder::Destroy() {
    vkDestroyCommandPool(device_.Access(), commandPool_, nullptr);
}

Buffer BufferBuilder::BuildBuffer(const void *data, VkDeviceSize size
    , VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags) {

    VkBuffer buffer;
    VkDeviceMemory memory;
    if(data == nullptr) {
        CreateBuffer(buffer, memory, size, usageFlags, memoryFlags);
        return { buffer, memory };
    }
    if(memoryFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
        CreateDeviceLocalBuffer(buffer, memory, data, size
            , usageFlags, memoryFlags);
        return { buffer, memory };
    }
    CreateBuffer(buffer, memory, size, usageFlags, memoryFlags);

    void *mappedData;
    vkMapMemory(device_.Access(), memory, 0, size, 0, &mappedData);
    memcpy(mappedData, data, static_cast<size_t>(size));
    vkUnmapMemory(device_.Access(), memory);
    return { buffer, memory };
}

void BufferBuilder::CreateBuffer(VkBuffer& buffer, VkDeviceMemory& memory
    , VkDeviceSize buffSize, VkBufferUsageFlags usageFlags
    , VkMemoryPropertyFlags memoryFlags) {

    QueueFamilies queues = device_.AccessQueues(); 
    uint32_t indexCount = 2;
    uint32_t queueIndices[indexCount];
    queueIndices[0] = queues.graphic.index.value();
    VkSharingMode sharingMode;

    if(queues.graphic.index.value() == queues.transfer.index.value()) {
        indexCount = 1;
        sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    } else {
        queueIndices[1] = queues.transfer.index.value();
        sharingMode = VK_SHARING_MODE_CONCURRENT;
    }

    VkBufferCreateInfo bufferInfo {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = buffSize;
    bufferInfo.usage = usageFlags;
    bufferInfo.sharingMode = sharingMode;
    bufferInfo.queueFamilyIndexCount = indexCount;
    bufferInfo.pQueueFamilyIndices = queueIndices;

    VkResult result = vkCreateBuffer(device_.Access(), &bufferInfo, nullptr
        , &buffer);
    ErrorManager::Validate(result, "Vertex buffer creation");

    AllocateBuffer(buffer, memory, memoryFlags);
    vkBindBufferMemory(device_.Access(), buffer, memory, 0);
}

void BufferBuilder::AllocateBuffer(VkBuffer& buffer, VkDeviceMemory& memory
    , VkMemoryPropertyFlags flags) {

    VkMemoryRequirements memoryRequirements;
    vkGetBufferMemoryRequirements(device_.Access(), buffer, &memoryRequirements);

    int32_t memoryTypeIndex = device_.FindMemoryProperties(
        memoryRequirements.memoryTypeBits, flags); 
    ErrorManager::Validate(Error(ERROR_TYPE_UNSOLVABLE, memoryTypeIndex == -1)
        , "Failed to find suitable memory type"
        , "Vertex buffer memory allocation");
    
    VkMemoryAllocateInfo allocateInfo {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirements.size;
    allocateInfo.memoryTypeIndex = memoryTypeIndex;

    VkResult result = vkAllocateMemory(device_.Access(), &allocateInfo, nullptr
        , &memory);
    ErrorManager::Validate(result, "Vertex buffer memory allocation");
}

void BufferBuilder::CreateDeviceLocalBuffer(VkBuffer& buffer
    , VkDeviceMemory& memory, const void *data
    , VkDeviceSize size, VkBufferUsageFlags usageFlags
    , VkMemoryPropertyFlags memoryFlags) {

    usageFlags |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

    VkBufferUsageFlags stagingUsageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    VkMemoryPropertyFlags stagingMemoryFlags = 
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
      | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    CreateBuffer(stagingBuffer, stagingMemory, size, stagingUsageFlags
        , stagingMemoryFlags);

    void* tempPointer;
    vkMapMemory(device_.Access(), stagingMemory, 0, size, 0, &tempPointer);
    memcpy(tempPointer, data, static_cast<size_t>(size));
    vkUnmapMemory(device_.Access(), stagingMemory);

    CreateBuffer(buffer, memory, size, usageFlags, memoryFlags);
    CopyBuffer(stagingBuffer, buffer, size);
    
    vkDestroyBuffer(device_.Access(), stagingBuffer, nullptr);
    vkFreeMemory(device_.Access(), stagingMemory, nullptr);
}

void BufferBuilder::CopyBuffer(VkBuffer& source, VkBuffer& destination
    , VkDeviceSize size) {

    VkCommandBuffer copyCommand = CommandManager::BeginSingleCommand(
        device_.Access(), commandPool_);

    VkBufferCopy copy {};
    copy.size = size;
    vkCmdCopyBuffer(copyCommand, source, destination, 1, &copy);

    CommandManager::EndSingleCommand(device_.Access(), commandPool_
        , device_.AccessQueues().transfer.queue, copyCommand);
}

} // vk

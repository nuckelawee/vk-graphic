#include "vk/vk_data_loader.hpp"
#include "vk/vk_command_manager.hpp"

namespace vk {

BufferInfo DataLoader::LoadData(const Device& device
    , CommandManager& commandManager, const DataInfo& dataInfo
    , bool useStagingBuffer) {

    VkBuffer buffer;
    VkDeviceMemory memory;
    void *pBufferMapped;
    
    VkDeviceSize bufferMemorySize = dataInfo.elementSize * dataInfo.elementCount;
    switch(dataInfo.type) { 
    case BUFFER_TYPE_VERTEX:
        Create(device, buffer, memory, dataInfo.pData, bufferMemorySize
            , *(commandManager.Access(copyCommands_))
            , VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
            , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);
        break;
    case BUFFER_TYPE_INDEX:
        Create(device, buffer, memory, dataInfo.pData, bufferMemorySize
            , *(commandManager.Access(copyCommands_))
            , VK_BUFFER_USAGE_INDEX_BUFFER_BIT
            , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);
        break;
    case BUFFER_TYPE_UNIFORM:
        CreateBuffer(device, bufferMemorySize
        , VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
        , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
        | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer, memory);
        break;
    case BUFFER_TYPE_COMPLEX:
        ErrorManager::Validate(WARNING, "Complex buffer must be created "\
            "by a function "\
            "BufferInfo DataLoader::LoadComplexData(const Device& device "\
            ", CommandManager& commandManager, const *pDataInfo infos "\
            ", size_t infoCount, ObjectInfo *pObjects) ", "Data loading");

        Create(device, buffer, memory, dataInfo.pData, bufferMemorySize
            , *(commandManager.Access(copyCommands_))
            , VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT
            , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, true);
        break;
    default:
        ErrorManager::Validate(WARNING, "Trying to create unsupported "\
            "buffer", "Data loading");
        break;
    }

    vkMapMemory(device.Access(), memory, 0, bufferMemorySize, 0, &pBufferMapped);

    BufferInfo bufferInfo {};
    bufferInfo.type = dataInfo.type;
    bufferInfo.elementCount = dataInfo.elementCount;

    PushData(buffer, memory, bufferInfo, pBufferMapped);
    return bufferInfo;
}

void DataLoader::PushData(const VkBuffer& buffer, const VkDeviceMemory& memory
    , BufferInfo& info, void *pBufferMapped) {

    auto pair = data_.find(info.type);
    if(pair == data_.end()) {
        data_.emplace(info.type, BufferData());
        pair = data_.find(info.type);
    }
    info.bufferIndex = pair->second.infos.size();
    pair->second.buffers.push_back(buffer);
    pair->second.memory.push_back(memory);
    pair->second.infos.push_back(info);
    pair->second.buffersMapped.push_back(pBufferMapped);
}

void DataLoader::ShiftIndexes(DataInfo& info, uint16_t vertexShift) {
    uint16_t *ptr = static_cast<uint16_t*>(info.pData);
    for(size_t i = 0; i < info.elementCount; i++) {
        ptr[i] += vertexShift;
    }
}

VkDeviceSize DataLoader::MergeData(DataInfo **pInfos, unsigned char **pData
    , ObjectInfo *pObjects, BufferInfo& bufferInfo, size_t objectCount) {

    uint16_t vertexElementCount = 0;
    uint16_t indexElementCount = 0;
    VkDeviceSize vertexMemoryShift = 0;
    VkDeviceSize indexMemoryShift = 0;
    for(size_t i = 0; i < objectCount; i++) {
        ShiftIndexes(*(pInfos[i+objectCount]), vertexElementCount);

        vertexMemoryShift += pInfos[i]->elementCount * pInfos[i]->elementSize;
        vertexElementCount += pInfos[i]->elementCount;

        pObjects[i].indexShift = indexElementCount;
        pObjects[i].indexCount = pInfos[i+objectCount]->elementCount;

        indexElementCount += pInfos[i+objectCount]->elementCount;
        indexMemoryShift += pInfos[i+objectCount]->elementCount 
            * pInfos[i]->elementSize;
    }

    VkDeviceSize memoryShift = vertexMemoryShift + indexMemoryShift;
    *pData = new unsigned char[memoryShift];
    memoryShift = 0;
    
    for(size_t i = 0; i < 2*objectCount; i++) {
        memcpy(*pData + memoryShift, pInfos[i]->pData
            , pInfos[i]->elementCount * pInfos[i]->elementSize);
        memoryShift += pInfos[i]->elementCount * pInfos[i]->elementSize;
    }

    bufferInfo.type = BUFFER_TYPE_COMPLEX;
    bufferInfo.elementCount = vertexElementCount + indexElementCount;
    bufferInfo.indexMemoryShift = vertexMemoryShift;
    bufferInfo.pObjectInfos = pObjects;
    bufferInfo.objectCount = objectCount;
    return memoryShift;
}

BufferInfo DataLoader::LoadComplexData(const Device& device
    , CommandManager& commandManager, DataInfo **pDataInfos
    , ObjectInfo *pObjects, size_t objectCount) {

    unsigned char *pData = nullptr;
    void *pBufferMapped = nullptr;
    BufferInfo bufferInfo {};

    VkDeviceSize memoryShift = MergeData(pDataInfos, &pData, pObjects
        , bufferInfo, objectCount);

    VkBufferUsageFlags bufferFlags = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

    VkBuffer buffer;
    VkDeviceMemory memory;
    Create(device, buffer, memory, pData, memoryShift
        , *(commandManager.Access(copyCommands_))
        , bufferFlags, memoryFlags, true);

    delete[] pData;
    
    bufferInfo.type = BUFFER_TYPE_COMPLEX;
    PushData(buffer, memory, bufferInfo, pBufferMapped);

    return bufferInfo;
}

const BufferData& DataLoader::Access(bufferType type) const {
    auto buffers = data_.find(type);
    if(buffers == data_.end()) {
        ErrorManager::Validate(WARNING, "This type of buffers doesn't exist"
            , "Data loader access");
        return buffers->second;
    }
    return buffers->second;
}

BufferData& DataLoader::Access(bufferType type) {
    auto buffers = data_.find(type);
    if(buffers == data_.end()) {
        ErrorManager::Validate(WARNING, "This type of buffers doesn't exist"
            , "Data loader access");
        return buffers->second;
    }
    return buffers->second;
}

void DataLoader::Begin(const Device& device, CommandManager& commandManager) {
    copyCommands_ = commandManager.Allocate(device
        , CommandInfo(COMMAND_TYPE_TRANSFER, 1));
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

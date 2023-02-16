#pragma once

#include "vk_command_manager.hpp"

namespace vk {

enum bufferType { BUFFER_TYPE_VERTEX, BUFFER_TYPE_INDEX, BUFFER_TYPE_UNIFORM
    , BUFFER_TYPE_COMPLEX };

struct DataInfo {
    void *pData;
    size_t elementCount;
    VkDeviceSize elementSize;
    bufferType type;
};

struct ObjectInfo {
    size_t indexCount;
    size_t indexShift;
};

struct BufferInfo {
    bufferType type;
    size_t elementCount;
    size_t bufferIndex;
    VkDeviceSize indexMemoryShift;
    ObjectInfo *pObjectInfos = nullptr;
    size_t objectCount;
};

struct BufferData {
    std::vector<VkBuffer> buffers;
    std::vector<VkDeviceMemory> memory;
    std::vector<BufferInfo> infos; 
};

class DataLoader {
    std::map<bufferType, BufferData> data_;
    CommandInfo copyCommands_;

public:

    BufferInfo LoadData(const Device& device, CommandManager& commandManager
        , const DataInfo& info, bool useStagingBuffer = true);

    BufferInfo LoadComplexData(const Device& device
        , CommandManager& commandManager, DataInfo **pDataInfos
        , ObjectInfo *pObjects, size_t objectCount);

    const BufferData& Access(bufferType type);

    void Begin(const Device& device, CommandManager& commandManager);
    void Begin(const CommandInfo& copyCommands);
    
    void End(const Device& device, CommandManager& commandManager);

    void Destroy(const Device& device);

    DataLoader() {}
    ~DataLoader() {}

private:

    void ShiftIndexes(DataInfo& info, uint16_t vertexShift);

    VkDeviceSize MergeData(DataInfo **pInfos, unsigned char **pData
        , ObjectInfo *pObjects, BufferInfo& bufferInfo, size_t objectCount);

    void CreateBuffer(const Device& device, VkDeviceSize size
        , VkBufferUsageFlags bufferFlags, VkMemoryPropertyFlags memoryFlags
        , VkBuffer& buffer, VkDeviceMemory& memory);

    void AllocateMemory(const Device& device, VkBuffer& buffer
        , VkMemoryPropertyFlags memoryFlags, VkDeviceMemory& memory);

    void CopyBuffer(const Device& device, const VkCommandBuffer& commandBuffer
        , VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);

    void CreateWithStagingBuffer(const Device& device, VkBuffer& buffer
        , VkDeviceMemory& memory, const void *pData
        , VkDeviceSize size, const VkCommandBuffer& commandBuffer
        , VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags);

    void Create(const Device& device, VkBuffer& buffer, VkDeviceMemory& memory
        , const void *pData, VkDeviceSize size
        , const VkCommandBuffer& commandBuffer, VkBufferUsageFlags usageFlags
        , VkMemoryPropertyFlags memoryFlags, bool useStagingBuffer);

    void CreateUniformBuffer(const Device& device, VkBuffer& buffer
        , VkDeviceMemory& memory, VkDeviceSize size);

 
};

} // vk

#pragma once

#include "vk_command_manager.hpp"

namespace vk {

enum bufferType { VERTEX, INDEX, UNIFORM };

struct DataInfo {
    const void *pData;
    size_t elementCount;
    size_t elementSize;
    bufferType type;
};

struct BufferInfo {
    bufferType type;
    size_t elementCount;
    size_t bufferIndex;
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

    BufferInfo LoadData(const Device& device, const DataInfo& info
        , CommandManager& commandManager, bool useStagingBuffer = true);

    const BufferData& Access(bufferType type);

    void Begin(const Device& device, CommandManager& commandManager);
    void Begin(const CommandInfo& copyCommands);
    
    void End(const Device& device, CommandManager& commandManager);

    void Destroy(const Device& device);

    DataLoader() {}
    ~DataLoader() {}

private:

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

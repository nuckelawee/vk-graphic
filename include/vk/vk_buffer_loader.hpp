#pragma once

namespace vk {

struct LoadBufferInfo {
    LoadDataType type;
    const void *pData; 
    VkDeviceSize memorySize;
    VkBufferUsageFlags usageFlags;
    VkMemoryPropertyFlags memoryFlags;
    bool useStagingBuffer;
};

class BufferLoader {
    std::vector<VkBuffer> buffer_;
    std::vector<VkDeviceMemory> memory_;
    std::vector<void*> buffersMapped_;

private:
    void CreateBuffer(const Device& device, VkDeviceSize size
        , VkBufferUsageFlags bufferFlags, VkMemoryPropertyFlags memoryFlags
        , VkBuffer& buffer, VkDeviceMemory& memory);

    void AllocateMemory(const Device& device, VkBuffer& buffer
        , VkMemoryPropertyFlags propertyFlags, VkDeviceMemory& memory);

    void CopyBuffer(const Device& device, const VkCommandBuffer& commandBuffer
        , VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);

    void CreateWithStagingBuffer(const Device& device, VkBuffer& buffer
        , VkDeviceMemory& memory, const void *pData
        , VkDeviceSize size, const VkCommandBuffer& commandBuffer
        , VkBufferUsageFlags bufferFlags, VkMemoryPropertyFlags memoryFlags);

    void LoadBuffer(const Device& device, VkBuffer& buffer
        , VkDeviceMemory& memory, const void *pData
        , VkDeviceSize size, const VkCommandBuffer& commandBuffer
        , VkBufferUsageFlags bufferFlags, VkMemoryPropertyFlags memoryFlags
        , bool useStagingBuffer);

public:

    SentDataInfo Load(const Device& device, const LoadBufferInfo& loadInfo
        , const VkCommandBuffer& commandBuffer);

    const VkBuffer& AccessBuffer(const SentDataInfo& sentInfo) const;
    const VkDeviceMemory& AccessMemory(const SentDataInfo& sentInfo) const;
    const void* AccessMappedBuffer(const SentDataInfo& sentInfo) const;

    BufferLoader() {}
    ~BufferLoader() {}

};

} // vk

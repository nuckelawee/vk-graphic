#pragma once

#include "vk_device.hpp"

namespace vk {

struct Buffer {
    VkBuffer buffer;
    VkDeviceMemory memory;

    void Destroy(VkDevice device);
};

class BufferBuilder {
    
    Device device_;
    VkCommandPool commandPool_;

private:

    void CreateDeviceLocalBuffer(VkBuffer& buffer, VkDeviceMemory& memory
        , const void *data, VkDeviceSize size, VkBufferUsageFlags usageFlags
        , VkMemoryPropertyFlags memoryFlags);

    void CreateBuffer(VkBuffer& buffer, VkDeviceMemory& memory, VkDeviceSize buffSize
        , VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags);

    void AllocateBuffer(VkBuffer& buffer, VkDeviceMemory& memory
        , VkMemoryPropertyFlags flags);

    void CopyBuffer(VkBuffer& source, VkBuffer& destination, VkDeviceSize size);

public:
    
    Buffer BuildBuffer(const void *data, VkDeviceSize size
        , VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryFlags);

    void Init(const Device& device);

    void Destroy();

    BufferBuilder() {}
    ~BufferBuilder() {}
};

} // vk

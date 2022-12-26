#pragma once

#include "vk_command_buffers.hpp"
#include "vk_vertex.hpp"

namespace vk {

class CommandBuffers;

enum BufferType { VERTEX_BUFFER };

class Buffer {

    VkBuffer buffer_;
    VkDeviceMemory memory_;

private:

    void CreateBuffer(const Device& device, VkDeviceSize size
        , VkBufferUsageFlags bufferFlags, VkMemoryPropertyFlags memoryFlags
        , VkBuffer& buffer, VkDeviceMemory& memory);

    void AllocateMemory(const Device& device, VkBuffer& buffer
        , VkMemoryPropertyFlags memoryFlags, VkDeviceMemory& memory);

    void CopyBuffer(const Device& device, CommandBuffers& commandBuffers
        , VkBuffer& srcBuffer, VkBuffer& dstBuffer, VkDeviceSize size);

public:

    void Create(const Device& device, CommandBuffers& commandBuffers
        , BufferType type, const void *pData, VkDeviceSize size);

    void Destroy(const Device& device);

    VkBuffer& Access() { return buffer_; }

    Buffer() {}
    ~Buffer() {}

};

} // vk

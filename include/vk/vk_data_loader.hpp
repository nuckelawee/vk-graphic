#pragma once

#include <map>

#include "vk_device.hpp"
#include "vk_command_pool.hpp"
#include "file_manager.hpp"
#include "vk_vertex_binding.hpp"

namespace vk {

class CommandManager;
class Swapchain;

struct CommandInfo {
    commandType type;
    size_t bufferCount;
    size_t offset = 0;

    CommandInfo() {}
    CommandInfo(commandType nType, size_t nBufferCount = 0, size_t nOffset = 0) 
        : type(nType), bufferCount(nBufferCount), offset(nOffset) {}
};

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
    std::vector<void*> buffersMapped;
};

struct Image {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView imageView;
    VkSampler sampler;
};

class DataLoader {
    std::map<bufferType, BufferData> data_;
    std::vector<Image> images_; 
    CommandInfo copyCommands_;

    Image depthImage_;

public:

    void CreateDepthImage(const Device& device, const Swapchain& swapchain
        , CommandManager& commandManager);

    void LoadModel(const Device& device, const char* filepath
        , std::vector<Vertex3D>& vertices, std::vector<uint32_t>& indices);

    void LoadTexture(const Device& device, CommandManager& commandManager
        , const char* filepath);

    BufferInfo LoadData(const Device& device, CommandManager& commandManager
        , const DataInfo& info, bool useStagingBuffer = true);

    BufferInfo LoadComplexData(const Device& device
        , CommandManager& commandManager, DataInfo **pDataInfos
        , ObjectInfo *pObjects, size_t objectCount);

    const BufferData& Access(bufferType type) const;
    BufferData& Access(bufferType type);
    
    const Image& AccessImage() const;
    Image& AccessImage();

    const Image& AccessDepthImage() const;
    Image& AccessDepthImage();

    void Begin(const Device& device, CommandManager& commandManager);
    void Begin(const CommandInfo& copyCommands);
    
    void End(const Device& device, CommandManager& commandManager);

    void Destroy(const Device& device);

    DataLoader() {}
    ~DataLoader() {}

private:

    VkSampler CreateSampler(const Device& device) const;
    
    VkImageView CreateImageView(const Device& device, VkImage image
        , VkFormat format, VkImageAspectFlags aspectFlags) const;

    void CreateTexture(const Device& device, VkImage& image, VkDeviceMemory& memory
        , const Texture& texture, VkImageTiling tiling
        , VkImageUsageFlags usage, VkMemoryPropertyFlags properties) const;

    void TransitionImageLayout(const Device& device, CommandManager& commandManager
        , VkImage image, VkFormat format, VkImageLayout oldLayout
        , VkImageLayout newLayout);

    void PushData(const VkBuffer& buffer, const VkDeviceMemory& memory
        , BufferInfo& info, void *pBufferMapped);

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

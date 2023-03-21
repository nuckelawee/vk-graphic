#include "vk/vk_data_loader.hpp"
#include "vk/vk_command_manager.hpp"
#include "vk/vk_swapchain.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace vk {

void DataLoader::LoadModel(const Device& device, const char* filepath
    , std::vector<Vertex3D>& vertices, std::vector<uint32_t>& indices) {

    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnings, errors;

    bool result = tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings
        , &errors, filepath);
    ErrorManager::Validate( { ERROR_TYPE_UNKNOWN, result }, warnings, errors);

    for(const auto& shape : shapes) {
        for(const auto& index : shape.mesh.indices) {
            Vertex3D vertex;
            vertex.pos = {
                attributes.vertices[3 * index.vertex_index + 0],
                attributes.vertices[3 * index.vertex_index + 1],
                attributes.vertices[3 * index.vertex_index + 2],
            };
            vertex.texPos = {
                attributes.texcoords[2 * index.texcoord_index + 0], 
                1.0f - attributes.texcoords[2 * index.texcoord_index + 1], 
            };
            vertex.color = { 1.0f, 1.0f, 1.0f };

            vertices.push_back(vertex);
            indices.push_back(indices.size());
        }
    }

}

void DataLoader::CreateDepthImage(const Device& device
    , const Swapchain& swapchain, CommandManager& commandManager) {

    VkFormat depthFormat = device.FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT
        , VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL
        , VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    Texture texture;
    texture.format = depthFormat;
    texture.width = swapchain.AccessExtent().width;
    texture.height = swapchain.AccessExtent().height;

    CreateTexture(device, depthImage_.image, depthImage_.memory, texture
        , VK_IMAGE_TILING_OPTIMAL
        , VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
        , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    depthImage_.imageView = CreateImageView(device, depthImage_.image
        , depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
/*
    TransitionImageLayout(device, commandManager, depthImage_.image, depthFormat
        , VK_IMAGE_LAYOUT_UNDEFINED
        , VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
*/
}

void DataLoader::LoadTexture(const Device& device, CommandManager& commandManager
    , const char *filepath) {

    Texture texture;
    FileResult result = FileManager::ReadImageTga(texture, filepath);
    ErrorManager::Validate(result, "Texture loading");
    VkDeviceSize imageSize = texture.width * texture.height * texture.bytesPerPixel;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    CreateBuffer(device, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
            , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT
            , stagingBuffer, stagingMemory);

    void *data;
    vkMapMemory(device.Access(), stagingMemory, 0, imageSize, 0, &data);
    memcpy(data, texture.pixels, imageSize);
    vkUnmapMemory(device.Access(), stagingMemory);

    VkImage image;
    VkDeviceMemory memory;

    CreateTexture(device, image, memory, texture, VK_IMAGE_TILING_OPTIMAL
        , VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
        , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    TransitionImageLayout(device, commandManager, image, texture.format
            , VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    commandManager.CopyBufferToImage(device, stagingBuffer, image, texture.width
            , texture.height);
    TransitionImageLayout(device, commandManager, image, texture.format
            , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    vkDestroyBuffer(device.Access(), stagingBuffer, nullptr);
    vkFreeMemory(device.Access(), stagingMemory, nullptr);

    VkImageView imageView = CreateImageView(device, image, texture.format
        , VK_IMAGE_ASPECT_COLOR_BIT);
    VkSampler sampler = CreateSampler(device);

    images_.push_back( { image, memory, imageView, sampler } );
    delete [] texture.pixels;
}

VkSampler DataLoader::CreateSampler(const Device& device) const {
    VkPhysicalDeviceProperties properties {};
    vkGetPhysicalDeviceProperties(device.AccessGpu(), &properties);

    VkSamplerCreateInfo samplerInfo {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    VkSampler sampler;
    VkResult result = vkCreateSampler(device.Access(), &samplerInfo, nullptr, &sampler);
    ErrorManager::Validate(result, "Sampler creation");
    return sampler;
}

VkImageView DataLoader::CreateImageView(const Device& device, VkImage image
    , VkFormat format, VkImageAspectFlags aspectFlags) const {

    VkImageViewCreateInfo imageViewInfo {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.image = image;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = format;
    imageViewInfo.subresourceRange.aspectMask = aspectFlags;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = 1;

    VkImageView imageView;
    VkResult result = vkCreateImageView(device.Access(), &imageViewInfo, nullptr
            , &imageView);
    ErrorManager::Validate(result, "Create image view");
    return imageView;
}

void DataLoader::CreateTexture(const Device& device, VkImage& image
    , VkDeviceMemory& memory, const Texture& texture, VkImageTiling tiling
    , VkImageUsageFlags usage, VkMemoryPropertyFlags properties) const {

    VkImageCreateInfo imageInfo {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = texture.width;
    imageInfo.extent.height = texture.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;
    imageInfo.format = texture.format;
    imageInfo.tiling = tiling;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = usage;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

    VkResult result = vkCreateImage(device.Access(), &imageInfo, nullptr, &image);
    ErrorManager::Validate(result, "Image creation\n");

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device.Access(), image, &memRequirements);

    VkMemoryAllocateInfo allocateInfo {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memRequirements.size;
    allocateInfo.memoryTypeIndex = device.FindMemoryProperties(
            memRequirements.memoryTypeBits, properties);

    result = vkAllocateMemory(device.Access(), &allocateInfo, nullptr
            , &memory);

    ErrorManager::Validate(result, "Image allocation\n");
    vkBindImageMemory(device.Access(), image, memory, 0);

}

void DataLoader::TransitionImageLayout(const Device& device
        , CommandManager& commandManager, VkImage image
        , VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout) {

    VkCommandBuffer commandBuffer = commandManager.BeginSingleCommand(device
        , COMMAND_TYPE_GRAPHICS);

    VkImageMemoryBarrier barrier {};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = oldLayout;
    barrier.newLayout = newLayout;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = image;
    barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    barrier.subresourceRange.baseMipLevel = 0;
    barrier.subresourceRange.levelCount = 1;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags srcStage;
    VkPipelineStageFlags dstStage;

    if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
        && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT
            | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    
        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    } else if(oldLayout == VK_IMAGE_LAYOUT_UNDEFINED
            && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {

        barrier.srcAccessMask = 0;
        barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

        srcStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
        dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
    } else if(oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL
            && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {

        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
        dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
    } else {
        ErrorManager::Validate(ERROR_TYPE_UNSOLVABLE, "Failed to change image layout"
            , "Transition image layout");
    }

    vkCmdPipelineBarrier(commandBuffer, srcStage, dstStage, 0, 0, nullptr
            , 0, nullptr, 1, &barrier);

    commandManager.EndSingleCommand(device, commandBuffer, COMMAND_TYPE_GRAPHICS);
}

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
        ErrorManager::Validate(ERROR_TYPE_WARNING, "Complex buffer must be created "\
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
        ErrorManager::Validate(ERROR_TYPE_WARNING, "Trying to create unsupported "\
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
        ErrorManager::Validate(ERROR_TYPE_WARNING, "This type of buffers doesn't exist"
            , "Data loader access");
        return buffers->second;
    }
    return buffers->second;
}

BufferData& DataLoader::Access(bufferType type) {
    auto buffers = data_.find(type);
    if(buffers == data_.end()) {
        ErrorManager::Validate(ERROR_TYPE_WARNING, "This type of buffers doesn't exist"
            , "Data loader access");
        return buffers->second;
    }
    return buffers->second;
}

const Image& DataLoader::AccessImage() const {
    return images_[0];
}

Image& DataLoader::AccessImage() {
    return images_[0];
}

const Image& DataLoader::AccessDepthImage() const {
    return depthImage_;
}

Image& DataLoader::AccessDepthImage() {
    return depthImage_;
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
    for(auto& image : images_) {
        vkDestroySampler(device.Access(), image.sampler, nullptr);
        vkDestroyImageView(device.Access(), image.imageView, nullptr);
        vkDestroyImage(device.Access(), image.image, nullptr);
        vkFreeMemory(device.Access(), image.memory, nullptr);
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
    ErrorManager::Validate(Error(ERROR_TYPE_UNSOLVABLE, memoryTypeIndex == -1)
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

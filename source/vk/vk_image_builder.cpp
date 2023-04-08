#include "vk/vk_image_builder.hpp"
#include "vk/vk_buffer_builder.hpp"
#include "vk/vk_command_manager.hpp"
#include "vk/vk_image.hpp"
#include "file_manager.hpp"

namespace vk {

void ImageBuilder::Init(const Device& device) {
    device_ = device;
    transferPool_ = CommandManager::CreatePool(device_.Access()
        , device_.AccessQueues().transfer.index.value()
        , VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
    graphicPool_ = CommandManager::CreatePool(device_.Access()
        , device_.AccessQueues().graphic.index.value()
        , VK_COMMAND_POOL_CREATE_TRANSIENT_BIT);
}

void ImageBuilder::Destroy() {
    vkDestroyCommandPool(device_.Access(), transferPool_, nullptr);
    vkDestroyCommandPool(device_.Access(), graphicPool_, nullptr);
}

Image ImageBuilder::BuildDepthImage(const VkExtent2D& extent) {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    VkSampler sampler {};

    VkFormat depthFormat = device_.FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT
        , VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL
        , VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
    Texture texture;
    texture.format = depthFormat;
    texture.width = extent.width;
    texture.height = extent.height;

    CreateImage(image, memory, texture
        , VK_IMAGE_TILING_OPTIMAL
        , VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT
        , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    view = CreateImageView(device_.Access(), image
        , depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);

    return { image, memory, view, sampler };
}

Image ImageBuilder::BuildImage(BufferBuilder& bufferBuilder
    , const Texture& texture) {

    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    VkSampler sampler;

    Buffer staging = bufferBuilder.BuildBuffer(texture.pixels
        , texture.size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    CreateImage(image, memory, texture, VK_IMAGE_TILING_OPTIMAL
        , VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT
        , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    TransitionImageLayout(image, texture.format
            , VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    CopyBufferToImage(staging.buffer, image, texture.width
            , texture.height);
    TransitionImageLayout(image, texture.format
            , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    staging.Destroy(device_.Access());

    view = CreateImageView(device_.Access(), image, texture.format
        , VK_IMAGE_ASPECT_COLOR_BIT);
    sampler = CreateSampler();

    return { image, memory, view, sampler };
}

void ImageBuilder::CreateImage(VkImage& image
    , VkDeviceMemory& memory, const Texture& texture, VkImageTiling tiling
    , VkImageUsageFlags usage, VkMemoryPropertyFlags memoryFlags) {

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
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.sharingMode = sharingMode;
    imageInfo.queueFamilyIndexCount = indexCount;
    imageInfo.pQueueFamilyIndices = queueIndices;

    VkResult result = vkCreateImage(device_.Access(), &imageInfo, nullptr, &image);
    ErrorManager::Validate(result, "Image creation\n");

    AllocateImage(image, memory, memoryFlags);
    vkBindImageMemory(device_.Access(), image, memory, 0);
}

void ImageBuilder::AllocateImage(VkImage& image, VkDeviceMemory& memory
    , VkMemoryPropertyFlags memoryFlags) {

    VkMemoryRequirements memRequirements;
    vkGetImageMemoryRequirements(device_.Access(), image, &memRequirements);

    VkMemoryAllocateInfo allocateInfo {};
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memRequirements.size;
    allocateInfo.memoryTypeIndex = device_.FindMemoryProperties(
            memRequirements.memoryTypeBits, memoryFlags);

    VkResult result = vkAllocateMemory(device_.Access(), &allocateInfo, nullptr
            , &memory);
    ErrorManager::Validate(result, "Image allocation\n");
}

VkImageView ImageBuilder::CreateImageView(VkDevice device, VkImage image
    , VkFormat format, VkImageAspectFlags aspectFlags) {

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
    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

    VkImageView imageView;
    VkResult result = vkCreateImageView(device, &imageViewInfo, nullptr
        , &imageView);
    ErrorManager::Validate(result, "Create image view");
    return imageView;
}



void ImageBuilder::TransitionImageLayout(VkImage& image, VkFormat format
    , VkImageLayout oldLayout, VkImageLayout newLayout) {

    VkCommandBuffer commandBuffer = CommandManager::BeginSingleCommand(
        device_.Access(), graphicPool_);

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

    CommandManager::EndSingleCommand(device_.Access(), graphicPool_
        , device_.AccessQueues().graphic.queue, commandBuffer);
}

VkSampler ImageBuilder::CreateSampler() {
    VkPhysicalDeviceProperties properties {};
    vkGetPhysicalDeviceProperties(device_.AccessGpu(), &properties);

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
    VkResult result = vkCreateSampler(device_.Access(), &samplerInfo, nullptr
        , &sampler);
    ErrorManager::Validate(result, "Sampler creation");
    return sampler;
}

void ImageBuilder::CopyBufferToImage(VkBuffer& buffer
    , VkImage& image, uint32_t width, uint32_t height) {

    VkCommandBuffer commandBuffer = CommandManager::BeginSingleCommand(
        device_.Access(), transferPool_); 

    VkBufferImageCopy region {};
    region.bufferOffset = 0;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    region.imageSubresource.mipLevel = 0;
    region.imageSubresource.baseArrayLayer = 0;
    region.imageSubresource.layerCount = 1;

    region.imageOffset = { 0, 0 };
    region.imageExtent = {
        width,
        height,
        1
    };
    vkCmdCopyBufferToImage(commandBuffer, buffer, image
        , VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

    CommandManager::EndSingleCommand(device_.Access(), transferPool_
        , device_.AccessQueues().transfer.queue, commandBuffer);
}

} //vk

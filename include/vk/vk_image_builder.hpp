#pragma once

#include "vk_device.hpp"

struct Texture;

namespace vk {

class BufferBuilder;

struct Image {
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    VkSampler sampler;
};

class ImageBuilder {

    Device device_;

    VkCommandPool transferPool_;
    VkCommandPool graphicPool_;

private:

    void CreateImage(VkImage& image, VkDeviceMemory& memory, const Texture& texture
        , VkImageTiling tiling, VkImageUsageFlags usageFlags
        , VkMemoryPropertyFlags memoryFlags);

    void AllocateImage(VkImage& image, VkDeviceMemory& memory
        , VkMemoryPropertyFlags memoryFlags);

    void TransitionImageLayout(VkImage& image, VkFormat format
        , VkImageLayout oldLayout, VkImageLayout newLayout);

    VkSampler CreateSampler();

    void CopyBufferToImage(VkBuffer& buffer, VkImage& image, uint32_t width
        , uint32_t height);

public:

    static VkImageView CreateImageView(VkDevice device, VkImage image
        , VkFormat format, VkImageAspectFlags aspectFlags);


    Image BuildDepthImage(const VkExtent2D& extent); 

    Image BuildImage(BufferBuilder& bufferBuilder, const Texture& texture);

    void Init(const Device& device);

    void Destroy();

    ImageBuilder() {}
    ~ImageBuilder() {}
};

} // vk

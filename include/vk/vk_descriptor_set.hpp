#pragma once

#include <vector>
#include <glm/glm.hpp>

#include "vk_settings.hpp"

namespace vk {

class Buffer;
class Image;

class DescriptorSet {

    VkDescriptorSet descriptorSets_[vk::Settings::frames];
    VkDescriptorSetLayout descriptorSetLayout_;
    VkDescriptorPool pool_;
    VkDevice device_;

public:

    void Create(VkDevice device);

    void BindBuffer(const VkDescriptorBufferInfo& bufferInfo, uint32_t index) noexcept;
    void BindImage(const VkDescriptorImageInfo& imageInfo, uint32_t index) noexcept;

    const VkDescriptorSetLayout& AccessLayout() const noexcept;
    VkDescriptorSetLayout& AccessLayout() noexcept;

    const VkDescriptorSet* Access() const noexcept;
    VkDescriptorSet* Access() noexcept;

    void Destroy() noexcept;
private:

    void CreatePool();
    void Allocate();

};

} // vk

#pragma once

#include <glm/glm.hpp>

#include "vk_descriptor_pool.hpp"

struct MvpMatrix {
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 projection;
};

namespace vk {

class Buffer;
class Image;

class DescriptorSet {

    VkDescriptorSetLayout descriptorSetLayout_;
    VkDescriptorSet descriptorSets_[vk::Setting::frames];

public:
    
    void Create(const Device& device, const DescriptorPool& pool
        , Buffer *ubos, Image& image);

    void Destroy(const Device& device, const DescriptorPool& pool);

    const VkDescriptorSetLayout& AccessLayout() const;
    VkDescriptorSetLayout& AccessLayout();

    const VkDescriptorSet* Access() const;
    VkDescriptorSet* Access();

    DescriptorSet() {}
    ~DescriptorSet() {}

private:

    void Allocate(const Device& device, const DescriptorPool& pool);
    void UpdateDescriptorSet(const Device& device, Buffer *ubos, Image& image);

};

} // vk

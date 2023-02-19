#include "vk/vk_descriptor_pool.hpp"

namespace vk {

void DescriptorPool::Create(const Device& device) {
    VkDescriptorPoolSize poolSize {};
    poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSize.descriptorCount = vk::Setting::frames;

    VkDescriptorPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = &poolSize;
    poolInfo.maxSets = vk::Setting::frames;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;

    VkResult result = vkCreateDescriptorPool(device.Access(), &poolInfo
        , nullptr, &pool_);
    ErrorManager::Validate(result, "Descriptor pool creation");
}

void DescriptorPool::Destroy(const Device& device) {
    vkDestroyDescriptorPool(device.Access(), pool_, nullptr);
}

const VkDescriptorPool& DescriptorPool::Access() const {
    return pool_;
}

VkDescriptorPool& DescriptorPool::Access() {
    return pool_;
}

} // vk

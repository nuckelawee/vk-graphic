#include "vk/vk_descriptor_set.hpp"

namespace vk {

void DescriptorSet::Create(const Device& device, const DescriptorPool& pool
    , const DataLoader& dataLoader) {

    VkDescriptorSetLayoutBinding layoutBinding {};
    layoutBinding.binding = 0;
    layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    layoutBinding.descriptorCount = 1;
    layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    layoutBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutCreateInfo layoutInfo {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = 1;
    layoutInfo.pBindings = &layoutBinding;

    VkResult result = vkCreateDescriptorSetLayout(device.Access()
        , &layoutInfo, nullptr, &descriptorSetLayout_);
    ErrorManager::Validate(result, "Descriptor set creation");

    Allocate(device, pool);
    UpdateDescriptorSet(device, dataLoader);
}

void DescriptorSet::Allocate(const Device& device, const DescriptorPool& pool) {
    VkDescriptorSetLayout pSetLayouts[vk::Setting::frames] 
        = { descriptorSetLayout_, descriptorSetLayout_ };
    VkDescriptorSetAllocateInfo descriptorInfo {};
    descriptorInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorInfo.descriptorPool = pool.Access();
    descriptorInfo.descriptorSetCount = vk::Setting::frames;
    descriptorInfo.pSetLayouts = pSetLayouts;

    VkResult result = vkAllocateDescriptorSets(device.Access(), &descriptorInfo
        , pDescriptorSets_);
    ErrorManager::Validate(result, "Descriptor set allocation");
}

void DescriptorSet::UpdateDescriptorSet(const Device& device
    , const DataLoader& dataLoader) {

    const BufferData& bufferData = dataLoader.Access(BUFFER_TYPE_UNIFORM);
    for(size_t i = 0; i < vk::Setting::frames; i++) {
        VkDescriptorBufferInfo bufferInfo {};
        bufferInfo.buffer = bufferData.buffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(MvpMatrix);

        VkWriteDescriptorSet writeDescriptorSet {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.dstSet = pDescriptorSets_[i];
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.dstArrayElement = 0;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.pBufferInfo = &bufferInfo;
        writeDescriptorSet.pImageInfo = nullptr;
        writeDescriptorSet.pTexelBufferView = nullptr; 

        vkUpdateDescriptorSets(device.Access(), 1, &writeDescriptorSet
            , 0, nullptr);
    }
}

const VkDescriptorSetLayout& DescriptorSet::AccessLayout() const {
    return descriptorSetLayout_;
}

VkDescriptorSetLayout& DescriptorSet::AccessLayout() {
    return descriptorSetLayout_;
}

const VkDescriptorSet* DescriptorSet::Access() const {
    return pDescriptorSets_;
}

VkDescriptorSet* DescriptorSet::Access() {
    return pDescriptorSets_;
}

void DescriptorSet::Destroy(const Device& device
    , const DescriptorPool& pool) {

    vkFreeDescriptorSets(device.Access(), pool.Access()
        , vk::Setting::frames, pDescriptorSets_);
    vkDestroyDescriptorSetLayout(device.Access(), descriptorSetLayout_, nullptr);
}

} // vk

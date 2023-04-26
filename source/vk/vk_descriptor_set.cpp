#include "vk/vk_descriptor_set.hpp"
#include "vk/vk_image.hpp"
#include "vk/vk_buffer.hpp"
#include "vk/vk_exception.hpp"
#include "mvp_matrix.hpp"

namespace vk {

void DescriptorSet::Create(VkDevice device) {
    device_ = device;

    VkDescriptorSetLayoutBinding uboBinding {};
    uboBinding.binding = 0;
    uboBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboBinding.descriptorCount = 1;
    uboBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    uboBinding.pImmutableSamplers = nullptr;

    VkDescriptorSetLayoutBinding samplerBinding {};
    samplerBinding.binding = 1;
    samplerBinding.descriptorCount = 1;
    samplerBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    samplerBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    samplerBinding.pImmutableSamplers = nullptr;

    size_t layoutBindingCount = 2;
    VkDescriptorSetLayoutBinding layoutBindings[layoutBindingCount]
        = { uboBinding, samplerBinding };

    VkDescriptorSetLayoutCreateInfo layoutInfo {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = layoutBindingCount;
    layoutInfo.pBindings = layoutBindings;

    VkResult result = vkCreateDescriptorSetLayout(device_, &layoutInfo
        , nullptr, &descriptorSetLayout_);
    if(result != VK_SUCCESS) {
        throw Exception("Failed to create descriptor set layout", result);
    }
    CreatePool();
    Allocate();
}

void DescriptorSet::Allocate() {
    VkDescriptorSetLayout setLayouts[vk::Settings::frames] 
        = { descriptorSetLayout_, descriptorSetLayout_ };
    VkDescriptorSetAllocateInfo descriptorInfo {};
    descriptorInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorInfo.descriptorPool = pool_;
    descriptorInfo.descriptorSetCount = vk::Settings::frames;
    descriptorInfo.pSetLayouts = setLayouts;

    VkResult result = vkAllocateDescriptorSets(device_, &descriptorInfo
        , descriptorSets_);
    if(result != VK_SUCCESS) {
        throw Exception("Failed to allocate descriptor sets", result);
    }
}

void DescriptorSet::BindImage(const VkDescriptorImageInfo& imageInfo
    , uint32_t index) noexcept {
    VkWriteDescriptorSet writeDescriptorSet {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSets_[index];
    writeDescriptorSet.dstBinding = 1;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(device_, 1, &writeDescriptorSet
        , 0, nullptr);
}

void DescriptorSet::BindBuffer(const VkDescriptorBufferInfo& bufferInfo
    , uint32_t index) noexcept {

    VkWriteDescriptorSet writeDescriptorSet {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.dstSet = descriptorSets_[index];
    writeDescriptorSet.dstBinding = 0;
    writeDescriptorSet.dstArrayElement = 0;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.pBufferInfo = &bufferInfo;

    vkUpdateDescriptorSets(device_, 1, &writeDescriptorSet
        , 0, nullptr);
}

void DescriptorSet::CreatePool() {
    size_t poolCount = 2;
    VkDescriptorPoolSize poolSizes[poolCount] = {};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = vk::Settings::frames;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = vk::Settings::frames;

    VkDescriptorPoolCreateInfo poolInfo {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolCount;
    poolInfo.pPoolSizes = poolSizes;
    poolInfo.maxSets = vk::Settings::frames;

    VkResult result = vkCreateDescriptorPool(device_, &poolInfo, nullptr, &pool_);
    if(result != VK_SUCCESS) {
        throw Exception("Descriptor pool creation", result);
    }
}

const VkDescriptorSetLayout& DescriptorSet::AccessLayout() const noexcept {
    return descriptorSetLayout_;
}

VkDescriptorSetLayout& DescriptorSet::AccessLayout() noexcept {
    return descriptorSetLayout_;
}

const VkDescriptorSet* DescriptorSet::Access() const noexcept {
    return descriptorSets_;
}

VkDescriptorSet* DescriptorSet::Access() noexcept {
    return descriptorSets_;
}

void DescriptorSet::Destroy() noexcept {
    vkDestroyDescriptorPool(device_, pool_, nullptr);
    vkDestroyDescriptorSetLayout(device_, descriptorSetLayout_, nullptr);
}

} // vk

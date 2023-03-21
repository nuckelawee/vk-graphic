#include "vk/vk_descriptor_set.hpp"

namespace vk {

void DescriptorSet::Create(const Device& device, const DescriptorPool& pool
    , const DataLoader& dataLoader) {

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

        VkDescriptorImageInfo imageInfo {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = dataLoader.AccessImage().imageView;
        imageInfo.sampler = dataLoader.AccessImage().sampler;


        size_t writeCount = 2;
        VkWriteDescriptorSet writeDescriptorSets[writeCount] = {};
        writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[0].dstSet = pDescriptorSets_[i];
        writeDescriptorSets[0].dstBinding = 0;
        writeDescriptorSets[0].dstArrayElement = 0;
        writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSets[0].descriptorCount = 1;
        writeDescriptorSets[0].pBufferInfo = &bufferInfo;

        writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[1].dstSet = pDescriptorSets_[i];
        writeDescriptorSets[1].dstBinding = 1;
        writeDescriptorSets[1].dstArrayElement = 0;
        writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSets[1].descriptorCount = 1;
        writeDescriptorSets[1].pImageInfo = &imageInfo;


        vkUpdateDescriptorSets(device.Access(), writeCount, writeDescriptorSets
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

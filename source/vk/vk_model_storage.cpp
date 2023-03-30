#include "vk/vk_model_storage.hpp"

namespace vk {

void ModelStorage::Init(const Device& device) {
    device_ = device;
    bufferBuilder_.Init(device);
    imageBuilder_.Init(device);
    meshLoader_.Init(device);
}

void ModelStorage::Destroy() {
    VkDevice device = device_.Access();
    buffers_.first.Destroy(device);
    buffers_.second.Destroy(device);

    for(auto model : modelViews_) {
        vkDestroyImageView(device, model.image.view, nullptr);
        vkDestroyImage(device, model.image.image, nullptr);
        vkDestroySampler(device, model.image.sampler, nullptr);
        vkFreeMemory(device, model.image.memory, nullptr); 
    }

    imageBuilder_.Destroy();
    bufferBuilder_.Destroy();
}

Model ModelStorage::Add(const ModelInfo& modelInfo) {
    auto search = models_.find(modelInfo.meshpath);
    if(search != models_.end()) {
//        modelViews_[search->second.descriptor_].modelsCount++;
        return search->second;
    }
    Mesh mesh = meshLoader_.Load(modelInfo.meshpath.c_str());
    Image image = textureLoader_.Load(bufferBuilder_, imageBuilder_
        , modelInfo.texturepath.c_str());

    Model model(modelViews_.size());
    models_.emplace(modelInfo.meshpath, model); 
    modelViews_.push_back( { image, mesh } );

    return model; 
}

std::pair<std::vector<ModelView>, std::pair<Buffer, Buffer>> ModelStorage::SubmitModels() {
    buffers_ = meshLoader_.Submit(bufferBuilder_);
    return { modelViews_, buffers_ };
}

void ModelStorage::DrawModels(VkCommandBuffer commandBuffer
    , VkPipelineLayout pipelineLayout, VkDescriptorSet descriptorSet) {

    VkBuffer vertexBuffer = buffers_.first.buffer;
    VkBuffer indexBuffer = buffers_.second.buffer;

    const VkDeviceSize& offset = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1
        , &vertexBuffer, &offset);

    vkCmdBindIndexBuffer(commandBuffer, indexBuffer
        , 0, VK_INDEX_TYPE_UINT32);

    for(auto model : modelViews_) {
        Mesh &mesh = model.mesh;
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS
            , pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
        
        vkCmdDrawIndexed(commandBuffer, mesh.indexCount, 1, mesh.firstIndex
            , mesh.vertexOffset, 0);
    }
}

}

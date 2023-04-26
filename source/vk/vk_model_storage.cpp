#include "vk/vk_model_storage.hpp"

namespace vk {

void ModelStorage::Init(VkDevice device) noexcept { device_ = device; }

void ModelStorage::Destroy() noexcept {
    for(auto& family : families_) {
        family.vertices.Destroy(device_);
        family.indices.Destroy(device_);
    }
}

uint32_t ModelStorage::Insert(Buffer vertices, Buffer indices
    , std::vector<Mesh>&& meshes, std::vector<Image>&& images) noexcept {

    uint32_t desc = families_.size();
    families_.push_back( { std::move(meshes), std::move(images)
        , std::move(vertices), std::move(indices) } );
    return desc;
}

void ModelStorage::DrawModels(const std::vector<Model>& models
    , VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout
    , VkDescriptorSet descriptorSet) noexcept {

    VkBuffer vertexBuffer = families_[0].vertices.buffer;
    VkBuffer indexBuffer = families_[0].indices.buffer;

    const VkDeviceSize& offset = 0;
    vkCmdBindVertexBuffers(commandBuffer, 0, 1
        , &vertexBuffer, &offset);

    vkCmdBindIndexBuffer(commandBuffer, indexBuffer
        , 0, VK_INDEX_TYPE_UINT32);

    for(const auto& mesh : families_[0].meshes) {
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS
            , pipelineLayout, 0, 1, &descriptorSet, 0, nullptr);
        
        vkCmdDrawIndexed(commandBuffer, mesh.indexCount, 1, mesh.firstIndex
            , mesh.vertexOffset, 0);
    }
}

} //vk
/*
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
*/

#pragma once

#include "vk_mesh_loader.hpp"
#include "vk_texture_loader.hpp"
#include "vk_model.hpp"
#include "vk_image.hpp"

#include <unordered_map>

namespace vk {

struct ModelView {
    Image image;
    Mesh mesh;
};

class ModelStorage {

    std::unordered_map<std::string, Model> models_;
    std::vector<ModelView> modelViews_;
    std::pair<Buffer, Buffer> buffers_;

    Device device_;
    MeshLoader meshLoader_;
    TextureLoader textureLoader_;   
    BufferBuilder bufferBuilder_;
    ImageBuilder imageBuilder_; 

public:

    void Init(const Device& device);
    void Destroy();

    Model Add(const ModelInfo& modelInfo);
    
    std::pair<std::vector<ModelView>, std::pair<Buffer, Buffer>> SubmitModels();

    void DrawModels(VkCommandBuffer commandBuffer, VkPipelineLayout pipelineLayout
        , VkDescriptorSet descriptorSet);

    ModelStorage() {}
    ~ModelStorage() {}

private:

    const ModelStorage& operator=(const ModelStorage& modelStorage) = delete;
    ModelStorage(const ModelStorage& modelStorage) = delete;

};

} //vk

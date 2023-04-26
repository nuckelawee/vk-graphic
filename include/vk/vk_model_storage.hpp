#pragma once

#include "vk/vk_buffer.hpp"
#include "vk/vk_image.hpp"
#include "mesh.hpp"
#include "model.hpp"

namespace vk {

class ModelStorage {

    struct ModelFamily {
        std::vector<Mesh> meshes;
        std::vector<Image> images;

        Buffer vertices;
        Buffer indices;
    };

    std::vector<ModelFamily> families_;
    VkDevice device_;

public:
    void Init(VkDevice device) noexcept;

    uint32_t Insert(Buffer vertices, Buffer indices, std::vector<Mesh>&& meshes
        , std::vector<Image>&& images) noexcept;

    void DrawModels(const std::vector<Model>& models, VkCommandBuffer commandBuffer
        , VkPipelineLayout pipelineLayout, VkDescriptorSet descriptorSet) noexcept;

    void Destroy() noexcept;
};
/*
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
*/
} //vk

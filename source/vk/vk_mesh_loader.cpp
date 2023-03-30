#include "vk/vk_mesh_loader.hpp"

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace vk {

void MeshLoader::Init(const Device& device) {
    device_ = device.Access();
}

Mesh MeshLoader::Load(const char *filepath) {

    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnings, errors;

    bool result = tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings
        , &errors, filepath);
    ErrorManager::Validate( { ERROR_TYPE_UNKNOWN, result }, warnings, errors);

    uint32_t indexCount = 0;
    uint32_t vertexOffest = vertices_.size();
    uint32_t firstIndex = indices_.size();
    for(const auto& shape : shapes) {
        for(const auto& index : shape.mesh.indices) {
            Vertex3D vertex;
            vertex.pos = {
                attributes.vertices[3 * index.vertex_index + 0],
                attributes.vertices[3 * index.vertex_index + 1],
                attributes.vertices[3 * index.vertex_index + 2],
            };
            vertex.color = {
                attributes.colors[3 * index.vertex_index + 0],
                attributes.colors[3 * index.vertex_index + 1],
                attributes.colors[3 * index.vertex_index + 2],
            };
            vertex.texPos = {
                attributes.texcoords[2 * index.texcoord_index + 0], 
                1.0f - attributes.texcoords[2 * index.texcoord_index + 1], 
            };
            vertex.normal = {
                attributes.normals[3 * index.normal_index + 0],
                attributes.normals[3 * index.normal_index + 1],
                attributes.normals[3 * index.normal_index + 2],
            };
            vertices_.push_back(vertex);
            indices_.push_back(indices_.size());
            indexCount++;
        }
    }
    Mesh mesh;
    mesh.indexCount = indexCount;
    mesh.firstIndex = firstIndex;
    mesh.vertexOffset = vertexOffest;

    return mesh;
}

std::pair<Buffer, Buffer> MeshLoader::Submit(BufferBuilder& bufferBuilder) {

    VkDeviceSize vertexBuffSize = sizeof(Vertex3D) * vertices_.size(); 
    Buffer vertexBuffer = bufferBuilder.BuildBuffer(vertices_.data()
        , vertexBuffSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkDeviceSize indexBuffSize = sizeof(uint32_t) * vertices_.size(); 
    Buffer indexBuffer = bufferBuilder.BuildBuffer(indices_.data()
        , indexBuffSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT
        , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    vertices_.clear();
    indices_.clear();

    return { vertexBuffer, indexBuffer };
}

} //vk

#include "resource_loader.hpp"
#include "model.hpp"
#include "mesh.hpp"
#include "vk/vk_vertex_binding.hpp"
#include "file_manager.hpp"

#include <iostream>
#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

Mesh loadMesh(std::vector<vk::Vertex3D>& vertices, std::vector<uint32_t>& indices
    , const std::string& filepath) {

    tinyobj::attrib_t attributes;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warnings, errors;

    bool result = tinyobj::LoadObj(&attributes, &shapes, &materials, &warnings
        , &errors, filepath.c_str());
    if(!result) {
        throw std::runtime_error(errors);
    }

    uint32_t indexCount = 0;
    uint32_t vertexOffest = vertices.size();
    uint32_t firstIndex = indices.size();
    for(const auto& shape : shapes) {
        for(const auto& index : shape.mesh.indices) {
            vk::Vertex3D vertex;
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
            vertices.push_back(vertex);
            indices.push_back(indices.size());
            indexCount++;
        }
    }
    Mesh mesh;
    mesh.indexCount = indexCount;
    mesh.firstIndex = firstIndex;
    mesh.vertexOffset = vertexOffest;
    return mesh;
} 

ModelSet ResourceLoader::LoadModel(std::vector<Modelpath>&& pathes) noexcept {
    ModelSet models {};
    models.textures.resize(pathes.size());
    models.meshes.resize(pathes.size());
    for(size_t i = 0; i < pathes.size(); ++i) {
        try {
            models.meshes[i] = loadMesh(models.vertices, models.indices
                , pathes[i].meshpath);
            models.textures[i] = FileManager::ReadImageTga(pathes[i].texturepath);
        } catch(std::invalid_argument& info) {
            std::cerr << info.what() << '\n';
            models.textures[i] = FileManager::ReadImageTga(missed_texture);
        } catch(std::exception& info) {
            std::cerr << info.what() << '\n';
            std::terminate();
        }
    }
    return models;
}

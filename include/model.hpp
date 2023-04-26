#pragma once

#include <cstdint>
#include <string>

#include "texture.hpp"
#include "mesh.hpp"
#include "vk/vk_vertex_binding.hpp"

namespace vk {
class Engine;
class ModelStorage;
}

struct Modelpath {
    std::string meshpath;
    std::string texturepath;
};

struct ModelSet {
    std::vector<vk::Vertex3D> vertices;
    std::vector<uint32_t> indices;
    std::vector<Texture> textures;
    std::vector<Mesh> meshes;
};

struct Model {
friend class vk::ModelStorage;
friend class vk::Engine;
private:
    uint32_t sampleIndex;
    uint32_t familyIndex;
};

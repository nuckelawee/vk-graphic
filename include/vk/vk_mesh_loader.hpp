#pragma once

#include "vk/vk_device.hpp"
#include "vk/vk_vertex_binding.hpp"
#include "vk/vk_buffer_builder.hpp"

namespace vk {

struct Mesh {
    uint32_t indexCount;
    uint32_t firstIndex;
    uint32_t vertexOffset;
};

class MeshLoader {

    VkDevice device_;
    std::vector<Vertex3D> vertices_;
    std::vector<uint32_t> indices_;
    

public:

    Mesh Load(const char *filepath);

    std::pair<Buffer, Buffer> Submit(BufferBuilder& bufferBuilder);

    void Init(const Device& device);

    MeshLoader() {}
    ~MeshLoader() {}

};

} //vk

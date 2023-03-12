#pragma once

namespace vk {

struct descirptorSetInfo {
    jjjj
}

class MeshInfo {
    void *pVertices;
    void *pIndices;
    //VetrexType ;
    VkDeviceSize vertexElementSize;
    size_t vertexElementCount;
    size_t indexElementCount;

    ShaderInfo shaderInfo;


     
};

} //vk

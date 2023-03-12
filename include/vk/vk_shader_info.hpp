#pragma once

namespace vk {

struct Shader {
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        
};

struct ShaderpathsPack {
    const char* pVertex;
    const char* pFragment;
    const char* pGeometry;

    bool operator==(const ShaderpathsPack& other) const {
        if(strcmp(other.pVertex, pVertex)
            || strcmp(other.pFragment, pFragment)
            || strcmp(other.pGeometry, pGeometry)) { return false; }
        return true;
    }
}

struct ShaderStageIndices {
    unsigned int vertex;
    unsigned int fragment;
    unsigned int geometry;
    bool operator==(const ShaderStageIndices& other) const {
        if(other.vertex == vertex && other.fragment == fragment
            && other.geometry == geometry) { return true; }
        return false;
    }
}

struct ShaderStage {
    VkPipelineShaderStageCreateInfo shaderStage;
    std::string shaderpath;
};

struct ShaderInfo {
    ShaderpathsPack shadersPack;

    VkVertexInputBindingDescription *pBindingDescriptions;
    VkVertexInputAttributeDescription *pAttributeDescription;

    VkDescriptorSetLayoutBinding *pDescriptorBindings;
    size_t descriptorBindingCount;
};

struct Shader {

}

} //vk

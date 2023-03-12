#pragma once

namespace vk {

class ShaderLoader {
    std::vector<ShaderStage> shaderStages_;
    std::vector<VkDescriptorSetLayouts> descriptorSetLayouts_;

public:
    
    Load

private:

    SentDataInfo CreateShaderModule(const VkDevice& device, const *pFilepath);
    int FindExistedShaderModule(const char* pShaderpath);

public:
    const VkShaderModule& Access(const SentDataInfo& sentInfo) const;

    ShaderLoader() {}
    ~ShaderLoader() {}
};

} //vk

#include "vk/vk_shader_loader.hpp"

namespace vk {

void ShaderLoader::AddUniqueShaderpathsPack(ShaderpathsPack *pPacks
    , ShaderpathsPack& shadersPack, int& uniqueShadersPackCount
    , int& commonPacksIndex) {

    for(size_t j = 0; j < uniqueShadersPackCount; j++) {
        if(pPacks[j] == shadersPack) {
            commonPacksIndex = j; 
            return;
        }
    }
    commonPacksIndex = uniqueShadersPackCount; 
    pPacks[uniqueShadersPackCount] = shadersPack;
    uniqueShadersPackCount++;
    return;
}
        
void ShaderLoader::CreateShaderStages(const Device& device
    , const ShaderInfo *pShaderInfo, size_t shaderInfoCount) {

    ShaderpathsPack pShadersPacks[shaderInfoCount];
    int uniqueShadersPackCount;
    int pCommonPacks[shaderInfoCount];
    int shaderStageIndex = 0; 

    int uniqueShadersPacks = 0;
    for(size_t i = 0; i < shaderInfoCount; i++) {
        AddUniqueShaderpathsPacks(pShadersPacks, pShaderInfo[i].shadersPacks
            , uniqueShaderPackCount, pCommonPacks[i]);
        AddUnique
    }

    ShaderStageIndices stageIndices[uniqueShadersPackCount];
    for(size_t i = 0; i < uniqueShadersPackCount; i++) {
        stageIndices.vertex = CreateShaderStage(device
            , VK_SHADER_STAGE_VERTEX_BIT, pShadersPacks[i].pVertex);
        stageIndices.fragment = CreateShaderStage(device
            , VK_SHADER_STAGE_FRAGMENT_BIT, pShadersPacks[i].pFragment);
        stageIndices.geometry = CreateShaderStage(device
            , VK_SHADER_STAGE_GEOMETRY_BIT, pShadersPacks[i].pGeometry);
    }

    shaders_.resize(uniqueShadersPackCount);
    for(size_t i = 0; i < uniqueShadersPackCount; i++) {
        ShaderStageIndices& stageIndices = stageIndices[pCommonPacks[i]];
        shaders[i].shaderStages.push_back(shaderStages_[stageIndices.vertex]);
        shaders[i].shaderStages.push_back(shaderStages_[stageIndices.fragment]);
        if(stageIndices.geometry != -1)
        { shaders[i].shaderStages.push_back(shaderStages_[stageIndices.geometry]); }
    }
}

SentDataInfo ShaderLoader::Load(const Device& device
    , const ShaderInfo *pShaderInfo, size_t shaderInfoCount) {

    VkDescriptorSetLayout descriptorSetLayout;

    CreateShaderStages(device, pShaderInfo, shaderInfoCount);

    CreateDescriptorSetLayout(device, shaderInfo);
      
}

void ShaderLoader::CreateDescriptorSetLayout(const Device& device
    , const ShaderInfo& shaderInfo, size_t shaderInfoCount) {

    VkVertex


    VkDescriptorSetLayoutCreateInfo layoutInfo {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.bindingCount = shaderInfo.descriptorBindingsCount;
    layoutInfo.pBindings = shaderInfo.pDescriptorBindings;

    VkResult result = vkCreateDescriptorSetLayout(device.Access()
        , &layoutInfo, nullptr, &descriptorSetLayout);
    ErrorManager::Validate(result, "Descriptor set creation");
    
}

int ShaderLoader::CreateShaderStage(
      const Device& device, VkPipelineShaderStageCreateFlagBits flags
    , const char* pShaderpath) {
 
    if(pShaderpath == nullptr) { return -1; }

    int descriptor = ShaderStageExist(pShaderpath);
    if(descriptor != -1) {
        return shaderStages_[descriptor];
    }
    VkShaderMoudle shaderModule = CreateShaderModule(device, pShaderpath);

    VkPipelineShaderStageCreateInfo stageInfo {};
    stageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    stageInfo.stage = flags;
    stageInfo.module = shaderModule;
    stageInfo.pName = "main";
    
    shaderStages_.push_back(stageInfo);
    return shaderStages.size()-1;
}

VkShaderModule ShaderLoader::CreateShaderModule(const VkDevice& device
        , const char *pFilepath) {

    const std::string vertCode = FileManager::ReadFile(pFilepath
        , std::ios::ate | std::ios::binary);

    VkShaderModule shaderModule;
    VkShaderModuleCreateInfo shaderInfo {};
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = code.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkResult result = vkCreateShaderModule(device, &shaderInfo
        , nullptr, &shaderModule);
    ErrorManager::Validate(result, "Shader module creation");

    return shaderModule;
}

int ShaderLoader::ShaderStageExist(const char* pShaderpath) {
    for(size_t i = 0; i < shader_.size(); i++) {
        if(shaderStages_[i].shaderpath == pShaderpath)
        { return i; }
    }
    return -1;
}

const VkShaderModule& ShaderLoader::Access(const SentDataInfo& sentInfo) const {
    return shaders_[sentInfo.descriptor];
}

} //vk

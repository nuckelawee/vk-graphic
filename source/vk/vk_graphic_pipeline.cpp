#include "vk/vk_graphic_pipeline.hpp"
#include "vk/vk_descriptor_set.hpp"
#include "vk/vk_pipeline_states.hpp"
#include "vk/vk_vertex_binding.hpp"
#include "vk/vk_settings.hpp"
#include "vk/vk_exception.hpp"

namespace vk {

VkPipelineLayout GraphicPipeline::AccessLayout() const noexcept
{ return pipelineLayout_; }

VkPipeline GraphicPipeline::Access() const noexcept
{ return pipeline_; }

VkRenderPass GraphicPipeline::AccessRenderPass() const noexcept
{ return renderPass_; }

VkShaderModule GraphicPipeline::CreateShaderModule(const std::string& code) const {

    VkShaderModule shaderModule;
    VkShaderModuleCreateInfo shaderInfo {};
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = code.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkResult result = vkCreateShaderModule(device_, &shaderInfo, nullptr, &shaderModule);
    if(result != VK_SUCCESS) {
        throw Exception("Failed to create shader module", result);
    }
    return shaderModule;
}

PipelineStates describePipelineStates() {

    VkPipelineVertexInputStateCreateInfo inputInfo {};
    VkPipelineDynamicStateCreateInfo dynamicStateInfo {};
    VkPipelineInputAssemblyStateCreateInfo assemblyInfo {};
    VkPipelineViewportStateCreateInfo viewportInfo {};
    VkPipelineRasterizationStateCreateInfo rasterizerInfo {};
    VkPipelineMultisampleStateCreateInfo multisampleInfo {};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo {};
    VkPipelineColorBlendStateCreateInfo colorBlendStateInfo {};

    depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable = VK_TRUE;
    depthStencilInfo.depthWriteEnable = VK_TRUE;
    depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilInfo.minDepthBounds = 0.0f;
    depthStencilInfo.maxDepthBounds = 1.0f;
    depthStencilInfo.stencilTestEnable = VK_FALSE;
    depthStencilInfo.front = {};
    depthStencilInfo.back = {};

    VkViewport viewport {};
    VkRect2D scissor {};

    VkDynamicState pDynamicStates[2] = {
        VK_DYNAMIC_STATE_VIEWPORT,  
        VK_DYNAMIC_STATE_SCISSOR,
     };
    dynamicStateInfo.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.dynamicStateCount = 2;
    dynamicStateInfo.pDynamicStates = pDynamicStates;

    VkVertexInputBindingDescription bindingDescription
        = VertexBinding::BindingDescription();
    std::vector<VkVertexInputAttributeDescription> attributeDescription
        = VertexBinding::AttributeDescription();

    inputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    inputInfo.vertexBindingDescriptionCount = 1;
    inputInfo.pVertexBindingDescriptions = &bindingDescription;
    inputInfo.vertexAttributeDescriptionCount = attributeDescription.size();
    inputInfo.pVertexAttributeDescriptions = attributeDescription.data();

    assemblyInfo.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assemblyInfo.primitiveRestartEnable = VK_FALSE;

    const VkExtent2D& extent = vk::Settings::GetInstance().Extent();
    viewport.x = 0.0f;
    viewport.y = 0.0f; 
    viewport.width = static_cast<float>(extent.width);
    viewport.height = static_cast<float>(extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    scissor.offset = { 0, 0 };
    scissor.extent = extent;
        
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = &viewport;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = &scissor;

    rasterizerInfo.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerInfo.depthClampEnable = VK_FALSE;
    rasterizerInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizerInfo.lineWidth = 1.0f;
    rasterizerInfo.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    //rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizerInfo.depthBiasEnable = VK_FALSE;
    rasterizerInfo.depthBiasConstantFactor = 0.0f;
    rasterizerInfo.depthBiasClamp = 0.0f;
    rasterizerInfo.depthBiasSlopeFactor = 0.0f;

    multisampleInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampleInfo.sampleShadingEnable = VK_FALSE;
    multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisampleInfo.minSampleShading = 1.0f;
    multisampleInfo.pSampleMask = nullptr;
    multisampleInfo.alphaToCoverageEnable = VK_FALSE;
    multisampleInfo.alphaToOneEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT
        | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT
        | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

    colorBlendStateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendStateInfo.logicOpEnable = VK_FALSE;
    colorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendStateInfo.blendConstants[0] = 0.0f;
    colorBlendStateInfo.blendConstants[1] = 0.0f;
    colorBlendStateInfo.blendConstants[2] = 0.0f;
    colorBlendStateInfo.blendConstants[3] = 0.0f;
    colorBlendStateInfo.attachmentCount = 1;
    colorBlendStateInfo.pAttachments = &colorBlendAttachment;

    PipelineStates states {};
    states.DynamicInfo(dynamicStateInfo);
    states.InputInfo(inputInfo); 
    states.AssemblyInfo(assemblyInfo); 
    states.ViewportInfo(viewportInfo);
    states.RasterizerInfo(rasterizerInfo);
    states.MultisampleInfo(multisampleInfo);
    states.DepthStencilInfo(depthStencilInfo);
    states.ColorBlendInfo(colorBlendStateInfo);
    return states;
}

void GraphicPipeline::CreateRenderPass(VkFormat depthFormat
    , VkFormat imageFormat) {

    VkAttachmentDescription colorAttachment {};
    colorAttachment.format = imageFormat;
    colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorAttachmentRef {};
    colorAttachmentRef.attachment = 0;
    colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentDescription depthAttachment {};
    depthAttachment.format = depthFormat;
    depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
    depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    VkAttachmentReference depthAttachmentRef {};
    depthAttachmentRef.attachment = 1;
    depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    
    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;
    subpass.pDepthStencilAttachment = &depthAttachmentRef;

    VkSubpassDependency subpassDependency {};
    subpassDependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependency.dstSubpass = 0;
    subpassDependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.srcAccessMask = 0;
    subpassDependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
        | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
    subpassDependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT
        | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

    uint32_t attachmentCount = 2;
    VkAttachmentDescription attachments[attachmentCount]
        = { colorAttachment, depthAttachment };

    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = attachmentCount;
    renderPassInfo.pAttachments = attachments;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &subpassDependency;

    VkResult result = vkCreateRenderPass(device_, &renderPassInfo, nullptr
    , &renderPass_);
    if(result != VK_SUCCESS) {
        throw Exception("Failed to create renderpass", result);
    }
}

void GraphicPipeline::Create(const Device& device
    , const Swapchain& swapchain
    , const std::vector<std::string>& shaderFiles
    , const DescriptorSet& descriptorSet) {

    device_ = device.Access();
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    VkShaderModule geomShaderModule;
    bool geomShaderUse = (shaderFiles.size() > 2);
                            
    const std::string vertCode = FileManager::ReadFile(shaderFiles[0]
        , std::ios::ate | std::ios::binary);
    const std::string fragCode = FileManager::ReadFile(shaderFiles[1]
        , std::ios::ate | std::ios::binary);    

    vertShaderModule = CreateShaderModule(vertCode);
    fragShaderModule = CreateShaderModule(fragCode);
    if(geomShaderUse == true) {
        const std::string geomCode = FileManager::ReadFile(shaderFiles[2]
            , std::ios::ate | std::ios::binary);
        geomShaderModule = CreateShaderModule(geomCode);
    }

    VkPipelineShaderStageCreateInfo vertStageInfo {};
    vertStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertStageInfo.module = vertShaderModule;
    vertStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragStageInfo {};
    fragStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragStageInfo.module = fragShaderModule;
    fragStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo geomStageInfo {};
    geomStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    geomStageInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
    geomStageInfo.module = geomShaderModule;
    geomStageInfo.pName = "main";

    std::vector<VkPipelineShaderStageCreateInfo> shaderStages =
        { vertStageInfo, fragStageInfo };
    if(geomShaderUse == true) {
        shaderStages.push_back(geomStageInfo);
    }

    VkFormat depthFormat = device.FindSupportedFormat(
        { VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT
        , VK_FORMAT_D24_UNORM_S8_UINT }, VK_IMAGE_TILING_OPTIMAL
        , VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);
 
    CreateRenderPass(depthFormat, swapchain.AccessImageFormat());

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 1;
    pipelineLayoutInfo.pSetLayouts = &descriptorSet.AccessLayout();
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    VkResult result = vkCreatePipelineLayout(device_, &pipelineLayoutInfo
        , nullptr, &pipelineLayout_);
    if(result != VK_SUCCESS) {
        throw Exception("Failed to create pipeline layout", result);
    }

    PipelineStates states = describePipelineStates();

    VkGraphicsPipelineCreateInfo pipelineInfo {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &(states.AccessInput());
    pipelineInfo.pInputAssemblyState = &(states.AccessAssembly());
    pipelineInfo.pViewportState = &(states.AccessViewport());
    pipelineInfo.pRasterizationState = &(states.AccessRasterizer());
    pipelineInfo.pMultisampleState = &(states.AccessMultisample());
    pipelineInfo.pDepthStencilState = &(states.AccessDepthStencil());
    pipelineInfo.pColorBlendState = &(states.AccessColorBlend());
    pipelineInfo.pDynamicState = &(states.AccessDynamic());
    pipelineInfo.layout = pipelineLayout_;
    pipelineInfo.renderPass = renderPass_;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = -1;

    result = vkCreateGraphicsPipelines(device_, VK_NULL_HANDLE, 1
        , &pipelineInfo, nullptr, &pipeline_);
    if(result != VK_SUCCESS) {
        throw Exception("Failed to create graphic pipeline", result);
    }
   
    vkDestroyShaderModule(device_, vertShaderModule, nullptr);
    vkDestroyShaderModule(device_, fragShaderModule, nullptr);
    if(geomShaderUse == true) {
        vkDestroyShaderModule(device_, geomShaderModule, nullptr);
    }
}

void GraphicPipeline::RenderPassBegin(VkCommandBuffer commandBuffer
    , VkFramebuffer framebuffer) noexcept {

    VkRenderPassBeginInfo renderPassInfo {};

    const VkClearValue* clearValues = vk::Settings::GetInstance().ClearValues().data();
    uint32_t clearValueCount = vk::Settings::GetInstance().ClearValues().size();

    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass_;
    renderPassInfo.framebuffer = framebuffer;
    renderPassInfo.renderArea.offset = { 0, 0 };
    renderPassInfo.renderArea.extent = vk::Settings::GetInstance().Extent();
    renderPassInfo.clearValueCount = clearValueCount;
    renderPassInfo.pClearValues = clearValues;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo
        , VK_SUBPASS_CONTENTS_INLINE);
    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS
        , pipeline_);
}

void GraphicPipeline::Destroy() noexcept {
    vkDestroyPipeline(device_, pipeline_, nullptr);
    vkDestroyPipelineLayout(device_, pipelineLayout_, nullptr);
    vkDestroyRenderPass(device_, renderPass_, nullptr);
}

} // vk

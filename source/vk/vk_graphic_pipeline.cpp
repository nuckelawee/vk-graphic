#include "vk/vk_graphic_pipeline.hpp"

namespace vk {


VkShaderModule GraphicPipeline::CreateShaderModule(const VkDevice& device
    , const std::string& code) {

    VkShaderModule shaderModule;
    VkShaderModuleCreateInfo shaderInfo {};
    shaderInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderInfo.codeSize = code.size();
    shaderInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

    VkResult result = vkCreateShaderModule(device, &shaderInfo, nullptr, &shaderModule);
    ErrorManager::Validate(result, "Shader module creation");
    return shaderModule;
}

PipelineStates GraphicPipeline::DescribePipelineStates(
    const Swapchain& swapchain, void *pUserData) {

    VkPipelineVertexInputStateCreateInfo inputInfo {};
    VkPipelineDynamicStateCreateInfo dynamicStateInfo {};
    VkPipelineInputAssemblyStateCreateInfo assemblyInfo {};
    VkPipelineViewportStateCreateInfo viewportInfo {};
    VkPipelineRasterizationStateCreateInfo rasterizerInfo {};
    VkPipelineMultisampleStateCreateInfo multisampleInfo {};
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo {};
    VkPipelineColorBlendStateCreateInfo colorBlendStateInfo {};

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

    inputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    inputInfo.vertexBindingDescriptionCount = 0;
    inputInfo.pVertexBindingDescriptions = nullptr;
    inputInfo.vertexAttributeDescriptionCount = 0;
    inputInfo.pVertexAttributeDescriptions = nullptr;

    assemblyInfo.sType = 
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    assemblyInfo.primitiveRestartEnable = VK_FALSE;

    viewport.x = 0.0f;
    viewport.y = 0.0f; 
    viewport.width = static_cast<float>(swapchain.AccessExtent().width);
    viewport.height = static_cast<float>(swapchain.AccessExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    scissor.offset = { 0, 0 };
    scissor.extent = swapchain.AccessExtent();
        
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
    rasterizerInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
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

void GraphicPipeline::CreateRenderPass(const VkDevice& device
    , const Swapchain& swapchain, void *pUserData) {

    VkAttachmentDescription colorAttachment {};
    colorAttachment.format = swapchain.AccessImageFormat();
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
    
    VkSubpassDescription subpass {};
    subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpass.colorAttachmentCount = 1;
    subpass.pColorAttachments = &colorAttachmentRef;

    VkRenderPassCreateInfo renderPassInfo {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachment;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpass;

    VkResult result = vkCreateRenderPass(device, &renderPassInfo, nullptr
    , &renderPass_);
    ErrorManager::Validate(result, "Render pass creation");
}

void GraphicPipeline::Create(const Device& device
    , const Swapchain& swapchain
    , const std::vector<std::string>& shaderFiles
    , std::function<PipelineStates(const Swapchain& swapchain
    , void *pUserData)> fillPipelineStates) {

    VkDevice logicDevice = device.Access();
    VkShaderModule vertShaderModule;
    VkShaderModule fragShaderModule;
    VkShaderModule geomShaderModule;
    bool geomShaderUse = (shaderFiles.size() > 2);
                            
    const std::string vertCode = FileManager::ReadFile(shaderFiles[0]
        , std::ios::ate | std::ios::binary);
    const std::string fragCode = FileManager::ReadFile(shaderFiles[1]
        , std::ios::ate | std::ios::binary);

    vertShaderModule = CreateShaderModule(logicDevice, vertCode);
    fragShaderModule = CreateShaderModule(logicDevice, fragCode);
    if(geomShaderUse == true) {
        const std::string geomCode = FileManager::ReadFile(shaderFiles[2]
            , std::ios::ate | std::ios::binary);
        geomShaderModule = CreateShaderModule(logicDevice, geomCode);
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

    CreateRenderPass(logicDevice, swapchain, nullptr);

    VkPipelineLayoutCreateInfo pipelineLayoutInfo {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 0;
    pipelineLayoutInfo.pSetLayouts = nullptr;
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    VkResult result = vkCreatePipelineLayout(logicDevice, &pipelineLayoutInfo
        , nullptr, &pipelineLayout_);
    ErrorManager::Validate(result, "Pipeline creation");

    PipelineStates states = fillPipelineStates(swapchain, nullptr);

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

    result = vkCreateGraphicsPipelines(logicDevice, VK_NULL_HANDLE, 1
        , &pipelineInfo, nullptr, &pipeline_);
    ErrorManager::Validate(result, "Graphic pipeline creation");
   
    vkDestroyShaderModule(logicDevice, vertShaderModule, nullptr);
    vkDestroyShaderModule(logicDevice, fragShaderModule, nullptr);
    if(geomShaderUse == true) {
        vkDestroyShaderModule(logicDevice, geomShaderModule, nullptr);
    }
}

void GraphicPipeline::Destroy(const Device& device) {
    vkDestroyPipeline(device.Access(), pipeline_, nullptr);
    vkDestroyPipelineLayout(device.Access(), pipelineLayout_, nullptr);
    vkDestroyRenderPass(device.Access(), renderPass_, nullptr);
}

} // vk

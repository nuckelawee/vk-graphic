#include "vk/vk_engine.hpp"

namespace vk {

void Engine::Acquire(Surface& surface) {
    VkResult result = regulator_.BeginRender(device_, swapchain_
        , setting_.Vulkan());
    if(result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapchain_.Recreate(device_, surface, pipeline_);
    } else {
        ErrorManager::Validate(result, "Image acquiring");
    }
}
     
void Engine::Present(Surface& surface, VkPresentInfoKHR& presentInfo) {
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain_.Access();
    presentInfo.pImageIndices = &setting_.Vulkan().ImageIndex();

    VkResult result = vkQueuePresentKHR(device_.AccessQueues().present.queue
        , &presentInfo);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        swapchain_.Recreate(device_, surface, pipeline_);
    } else {
        ErrorManager::Validate(result, "Presentation");
    }
}

void Engine::Update(Surface& surface, const Camera& camera) {

    BufferData& bufferData = dataLoader_.Access(BUFFER_TYPE_UNIFORM);
    void *pBufferMapped = bufferData.buffersMapped[setting_.Vulkan().CurrentFrame()];

    memcpy(pBufferMapped, &camera.Access(), sizeof(MvpMatrix));
    
    Acquire(surface);

    CommandInfo commandInfo;
    commandInfo.type = COMMAND_TYPE_GRAPHICS;
    commandInfo.bufferCount = 1;
    commandInfo.offset = setting_.Vulkan().CurrentFrame();

    commandManager_.RecordDrawCommands(device_, setting_.Vulkan()
        , pipeline_, swapchain_, dataLoader_, commandInfo, descriptorSet_);

    VkSubmitInfo submitInfo {};
    VkPresentInfoKHR presentInfo {};

    regulator_.Sync(setting_.Vulkan(), submitInfo, presentInfo);

    commandManager_.Submit(device_, setting_.Vulkan(), regulator_
        , submitInfo, commandInfo);
    Present(surface, presentInfo);

    setting_.Vulkan().CurrentFrame() = (setting_.Vulkan().ImageIndex()+1) 
        % vk::Setting::frames;
}

void Engine::Init(Surface& surface) {

    instance_.IncludeDefaultLayersAndExtensions(attachments_);
    instance_.Create(setting_, attachments_);

    surface.Create(instance_);
    device_.PickGpu(instance_, surface, attachments_);
    device_.SetQueueFamilies(surface);
    device_.CreateLogicalDevice(surface, attachments_);

#ifdef DEBUG
    LayersAndExtensions::PrintAvailableLayersAndExtensions(device_.AccessGpu());
#endif

    swapchain_.Create(device_, surface);

    commandManager_.Create(device_);
    commandManager_.Allocate(device_, CommandInfo(COMMAND_TYPE_GRAPHICS
        , vk::Setting::frames));

    const std::vector<std::string> shadersPath = {
        "build/vert_texture.spv",
        "build/frag_texture.spv"
    };

/*
    Vertex3D pVerticesCube[] = {
        { { -0.25f, -0.25f, -0.25f }, { 1.0f, 0.0f, 0.0f } },
        { { -0.25f,  0.25f, -0.25f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.25f, -0.25f, -0.25f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.25f,  0.25f, -0.25f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.25f, -0.25f,  0.25f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.25f,  0.25f,  0.25f }, { 1.0f, 0.0f, 0.0f } },
        { { -0.25f, -0.25f,  0.25f }, { 1.0f, 0.0f, 0.0f } },
        { { -0.25f,  0.25f,  0.25f }, { 1.0f, 0.0f, 0.0f } },
    };

    uint16_t pIndicesCube[] = {
        0, 1, 2,
        2, 1, 3,
        2, 3, 4,
        4, 3, 5,
        4, 5, 6,
        6, 5, 7,
        6, 7, 0,
        0, 7, 1,
        6, 0, 2,
        2, 4, 6,
        7, 5, 3,
        7, 3, 1,
    };
    DataInfo cubeVertexInfo {};
    cubeVertexInfo.pData = pVerticesCube;
    cubeVertexInfo.elementCount = 8;
    cubeVertexInfo.elementSize = sizeof(Vertex3D);
    cubeVertexInfo.type = BUFFER_TYPE_VERTEX;
    
    DataInfo cubeIndexInfo {};
    cubeIndexInfo.pData = pIndicesCube;
    cubeIndexInfo.elementCount = 36;
    cubeIndexInfo.elementSize = sizeof(uint16_t);
    cubeIndexInfo.type = BUFFER_TYPE_INDEX;

    DataInfo* pDataObjInfos[] = {
        &cubeVertexInfo,
        &cubeIndexInfo,
    };
*/

    Vertex3D pVerticesSquare[] = {
        { {  0.3f, -0.3f, 0.0f }, { 1.0f, 0.0f, 0.0f }, { 1.0f, 1.0f } },
        { {  0.3f,  0.3f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 1.0f, 0.0f } },
        { { -0.3f,  0.3f, 0.0f }, { 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f } },
        { { -0.3f, -0.3f, 0.0f }, { 1.0f, 1.0f, 0.0f }, { 0.0f, 1.0f } },
    };
    uint16_t pIndicesSquare[] = {
        0, 1, 2, 0, 2, 3
    };

    DataInfo vertexInfoSquare;
    vertexInfoSquare.pData = pVerticesSquare;
    vertexInfoSquare.elementCount = 4;
    vertexInfoSquare.elementSize = sizeof(Vertex3D);
    vertexInfoSquare.type = BUFFER_TYPE_VERTEX;

    DataInfo indexInfoSquare;
    indexInfoSquare.pData = pIndicesSquare;
    indexInfoSquare.elementCount = 6;
    indexInfoSquare.elementSize = sizeof(uint16_t);
    indexInfoSquare.type = BUFFER_TYPE_INDEX;

    DataInfo cameraInfo {};
    cameraInfo.elementCount = 1;
    cameraInfo.elementSize = sizeof(MvpMatrix);
    cameraInfo.type = BUFFER_TYPE_UNIFORM;

    DataInfo* pDataObjInfos[] = {
        &vertexInfoSquare,
        &indexInfoSquare,
    };
    
    dataLoader_.Begin(device_, commandManager_);
    dataLoader_.LoadComplexData(device_, commandManager_, pDataObjInfos
        , pObjects_, 1);
    for(size_t i = 0; i < vk::Setting::frames; i++) {
        dataLoader_.LoadData(device_, commandManager_, cameraInfo);
        dataLoader_.LoadData(device_, commandManager_, cameraInfo);
    }
    
    dataLoader_.LoadTexture(device_, commandManager_, "resources/picture.tga");
    dataLoader_.End(device_, commandManager_);

    descriptorPool_.Create(device_);
    descriptorSet_.Create(device_, descriptorPool_, dataLoader_);   

    pipeline_.Create(device_, swapchain_, shadersPath, descriptorSet_);
    swapchain_.CreateFramebuffers(device_, pipeline_);

    regulator_.Create(device_);
  
    setting_.Vulkan().CurrentFrame() = 0;
}

void Engine::Terminate(Surface& surface) {
    vkDeviceWaitIdle(device_.Access());

    descriptorSet_.Destroy(device_, descriptorPool_);   
    descriptorPool_.Destroy(device_);
    regulator_.Destroy(device_);
    dataLoader_.Destroy(device_);
    commandManager_.Destroy(device_);
    swapchain_.Destroy(device_);
    pipeline_.Destroy(device_);
    device_.Destroy();
    surface.Destroy(instance_);
}
/*
    Vertex2D pVerticesSquare[] = {
        { {  0.3f, -0.3f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.3f,  0.3f }, { 1.0f, 1.0f, 0.0f } },
        { { -0.3f,  0.3f }, { 1.0f, 0.0f, 1.0f } },
        { { -0.3f, -0.3f }, { 1.0f, 1.0f, 0.0f } },
    };
    uint16_t pIndicesSquare[] = {
        0, 1, 2, 0, 2, 3
    };

    Vertex2D pVerticesTriangle[] = {
        { { -0.6f, -0.8f }, { 1.0f, 0.0f, 0.0f } },
        { { -0.4f, -0.4f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.8f, -0.4f }, { 0.0f, 0.0f, 1.0f } },
    };
    uint16_t pIndicesTriangle[] = {
        0, 1, 2 
    };

    DataInfo vertexInfoSquare;
    vertexInfoSquare.pData = pVerticesSquare;
    vertexInfoSquare.elementCount = 4;
    vertexInfoSquare.elementSize = sizeof(Vertex2D);
    vertexInfoSquare.type = BUFFER_TYPE_VERTEX;

    DataInfo indexInfoSquare;
    indexInfoSquare.pData = pIndicesSquare;
    indexInfoSquare.elementCount = 6;
    indexInfoSquare.elementSize = sizeof(uint16_t);
    indexInfoSquare.type = BUFFER_TYPE_INDEX;

    DataInfo vertexInfoTriangle;
    vertexInfoTriangle.pData = pVerticesTriangle;
    vertexInfoTriangle.elementCount = 3;
    vertexInfoTriangle.elementSize = sizeof(Vertex2D);
    vertexInfoTriangle.type = BUFFER_TYPE_VERTEX;

    DataInfo indexInfoTriangle;
    indexInfoTriangle.pData = pIndicesTriangle;
    indexInfoTriangle.elementCount = 3;
    indexInfoTriangle.elementSize = sizeof(uint16_t);
    indexInfoTriangle.type = BUFFER_TYPE_INDEX;

    DataInfo *dataObjInfos[4] = {
        &vertexInfoSquare,
        &vertexInfoTriangle,
        &indexInfoSquare,
        &indexInfoTriangle
    };
*/

} //vk

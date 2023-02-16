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

void Engine::Update(Surface& surface) {
    
    Acquire(surface);

    CommandInfo commandInfo;
    commandInfo.type = COMMAND_TYPE_GRAPHICS;
    commandInfo.bufferCount = 1;
    commandInfo.offset = setting_.Vulkan().CurrentFrame();

    commandManager_.RecordDrawCommands(device_, setting_.Vulkan()
        , pipeline_, swapchain_, dataLoader_, commandInfo);

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

    const std::vector<std::string> shadersPath = {
        "build/vert_vertex_buffer.spv",
        "build/frag_trivial.spv"
    };
    pipeline_.Create(device_, swapchain_, shadersPath);
    swapchain_.CreateFramebuffers(device_, pipeline_);

    commandManager_.Allocate(device_, CommandInfo(COMMAND_TYPE_GRAPHICS
        , vk::Setting::frames));

    Vertex pVerticesSquare[] = {
        { {  0.3f, -0.3f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.3f,  0.3f }, { 1.0f, 1.0f, 0.0f } },
        { { -0.3f,  0.3f }, { 1.0f, 0.0f, 1.0f } },
        { { -0.3f, -0.3f }, { 1.0f, 1.0f, 0.0f } },
    };
    uint16_t pIndicesSquare[] = {
        0, 1, 2, 0, 2, 3
    };

    Vertex pVerticesTriangle[] = {
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
    vertexInfoSquare.elementSize = sizeof(Vertex);
    vertexInfoSquare.type = BUFFER_TYPE_VERTEX;

    DataInfo indexInfoSquare;
    indexInfoSquare.pData = pIndicesSquare;
    indexInfoSquare.elementCount = 6;
    indexInfoSquare.elementSize = sizeof(uint16_t);
    indexInfoSquare.type = BUFFER_TYPE_INDEX;

    DataInfo vertexInfoTriangle;
    vertexInfoTriangle.pData = pVerticesTriangle;
    vertexInfoTriangle.elementCount = 3;
    vertexInfoTriangle.elementSize = sizeof(Vertex);
    vertexInfoTriangle.type = BUFFER_TYPE_VERTEX;

    DataInfo indexInfoTriangle;
    indexInfoTriangle.pData = pIndicesTriangle;
    indexInfoTriangle.elementCount = 3;
    indexInfoTriangle.elementSize = sizeof(uint16_t);
    indexInfoTriangle.type = BUFFER_TYPE_INDEX;

    DataInfo *dataInfos[4] = {
        &vertexInfoSquare,
        &vertexInfoTriangle,
        &indexInfoSquare,
        &indexInfoTriangle
    };
    dataLoader_.Begin(device_, commandManager_);

    dataLoader_.LoadComplexData(device_, commandManager_, dataInfos
        , pObjects_, 2);

    dataLoader_.End(device_, commandManager_);

    regulator_.Create(device_);

    setting_.Vulkan().CurrentFrame() = 0;
}

void Engine::Terminate(Surface& surface) {
    vkDeviceWaitIdle(device_.Access());

    regulator_.Destroy(device_);
    dataLoader_.Destroy(device_);
    commandManager_.Destroy(device_);
    swapchain_.Destroy(device_);
    pipeline_.Destroy(device_);
    device_.Destroy();
    surface.Destroy(instance_);
}

} //vk

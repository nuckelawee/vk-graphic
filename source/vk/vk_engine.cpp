#include "vk/vk_engine.hpp"

namespace vk {

void Engine::Acquire(Surface& surface) {
    VkResult result = pRegulator_->BeginRender(*pDevice_, *pSwapchain_
        , setting_.Vulkan());
    if(result == VK_ERROR_OUT_OF_DATE_KHR) {
        pSwapchain_->Recreate(*pDevice_, surface, *pPipeline_);
    } else {
        ErrorManager::Validate(result, "Image acquiring");
    }
}
     
void Engine::Present(Surface& surface, VkPresentInfoKHR& presentInfo) {
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &pSwapchain_->Access();
    presentInfo.pImageIndices = &setting_.Vulkan().ImageIndex();

    VkResult result = vkQueuePresentKHR(pDevice_->AccessQueues().present.queue
        , &presentInfo);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        pSwapchain_->Recreate(*pDevice_, surface, *pPipeline_);
    } else {
        ErrorManager::Validate(result, "Presentation");
    }
}

void Engine::Update(Surface& surface) {

    Acquire(surface);

    CommandInfo commandInfo;
    commandInfo.type = GRAPHICS;
    commandInfo.bufferCount = 1;
    commandInfo.offset = setting_.Vulkan().CurrentFrame();

    pCommandManager_->RecordDrawCommands(*pDevice_, setting_.Vulkan()
        , *pPipeline_, *pSwapchain_, *pDataLoader_, commandInfo);

    VkSubmitInfo submitInfo {};
    VkPresentInfoKHR presentInfo {};

    pRegulator_->Sync(setting_.Vulkan(), submitInfo, presentInfo);

    pCommandManager_->Submit(*pDevice_, setting_.Vulkan(), *pRegulator_
        , submitInfo, commandInfo);
    Present(surface, presentInfo);

    setting_.Vulkan().CurrentFrame() = (setting_.Vulkan().ImageIndex()+1) 
        % vk::Setting::frames;
}

void Engine::Init(Surface& surface) {
    pAttachments_ = new LayersAndExtensions;
    pInstance_ = new Instance;
    pDevice_ = new Device;
    pSwapchain_ = new Swapchain; 
    pPipeline_ = new GraphicPipeline;
    pCommandManager_ = new CommandManager;
    pDataLoader_ = new DataLoader;
    pRegulator_ = new Regulator;

    pInstance_->IncludeDefaultLayersAndExtensions(*pAttachments_);
    pInstance_->Create(setting_, *pAttachments_);

    surface.Create(*pInstance_);
    pDevice_->PickGpu(*pInstance_, surface, *pAttachments_);
    pDevice_->SetQueueFamilies(surface);
    pDevice_->CreateLogicalDevice(surface, *pAttachments_);

#ifdef DEBUG
    LayersAndExtensions::PrintAvailableLayersAndExtensions(pDevice_->AccessGpu());
#endif

    pSwapchain_->Create(*pDevice_, surface);

    const std::vector<std::string> shadersPath = {
        "build/vert_vertex_buffer.spv",
        "build/frag_trivial.spv"
    };
    pPipeline_->Create(*pDevice_, *pSwapchain_, shadersPath);
    pSwapchain_->CreateFramebuffers(*pDevice_, *pPipeline_);

    pCommandManager_->Allocate(*pDevice_, CommandInfo(GRAPHICS
        , vk::Setting::frames));

    const Vertex pVertices[] = {
        { {  0.3f, -0.3f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.3f,  0.3f }, { 1.0f, 1.0f, 0.0f } },
        { { -0.3f,  0.3f }, { 1.0f, 0.0f, 1.0f } },
        { { -0.3f, -0.3f }, { 1.0f, 1.0f, 0.0f } },

        { { -0.6f, -0.8f }, { 1.0f, 0.0f, 0.0f } },
        { { -0.4f, -0.4f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.8f, -0.4f }, { 0.0f, 0.0f, 1.0f } },
 
    };

    const uint16_t pIndices[] = {
          0, 1, 2, 0, 2, 3
        , 4, 5, 6 
    };

    DataInfo vertexInfo;
    vertexInfo.pData = pVertices;
    vertexInfo.elementCount = 7;
    vertexInfo.elementSize = sizeof(Vertex);
    vertexInfo.type = VERTEX;

    DataInfo indexInfo;
    indexInfo.pData = pIndices;
    indexInfo.elementCount = 9;
    indexInfo.elementSize = sizeof(uint16_t);
    indexInfo.type = INDEX;

    pDataLoader_->Begin(*pDevice_, *pCommandManager_);

    pDataLoader_->LoadData(*pDevice_, vertexInfo, *pCommandManager_);
    pDataLoader_->LoadData(*pDevice_, indexInfo, *pCommandManager_);

    pDataLoader_->End(*pDevice_, *pCommandManager_);

    pRegulator_->Create(*pDevice_);

    setting_.Vulkan().CurrentFrame() = 0;
}

void Engine::Terminate(Surface& surface) {
    vkDeviceWaitIdle(pDevice_->Access());

    pRegulator_->Destroy(*pDevice_);
    pDataLoader_->Destroy(*pDevice_);
    pCommandManager_->Destroy(*pDevice_);
    pSwapchain_->Destroy(*pDevice_);
    pPipeline_->Destroy(*pDevice_);
    pDevice_->Destroy();
    surface.Destroy(*pInstance_);

    delete pAttachments_;
    delete pInstance_;
    delete pDevice_;
    delete pSwapchain_;
    delete pPipeline_;
    delete pDataLoader_;
    delete pCommandManager_;
    delete pRegulator_;
}

} //vk

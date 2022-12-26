#include "vk/vk_engine.hpp"

namespace vk {

uint32_t Engine::Acquire(Surface& surface) {
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(pDevice_->Access(), pSwapchain_->Access()
        , UINT64_MAX, imageAvailable_[currentFrame_].Access()
        , VK_NULL_HANDLE, &imageIndex);

    if(result == VK_ERROR_OUT_OF_DATE_KHR) {
        pSwapchain_->Recreate(*pDevice_, surface, *pPipeline_);
    } else {
        ErrorManager::Validate(result, "Image acquiring");
    }
    return imageIndex;
}

void Engine::Submit(VkSemaphore *pWaitSemaphores, VkSemaphore *pSignalSemaphores) {
    auto commandBuffer = pCommandBuffers_->AccessGraphic();
    advance(commandBuffer, currentFrame_);

    VkPipelineStageFlags waitStages[] = 
        { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    VkSubmitInfo submitInfo {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = pWaitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &(*commandBuffer);
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = pSignalSemaphores;
    
    VkResult result = vkQueueSubmit(pDevice_->AccessQueues().graphic.queue
        , 1, &submitInfo, inFlight_[currentFrame_].Access());
    ErrorManager::Validate(result, "Drawing");
}

void Engine::Present(Surface& surface, VkSemaphore *pWaitSemaphores
    , uint32_t imageIndex) {

    VkSwapchainKHR swapchain = pSwapchain_->Access();
    VkPresentInfoKHR presentInfo {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = pWaitSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &imageIndex;

    VkResult result = vkQueuePresentKHR(pDevice_->AccessQueues().present.queue
        , &presentInfo);
    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        pSwapchain_->Recreate(*pDevice_, surface, *pPipeline_);
    } else {
        ErrorManager::Validate(result, "Presentation");
    }
}

void Engine::Update(Surface& surface) {
    vkWaitForFences(pDevice_->Access(), 1, &(inFlight_[currentFrame_].Access())
        , VK_TRUE, UINT64_MAX);

    uint32_t imageIndex = Acquire(surface);

    vkResetFences(pDevice_->Access(), 1, &(inFlight_[currentFrame_].Access()));

    auto commandBuffer = pCommandBuffers_->AccessGraphic();
    advance(commandBuffer, currentFrame_);
    vkResetCommandBuffer(*commandBuffer, 0);
    pCommandBuffers_->RecordDrawCommands(*pDevice_, *pPipeline_, *pSwapchain_
        , imageIndex, *pVertexBuffer_, 0, commandBuffer);

    VkSemaphore pSignalSemaphores[] = { renderFinished_[currentFrame_].Access() };
    VkSemaphore pWaitSemaphores[] = { imageAvailable_[currentFrame_].Access() };
    
    Submit(pWaitSemaphores, pSignalSemaphores);
    Present(surface, pSignalSemaphores, imageIndex);

    currentFrame_ = (imageIndex+1) % AppSetting::frames;
}

void Engine::Init(Surface& surface) {
    pAttachments_ = new LayersAndExtensions;
    pInstance_ = new Instance;
    pDevice_ = new Device;
    pSwapchain_ = new Swapchain; 
    pPipeline_ = new GraphicPipeline;
    pCommandPools_ = new CommandPool[2];
    pCommandBuffers_ = new CommandBuffers;
    pVertexBuffer_ = new Buffer;

    pInstance_->IncludeDefaultLayersAndExtensions(*pAttachments_);
    pInstance_->Create(*pAttachments_);

    surface.Create(*pInstance_);

    pDevice_->PickGpu(*pInstance_, surface, *pAttachments_);
    pDevice_->SetQueueFamilies(surface);
    pDevice_->CreateLogicalDevice(surface, *pAttachments_);

#ifdef DEBUG
    LayersAndExtensions::PrintAvailableLayersAndExtensions(pDevice_->AccessGpu());
#endif

    pSwapchain_->Create(*pDevice_, surface);

    const std::vector<std::string> shadersPath = {
    //    "build/vert_trivial.spv",
        "build/vert_vertex_buffer.spv",
        "build/frag_trivial.spv"
    };
    pPipeline_->Create(*pDevice_, *pSwapchain_, shadersPath);
    pSwapchain_->CreateFramebuffers(*pDevice_, *pPipeline_);

    pCommandPools_[GRAPHICS].Create(*pDevice_, GRAPHICS);
    pCommandPools_[TRANSFER].Create(*pDevice_, TRANSFER);
    pCommandBuffers_->Allocate(*pDevice_, GRAPHICS, pCommandPools_[GRAPHICS]
        , AppSetting::frames);
    pCommandBuffers_->Allocate(*pDevice_, TRANSFER, pCommandPools_[TRANSFER], 1);

    const std::vector<Vertex> vertices {
        { {  0.0f, -0.5f }, { 1.0f, 0.0f, 0.0f } },
        { {  0.5f,  0.5f }, { 0.0f, 1.0f, 0.0f } },
        { { -0.5f,  0.5f }, { 0.0f, 0.0f, 1.0f } }
    };

    pVertexBuffer_->Create(*pDevice_, *pCommandBuffers_, VERTEX_BUFFER
        , vertices.data(), sizeof(Vertex) * vertices.size());

    for(size_t i = 0; i < AppSetting::frames; i++) {
        imageAvailable_[i].Create(*pDevice_);
        renderFinished_[i].Create(*pDevice_);
        inFlight_[i].Create(*pDevice_);
    }

    pCommandBuffers_->Free(*pDevice_, pCommandPools_[TRANSFER]
        , pCommandBuffers_->AccessTransfer(), 1);
    pCommandPools_[TRANSFER].Destroy(*pDevice_);

}

void Engine::Terminate(Surface& surface) {
    vkDeviceWaitIdle(pDevice_->Access());

    for(size_t i = 0; i < AppSetting::frames; i++) {
        imageAvailable_[i].Destroy(*pDevice_);
        renderFinished_[i].Destroy(*pDevice_);
        inFlight_[i].Destroy(*pDevice_);
    }
    pVertexBuffer_->Destroy(*pDevice_);
    pCommandBuffers_->Free(*pDevice_, pCommandPools_[GRAPHICS]
        , pCommandBuffers_->AccessGraphic(), 1);
    pCommandPools_[GRAPHICS].Destroy(*pDevice_);
    pSwapchain_->Destroy(*pDevice_);
    pPipeline_->Destroy(*pDevice_);
    pDevice_->Destroy();
    surface.Destroy(*pInstance_);

    delete pAttachments_;
    delete pInstance_;
    delete pDevice_;
    delete pSwapchain_;
    delete pPipeline_;
    delete pCommandBuffers_;
    delete pVertexBuffer_;
}

} //vk

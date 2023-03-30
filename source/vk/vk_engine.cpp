#include "vk/vk_engine.hpp"

namespace vk {

void Engine::Acquire(Surface& surface) {
    VkResult result = regulator_.BeginRender(device_, swapchain_
        , setting_.Vulkan());
    if(result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapchain_.Recreate(device_, surface, imageBuilder_, setting_.Vulkan()
            , pipeline_.AccessRenderPass());
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
        swapchain_.Recreate(device_, surface, imageBuilder_, setting_.Vulkan()
            , pipeline_.AccessRenderPass());
    } else {
        ErrorManager::Validate(result, "Presentation");
    }
}

void Engine::Update(Surface& surface, const Camera& camera) {

    uint32_t &currentFrame = setting_.Vulkan().CurrentFrame();

    void *mappedData;
    vkMapMemory(device_.Access(), ubos_[currentFrame].memory
        , 0, sizeof(MvpMatrix), 0, &mappedData);
    memcpy(mappedData, &camera.Access(), sizeof(MvpMatrix));
    vkUnmapMemory(device_.Access(), ubos_[currentFrame].memory);

    Acquire(surface);

    CommandManager::RecordDrawCommands(graphicCommands_[currentFrame]
        , swapchain_.AccessFramebuffer(setting_.Vulkan().ImageIndex())
        , descriptorSet_.Access()[currentFrame], setting_.Vulkan()
        , pipeline_, modelStorage_);

    VkSubmitInfo submitInfo {};
    VkPresentInfoKHR presentInfo {};

    regulator_.Sync(setting_.Vulkan(), submitInfo, presentInfo);

    CommandManager::Submit(graphicCommands_.data() + currentFrame, 1
        , device_.AccessQueues().graphic.queue
        , regulator_.AccessFence(currentFrame), submitInfo);

    Present(surface, presentInfo);

    currentFrame = (currentFrame+1) % vk::Setting::frames;
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

    bufferBuilder_.Init(device_);
    imageBuilder_.Init(device_);
    swapchain_.Create(device_, surface, imageBuilder_, setting_.Vulkan());
    
    graphicPool_ = CommandManager::CreatePool(device_.Access()
        , device_.AccessQueues().graphic.index.value()
        , VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
        | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

    graphicCommands_ = CommandManager::CreateCommandBuffers(
        device_.Access(), graphicPool_, vk::Setting::frames);

    modelStorage_.Init(device_);
    modelStorage_.Add( { "resources/teapot.obj", "resources/picture1.tga" } );
    std::pair<std::vector<ModelView>, std::pair<Buffer, Buffer>>
        modelViews = modelStorage_.SubmitModels();

    for(size_t i = 0; i < vk::Setting::frames; i++) {
        ubos_[i] = bufferBuilder_.BuildBuffer(nullptr, sizeof(MvpMatrix)
            , VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
            , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    }
    const std::vector<std::string> shadersPath = {
        "build/vert_texture.spv",
        "build/frag_texture.spv"
    };

    descriptorPool_.Create(device_);
    descriptorSet_.Create(device_, descriptorPool_, ubos_, modelViews.first[0].image);   

    pipeline_.Create(device_, swapchain_, shadersPath, descriptorSet_);
    swapchain_.CreateFramebuffers(pipeline_.AccessRenderPass());

    regulator_.Create(device_);
  
    setting_.Vulkan().CurrentFrame() = 0;
}

void Engine::Terminate(Surface& surface) {
    vkDeviceWaitIdle(device_.Access());

    for(size_t i = 0; i < vk::Setting::frames; i++) {
        ubos_[i].Destroy(device_.Access());
    }
    vkDestroyCommandPool(device_.Access(), graphicPool_, nullptr);
    modelStorage_.Destroy();
    imageBuilder_.Destroy();
    bufferBuilder_.Destroy();
    descriptorSet_.Destroy(device_, descriptorPool_);   
    descriptorPool_.Destroy(device_);
    regulator_.Destroy(device_);
    swapchain_.CleanUp();
    pipeline_.Destroy(device_);
    device_.Destroy();
    surface.Destroy(instance_);
}

} //vk

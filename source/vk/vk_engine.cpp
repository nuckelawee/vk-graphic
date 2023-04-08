#include "vk/vk_engine.hpp"
#include "vk/vk_surface.hpp"
#include "vk/vk_camera.hpp"
#include "vk/vk_exception.hpp"

namespace vk {

void Engine::Acquire() noexcept {
    VkResult result = regulator_.BeginRender(swapchain_.Access());
    if(result == VK_ERROR_OUT_OF_DATE_KHR) {
        swapchain_.Recreate(device_, *surface_, imageBuilder_
            , pipeline_.AccessRenderPass());
    } else {
        ErrorManager::Validate(result, "Image acquiring");
    }
}
     
void Engine::Present(VkPresentInfoKHR& presentInfo) {
    VkSwapchainKHR swapchain = swapchain_.Access();
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &swapchain;
    presentInfo.pImageIndices = &vk::Settings::GetInstance().ImageIndex();

    VkResult result = vkQueuePresentKHR(device_.AccessQueues().present.queue
        , &presentInfo);

    if(result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {
        swapchain_.Recreate(device_, *surface_, imageBuilder_
            , pipeline_.AccessRenderPass());
    } else {
        ErrorManager::Validate(result, "Image presentation");
    }
}

void Engine::Update() {

    Acquire();

    uint32_t currentFrame = vk::Settings::GetInstance().CurrentFrame();
    uint32_t imageIndex = vk::Settings::GetInstance().ImageIndex();

    CommandManager::RecordDrawCommands(graphicCommands_[currentFrame]
        , swapchain_.AccessFramebuffer(imageIndex)
        , descriptorSet_.Access()[currentFrame]
        , pipeline_, modelStorage_);

    VkSubmitInfo submitInfo {};
    VkPresentInfoKHR presentInfo {};

    regulator_.Sync(submitInfo, presentInfo);

    CommandManager::Submit(graphicCommands_.data() + currentFrame, 1
        , device_.AccessQueues().graphic.queue
        , regulator_.AccessFence(currentFrame), submitInfo);

    Present(presentInfo);

    vk::Settings::GetInstance().CurrentFrame() =
        (currentFrame+1) % vk::Settings::frames;
}

Window* Engine::Init() {

    try {
        instance_.IncludeDefaultLayersAndExtensions(attachments_);
        instance_.Create(attachments_);

        surface_ = new Surface(instance_.Access());

        device_.PickGpu(instance_, *surface_, attachments_);
        device_.SetQueueFamilies(*surface_);
        device_.CreateLogicalDevice(*surface_, attachments_);

#ifdef DEBUG
        LayersAndExtensions::PrintAvailableLayersAndExtensions(device_.AccessGpu());
#endif

        bufferBuilder_.Init(device_);
        imageBuilder_.Init(device_);
        swapchain_.Create(device_, *surface_, imageBuilder_);
    
        graphicPool_ = CommandManager::CreatePool(device_.Access()
            , device_.AccessQueues().graphic.index.value()
            , VK_COMMAND_POOL_CREATE_TRANSIENT_BIT
            | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT);

        graphicCommands_ = CommandManager::CreateCommandBuffers(
            device_.Access(), graphicPool_, vk::Settings::frames);

        modelStorage_.Init(device_);
        modelStorage_.Add( { "resources/teapot.obj", "resources/picture1.tga" } );
        std::pair<std::vector<ModelView>, std::pair<Buffer, Buffer>>
            modelViews = modelStorage_.SubmitModels();

        const std::vector<std::string> shadersPath = {
            "build/vert_texture.spv",
            "build/frag_texture.spv"
        };

        for(size_t i = 0; i < vk::Settings::frames; ++i) {
            ubos_[i] = bufferBuilder_.BuildBuffer(nullptr, sizeof(MvpMatrix)
                , VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
                , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
                | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        }

        descriptorPool_.Create(device_);
        descriptorSet_.Create(device_, descriptorPool_, ubos_.data()
            , modelViews.first[0].image);   

        pipeline_.Create(device_, swapchain_, shadersPath, descriptorSet_);
        swapchain_.CreateFramebuffers(pipeline_.AccessRenderPass());

        regulator_.Create(device_.Access());
  
    } catch(const Exception& info) {
        ErrorManager::Validate(info.Result(), info.What());
    }
    return static_cast<Window*>(surface_);
}

BaseCamera* Engine::CreateCamera() noexcept {
   return static_cast<BaseCamera*>(new vk::Camera(device_.Access(), std::move(ubos_)));
}

Engine::~Engine() {
    vkDeviceWaitIdle(device_.Access());

    vkDestroyCommandPool(device_.Access(), graphicPool_, nullptr);
    modelStorage_.Destroy();
    imageBuilder_.Destroy();
    bufferBuilder_.Destroy();
    descriptorSet_.Destroy(device_, descriptorPool_);   
    descriptorPool_.Destroy(device_);
    regulator_.Destroy();
    swapchain_.CleanUp();
    pipeline_.Destroy();
    device_.Destroy();
    delete surface_;
}

} //vk

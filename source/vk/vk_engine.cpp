#include "vk/vk_engine.hpp"
#include "vk/vk_surface.hpp"
#include "vk/vk_exception.hpp"
#include "base_camera.hpp"
//#include "mvp_matrix.hpp"

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

void Engine::UpdateCamera() noexcept {
    uint32_t currentFrame = vk::Settings::GetInstance().CurrentFrame();
    void *mappedPtr;
    vkMapMemory(device_.Access(), ubos_[currentFrame].memory
        , 0, sizeof(MvpMatrix), 0, &mappedPtr);
    memcpy(mappedPtr, cameraData_, sizeof(MvpMatrix));
    vkUnmapMemory(device_.Access(), ubos_[currentFrame].memory);
}

void Engine::Render(const std::vector<Model>& models) {

    Acquire();

    uint32_t currentFrame = vk::Settings::GetInstance().CurrentFrame();
    uint32_t imageIndex = vk::Settings::GetInstance().ImageIndex();

    UpdateCamera();
    CommandManager::RecordDrawCommands(graphicCommands_[currentFrame]
        , models
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

        modelStorage_.Init(device_.Access());

        const std::vector<std::string> shadersPath = {
            "build/vert_texture.spv",
            "build/frag_texture.spv"
        };

        descriptorSet_.Create(device_.Access());

        pipeline_.Create(device_, swapchain_, shadersPath, descriptorSet_);
        swapchain_.CreateFramebuffers(pipeline_.AccessRenderPass());

        regulator_.Create(device_.Access());
  
        ubos_.resize(vk::Settings::frames);
    } catch(const Exception& info) {
        ErrorManager::Validate(info.Result(), info.What());
    }
    return static_cast<Window*>(surface_);
}

std::vector<Model> Engine::Bind(ModelSet& modelSet) {
    VkDeviceSize vertexBuffSize = sizeof(Vertex3D) * modelSet.vertices.size(); 
    Buffer vertexBuffer = bufferBuilder_.BuildBuffer(modelSet.vertices.data()
        , vertexBuffSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT
        , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VkDeviceSize indexBuffSize = sizeof(uint32_t) * modelSet.indices.size(); 
    Buffer indexBuffer = bufferBuilder_.BuildBuffer(modelSet.indices.data()
        , indexBuffSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT
        , VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    std::vector<Image> images(modelSet.textures.size());
    std::vector<Model> models(modelSet.textures.size());
    for(size_t i = 0; i < models.size(); ++i) {
        images[i] = imageBuilder_.BuildImage(bufferBuilder_, modelSet.textures[i]);
        models[i].sampleIndex = i;

        VkDescriptorImageInfo imageInfo {};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = images[i].view;
        imageInfo.sampler = images[i].sampler;

        descriptorSet_.BindImage(imageInfo, 0);
        descriptorSet_.BindImage(imageInfo, 1);
    }


    uint32_t familyIndex = modelStorage_.Insert(vertexBuffer, indexBuffer
        , std::move(modelSet.meshes), std::move(images));
    for(size_t i = 0; i < models.size(); ++i)
    { models[i].familyIndex = familyIndex; }

    return models;
}

BaseCamera* Engine::CreateCamera() noexcept {
    for(size_t i = 0; i < vk::Settings::frames; ++i) {
        ubos_[i] = bufferBuilder_.BuildBuffer(nullptr, sizeof(MvpMatrix)
            , VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT
            , VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

        VkDescriptorBufferInfo bufferInfo {};
        bufferInfo.buffer = ubos_[i].buffer;
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(MvpMatrix);

        descriptorSet_.BindBuffer(bufferInfo, i);
    }
    BaseCamera* camera = new BaseCamera;
    cameraData_ = camera->Matrix();
    return camera;
}

void Engine::DestroyCamera(BaseCamera* camera) noexcept {
    vkDeviceWaitIdle(device_.Access());
    for(size_t i = 0; i < vk::Settings::frames; ++i) {
        ubos_[i].Destroy(device_.Access());
    }
    delete camera;
}

Engine::~Engine() {
    vkDeviceWaitIdle(device_.Access());

    vkDestroyCommandPool(device_.Access(), graphicPool_, nullptr);
    modelStorage_.Destroy();
    imageBuilder_.Destroy();
    bufferBuilder_.Destroy();
    descriptorSet_.Destroy();   
    regulator_.Destroy();
    swapchain_.CleanUp();
    pipeline_.Destroy();
    device_.Destroy();
    delete surface_;
}

} //vk

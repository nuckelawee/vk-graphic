#include "vk/vk_engine.hpp"

namespace vk {

void VulkanEngine::Update(AppSetting& setting, Window& window) {

}

void VulkanEngine::Init(AppSetting& setting, Window& window) {
    instance_.IncludeDefaultLayersAndExtensions(attachments_);
    instance_.Create(attachments_, setting);

    window.CreateSurface(instance_.Access());

    device_.PickGpu(instance_.Access(), window.AccessSurface(), attachments_);
    device_.CreateLogicalDevice(instance_.Access(), window.AccessSurface()
        , attachments_);
    device_.SetQueueFamilies(window.AccessSurface());

    swapchain_.Create(device_, window.AccessSurface());
#ifdef DEBUG
    VulkanLayersAndExtensions::PrintAvailableLayersAndExtensions(device_.AccessGpu());
#endif
}

void VulkanEngine::Terminate(Window& window) {
    swapchain_.Destroy(device_.Access());
    device_.Destroy();
    window.DestroySurface(instance_.Access());
}

} //vk

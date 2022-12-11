#include "vk/vk_engine.hpp"

namespace vk {

void Engine::Update(AppSetting& setting, Surface& surface) {

}

void Engine::Init(AppSetting& setting, Surface& surface) {
    instance_.IncludeDefaultLayersAndExtensions(attachments_);
    instance_.Create(attachments_, setting);

    surface.Create(instance_);

    device_.PickGpu(instance_, surface, attachments_);
    device_.CreateLogicalDevice(surface, attachments_);
    device_.SetQueueFamilies(surface);

#ifdef DEBUG
    LayersAndExtensions::PrintAvailableLayersAndExtensions(device_.AccessGpu());
#endif

    swapchain_.Create(device_, surface);

    const std::vector<std::string> shadersPath = {
        "build/trivial_vert.spv",
        "build/trivial_frag.spv"
    };
    pipeline_.Create(device_, swapchain_, shadersPath);
    swapchain_.CreateFramebuffers(device_, pipeline_);

    commandPool_.Create(device_);
    commandBuffer_.Create(device_, commandPool_);
}

void Engine::Terminate(Surface& surface) {
    commandPool_.Destroy(device_);
    swapchain_.Destroy(device_);
    pipeline_.Destroy(device_);
    device_.Destroy();
    surface.Destroy(instance_);
}

} //vk

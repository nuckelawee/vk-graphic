#include "app.hpp"

void App::Run() {
    Init();
    while(!window_.ShouldClosed()) {

    }
}

void App::Init() {
    window_.CreateWindow(globalSetting_);

    instance_.IncludeDefaultLayersAndExtensions(attachments_);
    instance_.Create(attachments_, globalSetting_);

    window_.CreateSurface(instance_.Access());

    device_.PickGpu(instance_.Access(), window_.AccessSurface(), attachments_);
    device_.CreateLogicalDevice(instance_.Access(), window_.AccessSurface()
        , attachments_);
    device_.SetQueueFamilies(window_.AccessSurface());

#ifdef DEBUG
    VulkanLayersAndExtensions::PrintAvailableLayersAndExtensions(device_.AccessGpu());
#endif
}

App::~App() {
    device_.DestroyDevice();
    window_.DestroySurface(instance_.Access());
}

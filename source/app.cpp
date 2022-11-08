#include "app.hpp"

bool App::CheckState() {
    if(globalSetting_.State() == APP_TERMINATE) {
        window_.CloseWindow();
        return false;
    }
    if(globalSetting_.State() == APP_STOP) {
        return true;
    }
    return false;
}

void App::Update() {
    bool notWork = true;
    while(notWork) {
        notWork = CheckState();
    }
    statistic_.Update();
}

void App::Run() {
    threadPool_.Submit(Task([&]() {
        Console::Input(globalSetting_);
    }));
    Init();
    while(!window_.ShouldClosed()) {
        Update(); 
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

App::App() : threadPool_(std::thread::hardware_concurrency()-1) {}

App::~App() {
    device_.DestroyDevice();
    window_.DestroySurface(instance_.Access());
}

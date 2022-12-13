#include "app.hpp"

bool App::CheckState() {
    if(AppSetting::Get().State() == APP_TERMINATE) {
        window_.CloseWindow();
        return false;
    }
    if(AppSetting::Get().State() == APP_STOP) {
        return true;
    }
    return false;
}

void App::Update() {
    bool notWork = true;
    while(notWork) {
        notWork = CheckState();
    }
    renderer_.Update(static_cast<vk::Surface&>(window_));
    statistic_.Update();
}

void App::Run() {
/*
    threadPool_.Submit(Task([&]() {
        Console::Input(appSetting_);
    }));
*/
    Init();
    while(!window_.ShouldClosed()) {
        Update(); 
        glfwPollEvents();
    }
    AppSetting::Get().ChangeState(APP_TERMINATE);
    AppSetting::ShutDown();
}

void App::Init() {
    AppSetting::StartUp();
    window_.CreateWindow();
    renderer_.Init(window_);
}

App::App() : threadPool_(std::thread::hardware_concurrency()-1) {}

App::~App() {
    renderer_.Terminate(window_);
}

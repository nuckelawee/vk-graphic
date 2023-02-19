#include "app.hpp"

bool App::CheckState() {
    if(setting_.State() == APP_TERMINATE) {
        pWindow_->CloseWindow();
        return false;
    }
    if(setting_.State() == APP_STOP) {
        return true;
    }
    return false;
}

void App::Update() {
    bool notWork = true;
    while(notWork) {
        notWork = CheckState();
    }
    camera.Update(setting_);
    pRenderer_->Update(static_cast<vk::Surface&>(*pWindow_), camera);
    setting_.Update();
}

void App::Run() {
/*
    threadPool_.Submit(Task([&]() {
        Console::Input(appSetting_);
    }));
*/
    Init();
    while(!pWindow_->ShouldClosed()) {
        Update(); 
        glfwPollEvents();
    }
}

void App::Init() {
    pWindow_ = new GlfwWindow(setting_);
    pWindow_->CreateWindow();
    pRenderer_ = new vk::Engine(setting_);
    pRenderer_->Init(*pWindow_);
}

App::App() : threadPool_(std::thread::hardware_concurrency()-1) {}

App::~App() {
    pRenderer_->Terminate(*pWindow_);
    delete pRenderer_;
    delete pWindow_;
}
